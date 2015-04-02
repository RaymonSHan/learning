#pragma once

// #undef boolean													// for oratypes.h define boolean as int
// #include	<openssl/ssl.h>
// #include	<openssl/err.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	All the following segment, only for good looking, make the template Class looks like a normal C++ program						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define TemplateBase				template <typename _BASE>																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	These are four kind of MEMORYLIST class, every one can work, but this one is most debuged										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define	CMemoryListUsed				CMemoryListCriticalSection																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The usable size of MEMORYLIST is MAX-ITEM_SAFE_REMAIN																			//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define ITEM_SAFE_REMAIN			0																								//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The last free item in MEMORYLIST is pointed by FreeBufferEnd, and FreeBufferEnd -> nextList = FREE_END_MARK						//
//	If the item is used, the ->nextList = MARK_USED																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MARK_FREE_END				(CListItem*)(VOID*)0x20																			//
#define MARK_USED					(CListItem*)(VOID*)0x10																			//
#define MARK_CLIBUFFER_END			(CListItem*)(VOID*)0x30																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The two is for synchronous used, in InterCmpExg																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MARK_NOT_IN_PROCESS			(MYINT)0xa0																						//
#define MARK_IN_PROCESS				(MYINT)0xa1																						//
#define MARK_FILE_IN_USE			((__int64)-1)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The item -> nextList is pointed to next item, but if the value less than MARK_MAX, it means some other							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MARK_MAX_INT				0x100																							//
#define MARK_MAX					(CListItem*)(VOID*)MARK_MAX_INT																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	In ReadFileScatter and WriteFileGather which use FILE_SEGMENT_ELEMEN struct, this is the MAX number of useful PAGE				// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define FILE_SEGMENT_NUMBER			256																								//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Every I/O use buffer to storage data. The following is the default buffer size.													//
//	SMALL_BUFFER_SIZE should large than MTU, which is 1536, Now 10 times large														//
//	NORMAL_BUFFER_SIZE should large then slide windows of Windows, now double it, just FILE_SEGMENT_NUMBER times of PAGE			//
//	LARGE_BUFFER is not sure how large																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define SMALL_BUFFER_SIZE			15360																							//
#define NORMAL_BUFFER_SIZE			(FILE_SEGMENT_NUMBER+1)*PAGE_SIZE																//
#define LARGE_BUFFER_SIZE			4*1024*1024																						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define	CONCURRENCY_OK				1							//	used for pad->inConcurency	// Apr. 26 '14
#define CONCURRENCY_GOON			0
#define CONCURRENCY_PAUSE			1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CMemoryBlock is a block of memory, which can contain NUMBER items, each is SIZE large, and BORDER size byte border				//
//	LIFECYCLE: new -> SetMemoryBlock -> free,																						//
//	Create by CMemoryList::SetBuffer();																								//
//	Destroy by ~CMemoryList();																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CMemoryBlock																													//
{																																	//
private:																															//
	void				*StartBlock;							//	The Memory start, used for free									//
	void				*TotalBlock;							//	The valid data start											//
public:															//////////////////////////////////////////////////////////////////////
	long				BorderSize;								//	byte border														//
	size_t				TotalSize;								//	Total memory size in byte, size_t for x64						//
public:																																//
	CMemoryBlock();																													//
	~CMemoryBlock();																												//
	void* SetMemoryBlock(long size, long number, long border);	//	malloc the memory												//
																//////////////////////////////////////////////////////////////////////
	inline void* GetMemory(void) 								//	Return the valid data start										//
	{																																//
		return TotalBlock; 																											//
	};																																//
																//////////////////////////////////////////////////////////////////////
	inline long GetBorderSize(void)								//	Return the border size ( NOT used )								//
	{																																//
		return BorderSize;																											//
	};																																//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The following struct is IOCP perSession data																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
struct ContextStruct																												//
{																																	//
	CProtocol*			pProtocol;								//	the PROTOCOL it used											//
	CApplication*		pApplication;							//	the APPLICATION it used											//
	CContextItem*		pPeer;									//	peer CONTEXT													//
	CMemoryListUsed*	contextType;							//	the MEMORYLIST it used											//
	HANDLE				bHandle;								//	the HANDLE/SOCKET it used										//
	__int64				overlapOffset;							//	the PACKET OFFSET of the SESSION								//
	__int64				bodyRemain;								//	for HTTP style, just for the Content-Length
	CListItem*			moreBuffer;								//	normal DATA in perI/O struct, but more DATA here				//
	CListItem*			cliBuffer;
	CListItem*			firstContent;
// 	long				bufferOffset;							//	moreBuffer start from bufferOffset								//
	CContextItem*		encapContext;
	CContextItem*		decapContext;
	CContextItem*		logFileContext;
	long				contentMode;							//	for CONTEXT_LENGTH CONTEXT_HEAD
// 	CListItem*			headerBuffer;							//	NOT used yet
	long				transferEncoding;						//	for ENCODING_LENGTH, ENCODING_CHUNKED
// 	long				tagID;									//	now for tunnel tag id 

	char				serverName[SMALL_CHAR];					//	lazy way for save server side pad name,		// Apr. 26 '14
	ProtocolId*			peerProtocol;
	CContextItem*		oldPeer;								//	for keep-alive
	long				opSideControl;							//	now, for WAIT only

	long				dyControl;								//	for SETCONTEXT command
	COleStreamFile*		dyMemoryFile;

};																//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For save some space in IOCP perI/O data																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
struct TCPClientBufferList
{
	CListItem*			cliListNext;
//	CListItem*			cliListPrev;
	MYINT				cliWaitData;
};

// struct DeriveUnion

struct UDPInfos
{
	sockaddr_in			fromAddr;								//	for UDP only, to record the SOURCE UDP, (NOT SURE YET)			//
	CContextItem*		usedContext;
};

struct DeriveUnion																													//
{																																	//
	UDPInfos			UL;
	TCPClientBufferList	BL;										//	for TCP only, for close ACCEPT without input					//
};																//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The following struct is IOCP perI/O data																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
struct BufferStruct																													//
{																																	//
	WSAOVERLAPPED		oLapped;								//	perI/O must start with OVERLAPPED								//
	DWORD				nOperation;								//	perI/O OPERATION												//
	DWORD				nProcessSize;							//	DATA size of this I/O											//
	CMemoryListUsed*	bufferType;								//	the MEMORYLIST it used											//
	HANDLE				cliHandle;								//	for OnAccept, get the CONNECTED SOCKET							//
	ContentPad*			headInfo;
	long				operationSide;
	DeriveUnion			DU;										//	Other data for special PROTOCOL									//
	long				forTunnelEncap;
	long				forTunnelDataSize;						//	MUST save 8 byte for tunnel ahead
};																//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	This UNION is for save code only, this is the basic struct to record IOCP info													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
union ContextUnion																													//
{																																	//
	ContextStruct		CS;																											//
	BufferStruct		BS;																											//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	This is the basic struct to link all item to a list																				//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CListItem 																													//
{																																	//
public:																																//
	ContextUnion		GU;										//	IOCP struct														//
																////////////////////////////////////////////////////////////		//
	CListItem*			nextList;								//	LIST for free item, start with CMemoryList::FreeBufferStart;	//
																//	or be MASK_USED, or MASK_FREE_END								//
	CListItem*			usedList;								//	LIST for used item, start with CMemoryList::UsedItem			//
																//	the most use is NonDirectly free								//
	MYINT				countDown;								//	In NonDirectly free, free the item when equal TIMEOUT_QUIT		//
	MYINT				listFlag;								//	the behavior the item, for these const start with FLAG_			//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ContentPad
{
	CListItem*	nextContent;
	// 	ContentPad* prevContent;
	char		nameFor[SMALL_CHAR];
	char*		checkStart;
	char*		checkEnd;
// 	long		keyNumber;
	char		serverAction[MAX_URL];							//	for define
	char		serverMethod[SMALL_CHAR];						//	for define & refer
	char		serverHost[SMALL_CHAR];							//	for define
	char		serverName[SMALL_CHAR];							//	for define
	char		serverKey[8192];								//	for define & refer		// have to so large for POST Key
	char		serverReferer[MAX_URL*4];						//	for define
	char		serverCookie[MAX_URL];						//	for define
	char		serverReferName[SMALL_CHAR];					//	for define & refer

	char		serverStartName[SMALL_CHAR];					//	for refer
	char		serverEndName[SMALL_CHAR];						//	for refer
	char		serverLoop[SMALL_CHAR];							//	for refer
	long		serverStartPlace;								//	for refer
	long		serverEndPlace;									//	for refer
	long		serverKeyFrom;									//	for refer
	char		serverID[SMALL_CHAR];							//	for define & refer
	char		serverError[SMALL_CHAR];						//	for define & refer
	char		serverDefault[MAX_URL];							//	for refer, now only used in Scanf for one key, when not found.
	char		serverIf[MAX_URL];								//	for refer, now only used in Scanf for whether add or not
	char*		serverNowStart;									//	for replace use only now.
	char*		serverNowEnd;									//	for Loop use in Scanf, this means the end of this Scanf, next Scanf begin start with here, LoopEnd clear it.

//	The following is for concurrency use /////////////////////////	for FILEpad, normal is 0, after send all concurrency request is 1, when all concurrency finish and begin next define is 0
	MYINT		inConcurrency;									//	for CLIENTpad, record the number of unfinished request. while FILEpad is 1, and CLIENTpad is 0, means all request finished
//////////////////////////////////////////////////////////////////	for otherPad, 0 for not finish, 1 for finished


#ifdef CONFIRMDNS_FUNCTION
	char		clientIP[16];
	char		clientMac[16];
#endif CONFIRMDNS_FUNCTION

	char*		resultKey[MAX_CONTENT_KEY+SYSTEM_CONTENT_KEY];
	long		getLength[MAX_CONTENT_KEY+SYSTEM_CONTENT_KEY];
	ValParaTran			valPlace[MAX_PARAMETER+SYSTEM_CONTENT_KEY];				//	for searching use only

// 	char		userCookie[MAX_COOKIE_SIZE];
}ContntPad;

typedef struct VarPad											//	For startup para get, the following is the only needed in ContentPad
{
	char*		resultKey[MAX_CONTENT_KEY+SYSTEM_CONTENT_KEY];
	long		getLength[MAX_CONTENT_KEY+SYSTEM_CONTENT_KEY];
}VarPad;

#define HEADINFO_BOUNDARY			MAX_CONTENT_KEY + 1			//	is 97
// #define WAFA_ONLY_USE					MAX_CONTENT_KEY+2			//	is 98
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The following is for save code only																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define CONTENT_MODE_HEAD			0
#define CONTENT_MODE_AFTERHEAD		1
#define CONTENT_MODE_LENGTH			2

#define CONTENT_MODE_NOTCONTROL		0xff


#define PProtocol					GU.CS.pProtocol																					//
#define PApplication				GU.CS.pApplication																				//
#define PPeer						GU.CS.pPeer																						//
#define ContextType					GU.CS.contextType																				//
#define	BHandle						GU.CS.bHandle																					//
#define OverlapOffset				GU.CS.overlapOffset																				//
#define BodyRemain					GU.CS.bodyRemain
#define MoreBuffer					GU.CS.moreBuffer																				//
#define CliBuffer					GU.CS.cliBuffer				//	only for TCP ACCEPT, pointer to BUFFER with CliHandle			//
#define FirstContent				GU.CS.firstContent			//	only for ContentPad, to store remote server content
// #define BufferOffset				GU.CS.bufferOffset			//////////////////////////////////////////////////////////////////////
#define EncapContext				GU.CS.encapContext
#define DecapContext				GU.CS.decapContext
#define LogFileContext				GU.CS.logFileContext
#define MessageContext				GU.CS.logFileContext		//	the two use the same space
#define ContentMode					GU.CS.contentMode
#define TransferEncoding			GU.CS.transferEncoding
// #define TagID						GU.CS.tagID
#define ServerName					GU.CS.serverName
#define PeerProtocol				GU.CS.peerProtocol
#define OldPeer						GU.CS.oldPeer
#define OpSideControl				GU.CS.opSideControl

#define DyControl					GU.CS.dyControl
#define DyMemoryFile				GU.CS.dyMemoryFile

																																	//
#define	OLapped						GU.BS.oLapped																					//
#define	NOperation					GU.BS.nOperation																				//
#define	NProcessSize				GU.BS.nProcessSize																				//
#define	BufferType					GU.BS.bufferType																				//
#define	CliHandle					GU.BS.cliHandle																					//
#define HeadInfo					GU.BS.headInfo
#define OperationSide				GU.BS.operationSide			//	for InterApplication use
#define AdditionInfo				GU.BS.DU																						//
#define FromAddr					GU.BS.DU.UL.fromAddr																				//
#define UsedContext					GU.BS.DU.UL.usedContext
#define CliListNext					GU.BS.DU.BL.cliListNext																			//
#define CliWaitData					GU.BS.DU.BL.cliWaitData
																																	//
#define ListFlag					listFlag																						//

#define	CONTEXT_MEMORY_FILE			"MemoryFile"

#define	FLAG_MEMORY_FILE			0x1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct URLInfo
{
	char				getURL[MAX_URL+8];
	char				getSite[SMALL_CHAR+8];
	char				getKeepalive[SMALL_CHAR+8];
	char				getCookie[MAX_COOKIE+8];
	char				getCharset[SMALL_CHAR+8];
	char				getBoundary[SMALL_CHAR+8];
	long				getBodyRemain;
	long				int64getBodyRemain;					//	the two line should together

	long				httpResult;
	long				httpRedirect;
	long				sendTogether;
	long				getLength[MAX_PARAMETER+1];
	ValParaTran			valPlace[MAX_PARAMETER+1];
};

#define	ENCODING_LENGTH				0x01
#define ENCODING_CHUNKED			0x02

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	These are inherit class of CListItem, which should alloc frequently, each PROTOCOL and BUFFER for one class						//
//	For Application use different Protocol, and translate info between it. So all Context MUST the same	// Oct. 22 '13
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CContextItem : public CListItem																								//
{																																	//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTCPContext : public CContextItem																								//
{																																	//
public:																																//
	sockaddr_in			addrServer;																									//
	sockaddr_in			clientAddr;								//	record client IP by GetAcceptExSockaddrs 
	char				fileName[MAX_PATH+4];																						//
	long				fileNameLength;
	__int64				fileLength;

	URLInfo				urlInfo;
// 	char				userName[NORMAL_CHAR+8];
	char				userName[8];
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHTTPContext	CTCPContext
#define CUDPContext		CTCPContext
#define CFileContext	CTCPContext
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSmallBuffer : public CListItem																								//
{																																	//
#ifdef	_DEBUG																														//
	char				SmallBuffer[100];																							//
#else	_DEBUG																														//
	char				SmallBuffer[SMALL_BUFFER_SIZE];																				//
#endif	_DEBUG																														//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CNormalBuffer : public CListItem																								//
{																																	//
	char				NormalBuffer[NORMAL_BUFFER_SIZE];																			//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLaregBuffer : public CListItem																								//
{																																	//
	char				LargeBuffer[LARGE_BUFFER_SIZE];																				//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CItemTest : public CListItem																									//
{																																	//
public:																																//
	char				test[8];																									//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	To trace to malloc times, The only malloc in release version is in CMemoryList::SetBuffer										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#ifdef _DEBUG																														//
	#define	MYMALLOC					mymalloc																					//
	#define	MYFREE						myfree																						//
	void* mymalloc( size_t size );																									//
	void myfree( void *memblock );																									//
#else _DEBUG																														//
	#define	MYMALLOC					malloc																						//
	#define	MYFREE						free																						//
#endif _DEBUG																														//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CMemoryList is a user-define HEAP manager, I implement four kind of manager, and this is the base class for all					//
//	this is for FIXED-LENGTH memory alloc only.	The item should base on CListItem													//
//	LIFECYCLE: it is a virtual class, declare in inherit class																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CMemoryList																													//
{																																	//
protected:																															//
	MYINT				TotalNumber;																								//
	volatile MYINT		FreeNumber;																									//
	MYINT				m_ItemSaveRemain;						//	When start, FreeNumber = TotalNumber - m_ItemSaveRemain			//
																//////////////////////////////////////////////////////////////////////
	CListItem*			FreeBufferStart;																							//
	CListItem*			FreeBufferEnd;																								//
	CMemoryBlock*		MemoryBuffer;							//	The MEMORY block												//
	CListItem*			TotalBuffer;							//	When start, TotalBuffer = MemoryBuffer -> GetMemory()			//
																//////////////////////////////////////////////////////////////////////
	CListItem			*UsedItem;								//																	//
	volatile MYINT		InUsedListProcess;						//	EASY, for synchronous used										//
																																	//
public:																																//
	long				DirectFree;								//	weather free is in Timeout Process								//
	long				TimeoutInit;							//	The init value of timeout										//
	long				BufferSize;								//	the size store DATA												//
	char				ListName[NORMAL_CHAR];					//	The ID of the MEMORYLIST										//
																//////////////////////////////////////////////////////////////////////
public:																																//
#ifdef _DEBUG																														//
	MYINT				GetCount;																									//
	MYINT				GetSuccessCount;																							//
	MYINT				FreeCount;																									//
	MYINT				FreeSuccessCount;																							//
	MYINT				MinFree;								//	record the MIN number of free item								//
	HANDLE				hStart;									//	in DEBUG several thread should start at same time				//
#endif _DEBUG													//////////////////////////////////////////////////////////////////////
																																	//
public:																																//
	CMemoryList();																													//
	~CMemoryList();																													//
	void SetListDetail(char* listname, long directFree, long timeout);																//
	void FreeMemoryBuffer(void);																									//
																																	//
	virtual CListItem* GetOneList(void) = 0;																						//
	virtual MYINT FreeOneList(CListItem* nlist) = 0;																				//
	CListItem* GetOnlyOneList();																									//
																																	//
	void TimeoutContext(void);																										//
	CListItem* Comparekeyword(int start, int length, char* source);																//
																																	//
	void DisplayContext();																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Alloc the memory and init the nextList list																						//
//	return 0 for success																											//
//	template _BASE determine the size, the 'number' and 'bordersize' is same as CMemeoryBlock::SetMemoryBlock()						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	NONE																											//
//	ret_err 0x30 :	NONE																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	TemplateBase MYINT SetBuffer(long number, long bordersize)																		//
	{																																//
		MYINT			ret_err;																									//
		CListItem*		golist;																										//
		char*			nextlist;																									//
																																	//
		ret_err = 0x01;																												//
		while (TRUE)																												//
		{																															//
			ret_err = 0x10;																											//
			if (bordersize & (bordersize-1)) break;				// the border is not 2^N											//
																//////////////////////////////////////////////////////////////////////
			ret_err = 0x20;																											//
			MemoryBuffer = new CMemoryBlock;																						//
			if (!MemoryBuffer) break;																								//
																																	//
			ret_err = 0x30;																											//
			if (!MemoryBuffer->SetMemoryBlock(sizeof(_BASE), number, bordersize)) break;											//
			BufferSize = sizeof(_BASE)-sizeof(CListItem);		//	the space following CListItem is BufferSize						//

// #ifdef CONTENTPAD_APPLICATION
			if (sizeof(ContentPad) > 4*PAGE_SIZE) break;
			if (BufferSize > 10*PAGE_SIZE) BufferSize-=(5*PAGE_SIZE);	//	for FILE_SEGMENT_ELEMEN use one page more				//
			//	NOT change above, for setting of BUFFER2HEADINFO	// Nov 19 '14
// #else CONTENTPAD_APPLICATION
// 			if (BufferSize > 10*PAGE_SIZE) BufferSize-=(2*PAGE_SIZE);	//	for FILE_SEGMENT_ELEMEN use one page more					//
// #endif CONTENTPAD_APPLICATION

// 			if (BufferSize > SIZEOFCONTENT) BufferSize-=SIZEOFCONTENT;	
// 			//	for ContentPad use			// for HeadInfo use, 
																//////////////////////////////////////////////////////////////////////
			TotalBuffer = (CListItem*)(MemoryBuffer->GetMemory());																	//
			FreeBufferStart = TotalBuffer;						//	Get the real data start											//																	//
																//////////////////////////////////////////////////////////////////////
			golist = TotalBuffer;								//	Set the nextList, end with MARK_FREE_END						//
			for (MYINT i=0; i<number-1; i++)																						//
			{																														//
				nextlist = (char*)golist;																							//
				nextlist += MemoryBuffer->BorderSize;																				//
				golist->nextList = (CListItem*)nextlist;																			//
				golist = (CListItem*)nextlist;																						//
			}																														//
			FreeBufferEnd = golist;																									//
			golist->nextList = MARK_FREE_END;																						//
																																	//
			TotalNumber = number;								//////////////////////////////////////////////////////////////////////
			FreeNumber = number-m_ItemSaveRemain;				//	Set the number													//
#ifdef _DEBUG																														//
			MinFree = TotalNumber;																									//
#endif _DEBUG																														//
																																	//
			ret_err = 0;																											//
			break;																													//
		}																															//
		if (ret_err)																												//
		{																															//
			DEBUG_MESSAGE(MODULE_MEMORY, MESSAGE_ERROR_L2, "Error in SetBuffer, 0x%x\r\n", ret_err);								//
			RET_BETWEEN(0x30, 0x30) delete MemoryBuffer;																			//
			return 1;																												//
		}																															//
		return 0;																													//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline MYINT GetNumber()																										//
	{																																//
		return TotalNumber;																											//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline MYINT GetFreeNumber()																									//
	{																																//
		return FreeNumber;																											//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG		// for test function, such a multithread!

	typedef struct threadPara
	{
		CMemoryList*	pMemoryList;
		MYINT			threadID;
		MYINT			getnumber;
		MYINT			fullnumber;
	}threadPara, *pthreadPara;

	void ResetCount();
	virtual void CheckEmptyList(void);
	static UINT WINAPI ThreadItem(LPVOID lpParam);																					//
	void TestList(MYINT threadNumber, MYINT getNumber, MYINT fullTimes, BOOL wait=0);		// every thread get fullNumber times, then free all. Do this fullTimes.
#endif _DEBUG		// for test function, such a multithread!
};

#ifdef _DEBUG
	void TestListDemo(void);
#endif _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	There four type of CMemoryList, see the previous project AAADHCP, the CMemoryListCriticalSection class is slow, but safe one.	//
//	And the most tested one. It is CriticalSection, although it use Interlocked function only.										//
//	LIFECYCLE: declare -> SetBuffer() -> SetListDetail() -> exit																	//
//	Create by CResources::ResourceArray[MAX_RESOURCE] declare																		//
//	Destroy by CResources exit																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CMemoryListCriticalSection : public CMemoryList																				//
{																																	//
private:																															//
	volatile MYINT		InProcess;								//	the synchronous value											//
	volatile MYINT		*InGetProcess;							//	Get and Free may use one or two value, so use pointer here		//
	volatile MYINT		*InFreeProcess;							//////////////////////////////////////////////////////////////////////
																//																	//
	CListItem* GetOneList(void);								//	Really Get one item												//
	MYINT FreeOneList(CListItem* nlist);						//	Really Free one item											//
public:																																//
	CMemoryListCriticalSection();																									//
																																	//
	CListItem* GetContext(void);								//	For DirectFree mode, same as GetOneList							//
	virtual CListItem* FreeContext(CListItem* mlist);			//	For NON-Direct mode, only make a mark, and free it in Timeout	//
};																//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	I forget what it is ?
//	#define MAX_PROCESS					10							// for addition process, such as accept check, in CRecosure


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The CResources contain all memory used by this program, there are only one instance.											//
//	In CProtocol and CApplication, there are pointer point to it.																	//
//	You should create Context list, Buffer list, IOCP list at first,																//
//	Any PROTOCOL must use CONTEXT and IOCP list; Any APPLICATION must use PROTOCOL, BUFFER list and IOCP list.						//
//	LIFECYCLE: declare -> InitProcess() -> MainProcess() (loop) -> ExitProcess() -> exit											//
//	Create by CService::ServiceResources declare																					//
//	Destroy by CService exit																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CResources																													//
{																																	//
public:																																//
	long				ResourceNumber;							//	Number of ResourceArray											//
	CMemoryListUsed		ResourceArray[MAX_RESOURCE];			//	Every list in here, include IOCP list							//
																//////////////////////////////////////////////////////////////////////
	long				IOCPNumber;								//	Number of IOCP													//
	CIOCP*				IOCPList[MAX_IOCP];						//	Every IOCP record again											//
	long				AdditionNumber;																								//
	long				IsFreeError;							//	After Free error, do NOT SetBuffer agagin						//
//	MYINT				AdditionSign[MAX_PROCESS];																					//
//	CListItem*			AdditionList[MAX_PROCESS];																					//
//	ProtocolHandle		AdditionProcess[MAX_PROCESS];																				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:																																//
	CResources()																													//
	{																																//
		int i;																														//
		ResourceNumber = 0;																											//
		AdditionNumber = 0;																											//
		IOCPNumber = 0;																												//
		IsFreeError= 0;																												//
//		for (i=0; i<MAX_PROCESS; i++) AdditionSign[i]=MARK_NOT_IN_PROCESS;															//
		for (i=0; i<MAX_IOCP; i++) IOCPList[i]=NULL;																				//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	NONE																											//
//	ret_err 0x30 :	NONE																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	TemplateBase CMemoryListUsed* AddResource( char* name, long number, long bordersize, long directfree, long timeout )			//
	{																																//
		MYINT			ret_err;																									//
		MYINT			nowNumber;																									//
																																	//
		ret_err = 0x01;																												//
		while (TRUE)																												//
		{																															//
			ret_err = 0x10;																											//
			if (IsFreeError) break;																									//
																																	//
			ret_err = 0x20;																											//
			nowNumber = InterInc(&ResourceNumber)-1;																				//
			if ( nowNumber >= MAX_RESOURCE ) break;																				//
			if ( ResourceArray[nowNumber].SetBuffer<_BASE>(number, bordersize) ) break;												//
																																	//
			ret_err = 0x30;																											//
			ResourceArray[nowNumber].SetListDetail(name, directfree, timeout);														//
																																	//
			ret_err = 0;																											//
			break;																													//
		}																															//
		if (ret_err)																												//
		{																															//
			DEBUG_MESSAGE(MODULE_RESOURCE, MESSAGE_ERROR_L2, "Error in AddResource \"%s\", 0x%x\r\n", name, ret_err);				//
			InterDec(&ResourceNumber);																								//
			return NULL;																											//
		}																															//
		return &ResourceArray[nowNumber];																							//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ret_err 0x10 :	NONE																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	BOOL RemoveLastResource(char* name)																								//
	{																																//
		MYINT	ret_err;																											//
		MYINT	nowNumber;																											//
		CMemoryListUsed* list;																										//
																																	//
		ret_err = 0x01;																												//
		while (TRUE)																												//
		{																															//
			ret_err = 0x10;																											//
			list = GetResource(name);																								//
			nowNumber = InterDec(&ResourceNumber);																					//
																																	//
			if ( &(ResourceArray[nowNumber]) == list ) ResourceArray[nowNumber].FreeMemoryBuffer();								//
			else break;																												//
																																	//
			ret_err = 0;																											//
			break;																													//
		}																															//
		if (ret_err)																												//
		{																															//
			DEBUG_MESSAGE(MODULE_RESOURCE, MESSAGE_ERROR_L3, "Error in RemoveResource \"%s\"\r\n", name);							//
			IsFreeError = 1;																										//
			InterInc(&ResourceNumber);																								//
			return 1;																												//
		}																															//
		return 0;																													//
	}																																//

	BOOL RemoveAllResource()			// have not tested // testuse
	{
		MYINT	ret_err = 0x01;
		while (TRUE)
		{
			for (int i=ResourceNumber-1; i>=0; i--) ResourceArray[i].FreeMemoryBuffer();

			ResourceNumber = 0;
			ret_err = 0;																											//
			break;																													//
		}																															//
		if (ret_err)																												//
		{																															//
			DEBUG_MESSAGE(MODULE_RESOURCE, MESSAGE_ERROR_L3, "Error in RemoveAllResource \r\n");									//
			return 1;																												//
		}																															//
		return 0;																													//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CMemoryListUsed* GetResource(char* name);																						//
																																	//
	BOOL InitProcess (void);									// Called in CService::InitProcess()								//
	BOOL MainProcess (void);									// Called in CService::MainProcess()								//
	BOOL ExitProcess (void);									// Called in CService::ExitProcess()								//
};																//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           