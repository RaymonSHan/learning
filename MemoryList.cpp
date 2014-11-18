
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ULONG volatile GlobalTime	= 0;
extern CListItem*				isNULL;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	implement of CMemoryBlock																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CMemoryBlock::CMemoryBlock()																										//
{																																	//
	StartBlock = NULL;																												//
	TotalBlock = NULL;																												//
	BorderSize = 0;																													//
	TotalSize = 0;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	'size' of item, 'number' of item, every item base on 'border' byte edge															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
void* CMemoryBlock::SetMemoryBlock( long size, long number, long border )															//
{																																	//
	if (border <=0 ) return NULL;																									//
	if (border & (border-1)) return NULL;						//	the border is not 2^N											//
																//////////////////////////////////////////////////////////////////////
	BorderSize = ( (size-1)&(-1*border) ) + border;			//	the real size of item base on border							//
	TotalSize = BorderSize * number + border * 3;				//	Total size of memory block, with surplus						//
																//																	//
	StartBlock = MYMALLOC(TotalSize);							//	This is the ONLY malloc in release version						//
	if (StartBlock)																													//
	{																																//
		TotalBlock = (void*)((((MYINT)StartBlock)&(-1*border))+ 2*border - (sizeof(CListItem)%border) );							//
		ZeroMemory(StartBlock, TotalSize);						//	TotalBlock pointer to first border edge after StartBlock		//
	}															//////////////////////////////////////////////////////////////////////
	else																															//
	{																																//
		TotalSize = 0;																												//
		TotalBlock = 0;																												//
	}																																//
	return StartBlock;																												//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMemoryBlock::~CMemoryBlock()																										//
{																																	//
	TotalBlock = NULL;																												//
	BorderSize = 0;																													//
	TotalSize = 0;																													//
	if (StartBlock) MYFREE(StartBlock);							//	This is the ONLY free in release version						//
																//////////////////////////////////////////////////////////////////////
	StartBlock = NULL;																												//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Trace the called number of malloc																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
size_t TotaoMallocTimes = 0;																										//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* mymalloc( size_t size )																										//
{																																	//
	TotaoMallocTimes++;																												//
	return malloc(size);																											//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void myfree( void *memblock )																										//
{																																	//
	TotaoMallocTimes--;																												//
	free(memblock);																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	implement of CMemoryList																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CMemoryList::CMemoryList()																											//
{																																	//
	MemoryBuffer = NULL;																											//
	TotalBuffer = NULL;																												//
	TotalNumber = FreeNumber = 0;																									//
	FreeBufferStart = FreeBufferEnd = 0;																							//
	m_ItemSaveRemain = ITEM_SAFE_REMAIN;																							//
																																	//
	UsedItem = NULL;																												//
	InUsedListProcess = 0;																											//
																																	//
#ifdef _DEBUG																														//
	GetCount = 0;																													//
	GetSuccessCount = 0;																											//
	FreeCount = 0;																													//
	FreeSuccessCount = 0;																											//
	hStart = CreateEvent(0, TRUE, FALSE, 0);					//	Test thread should start at the same time						//
#endif _DEBUG													//																	//
}																//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMemoryList::~CMemoryList()																											//
{																																	//
	FreeMemoryBuffer();																												//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMemoryList::SetListDetail(char* listname, long directFree, long timeout)														//
{																																	//
	strcpy_s(ListName, NORMAL_CHAR, listname);																						//
	DirectFree = directFree;																										//
	TimeoutInit = timeout;																											//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMemoryList::FreeMemoryBuffer( void )																							//
{																																	//
	if (MemoryBuffer) delete (MemoryBuffer);																						//
	MemoryBuffer = 0;																												//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Some list, such as IOCP list, only contain one item, it return the only item													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CListItem* CMemoryList::GetOnlyOneList()																							//
{																																	//
	return TotalBuffer;																												//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Every CMemoryList has it own TimeoutContext, be called by system Timer one by one, for NonDirectly MemoryList.					//
//	Then free the item when countdown to 0, and release the connect																	//
//	Although Get/Free is multi-thread, this TimeoutContext is only single-thread, and NON-REENTRY.									//

// NOW only test for TCP.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
void CMemoryList::TimeoutContext(void)																								//
{																																	//
	CListItem **ppContext = 0, *doContext = 0;																						//
	static volatile MYINT inTimeout = 0;																							//
	int nSeconds, nLen;																												//
volatile MYINT*   listProcess;
CListItem **ppBuffer, *doBuffer;
																																	//
	GlobalTime ++;												//	used for another kind of countdown, In ConfirmDNS

	if (inTimeout) return;										//	This function is NON-REENTRY									//
	inTimeout = 1;												//////////////////////////////////////////////////////////////////////
																//																	//
	if (UsedItem)
	{
// DisplayContext();
//DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_STATUS, "(%x)", UsedItem);
		ppContext = &((CListItem*)UsedItem);						//	Traversal the list												//
		while (*ppContext)																												//
		{																																//
			if ( ((*ppContext)->ListFlag & FLAG_IS_CONTEXT) && ((*ppContext)->countDown==TIMEOUT_INFINITE) &&
				((*ppContext)->PProtocol->ProtocolNumber==PROTOCOL_TCP) && (*ppContext)->CliBuffer/*tcp is server side*/)		//
			{																															//
				nLen = sizeof(nSeconds);																								//

				ppBuffer = &((*ppContext)->CliBuffer->CliListNext);
				listProcess = &((CTCPProtocol*)((*ppContext)->PProtocol))->CliListInProcess;

				while (*ppBuffer != MARK_CLIBUFFER_END)
				{
					doBuffer = (*ppBuffer);
					if (doBuffer->CliWaitData == MARK_IN_PROCESS)
					{
						::getsockopt( (SOCKET)(doBuffer->CliHandle), SOL_SOCKET, SO_CONNECT_TIME, (char *)&nSeconds, &nLen);	//
						if ( nSeconds > TIMEOUT_ACCEPT && nSeconds != -1 )
						{
							doBuffer->CliWaitData = 0;
							::closesocket((SOCKET)(doBuffer->CliHandle));		//
// DEBUG_MESSAGE_CON(MODULE_MEMORY, MESSAGE_STATUS, "(c)");
						}
					}
					if (doBuffer->CliWaitData == 0)
					{
// DEBUG_MESSAGE_CON(MODULE_MEMORY, MESSAGE_STATUS, "(t)");
						while ( InterCmpExg(listProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );							//
						*ppBuffer = doBuffer->CliListNext;
						doBuffer->CliListNext = 0;
						*listProcess = MARK_NOT_IN_PROCESS;
					}
					if (*ppBuffer != MARK_CLIBUFFER_END) ppBuffer = &((*ppBuffer)->CliListNext);
				}
			}																															//
			if ( ((*ppContext)->countDown < TIMEOUT_INFINITE) && ( !((*ppContext)->countDown--) ) )									//
			{																															//
				while ( InterCmpExg(&InUsedListProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );					//
				doContext = (*ppContext);																								//
				(*ppContext) = (CListItem*)((*ppContext)->usedList);																	//
				InUsedListProcess = MARK_NOT_IN_PROCESS;			//	Remove the item from list for countdown							//
																	//////////////////////////////////////////////////////////////////////
				if ( ((doContext->ListFlag & (FLAG_IS_CONTEXT | FLAG_LATER_CLOSE)) == FLAG_IS_CONTEXT) && (doContext->BHandle) )		//
				{
					ProFunc(doContext->PProtocol, fPostClose)((CContextItem*)doContext, isNULL, 0, 0);									//
// 					__asm int 3	
// 					__asm int 3	
				}
																																		//
				doContext->countDown = 0;																								//
				doContext->usedList = 0;																								//
				FreeOneList(doContext);																									//
			}																															//
			if (*ppContext) ppContext = (CListItem**)(&((*ppContext)->usedList));																		//
		}																																//
	}
	inTimeout = 0;																													//
	return;																															//
}																																	//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	To find whether the same keyword is stored in the list.	To compare the item+'start' is equal with 'source' for 'length' byte.	//
//	If equal, return the item, or return NULL																						//
//	This function is a test version, may have bug in it. I will update it by a hash version for quickly								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CListItem* CMemoryList::Comparekeyword(int start, int length, char* source)
{
	CListItem **ppContext = 0, *doContext = 0;

sockaddr_in* sour = (sockaddr_in*)source;
printf("in comp:%x:%x\r\n", sour->sin_port, sour->sin_addr);
	ppContext = &((CListItem*)UsedItem);
	while (TRUE) 
	{
		doContext = *ppContext;
		if (doContext)
		{
			if ( !memcmp(((char*)doContext)+start, source, length ) ) return doContext;
			ppContext = (CListItem**)(&((*ppContext)->usedList));
		}
		else
		{
			if (doContext == *ppContext) return NULL;			//	?
			else doContext = *ppContext;						//	?
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	In fact, it is a debug function. it display message to console for debug use													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
void CMemoryList::DisplayContext()																									//
{																																	//
	CListItem* mList = UsedItem;																									//
	while (mList)																													//
	{																																//
		printf("0x%x:%d->", mList, mList->countDown);																				//
		mList = (CListItem*)mList->usedList;																						//
	}																																//
	if (UsedItem) printf("\r\n");																									//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CMemoryList::ResetCount()
{
	GetCount = 0;
	GetSuccessCount = 0;
	FreeCount = 0;
	FreeSuccessCount = 0;

	FreeNumber = TotalNumber;
	MinFree = TotalNumber;
	ResetEvent(hStart);
}

void CMemoryList::CheckEmptyList( void )
{
	MYINT i = 0;
	CListItem* listitem;

	listitem = (CListItem*)FreeBufferStart;
	do 
	{
		listitem = listitem->nextList;
		i++;
	} while (listitem != MARK_FREE_END);

	DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_DEBUG_DETAIL, "TotalItem:%5d, FreeItem:%5d, Remain:%5d, CheckInList:%5d\r\n", TotalNumber, FreeNumber, m_ItemSaveRemain, i);
	DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_DEBUG_DETAIL, "Get:%10d, Succ:%10d, Free:%10d, Succ:%10d\r\n", GetCount, GetSuccessCount, FreeCount, FreeSuccessCount);
	DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_DEBUG_DETAIL, "MinFree:%5d\r\n", MinFree);
}

void CMemoryList::TestList( MYINT threadNumber, MYINT getNumber, MYINT fullTimes, BOOL wait/*=0*/ ) /* every thread get fullNumber times, then free all. Do this fullTimes. */
{
	static volatile MYINT totalThread = 0;
	static HANDLE threadGetFree[MAXIMUM_WAIT_OBJECTS];
	MYINT i;
	pthreadPara para;
	unsigned controlThreadAddr;


	for (i=0; i<threadNumber; i++)
	{
		para = (pthreadPara)malloc(sizeof(threadPara));
		para->pMemoryList = this;
		para->threadID = InterInc(&totalThread)-1;
		para->getnumber = getNumber;
		para->fullnumber = fullTimes;
		threadGetFree[para->threadID] = 
			(HANDLE)_beginthreadex ( NULL, 0, ThreadItem, (LPVOID)para, 0, &controlThreadAddr );
	}
	if (wait) 
	{
		SetEvent(hStart);

		WaitForMultipleObjects((LONG)totalThread, threadGetFree, TRUE, INFINITE);			// in debug test
		for (i=0; i<threadNumber; i++) 
		{
			TerminateThread(threadGetFree[i], 1);
			CloseHandle(threadGetFree[i]);
		}
		totalThread = 0;		// this means, this test is finished, wait for another test. // I found this line, for an hour.
	}
}

UINT WINAPI CMemoryList::ThreadItem( LPVOID lpParam )
{
#define MAX_TRY_TIME		8

	pthreadPara lppara = (pthreadPara)lpParam;
	CMemoryList* pthis = lppara->pMemoryList;
	CListItem **itembuffer = (CListItem**) malloc(sizeof(void*)*(2+lppara->getnumber));
	CListItem **nowitem;
	MYINT i, j, k;
	MYINT ret;

	WaitForSingleObject(pthis->hStart, INFINITE);				// all thread start at the same time
	for (i=0; i<lppara->fullnumber; i++)
	{
		nowitem = itembuffer;
		for (j=0; j<lppara->getnumber; j++)
		{
			k = 0;
			do 
			{
				*nowitem = pthis->GetOneList();
				k++;
			}
			while (!(*nowitem) && k<MAX_TRY_TIME);
			nowitem++;
		}
		nowitem = itembuffer;
		for (j=0; j<lppara->getnumber; j++)
		{
			k = 0;
			do 
			{
				ret = pthis->FreeOneList(*nowitem);
				k++;
			}
			while (!ret && k<MAX_TRY_TIME);
			nowitem++;
		}
	}

	free(itembuffer);
	free(lpParam);
	return 0;
}

void TestListDemo(void)
{
	LARGE_INTEGER tstart, tend, tfreq;
	double passSec, getuSec;

#define MY_THREAD	3
#define MY_NUMBER	3
#define MY_TIMES	111111111

	CMemoryListCriticalSection cList;

	cList.SetBuffer<CItemTest>(30, CACHE_SIZE);		//24 NOT - 26 OK

	QueryPerformanceFrequency(&tfreq);
	QueryPerformanceCounter(&tstart);

	//	cList.TestList(1, 3000, 20000);
	//	cList.TestList(2, 1000, 2000);
	//	cList.TestList(3, 500, 3000);
	//	cList.TestList(4, 100, 4000, 1);			// the last one should set 1

	//	cList.TestList(3, 3, 111111111, 1);			// 1B times
	cList.TestList(MY_THREAD, MY_NUMBER, MY_TIMES, 1);

	QueryPerformanceCounter(&tend);
	passSec = ((double)(tend.QuadPart-tstart.QuadPart) / (tfreq.QuadPart));
	getuSec = passSec / (MY_THREAD*MY_NUMBER*MY_TIMES) * 1000 * 1000;

	printf(_T("Total Sec:%10.2f, Per uSec:%10.6f\r\n"), passSec, getuSec);
	// in My Core2 Quad Q8400 2.6G, _DEBUG for Interlocked. All the test for 1B times get/free.
	// FastbutError,	thread, per uSec:	1,0.23u; 2,0.34u; 3,0.44u; 4,0.51u; 5,0.53u; 6,0.57u; 
	//										7,0.61u; 8,0.65u; 9,0.70u; 10,0.65u							// the real interlock
	// CriticalSection,	thread, per uSec:	1,0.13u; 2,0.44u; 3,1.52u; 4,2.45/							// same cri

	cList.CheckEmptyList();
	cList.ResetCount();

};
#endif _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	implement of CMemoryList																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CMemoryListCriticalSection::CMemoryListCriticalSection() : CMemoryList()															//
{																																	//
	InProcess = MARK_NOT_IN_PROCESS;																								//
	InGetProcess = InFreeProcess = &InProcess;																						//
}																																	//
																																	//
CListItem* CMemoryListCriticalSection::GetOneList( void )																			//
{																																	//
	volatile CListItem		*nlist;																									//
	MYINT	nowProcess;																												//
																																	//
	while ( nowProcess = InterCmpExg(InGetProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );						//
																																	//
#ifdef _DEBUG																														//
	GetCount++;																														//
#endif _DEBUG																														//
	nlist = FreeBufferStart;									//	Get the first free item											//
																//////////////////////////////////////////////////////////////////////
	if (FreeBufferStart->nextList > MARK_MAX)					//	Have free														//
	{																																//
		FreeBufferStart=FreeBufferStart->nextList;																					//
		nlist->nextList = MARK_USED;																								//
		FreeNumber--;																												//
																																	//
#ifdef _DEBUG																														//
		if (FreeNumber<MinFree) MinFree = FreeNumber;																				//
		GetSuccessCount++;																											//
		if (!(GetSuccessCount%1000000)) printf(_T("SuccessCount:%d\r\n"), (MYINT)(GetSuccessCount/1000000));						//
#endif _DEBUG																														//
	}																																//
	else																															//
	{																																//
		nlist = NULL;																												//
// 		__asm int 3	
// 		__asm int 3	
// 		__asm int 3	
	}																																//
																																	//
	*InGetProcess = MARK_NOT_IN_PROCESS;																							//
	return (CListItem*)nlist;																										//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MYINT CMemoryListCriticalSection::FreeOneList( CListItem* nlist )																	//
{																																	//
	MYINT	nowProcess, ret = 0;																									//
																																	//
	while ( nowProcess = InterCmpExg(InFreeProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );					//
																																	//
#ifdef _DEBUG																														//
	FreeCount++;																													//
#endif _DEBUG																														//
																																	//
	if (nlist > MARK_MAX && nlist->nextList == MARK_USED)		//	It is a used item												//
	{															//////////////////////////////////////////////////////////////////////
		nlist->nextList = MARK_FREE_END;																							//
		FreeBufferEnd->nextList = nlist;																							//
		FreeBufferEnd = nlist;																										//
																																	//
#ifdef _DEBUG																														//
		FreeSuccessCount++;																											//
#endif _DEBUG																														//
																																	//
		FreeNumber++;																												//
		ret = 1;																													//
	}																																//
	else																															//
	{																																//
		DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_ERROR_L3, "Error L3 for FreeList Twice, in MemoryList - FreeOneList !\r\n  ");			//
		nlist = NULL;																												//
	}																																//
																																	//
	*InFreeProcess = MARK_NOT_IN_PROCESS;																							//
	return (MYINT)ret;																												//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CListItem* CMemoryListCriticalSection::GetContext(void)																				//
{																																	//
	CListItem* mlist = GetOneList();																								//

	if (!DirectFree && mlist)									//	For Non Direct mode, link it to head of usedList				//
	{															//////////////////////////////////////////////////////////////////////
		while ( InterCmpExg(&InUsedListProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );						//
		mlist->usedList = UsedItem;																									//
		UsedItem = mlist;																											//
		InUsedListProcess = MARK_NOT_IN_PROCESS;																					//
		mlist->countDown = TimeoutInit;																								//
	}																																//
	return mlist;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CListItem* CMemoryListCriticalSection::FreeContext(CListItem* mlist)																//
{
// 	DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_STATUS, "(0x%x)", mlist);
																																	//
	if (!DirectFree)																												//
	{																																//
		mlist->countDown = TIMEOUT_QUIT;						//	For Non Direct mode, only mark it to free						//
	}															//////////////////////////////////////////////////////////////////////
	else																															//
	{																																//
		FreeOneList(mlist);																											//
	}																																//
	return mlist;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	implement of CResources																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	break	Dec 30 '12																								//
//	ret_err 0x30 :	break	Dec 30 '12																								//
//	ret_err 0x40 :	NONE																											//
//	ret_err 0x50 :	NONE																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CResources::InitProcess( void )																								//
{																																	//
// 	CIOCP*		classIOCP;																											//
// 	long ret_err = 0x01;																											//
// 																																	//
// 	while (TRUE)																													//
// 	{																																//
// 		ret_err = 0x10;																												//
// 		if (!AddResource<CSmallBuffer>("TEL_BUFFER", 20, PAGE_SIZE, TRUE, 0)) break;												//
// 																																	//
// 		ret_err = 0x20;																												//
// 		if (!AddResource<CTCPContext>("TEL_TCP", 10, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;										//
// 																																	//
// 		ret_err = 0x30;																												//
// 		if (!AddResource<CIOCP>("TEL_IOCP", 1, sizeof(INT), TRUE, 0)) break;														//
// 																																	//
// 		ret_err = 0x40;																												//
// 		classIOCP = (CIOCP*)GetResource("TEL_IOCP")->GetOnlyOneList();																//
// 																																	//
// 		ret_err = 0x50;																												//
// 		classIOCP->InitProcess(1);																									//
// 																																	//
// 		ret_err = 0;																												//
// 		break;																														//
// 	}																																//
// 	if (ret_err)																													//
// 	{																																//
// 		DEBUG_MESSAGE(MODULE_RESOURCE, MESSAGE_ERROR_L2, "Error in InitProcess", ret_err);											//
// 																																	//
// 		RET_BETWEEN(0x40, 0x50) RemoveLastResource("TEL_IOCP");																		//
// 		RET_BETWEEN(0x30, 0x50) RemoveLastResource("TEL_TCP");																		//
// 		RET_BETWEEN(0x20, 0x50) RemoveLastResource("TEL_BUFFER");																	//
// 		return 1;																													//
// 	}																																//
																																	//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	implement of CResources																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CResources::MainProcess( void )																								//
{																																	//
	int i;																															//
	for (i=0; i<ResourceNumber; i++) ResourceArray[i].TimeoutContext();																//
																																	//
// #ifdef _DEBUG
#define RESOURCE_LOOP 5
	static int loop = 1;
	loop --;
	if (!loop)
	{
		for (i=0; i<ResourceNumber; i++) 
			if (strncmp(ResourceArray[i].ListName, "TEL_", 4))
				DEBUG_MESSAGE_CON(MODULE_RESOURCE, MESSAGE_STATUS, "%s:%d  ", ResourceArray[i].ListName, ResourceArray[i].GetFreeNumber());
		DEBUG_MESSAGE_CON(MODULE_RESOURCE, MESSAGE_STATUS, "\r\n");
// 		DisplayContextList();
		loop = RESOURCE_LOOP;
	}
// #endif _DEBUG
																																	//
	 return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMemoryListUsed* CResources::GetResource( char* name )																				//
{																																	//
	int i;																															//
																																	//
	for (i=0; i<ResourceNumber; i++)																								//
		if (name && !strncmp(ResourceArray[i].ListName, name, NORMAL_CHAR)) return &ResourceArray[i];								//
	return NULL;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CResources::ExitProcess( void )																								//
{																																	//
	for (int i=IOCPNumber-1; i>=0; i--)																								//
	{																																//
		IOCPList[i]->ExitProcess();																									//
		IOCPList[i]=0;																												//
	}																																//
	IOCPNumber = 0;																													//
																																	//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    