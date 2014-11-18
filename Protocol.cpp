
#include	"stdafx.h"
#include	<pcap.h>			//	Jul. 7 '14 for WinpCap add

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CService theService;

#ifdef	_DEBUG
long	Started = 0;
#endif	_DEBUG
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	These are static class number variable, declared here																			//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
LPFN_ACCEPTEX			CIOCP::m_lpfnAcceptEx;																						//
LPFN_CONNECTEX			CIOCP::m_lpfnConnectEx;																						//
LPFN_DISCONNECTEX		CIOCP::m_lpfnDisconnectEx;																					//
LPFN_GETACCEPTEXSOCKADDRS	CIOCP::m_lpfnGetAcceptExSockaddrs;																		//
sockaddr_in				CIOCP::tempaddr;																							//
																																	//
CResources*				CProtocol::Resoucce;																						//
CResources*				CApplication::Resoucce;																						//
																																	//
ProtocolHandles			CIOCP::fProtocolFunction[MAX_PROTOCOL];																		//
ApplicationHandles		CIOCP::fApplicationFunction[MAX_APPLICATION];																//
																																	//
CNoneProtocol*			CService::pNoneProtocol;																					//
CNoneApplication*		CService::pNoneApplication;																					//

CListItem*				isNULL = NULL;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	implement of CIOCP, init the _Ex pointer, start thread and wait for end															//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ret_err 0x10 :	SHOULD DO
//	ret_err 0x20 :	SHOULD DO
//	ret_err 0x30 :	SHOULD DO
//	ret_err 0x40 :	SHOULD DO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CIOCP::InitProcess(long threadNumber)																							//
{																																	//
	MYINT ret_err = 1;																												//
	long i;																															//
	mWorkThreadNumber = threadNumber;							//	The number of thread IOCP used									//
																//////////////////////////////////////////////////////////////////////
	tempaddr.sin_family = AF_INET;								//	tempaddr is used for bind SOCKET								//
	tempaddr.sin_port = htons(0);																									//
	tempaddr.sin_addr.s_addr = htonl(ADDR_ANY);																						//
	DWORD dwBytes;																													//
	SOCKET	tempsocket;																												//
																																	//
	WSADATA wsd;																													//
	GUID GuidAcceptEx = WSAID_ACCEPTEX;							//	These for get _Ex function pointer								//
	GUID GuidConnectEx = WSAID_CONNECTEX;						//////////////////////////////////////////////////////////////////////
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;																						//
	GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;																		//
																																	//
	unsigned controlThreadAddr;									//	For create thread												//
	MYINT	nowIOCPNumber;										//	Register IOCP, For release resource later						//
																//////////////////////////////////////////////////////////////////////
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)  break;																			//
																																	//
		ret_err = 0x20;																												//
		mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);														//
		if (!mIOCP)																													//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);															//
																																	//
		tempsocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);									//
		if (tempsocket == INVALID_SOCKET)																							//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);															//
																																	//
		::WSAIoctl(tempsocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx),								//
			&m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), &dwBytes, NULL, NULL);															//
		::WSAIoctl(tempsocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx),							//
			&m_lpfnConnectEx, sizeof(m_lpfnConnectEx), &dwBytes, NULL, NULL);														//
		::WSAIoctl(tempsocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx),						//
			&m_lpfnDisconnectEx, sizeof(m_lpfnDisconnectEx), &dwBytes, NULL, NULL);													//
		::WSAIoctl(tempsocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidGetAcceptExSockaddrs, sizeof(GuidGetAcceptExSockaddrs),		//
			&m_lpfnGetAcceptExSockaddrs, sizeof(m_lpfnGetAcceptExSockaddrs), &dwBytes, NULL, NULL);									//
																																	//
		ret_err = 0x30;											//////////////////////////////////////////////////////////////////////
		for (i=0; i<mWorkThreadNumber; i++)						//	Create several thread to handle IOCP							//
		{																															//
			mWorkThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkThread, (void*)this, 0, &controlThreadAddr );						//
			if (!mWorkThread[i] ) break;																							//
		}																															//
																																	//
		ret_err = 0x40;																												//
		nowIOCPNumber = InterInc(&(theService.ServiceResources.IOCPNumber)) - 1;													//
		theService.ServiceResources.IOCPList[nowIOCPNumber] = this;																	//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_IOCP, MESSAGE_ERROR_L3, "Error in IOCP CreateIOCP, 0x%x\r\n", ret_err);								//
		return TRUE;											//	If error here, EXIT the program									//
	}															//////////////////////////////////////////////////////////////////////
	return ERROR_SUCCESS;																											//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Post exit to all IOCP thread, the main thread wait all sub thread then exit.													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CIOCP::ExitProcess( void )																										//
{																																	//
	int i;																															//
	for (i=0; i<mWorkThreadNumber; i++) ::PostQueuedCompletionStatus(mIOCP, -1, 0, NULL);											//
																																	//
	WaitForMultipleObjects(mWorkThreadNumber, mWorkThread, TRUE, INFINITE);															//
																																	//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char*	OPSTR[] = {"ACCEPT", "CONNECT", "CLIENT_READ", "CLIENT_WRITE", "SERVER_READ", "SERVER_WRITE", "CLOSE", "PASSBY"};
int		OPNumber[] = {0,0,0,0,0,0,0};

// CNormalBuffer	testBuffer;																										//

unsigned int CIOCP::WorkThread(void)																								//
{																																	//
	DWORD size;																														//
	CContextItem* mContext = 0;																										//
	CListItem* mBuffer = 0;																											//
	BOOL ret;																														//
	long nOper;
	int lasterror = 0;

	while(TRUE)																														//
	{																																//
		ret = ::GetQueuedCompletionStatus(mIOCP, &size, (PULONG_PTR)&mContext, (LPOVERLAPPED*)&mBuffer, WSA_INFINITE);				//
		if (size == -1) break;									//	exit the thread													//

		nOper = mBuffer->NOperation;
//  		if (nOper == OP_ACCEPT || nOper == OP_CONNECT || size==0)
			printf ("OP:%s, PRO:%d, APP:%d, SIZE:%d, Context:%x, Peer:%x\r\n", \
				OPSTR[nOper-OP_BASE], mContext->PProtocol->ProtocolNumber, mContext->PApplication->ApplicationNumber, size, mContext, mContext->PPeer);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The easy way to do the same thing is by switch/case which I do NOT like															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
		if (size) NoneAppFunc(CService::pNoneApplication, nOper-OP_BASE)(mContext, mBuffer, size, 0);				//
		else 
		{
			lasterror = WSAGetLastError();
//	By debug UDP, for it return WSAError 995, UDP Accept should NOT close for ever. which is cause by UDP 10054, have change behavior
			if ((mContext->PProtocol->ProtocolNumber==PROTOCOL_SINGLEUDP)||(mContext->PProtocol->ProtocolNumber==PROTOCOL_UDP))
			{
				if (nOper==OP_ACCEPT) NoneAppFunc(CService::pNoneApplication, nOper-OP_BASE)(mContext, mBuffer, size, 0);	
			}
			else
// 			if (mContext->PProtocol->ProtocolNumber == PROTOCOL_TCP ) lasterror = WSAGetLastError();
// 			else lasterror = ERROR_SUCCESS+1;			//	MUST close
// 			if (ret==ERROR_SUCCESS || lasterror!=ERROR_SUCCESS || mContext->PPeer==mContext )	//	add in May 08 '13, thanks for GOOGLE, search lpNumberOfByte=0	
//
// 			if (nOper == OP_SERVER_READ && mContext->ContentMode == CONTENT_MODE_LENGTH && mContext->PPeer && mContext->PPeer->ContentMode == CONTENT_MODE_LENGTH)
// //	It seemed, there are some mistaken in IE6 when without CONTENT-LENGTH in HTTP respond, while IE setting HTTP1.0 for Proxy.
// //	Of course, the web server use Connection:Close, but when I close the Browser side, IE reload the page again, until it error
// 				NoneAppFunc(CService::pNoneApplication, fOnClose)(mContext, mBuffer, FLAG_GRACE_CLOSE, 0);	
// //	from Jul 01 to 02, about 10 hours debug to find, after GRACE_CLOSE, the client send again, then error happen.
// 			else
// 				NoneAppFunc(CService::pNoneApplication, fOnClose)(mContext, mBuffer, 0, 0);											//

				NoneAppFunc(CService::pNoneApplication, fOnClose)(mContext, mBuffer, 0, 0);											//	close all
		}
	}																																//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	In normal, one PACKET contain one COMMAND, one COMMAND in one PACKET.															//
//	But one packet can contain several COMMAND, and COMMAND	may be separate into two PACKET.										//
//	The CNoneProtcol and CNoneApplication is the solution for this.																	//
//	The CNoneProtcol and CNoneApplicaiton receive PACKET, while real PROCOTOL and APPLICATION receive COMMAND.						//
//	I used try this by set FLAG_BUFFER_WRITE, but now every COMMAND end with a CommandLineEnd, If without means no BUFFER.			//
//	COMMAND may send by CLIENT or SERVER, so there are two array to store CommandLineEnd in CApplication.							//
//	The MoreBuffer start at bufferOffset in ContextStruct store more DATA other than one COMMAND.									//
//	ATTENTION, the COMMAND size could NOT large than PACKET size, for the COMMAND send to real APPLICATION only in one PACKET.		//
//	For COMMAND size large than BUFFER, it will return unknown result, until next valid COMMAND.									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	If one COMMAND in two PACKETs, the old part is mContext->MoreBuffer with 'BufferOffset'; new part is mBuffer with 'size',		//
//	Return the COMMAND struct pointer, with size NProcessSize, if more data other than this COMMAND, remain in MoreBuffer			//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CListItem* ProcessClientCommand( CContextItem* mContext, CListItem* &mBuffer, long size )											//
{
	CApplication* pApp = mContext->PApplication;

	if (mContext->ContentMode == CONTENT_MODE_HEAD)
	{
		mBuffer=ProcessCommand(mContext, mBuffer, size, pApp->CliCommandEnd, pApp->CliCommandEndSize);
		if (mBuffer->NProcessSize > BUFFER_ERROR || !mBuffer->NProcessSize) return mBuffer;		// Jul 02 '13, for NOMOREBUFFER

		pApp->AnalysisHead(mContext, mBuffer, mBuffer->NProcessSize, OP_CLIENT_READ);

		if (pApp->CliCommandEndSize )						// for normal application, do not process body
		{
			mContext->PPeer->ContentMode = CONTENT_MODE_HEAD;
			if (mContext->BodyRemain > 0)
			{
				mContext->ContentMode = CONTENT_MODE_AFTERHEAD;
			}
			else mContext->BodyRemain = 0;
		}
	}
	else if (mContext->ContentMode == CONTENT_MODE_LENGTH)
	{
		mBuffer=ProcessCommand(mContext, mBuffer, size, 0, 0);
// 		if (!mContext->BodyRemain) mContext->ContentMode = CONTENT_MODE_HEAD;
	}
	return mBuffer;
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JQUERY_SPACE	2
#define JQUERY_TOTAL	5

extern unsigned char ASCII_HEX[];

CListItem* ProcessServerCommand( CContextItem* mContext, CListItem* &mBuffer, long size )											//
{																																	//
	CApplication* pApp = mContext->PApplication;

	if (mContext->ContentMode == CONTENT_MODE_HEAD)
	{
		mBuffer=ProcessCommand(mContext, mBuffer, size, pApp->SerCommandEnd, pApp->SerCommandEndSize);
		if (mBuffer->NProcessSize > BUFFER_ERROR || !mBuffer->NProcessSize) return mBuffer;		// Jul 02 '13, for NOMOREBUFFER

		pApp->AnalysisHead(mContext, mBuffer, mBuffer->NProcessSize, OP_SERVER_READ);

		if (pApp->SerCommandEndSize)									// for normal application, do not process body
		{
			mContext->PPeer->ContentMode = CONTENT_MODE_HEAD;
			if (mContext->BodyRemain > 0 || mContext->TransferEncoding==ENCODING_CHUNKED) 
				mContext->ContentMode = CONTENT_MODE_AFTERHEAD;
			else mContext->BodyRemain = 0;
		}
	}
	else if (mContext->ContentMode == CONTENT_MODE_LENGTH)
	{
		mBuffer=ProcessCommand(mContext, mBuffer, size, 0, 0);
// 		if (!mContext->BodyRemain) mContext->ContentMode = CONTENT_MODE_HEAD;
	}
	return mBuffer;
}	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CListItem* ProcessCommand( CContextItem* mContext, CListItem* mBuffer, long size, char* comEnd, long endSize  )					//
{																																	//
	char *oldstart, *oldend, *endplace = 0;																							//
	long cmdsize;																													//
	CListItem* morebuffer;																											//
	long buffersize, bufferoffset, newoffset;																						//


 	mBuffer->NProcessSize = size;								//	Record the size													//
	morebuffer = mContext->MoreBuffer;							//	The Addition buffer place										//

	if ( (!endSize || !size) && (!morebuffer || !morebuffer->NProcessSize))	//	NOT buffered													//
	{															//////////////////////////////////////////////////////////////////////
		if ( (mContext->TransferEncoding == ENCODING_LENGTH) && (mContext->ContentMode == CONTENT_MODE_LENGTH) )
			mContext->BodyRemain -= size;
		return mBuffer;											//	Just return the new coming										//
	}																																//

// #ifdef _DEBUG
// 	if (!morebuffer) __asm int 3
// #endif _DEBUG
	if (!morebuffer)
	{
		mBuffer->NProcessSize = BUFFER_NOMOREBUFFER;
		return mBuffer;
	}
																																	//
	buffersize = mBuffer->BufferType->BufferSize;				//	The BUFFER total size											//
	bufferoffset = morebuffer->NProcessSize;					//																	//
	oldstart = (char*)(morebuffer+1);							//	The real buffer start place, except CListItem					//
	oldend = oldstart + bufferoffset;							//	the end of the buffer											//
																//////////////////////////////////////////////////////////////////////
	if (size <= buffersize-bufferoffset)						//	The new coming is small enough to store in old buffer			//
	{															//																	//
		if (size)												//	Have new coming buffer											//
		{														//																	//
			memcpy(oldend, mBuffer+1, size);					//	Copy it to old buffer											//
			morebuffer->NProcessSize += size;
		}																															//
		newoffset = morebuffer->NProcessSize;

		if ( (mContext->TransferEncoding == ENCODING_CHUNKED) && (!endSize) )
		{
			endplace = oldstart + newoffset;
		}
		else if (mContext->BodyRemain)
		{
			if (mContext->BodyRemain <= newoffset)	
			{	
				endplace = oldstart+mContext->BodyRemain;
				mContext->BodyRemain = 0;
			}
			else 
			{
				endplace = oldstart+newoffset;					//	the error one is endplace = NULL; it take me about 8 hour to find
				mContext->BodyRemain -= newoffset;				//	for size may is 0
			}
		}														//	Try to find the END mark in the buffer							//
		else
		{
			if (endSize) endplace = memstr(oldstart, newoffset, comEnd, endSize) + endSize;										//
			else endplace = oldstart+newoffset;					//	for Proxy use Jul 01 '13, NOT control length
		}

 		if (endplace != (char*)endSize)							//	Find the END mark												//
		{														//////////////////////////////////////////////////////////////////////
			cmdsize = (endplace-oldstart);						//	Get the size from command start to END mark						//
																//	Copy the remain to old buffer									//
			memcpy((char*)(mBuffer+1), endplace, newoffset-cmdsize);																//
			mBuffer->NProcessSize = newoffset - cmdsize;		//	The remain command in mContext->MoreBuffer size "BufferOffset'	//
			morebuffer->NProcessSize = cmdsize;					//																	//
			mContext->MoreBuffer = mBuffer;																							//

			return morebuffer;									//	This COMMAND, with END mark										//
		}														//////////////////////////////////////////////////////////////////////
		else 
		{
			mBuffer->NProcessSize = 0;
			return mBuffer;										//	Do NOT find END mark, return 0 buffer							//
		}
	}															//////////////////////////////////////////////////////////////////////
	else														//	The old part of COMMAND, plus the new coming PACKET is large	//
	{															//	This means, there are a complete COMMAND in it.					//
#ifdef _DEBUG
// 		__asm int 3												//	sometime I will test the following
#endif _DEBUG

		memcpy(oldend, mBuffer+1, buffersize-bufferoffset);		//	Copy enough to old buffer										//

		if (mContext->BodyRemain)
		{
			cmdsize = (int)min(mContext->BodyRemain, buffersize);
			endplace = oldstart + cmdsize;
			mContext->BodyRemain = mContext->BodyRemain + size - cmdsize;
		}
		else endplace = memstr(oldstart, buffersize, comEnd, endSize) + endSize;										//
																//	Find END mark in the old buffer									//
 		if (endplace != (char*)endSize)							//	Find the END mark												//
		{														//																	//
			cmdsize = (endplace-oldstart);						//	Get the COMMAND size											//
																//	Copy the remain to old buffer									//
			memcpy(mBuffer+1, ((char*)(mBuffer+1))+cmdsize-bufferoffset, size-cmdsize+bufferoffset);								//
			mBuffer->NProcessSize = size-cmdsize+bufferoffset;
			morebuffer->NProcessSize = cmdsize;					//////////////////////////////////////////////////////////////////////
			mContext->MoreBuffer = mBuffer;																							//
			return morebuffer;									//	Return the COMMAND												//
		}														//////////////////////////////////////////////////////////////////////
		else													//																	//
		{														//	COMMAND large than BUFFER, ERROR								//
			mBuffer->NProcessSize = BUFFER_TOO_LARGE;
			morebuffer->NProcessSize = 0;
			DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in ProcessCommand, COMMAND too long, 0x%x\r\n  ", 0x10);		//
			return mBuffer;
//			return NULL;										//	The COMMAND MUST small than PACKET size, omit the command		//
		}														//////////////////////////////////////////////////////////////////////
	}
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Create APPLICATION by given PROTOCOL																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	NONE																											//
//	ret_err 0x30 :	break	Jan 09 '13																								//
//	ret_err 0x40 :	break	Jan 09 '13																								//
//	ret_err 0x50 :	break	Jan 09 '13																								//
//	ret_err 0x60 :	break	Jan 09 '13																								//
// should do
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//

CContextItem* CreateApplication(CApplication *pApp, CProtocol *pProCli, void* paraCli, long sizeCli,								//
					   CProtocol *pProSer, void* paraSer, long sizeSer, long acceptNumber, char* keySer)							//
//	should redo
{																																	//
	CContextItem *cliContext = 0, *serContext = 0;																					//
	CListItem *newBuffer;																											//
	long ret_err = 0x01;																											//
	long ret;																														//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		if (!pApp) break;
		
		if (pProCli)
		{
			ret_err = 0x30;																											//
			cliContext = pProCli->GetProtocolContext();																				//
			if (!cliContext) break;																									//
																																	//
			ret_err = 0x40;																											//
			cliContext->PApplication = pApp;																						//
			ret = ProFunc(pProCli, fCreateNew)(cliContext, paraCli, sizeCli);														//
			if (ret) break;																											//
			cliContext->PPeer = cliContext;																							//

			for ( int i=0; i<acceptNumber; i++)
			{
				ret_err = 0x55;																										//
				newBuffer = pApp->GetApplicationBuffer();																			//
				if (!newBuffer) break;																								//
				ret_err = 0x60;																										//
				ret = NoneProFunc(pProCli, fPostAccept)(cliContext, newBuffer, newBuffer->BufferType->BufferSize, OP_ACCEPT);		//
				if (ret) break;																										//
			}
		}																												 			//
		ret_err = 0x50;																												//
		if (pProSer)																												//
		{																															//
			if ( (pApp->ApplicationFlag & FLAG_MULTI_PROTOCOL) && keySer )
			{
				CMultiApplication *pmApp = (CMultiApplication*)pApp;
				serContext = pmApp->AddProtocolPeer(pProSer, paraSer, sizeSer, keySer);
				if (!serContext) break;																								//
			}
			else
			{
				serContext = pProSer->GetProtocolContext();																			//
				if (!serContext) break;																								//
				serContext->PApplication = pApp;																					//
				ret = ProFunc(pProSer, fCreateRemote)(serContext, paraSer, sizeSer);												//
				if (ret) break;
				serContext->PPeer = NULL;																							//
			}
		}																															//
		if (cliContext && serContext)
		{
			pApp->FirstServerContext = serContext;
			cliContext->PPeer = serContext;																//
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ATTENTION: after Accept, should NOT free cliContext and newBuffer, for it will free in CNoneApplication::OnClose				//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
		if (cliContext) pApp->FirstContext = cliContext;																			//
		else if (serContext) pApp->FirstContext = serContext;
		else pApp->FirstContext = 0;
// 		pApp->FirstServerContext = serContext;
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)						// should redo		//testuse																//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CreateApplication, 0x%x\r\n  ", ret_err);						//
		RET_BETWEEN(0x60, 0x60) if (pProSer) pProSer->FreeProtocolContext(serContext);												//
		RET_BETWEEN(0x50, 0x60) cliContext->PProtocol->MyCloseHandle(cliContext->BHandle);											//
		RET_BETWEEN(0x40, 0x60) pProCli->FreeProtocolContext(cliContext);															//
		RET_BETWEEN(0x60, 0x60) pApp->FreeApplicationBuffer(newBuffer);																//
		cliContext = NULL;																											//
	}																																//
	if (cliContext) return cliContext;																								//
	else if (serContext) return serContext;
	
	return 0;
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Only free the HANDLE for APPLICATION																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CloseApplication( CApplication *pApp )																							//
{																																	//
	HANDLE chandle;																													//
	CMultiApplication* mpApp;
																																	//
	if (pApp && (pApp->ApplicationFlag&FLAG_MULTI_PROTOCOL) )
	{
		mpApp = (CMultiApplication*)pApp;
		mpApp->FreeServerContext();
	}

	if (pApp && pApp->FirstContext)																							//
	{																																//
		chandle = pApp->FirstContext->BHandle;																				//
		pApp->FirstContext->BHandle = 0;																						//
		if (chandle) return pApp->FirstContext->PProtocol->MyCloseHandle(chandle);															//
	}																																//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Send the given BUFFER to peer																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long SendToPeer(CContextItem* mContext, CListItem* &mBuffer, long size, long op)													//
{																																	//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		if (mContext->PPeer)																										//
		{																															//
			if (NoneProFunc(mContext->PPeer->PProtocol, fPostSend)(mContext->PPeer, mBuffer, size, op, 0)) break;					//
		}																															//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L2, "Error in SendToPeer, Peer closed, 0x%x\r\n", ret_err);					//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Free the CONTEXT back to MEMORYLIST, and reInit the value, and determined whether close the CONNECT								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
CContextItem* CProtocol::ClearProtocolContext(CContextItem* mContext, BOOL forceclose, BOOL clearcontext)							//
{
	HANDLE handle;																													//
	CListItem* mBuffer;																												//
	if ( mContext->BHandle && 	(forceclose || !(mContext->ListFlag & FLAG_LATER_CLOSE)) )					//	For FLAG_LATER_CLOSE PROTOCOL such as UDP, NOT close			//
	{															//////////////////////////////////////////////////////////////////////
		handle = mContext->BHandle;																									//
		mContext->BHandle = 0;																										//
// printf("clear bhandle %x\r\n", mContext);
		mContext->PProtocol->MyCloseHandle(handle);				//	Normally close the CONNECT										//
	}															//////////////////////////////////////////////////////////////////////
	if (mContext->MoreBuffer)									//	For APPLICATION with cli/serCommandLineEnd, free MoreBuffer		//
	{																																//
		mBuffer = mContext->MoreBuffer;																								//
#ifdef	DEBUG_BUFFER
		printf("FreeMore:%08x,%s%4d\r\n", mBuffer, mBuffer->BufferType->ListName, mBuffer->BufferType->GetFreeNumber());
#endif
		mContext->MoreBuffer = 0;																									//
		mBuffer->CliHandle = 0;
		mBuffer->BufferType->FreeContext(mBuffer);																					//
	}															//////////////////////////////////////////////////////////////////////

	if (mContext->FirstContent)
	{
#ifdef	DEBUG_CONCURRENCY
		printf("Free FirstConent\r\n");
#endif	DEBUG_CONCURRENCY
		mBuffer = mContext->FirstContent;
#ifdef	DEBUG_BUFFER
		printf("FreeFirst:%08x,%s%4d\r\n", mBuffer, mBuffer->BufferType->ListName, mBuffer->BufferType->GetFreeNumber());
#endif
		mContext->FirstContent = 0;
		mContext->PApplication->FreeApplicationBuffer(mBuffer);
		// 			mBuffer->BufferType->FreeContext(mBuffer);
	}

	if (mContext->PProtocol->CompareParaLength)					//	Clear Keyword in CONTEXT										//
	{																																//
		memset( ((char*)mContext)+mContext->PProtocol->CompareParaStart, 0x99, mContext->PProtocol->CompareParaLength);				//
	}																																//
	if (clearcontext)
	{
		ZeroProtocolContext(mContext);
	}
	return mContext;
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CNoneProtocol::PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op)
{
#ifdef DEBUG_POST
	printf("In PostAccept mContext:%x, Peer:%x, mBuffer:%x, size:%d, op:%d\r\n", mContext, mContext->PPeer, mBuffer, size, op);
#endif DEBUG_POST
	mBuffer->NOperation = op;																										//

	if (mContext->BHandle == 0)									//	could no use passby
	{
		mContext->PApplication->FreeApplicationBuffer(mBuffer);
		return 0;
	}
	else return ProFunc(mContext->PProtocol, fPostAccept)(mContext, mBuffer, size, op);
}

long CNoneProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)									//
{
#ifdef DEBUG_POST
	printf("In PostConnect mContext:%x, Peer:%x, mBuffer:%x, size:%d, op:%d\r\n", mContext, mContext->PPeer, mBuffer, size, op);
#endif DEBUG_POST
	mBuffer->NOperation = op;																							//
	return ProFunc(mContext->PProtocol, fPostConnect)(mContext, mBuffer, size, op);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Do some common things for any PROCOTOL on SEND																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)							//
{																																	//
#ifdef DEBUG_POST
	printf("In PostSend mContext:%x, Peer:%x, mBuffer:%x, size:%d, op:%d\r\n", mContext, mContext->PPeer, mBuffer, size, op);
#endif DEBUG_POST

	mBuffer->NOperation = op;
	ReflushTimeout(mContext);																										//

	return ProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, op, opSide);											//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Do some common things for any PROCOTOL on RECEIVE																				//
//	After one side send DATA, the peer should PostReceive																			//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneProtocol::PostReceive( CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide )					//
{																																	//
#ifdef DEBUG_POST
	printf("In PostReceive mContext:%x, Peer:%x, mBuffer:%x, size:%d, op:%d\r\n", mContext, mContext->PPeer, mBuffer, size, op);
#endif DEBUG_POST
// 	CListItem* cmdBuffer = mBuffer;																									//
																																	//
	if (op == OP_SERVER_READ)									//	The CLIENT has send, Read from SERVER again						//
	{															//////////////////////////////////////////////////////////////////////
		mBuffer = ProcessServerCommand(mContext, mBuffer, 0);	//	Get SERVER COMMAND from BUFFER,									//
// 		if (mBuffer->NProcessSize == BUFFER_TOO_LARGE) return -1;			//	COMMAND too long									//
		if (mBuffer->NProcessSize > BUFFER_ERROR) return -1;

		if (mBuffer->NProcessSize)								//	More COMMAND in BUFFER, do NOT read again						//
			return AppFunc(mContext->PApplication, fOnServerRead)(mContext, mBuffer, mBuffer->NProcessSize, opSide);				//
	}																																//
																																	//
	if (op == OP_CLIENT_READ || op == OP_ACCEPT/*for UDP use*/)//	The Server has send, Read from Client again						//
	{															//////////////////////////////////////////////////////////////////////
		mBuffer = ProcessClientCommand(mContext, mBuffer, 0);	//	Get CLIENT COMMAND from BUFFER									//
// 		if (cmdBuffer->NProcessSize == BUFFER_TOO_LARGE) return -1;			//	COMMAND too long									//
		if (mBuffer->NProcessSize > BUFFER_ERROR) return -1;

		if (mBuffer->NProcessSize)																									//
			return AppFunc(mContext->PApplication, fOnClientRead)(mContext, mBuffer, mBuffer->NProcessSize, opSide);				//
	}																																//
																																	//
// 	if (mContext->PProtocol->ProtocolFlag & FLAG_ADD_OVERLAP)	//	Go here, means No buffer again, should read						//
// 	{															//////////////////////////////////////////////////////////////////////
// 		__int64 *pointer64 = (__int64*)&(mBuffer->OLapped.Offset);																	//
// 		*pointer64 = mContext->OverlapOffset;																						//
// 	}																																//
	ReflushTimeout(mContext);																										//
																																	//
	if ( (mContext->PProtocol->ProtocolFlag & FLAG_MULTI_INPUT) && (op == OP_CLIENT_READ) )										//
		mBuffer->NOperation = OP_ACCEPT;						// for UDP, receive from the main context							//
	else mBuffer->NOperation = op;								//////////////////////////////////////////////////////////////////////
	
	return ProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, size, op, opSide);										//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	bind SOCKET and attach to IOCP																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	NONE																											//
//	ret_err 0x30 :	break, have close the handle	Jan 01 '13																		//
//	ret_err 0x40 :	break, have close the handle	Jan 01 '13																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CIPProtocol::BindLocalSocket(CContextItem* mContext, CProtocol* pProtocol, sockaddr_in* addr)									//
{																																	//
	long ret_err = 0x01;																											//
	int ptype;																														//
	HANDLE IOPort;																													//
	int pNumber;
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		mContext->BHandle = 0;																										//
		if (pProtocol->ProtocolNumber == PROTOCOL_TCP)
		{
			ptype = SOCK_STREAM;														//
			pNumber = PROTOCOL_TCP;
		}
		else if ( (pProtocol->ProtocolNumber == PROTOCOL_UDP) || (pProtocol->ProtocolNumber == PROTOCOL_SINGLEUDP) )
		{
			ptype = SOCK_DGRAM;												//
			pNumber = PROTOCOL_UDP;
		}
		else break;																													//
																																	//
		ret_err = 0x20;																												//
		mContext->BHandle = (HANDLE)::WSASocket(AF_INET, ptype, pNumber, NULL, 0, WSA_FLAG_OVERLAPPED);			//
		if (mContext->BHandle == INVALID_HANDLE_VALUE)																				//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);														//
																																	//
		ret_err = 0x30;																												//
		if (::bind((SOCKET)mContext->BHandle, (struct sockaddr*)addr, sizeof(sockaddr_in)))											//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);														//
																																	//
		ret_err = 0x40;																												//
		IOPort = ::CreateIoCompletionPort((HANDLE)mContext->BHandle, ProtocolIOCP->mIOCP, (ULONG_PTR)mContext, 0);					//
		if (!IOPort) DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);												//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in BindLocalSocket, 0x%x\r\n  ", ret_err);							//
		RET_BETWEEN(0x30, 0x40) ::closesocket((SOCKET)mContext->BHandle);															//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CIPProtocol::BindLocalSocket(CContextItem* mContext, CProtocol* pProtocol, sockaddr_in* addr, int* isop)									//
{																																	//
	long ret_err = 0x01;																											//
	int ptype;																														//
	HANDLE IOPort;																													//
	int pNumber;
	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		mContext->BHandle = 0;																										//
		if (pProtocol->ProtocolNumber == PROTOCOL_TCP)
		{
			ptype = SOCK_STREAM;														//
			pNumber = PROTOCOL_TCP;
		}
		else if ( (pProtocol->ProtocolNumber == PROTOCOL_UDP) || (pProtocol->ProtocolNumber == PROTOCOL_SINGLEUDP) )
		{
			ptype = SOCK_DGRAM;												//
			pNumber = PROTOCOL_UDP;
		}
		else break;																													//
		//

		ret_err = 0x20;																												//
		mContext->BHandle = (HANDLE)::WSASocket(AF_INET, ptype, pNumber, NULL, 0, WSA_FLAG_OVERLAPPED);			//
		if (mContext->BHandle == INVALID_HANDLE_VALUE)																				//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);														//
		//
		ret_err = 0x30;																												//

		if (::bind((SOCKET)mContext->BHandle, (struct sockaddr*)addr, sizeof(sockaddr_in)))											//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);														//
		//
		ret_err = 0x40;																												//

		IOPort = ::CreateIoCompletionPort((HANDLE)mContext->BHandle, ProtocolIOCP->mIOCP, (ULONG_PTR)mContext, 0);					//
		if (!IOPort) DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);												//
		//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in BindLocalSocket, 0x%x\r\n  ", ret_err);							//
		RET_BETWEEN(0x30, 0x40) ::closesocket((SOCKET)mContext->BHandle);															//
	}																																//
	return ret_err;																													//
}																																	//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, CreateNew is bind and listen the socket which attached to IOCP															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	break, have close the handle	Jan 05 '13																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CTCPProtocol::CreateNew(CContextItem* mContext, void* para, long size)														//
{																																	//
	sockaddr_in* addr = (sockaddr_in*) para;																						//
	long ret;																														//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		ret = BindLocalSocket(mContext, this, addr);																				//
		if (ret) break;																												//
																																	//
		ret_err = 0x20;																												//
		mContext->countDown = TIMEOUT_INFINITE;																						//
		if (::listen((SOCKET)mContext->BHandle, SOMAXCONN)) DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);			//
																																	//
		ret_err = 0;																												//
		DEBUG_MESSAGE_IPADDRESS_PORT (MODULE_PROTOCOL, MESSAGE_STATUS, "Add TCP Listen:%s:%d\r\n", *addr);							//
		if (!mContext->ContextType->DirectFree)					//	For close ACCEPT SOCKET without input, only undirect free		//
		{
			mContext->CliBuffer = &CliListStart;
			CliListStart.CliListNext = MARK_CLIBUFFER_END;
			CliListStart.CliWaitData = 0;
			CliListStart.CliHandle = 0;
		}

		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CTCPProtocol::CreateNew, 0x%x\r\n  ", ret_err);					//
//		RET_BETWEEN(0x20, 0x20) ::closesocket((SOCKET)mContext->BHandle);															//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, CreateRemote is remember the remote address																			//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CTCPProtocol::CreateRemote(CContextItem* mContext, void* para, long size)														//
{																																	//
	sockaddr_in* addr = (sockaddr_in*) para;																						//
	CTCPContext* tcpContext = (CTCPContext*)mContext;																				//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		tcpContext->countDown = TIMEOUT_INFINITE;																					//
		tcpContext->addrServer = *addr;																								//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, PostAccept is AcceptEx																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CTCPProtocol::PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op)										//
{																																	//
	CTCPContext* tcpContext = (CTCPContext*)mContext;
	long ret_err = 0x01;																											//
	BOOL ret;																														//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		mBuffer->CliHandle = (HANDLE)::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);							//
		if (mBuffer->CliHandle == (HANDLE)INVALID_SOCKET) 																			//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L1);															//
																																	//
		ret_err = 0x20;																												//
		ret = CIOCP::m_lpfnAcceptEx((SOCKET)mContext->BHandle, (SOCKET)mBuffer->CliHandle, mBuffer+1,								//
			size-((sizeof(sockaddr_in)+16)*2), sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16,										//
// the size and the sub value could NOT change
			&(mBuffer->NProcessSize), &(mBuffer->OLapped));																			//

		if (!ret && ::WSAGetLastError() != WSA_IO_PENDING) DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);			//

		ret_err = 0;																												//
		if ( mContext->CliBuffer && !(mBuffer->CliListNext) )	// mBuffer maybe in CliList already, find in Feb 20 '13
		{
			while ( InterCmpExg(&CliListInProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );						//
			mBuffer->CliListNext = CliListStart.CliListNext;
			CliListStart.CliListNext = mBuffer;
// 			if (mBuffer->CliListNext) mBuffer->CliListNext->CliListPrev = mBuffer;
//			mBuffer->CliListPrev = &CliListStart;
			mBuffer->CliWaitData = MARK_IN_PROCESS;
			CliListInProcess = MARK_NOT_IN_PROCESS;
		}

//		mContext->CliBuffer = mBuffer;							//	For countdown ACCEPT SOCKET without DATA						//
		break;													//////////////////////////////////////////////////////////////////////
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CTCP - PostAccept 0x%x, mContext:%x\r\n", ret_err, mContext);	//
//		RET_BETWEEN(0x20, 0x20) ::closesocket((SOCKET)mBuffer->CliHandle);															//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, PostConnect is ConnectEx																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CTCPProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)									//
{																																	//
	sockaddr_in* addr = (sockaddr_in*)&(((CTCPContext*)mContext)->addrServer);														//
	long ret_err = 0x01;																											//
	BOOL ret;																														//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		ret = BindLocalSocket(mContext, this, &CIOCP::tempaddr);																	//
		if (ret) break;																												//
																																	//
		ret_err = 0x20;																												//
// 		mBuffer->NOperation = OP_CONNECT;																							//

		ReflushTimeout(mContext, TIMEOUT_TCPCONNECT);		//	for short timeout for connect	//	May 01 '14						//

		ret = CIOCP::m_lpfnConnectEx((SOCKET)mContext->BHandle, (sockaddr*)addr, sizeof(sockaddr),									//
			mBuffer+1, (DWORD)size, (DWORD*)&(mBuffer->NProcessSize), &(mBuffer->OLapped));											//
		if (!ret && ::WSAGetLastError() != WSA_IO_PENDING) DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);			//

#ifdef DEBUG_TCP
		*(REAL_BUFFER(mBuffer)+size) = 0;
		printf ("In TCP PostConnnect\r\n%sEndof PostConnect\r\n", REAL_BUFFER(mBuffer));
#endif DEBUG_TCP

		ret_err = 0;																												//
// 		DEBUG_MESSAGE_IPADDRESS_PORT (MODULE_PROTOCOL, MESSAGE_STATUS, "Add Forward:%d-%s:%d\r\n",									//
// 			mContext->PProtocol->ProtocolNumber, *addr);																			//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L3, "Error in CTCP - PostConnect 0x%x, mContext:%x\r\n", ret_err, mContext);	//
//		RET_BETWEEN(0x20, 0x20) ::closesocket((SOCKET)mContext->BHandle);															//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, PostSend is WSASend																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CTCPProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)							//
{																																	//
	WSABUF databuf;																													//
	DWORD dwFlags = 0;																												//
	long ret_err = 0x01;																											//
	long ret;																														//

	while (TRUE)																													//
	{																																//
		mBuffer->OLapped.Internal = 0;																								//
		mBuffer->OLapped.InternalHigh = 0;																							//
																																	//
		ret_err = 0x10;																												//
		databuf.buf = (char*)(mBuffer+1);																							//
		databuf.len = size;																											//
		ret = ::WSASend((SOCKET)mContext->BHandle, &databuf, 1, &(mBuffer->NProcessSize), dwFlags, &(mBuffer->OLapped), NULL);		//
		if ( ret )
		{
			int wsaret = ::WSAGetLastError();
//	10054 is so crazy, for TCP should manual close, while UDP should do nothing.		//	Add Oct. 27 '13 by ConfirmDNS debug		//
			if (wsaret == WSAECONNRESET) FreeProtocolContext(mContext);					//	Add this line.							//
//	when remote close, IOCP do NOT return anything, if no mBuffer attached it. have to close mContext manual						//
			if (wsaret != WSA_IO_PENDING ) 	DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);						//
		}

#ifdef DEBUG_TCP
		*(REAL_BUFFER(mBuffer)+size) = 0;
		printf ("In TCP PostSend\r\n%sEndof PostSend\r\n", REAL_BUFFER(mBuffer));
#endif DEBUG_TCP

// printf("send:%d,mcontext:%x\r\n", size, mContext);
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CTCP - PostSend 0x%x, mContext:%x\r\n", ret_err, mContext);		//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, PostReceive is WSAReceive																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CTCPProtocol::PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)						//
{																																	//
	WSABUF databuf;																													//
	DWORD dwFlags = 0;																												//
	long ret_err = 0x01;																											//
	long ret;																														//

	while (TRUE)																													//
	{																																//
		mBuffer->OLapped.Internal = 0;																								//
		mBuffer->OLapped.InternalHigh = 0;																							//
// printf("Real TCP PostReceive:%x, %s\r\n", mContext, OPSTR[op-OP_BASE]);																																	//
		ret_err = 0x10;																												//
		databuf.buf = (char*)(mBuffer+1);																							//
		databuf.len = size;																											//
		ret = ::WSARecv((SOCKET)mContext->BHandle, &databuf, 1, &(mBuffer->NProcessSize), &dwFlags, &(mBuffer->OLapped), NULL);	//

		if ( ret && ::WSAGetLastError() != WSA_IO_PENDING )
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);														//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CTCP - PostReceive 0x%x, mContext:%x\r\n", ret_err, mContext);	//
	}																																//
	return ret_err;																													//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int addrSize = sizeof(sockaddr_in);		// surely? if local, it will crash the stack.
//	More than 10 hours to find this

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For TCP, PostReceive is WSAReceive																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CUDPProtocol::CreateNew(CContextItem* mContext, void* para, long size)
{
	sockaddr_in* addr = (sockaddr_in*) para;
	long ret;
	long ret_err = 0x01;

	while (TRUE)
	{
		ret_err = 0x20;
		ret = BindLocalSocket(mContext, this, addr);
		if (ret) break;

		ret_err = 0x30;
		mContext->countDown = TIMEOUT_INFINITE;

//	6 hours for find this, UDP 10054, a popular BUG for UDP,		// Oct. 24 '13
		DWORD dwBytesReturned = 0;
		BOOL   bNewBehavior = FALSE;
		ret_err = 0x40;
		ret = WSAIoctl((SOCKET)mContext->BHandle, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior),
			NULL, 0, &dwBytesReturned, NULL, NULL);

		if (ret && ::WSAGetLastError() != WSA_IO_PENDING)
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);

		ret_err = 0;
		DEBUG_MESSAGE_IPADDRESS_PORT (MODULE_PROTOCOL, MESSAGE_STATUS, "Add UDP Listen:%s:%d\r\n", *addr);
		break;
	}
	if (ret_err)
	{
	}
	return ret_err;
}

long CUDPProtocol::CreateRemote(CContextItem* mContext, void* para, long size)
{
	sockaddr_in* addr = (sockaddr_in*) para;
	CUDPContext* udpContext = (CUDPContext*)mContext;
	long ret_err = 0x01;

	while (TRUE)
	{
		ret_err = 0x10;
		udpContext->countDown = TIMEOUT_INFINITE;
		udpContext->addrServer = *addr;

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
	}
	return 0;
}

long CUDPProtocol::PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op)
{
	return NoneProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, size, op, 0);		// hehe, here is Accept, UDP is so strange
}

long CUDPProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)
{
// 	sockaddr_in* addr = (sockaddr_in*)&(((CUDPContext*)mContext)->addrServer);
	long ret_err = 0x01;
	BOOL ret;

	while (TRUE)
	{
		ret_err = 0x10;
		ret = BindLocalSocket(mContext, this, &CIOCP::tempaddr);
		if (ret) break;

		ret_err = 0x20;
		NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, op, 0);	//	So strange, OP_CONNECT? I forget	// op should OP_CONNECT

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
		DEBUG_MESSAGE(MODULE_IOCP, MESSAGE_ERROR_L3, "Error in CTCP - PostConnect 0x%x\r\n", ret_err);
	}

	return ret_err;
}

long CUDPProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	WSABUF databuf;
	DWORD dwFlags = 0;
// 	int addrSize = sizeof(sockaddr_in);//	surely? if local, it will crash the stack. move the globle
	long ret_err = 0x01;
	CProtocol* pProtocol;

	CUDPContext* udpContext = (CUDPContext*)mContext;
	long ret;

	while(TRUE)
	{
		ret_err = 0x10;
		databuf.buf = (char*)(mBuffer+1);	//move to the real buffer
		databuf.len = size;
		mBuffer->OLapped.Internal = 0;
		mBuffer->OLapped.InternalHigh = 0;

// the Addition info in mBuffer, is used to detect the server part of UDP port
		pProtocol = mContext->PProtocol;
		memcpy( ((char*)&(mBuffer->AdditionInfo)), ((char*)(mContext))+pProtocol->CompareParaStart, pProtocol->CompareParaLength );

		ret = WSASendTo((SOCKET)mContext->BHandle, &databuf, 1, &(mBuffer->NProcessSize), dwFlags,
			(sockaddr*)&udpContext->addrServer, addrSize, &(mBuffer->OLapped), NULL);
		if (ret && ::WSAGetLastError() != WSA_IO_PENDING)
			DEBUG_MESSAGE (MODULE_IOCP, MESSAGE_ERROR_L3, "WSA Error code:%d\r\n  ", WSAGetLastError());								//

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
		DEBUG_MESSAGE(MODULE_IOCP, MESSAGE_ERROR_L1, "Error in IOCP PostUDPSend_Socket, 0x%x\r\n  ", ret_err);
		return 1;
	}
	return 0;
}

long CUDPProtocol::PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	WSABUF databuf;
	DWORD dwFlags = 0;
// 	int addrSize = sizeof(sockaddr_in);//	surely? if local, it will crash the stack. move the globle
	long ret_err = 0x01;
	CUDPContext* udpContext = (CUDPContext*)mContext;
	long ret;

	while (TRUE)
	{
		ret_err = 0x10;
		databuf.buf = (char*)(mBuffer+1);	//move to the real buffer
		databuf.len = size;
		mBuffer->OLapped.Internal = 0;
		mBuffer->OLapped.InternalHigh = 0;

		ret = WSARecvFrom((SOCKET)mContext->BHandle, &databuf, 1, &(mBuffer->NProcessSize), &dwFlags, 
			(sockaddr*)&(mBuffer->FromAddr), &addrSize, &(mBuffer->OLapped), NULL);
		if (ret && ::WSAGetLastError() != WSA_IO_PENDING)
			DEBUG_MESSAGE (MODULE_IOCP, MESSAGE_ERROR_L3, "WSA Error code:%d\r\n  ", WSAGetLastError());								//

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CUDP - PostReceive, 0x%x\r\n  ", ret_err);
		return 1;
	}
	return 0;
}

long CSingleUDPProtocol::CreateNew(CContextItem* mContext, void* para, long size)
{
	sockaddr_in* addr = (sockaddr_in*) para;
	long ret;
	long ret_err = 0x01;

	while (TRUE)
	{
		ret_err = 0x20;
		ret = BindLocalSocket(mContext, this, addr);
		if (ret) break;

		ret_err = 0x30;
		mContext->countDown = TIMEOUT_INFINITE;

//	6 hours for find this, UDP 10054, a popular BUG for UDP,		// Oct. 24 '13
		DWORD dwBytesReturned = 0;
		BOOL   bNewBehavior = FALSE;
		ret_err = 0x40;
		ret = WSAIoctl((SOCKET)mContext->BHandle, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior),
			NULL, 0, &dwBytesReturned, NULL, NULL);

		if (ret && ::WSAGetLastError() != WSA_IO_PENDING)
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);


		ret_err = 0;
		DEBUG_MESSAGE_IPADDRESS_PORT (MODULE_PROTOCOL, MESSAGE_STATUS, "Add UDP Listen:%s:%d\r\n", *addr);
		break;
	}
	if (ret_err)
	{
	}
	return ret_err;
}

long CSingleUDPProtocol::CreateRemote(CContextItem* mContext, void* para, long size)
{
	sockaddr_in* addr = (sockaddr_in*) para;
	CUDPContext* udpContext = (CUDPContext*)mContext;
	long ret_err = 0x01;

	while (TRUE)
	{
		ret_err = 0x10;
		udpContext->countDown = TIMEOUT_INFINITE;
		udpContext->addrServer = *addr;

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
	}
	return 0;
}

long CSingleUDPProtocol::PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op)
{
	return NoneProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, size, op, 0);		// hehe, here is Accept, UDP is so strange
}

long CSingleUDPProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)
{
// 	sockaddr_in* addr = (sockaddr_in*)&(((CUDPContext*)mContext)->addrServer);
	long ret_err = 0x01;
	BOOL ret;

	while (TRUE)
	{
		ret_err = 0x10;
		ret = BindLocalSocket(mContext, this, &CIOCP::tempaddr, (int*)&op);
		if (ret) break;

		ret_err = 0x20;
		NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, op, 0);	//	So strange, OP_CONNECT? I forget	// op should OP_CONNECT

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
		DEBUG_MESSAGE(MODULE_IOCP, MESSAGE_ERROR_L3, "Error in CTCP - PostConnect 0x%x\r\n", ret_err);
	}

	return ret_err;
}


long CSingleUDPProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	WSABUF databuf;
	DWORD dwFlags = 0;
// 	int addrSize = sizeof(sockaddr_in);		//	surely? if local, it will crash the stack. move the globle
	long ret_err = 0x01;
	CProtocol* pProtocol;

	CUDPContext* udpContext = (CUDPContext*)mContext;
	long ret;

	while(TRUE)
	{
		ret_err = 0x10;
		databuf.buf = (char*)(mBuffer+1);	//move to the real buffer
		databuf.len = size;
		mBuffer->OLapped.Internal = 0;
		mBuffer->OLapped.InternalHigh = 0;

		// the Addition info in mBuffer, is used to detect the server part of UDP port
		pProtocol = mContext->PProtocol;
//	only client send use this, server send is PostConnect
		mBuffer->UsedContext = mContext;	// close this context later

		ret = WSASendTo((SOCKET)mContext->BHandle, &databuf, 1, &(mBuffer->NProcessSize), dwFlags,
			(sockaddr*)&udpContext->addrServer, addrSize, &(mBuffer->OLapped), NULL);
		if (ret && ::WSAGetLastError() != WSA_IO_PENDING)
			DEBUG_MESSAGE (MODULE_IOCP, MESSAGE_ERROR_L3, "WSA Error code:%d\r\n  ", WSAGetLastError());								//

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
		DEBUG_MESSAGE(MODULE_IOCP, MESSAGE_ERROR_L1, "Error in IOCP PostUDPSend_Socket, 0x%x\r\n  ", ret_err);
		return 1;
	}
	return 0;
}

long CSingleUDPProtocol::PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	WSABUF databuf;
	DWORD dwFlags = 0;
// 	int addrSize = sizeof(sockaddr_in);//	surely? if local, it will crash the stack. move the globle
	long ret_err = 0x01;
	CUDPContext* udpContext = (CUDPContext*)mContext;
	long ret;

	while (TRUE)
	{
		ret_err = 0x10;
		databuf.buf = (char*)(mBuffer+1);	//move to the real buffer
		databuf.len = size;
		mBuffer->OLapped.Internal = 0;
		mBuffer->OLapped.InternalHigh = 0;

		ret = WSARecvFrom((SOCKET)mContext->BHandle, &databuf, 1, &(mBuffer->NProcessSize), &dwFlags, 
			(sockaddr*)&(mBuffer->FromAddr), &addrSize, &(mBuffer->OLapped), NULL);
		if (ret && ::WSAGetLastError() != WSA_IO_PENDING)
			DEBUG_MESSAGE (MODULE_IOCP, MESSAGE_ERROR_L3, "WSA Error code:%d\r\n  ", WSAGetLastError());								//

		ret_err = 0;
		break;
	}
	if (ret_err)
	{
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CUDP - PostReceive, 0x%x\r\n  ", ret_err);
		return 1;
	}
	return 0;
}

long CWinPCapProtocol::InitWinPCap()
{
	char errbuf[PCAP_ERRBUF_SIZE];

	if ( InterCmpExg(&WinPCapInit, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS ) return 0;
	if ( pcap_findalldevs(&AllDevices, errbuf) == -1 ) return 1;
	else return 0;
}

long CWinPCapProtocol::FreeWinPCap()
{
	if (WinPCapInit == MARK_NOT_IN_PROCESS) return 0;
	pcap_freealldevs(AllDevices);
	WinPCapInit = MARK_NOT_IN_PROCESS;

	return 0;
}

long CWinPCapProtocol::CreateNew(CContextItem* mContext, void* para, long size)														//
{
	return 0;
}

__int64 GetFileContextLength(CContextItem* mContext)
{
	CFileContext* fileContext = (CFileContext*)mContext;
	int ret;
// 	__int64 fileLength;

	if (!GetFileSizeEx(mContext->BHandle, (PLARGE_INTEGER)&(fileContext->fileLength) ) )
	{
		ret = ::GetLastError();
		fileContext->fileLength = 0;
	}
	if (!fileContext->fileLength)
	{
		ret = 1;
	}
	return fileContext->fileLength;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For File, CreateNew is Open file for open																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileProtocol::CreateNew(CContextItem* mContext, void* para, long size)														//
{																																	//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		mContext->BHandle = CreateFile																								//
			((char*)para, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);							//
		if (mContext->BHandle == INVALID_HANDLE_VALUE)																				//
			DEBUG_MESSAGE_FILE_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2);														//
																																	//
		ret_err = 0x20;																												//
		if (!::CreateIoCompletionPort((HANDLE)mContext->BHandle, ProtocolIOCP->mIOCP, (ULONG_PTR)mContext, 0))						//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);															//
DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_STATUS, "Open File:%s\r\n", (char*)para);											//
		mContext->countDown = TIMEOUT_INFINITE;																						//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CFile - CreateNew %s, 0x%x\r\n  ", para, ret_err);				//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILE, CreateRemote is remember the filename																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileProtocol::CreateRemote(CContextItem* mContext, void* para, long size)													//
{																																	//
	CFileContext* fileContext = (CFileContext*)mContext;																			//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		fileContext->countDown = TIMEOUT_INFINITE;																					//
		memcpy(fileContext->fileName, para, size);				//	size with \0 at the end
		fileContext->fileNameLength = size;
// 		strcpy_s(fileContext->fileName, MAX_PATH, (char*)para);																		//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILE, use PostReceive, no others																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileProtocol::PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op)									//
{																																	//
	return NoneProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, size, OP_CLIENT_READ, 0);								//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILE, open the file for write																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	break :	Jan 30 '13																								//
//	ret_err 0x20 :	break :	Jan 30 '13																								//
//	ret_err 0x30 :	break :	Jan 30 '13																								//
//	ret_err 0x0  :	break : add break just before ret_err=0, OnClose will be called with mContext = 0, find in Jan 30 '13			//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)									//
{																																	//
	CFileContext* fileContext = (CFileContext*)mContext;																			//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		mContext->BHandle = CreateFile(fileContext->fileName, GENERIC_WRITE, FILE_SHARE_READ,										//
			NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);																		//
		if (mContext->BHandle == INVALID_HANDLE_VALUE)																				//
			DEBUG_MESSAGE_FILE_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2);														//
																																	//
		ret_err = 0x20;																												//
		if (!::CreateIoCompletionPort((HANDLE)mContext->BHandle, ProtocolIOCP->mIOCP, (ULONG_PTR)mContext, 0))						//
			DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);															//
																																	//
		ret_err = 0x30;																												//
//		NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size,  OP_SERVER_WRITE, 0);									//
		NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size,  op, 0);	//	for Directory Connect					//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CFile - PostConnect, 0x%x\r\n  ", ret_err);						//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILE, send is WriteFile																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)							//
{																																	//
	DWORD fileWrite = 0;																											//
	__int64 *pointer64;
	long ret_err = 0x01;																											//
	long ret;																														//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
 		pointer64 = (__int64*)&(mBuffer->OLapped.Offset);																			//
		*pointer64 = mContext->OverlapOffset;
		ret = WriteFile(mContext->BHandle, mBuffer+1, size, &fileWrite, (LPOVERLAPPED)mBuffer );									//
		mContext->OverlapOffset = *pointer64 + size;			//	Add here or NoneApplication::OnClient/ServerWrite
		//	MUST add OverlapOffset just after send, or the next send before OnWrite will error, should find a good place in NonePro

		if (!ret && ::GetLastError() != WSA_IO_PENDING) DEBUG_MESSAGE_FILE_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);			//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CFile - PostSend, 0x%x\r\n  ", ret_err);							//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILE, receive is ReadFile																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileProtocol::PostReceive( CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)						//
{																																	//
	DWORD fileRead = 0;																												//
	__int64 *pointer64;

	long ret_err = 0x01;																											//
	long ret;																														//
	DWORD lasterror;																												//
	CFileContext* fileContext = (CFileContext*)mContext;
																																	//
	while (TRUE)																													//
	{
// printf("Real FILE PostReceive:%x, %s\r\n", mContext, OPSTR[op-OP_BASE]);																																	//
		ret_err = 0x10;																												//
		if (fileContext->fileLength <= mContext->OverlapOffset)
		//if (lasterror == ERROR_HANDLE_EOF)	// for windows do NOT return EOF for network file, at least in my XP
		// it return OK for ReadFile, so if EOF, could not ReadFile again, for it send mBuffer
		{
			return ( NoneAppFunc(mContext->PApplication, fOnClose)(mContext, mBuffer, mContext->PPeer->ListFlag & FLAG_KEEPALIVE, opSide) );	//
		}

		ret_err = 0x20;																												//
		pointer64 = (__int64*)&(mBuffer->OLapped.Offset);																			//
		*pointer64 = mContext->OverlapOffset;
		ret = ReadFile(mContext->BHandle, mBuffer+1, size, &fileRead, (LPOVERLAPPED)mBuffer);										//
// 		mContext->OverlapOffset = *pointer64 + size;			//	Add here or NoneApplication::OnClient/ServerRead

		if (!ret)																													//
		{																															//
			lasterror = ::GetLastError();																							//
			if (lasterror != WSA_IO_PENDING) DEBUG_MESSAGE_FILE_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);						//
		}																															//
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in CFile - PostReceive, 0x%x\r\n  ", ret_err);						//
	}																																//
	return ret_err;																													//
}																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
__int64 CFileProtocol::GetContextLength(CContextItem* mContext, CListItem* mBuffer)
{
	return ::GetFileContextLength(mContext);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILEREAD, CreateRemote is remember the file/directory																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileReadProtocol::CreateRemote(CContextItem* mContext, void* para, long size)												//
{																																	//
	return (mContext->PProtocol->* (&CIOCP::fProtocolFunction[PROTOCOL_FILE])->fCreateRemote) (mContext, para, size);				//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILEREAD, PostConnect is Open file for read																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileReadProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)								//
{																																	//
	CFileContext* fileContext = (CFileContext*)mContext;
	char* pointer = (char*)(mBuffer+1);
	while (*pointer)
	{
		if (*pointer == '/') *pointer = '\\'; 
		pointer++; 
	};
	strcat_s(fileContext->fileName, MAX_PATH, (char*)(mBuffer+1));
	// should add path (from fileContext->filename) and filename (from mBuffer+1)
	return (mContext->PProtocol->* (&CIOCP::fProtocolFunction[PROTOCOL_FILE])->fCreateNew)	
		(fileContext, fileContext->fileName, strlen(fileContext->fileName));
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For FILEREAD, receive is ReadFile																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CFileReadProtocol::PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)					//
{																																	//
	return (mContext->PProtocol->* (&CIOCP::fProtocolFunction[PROTOCOL_FILE])->fPostReceive)(mContext, mBuffer, size, op, opSide);	//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__int64 CFileReadProtocol::GetContextLength(CContextItem* mContext, CListItem* mBuffer)
{
	return ::GetFileContextLength(mContext);
}

long CDirectoryProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)								//
{
	CContextItem* dirContext = mContext->PApplication->FirstServerContext;
	long ret;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x10;
// printf("in connect\r\n");
	ret = GetFilename(mContext, dirContext, mBuffer, size);
	
	ret_err = 0x20;
	if (mContext->BHandle) ClearProtocolContext(mContext, TRUE, FALSE);
	mContext->OverlapOffset = 0;

	ret_err = 0x30;
	if (CFileProtocol::PostConnect(mContext, mBuffer, size, op)) break;

	__CATCH(MODULE_PROTOCOL, "CDirectoryProtocol - PostConnect")
}																																	//

long CDirectoryProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)					//
{
	CContextItem* dirContext = mContext->PApplication->FirstServerContext;
	long ret;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x10;
	if (opSide == OPSIDE_CLIENT)
	{
		ret_err = 0x20;
// printf("in send\r\n");
		ret = GetFilename(mContext, dirContext, mBuffer, size);
		if (!ret)
		{
			ret_err = 0x30;
			if (mContext->BHandle) ClearProtocolContext(mContext, TRUE, FALSE);
			mContext->OverlapOffset = 0;

			ret_err = 0x40;
			return CFileProtocol::PostConnect(mContext, mBuffer, size, op);
		}
	}
	return CFileProtocol::PostSend(mContext, mBuffer, size, op, 0);

	__CATCH(MODULE_PROTOCOL, "CDirectoryProtocol - PostSend")
}

long CDirectoryReadProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)							//
{
	CContextItem* dirContext = mContext->PApplication->FirstServerContext;
	long dirNameLength = ((CFileContext*)dirContext)->fileNameLength - 1;
	CFileContext* fileContext = (CFileContext*)mContext;
	long ret;
	long ret_err = 0x01;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	char* filename = (char*)(mBuffer+1);
	long filenamelen;

	__TRY

	ret_err = 0x10;
	ret = GetFilename(mContext, dirContext, mBuffer, size);

	ret_err = 0x20;
	if (mContext->BHandle) ClearProtocolContext(mContext, TRUE, FALSE);
	mContext->OverlapOffset = 0;

	ret_err = 0x30;
	hFind = FindFirstFile(fileContext->fileName, &findFileData);
	if (hFind ==INVALID_HANDLE_VALUE)
	{
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Do not find file:%s, \r\n", fileContext->fileName);			//
		break;
	}
	DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_STATUS, "Open file:%s, \r\n", findFileData.cFileName );			//

	ret_err = 0x40;
	filenamelen = strlen(findFileData.cFileName);
	memcpy(filename, ((CFileContext*)dirContext)->fileName, dirNameLength);
	memcpy(filename+dirNameLength, findFileData.cFileName, filenamelen);
	*(filename + dirNameLength + filenamelen) = 0;
	ret = (mContext->PProtocol->* (&CIOCP::fProtocolFunction[PROTOCOL_FILE])->fCreateNew)	
		(fileContext, (char*)(mBuffer+1), dirNameLength+filenamelen);
	if (ret) break;
	FindClose(hFind);

	ret_err = 0x50;
	(void)GetContextLength(mContext, mBuffer);

	ret_err = 0x60;
	ret = NoneProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, 0);
	if (ret) break;

	__CATCH(MODULE_PROTOCOL, "CDirectoryReadProtocol - PostConnect")
}

unsigned char FileNameTran[ASCII_NUMBER] = {
	0x00,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	
	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	
	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x2d,	0x2e,	0x2d,	
	0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,	0x38,	0x39,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	
	0x20,	0x41,	0x42,	0x43,	0x44,	0x45,	0x46,	0x47,	0x48,	0x49,	0x4a,	0x4b,	0x4c,	0x4d,	0x4e,	0x4f,	
	0x50,	0x51,	0x52,	0x53,	0x54,	0x55,	0x56,	0x57,	0x58,	0x59,	0x5a,	0x20,	0x20,	0x20,	0x20,	0x20,	
	0x20,	0x61,	0x62,	0x63,	0x64,	0x65,	0x66,	0x67,	0x68,	0x69,	0x6a,	0x6b,	0x6c,	0x6d,	0x6e,	0x6f,	
	0x70,	0x71,	0x72,	0x73,	0x74,	0x75,	0x76,	0x77,	0x78,	0x79,	0x7a,	0x20,	0x20,	0x20,	0x20,	0x20,	
	0x80,	0x81,	0x82,	0x83,	0x84,	0x85,	0x86,	0x87,	0x88,	0x89,	0x8a,	0x8b,	0x8c,	0x8d,	0x8e,	0x8f,	
	0x90,	0x91,	0x92,	0x93,	0x94,	0x95,	0x96,	0x97,	0x98,	0x99,	0x9a,	0x9b,	0x9c,	0x9d,	0x9e,	0x9f,	
	0xa0,	0xa1,	0xa2,	0xa3,	0xa4,	0xa5,	0xa6,	0xa7,	0xa8,	0xa9,	0xaa,	0xab,	0xac,	0xad,	0xae,	0xaf,	
	0xb0,	0xb1,	0xb2,	0xb3,	0xb4,	0xb5,	0xb6,	0xb7,	0xb8,	0xb9,	0xba,	0xbb,	0xbc,	0xbd,	0xbe,	0xbf,	
	0xc0,	0xc1,	0xc2,	0xc3,	0xc4,	0xc5,	0xc6,	0xc7,	0xc8,	0xc9,	0xca,	0xcb,	0xcc,	0xcd,	0xce,	0xcf,	
	0xd0,	0xd1,	0xd2,	0xd3,	0xd4,	0xd5,	0xd6,	0xd7,	0xd8,	0xd9,	0xda,	0xdb,	0xdc,	0xdd,	0xde,	0xdf,	
	0xe0,	0xe1,	0xe2,	0xe3,	0xe4,	0xe5,	0xe6,	0xe7,	0xe8,	0xe9,	0xea,	0xeb,	0xec,	0xed,	0xee,	0xef,	
	0xf0,	0xf1,	0xf2,	0xf3,	0xf4,	0xf5,	0xf6,	0xf7,	0xf8,	0xf9,	0xfa,	0xfb,	0xfc,	0xfd,	0xfe,	0xff,	
};

volatile static long FileSequence = 0;

long GetSequence2Filename(CContextItem* mContext, CContextItem* modelContext, CListItem* &mBuffer, long size)
{
	char* urlstart;
	long key;
	long ret_err = 0x01;
	CFileContext* fileContext = (CFileContext*)mContext;																			//
	CFileContext* dirContext = (CFileContext*)modelContext;
	char* filename = fileContext->fileName;
	char* dirname = dirContext->fileName;
	long nowSequence;
	time_t timer;

	__TRY

	ret_err = 0x10;
	urlstart = (char*)(mBuffer+1);
	key = *((long*)urlstart);
	if (key!=ISGET && key!=ISPOST && key!=ISCONNECT) break;

	time(&timer);
	memcpy(filename, dirname, dirContext->fileNameLength-1);			//	fileNameLength with \0
	nowSequence = InterInc(&FileSequence);
	Myitoa(nowSequence, filename+dirContext->fileNameLength-1, FileSequenceLength);
	Myitoa((long)timer, filename+dirContext->fileNameLength-1+FileSequenceLength, TimeSequenceLength);
	fileContext->fileNameLength = dirContext->fileNameLength-1+FileSequenceLength+TimeSequenceLength;


	__CATCH(MODULE_PROTOCOL, "GetSequence2Filename")
}

//	in mBuffer, modelContext->fileName
//	out mContext->fileName
long GetURL2Filename(CContextItem* mContext, CContextItem* modelContext, CListItem* &mBuffer, long size)
{
	char* urlstart;
	char* urlend;
	long key;
	long ret_err = 0x01;
	CFileContext* fileContext = (CFileContext*)mContext;																			//
	CFileContext* dirContext = (CFileContext*)modelContext;
	char* filename = fileContext->fileName;
	char* dirname = dirContext->fileName;
	long copysize;
	long nowSequence;
	unsigned char* spointer, *tpointer;
	int i;
	long siteLength = 0;
	long spaceLength = 0;

	__TRY

	ret_err = 0x10;
	urlstart = (char*)(mBuffer+1);
	key = *((long*)urlstart);
	if (key!=ISGET && key!=ISPOST && key!=ISCONNECT) break;

	urlend = memstr(urlstart, size, "\r\n", 2);
	if (!urlend) break;

	urlend = memstr(urlstart, urlend-urlstart, "HTTP", 4);
	if (!urlend) break;

	ret_err = 0x20;
#ifdef FILETRANSLATE_APPLICATION
	char* spacePlace;
	URLInfo* urlinfo = &((CHTTPContext*)mContext)->urlInfo;
#endif FILETRANSLATE_APPLICATION

// 	copysize = min(MAX_PATH-dirContext->fileNameLength-FileSequenceLength-siteLength-20, urlend-urlstart);
	copysize = urlend-urlstart;
	if (copysize > 200)
		copysize = 200;
// 	copysize = min(200, urlend-urlstart);

	memcpy(filename, dirname, dirContext->fileNameLength-1);			//	fileNameLength with \0

// 	if (modelContext->PProtocol->ProtocolNumber == PROTOCOL_DIRECTORY)
	if (modelContext->PProtocol->ProtocolNumber == PROTOCOL_DIRECTORY_READ)			// now, it is this
	{
		memcpy(filename+dirContext->fileNameLength-1, "??????????", FileSequenceLength);		// little large than FileSequenceLength
	}
	else if (modelContext->PProtocol->ProtocolNumber == PROTOCOL_INTER)				// for FileTranslate use, Jun 10 '13
	{
#ifdef FILETRANSLATE_APPLICATION
//	for log without proxy
// 		siteLength = 7+urlinfo->getLength[2];
// 		spacePlace = (char*)memchr(urlstart, ' ', 10);
// 		if (!spacePlace) break;
// 		spacePlace ++;
// 		spaceLength = spacePlace - urlstart;
// 		memcpy(filename+dirContext->fileNameLength-1+FileSequenceLength, urlstart, spaceLength);
// 		memcpy(filename+dirContext->fileNameLength-1+FileSequenceLength+spaceLength, "http --", 7);
// 		spointer = (unsigned char*)urlinfo->getSite;
// 		tpointer = (unsigned char*)(filename+dirContext->fileNameLength-1+FileSequenceLength+spaceLength+7);
// 		for (i=0; i<urlinfo->getLength[2]; i++) *tpointer++ = FileNameTran[*spointer++];

//	for log with proxy
#endif FILETRANSLATE_APPLICATION
	}
	else
	{
		nowSequence = InterInc(&FileSequence);
		Myitoa(nowSequence, filename+dirContext->fileNameLength-1, FileSequenceLength);
	}

	ret_err = 0x30;
	spointer = (unsigned char*)urlstart+spaceLength;
	tpointer = (unsigned char*)(filename+dirContext->fileNameLength-1+FileSequenceLength+spaceLength+siteLength);
	for (i=0; i<copysize-spaceLength; i++) *tpointer++ = FileNameTran[*spointer++];

	while (*(tpointer-1) > 128)
		tpointer--;			// add Jul 10 '13

	*((long*)tpointer) = ISTXT;
	*(tpointer+4) = 0;
	fileContext->fileNameLength = (tpointer-(unsigned char*)filename)+4;

// printf("%s\r\n", filename);

	__CATCH(MODULE_PROTOCOL, "GetURL2Filename")
}