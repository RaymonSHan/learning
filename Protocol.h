

#define		FileSequenceLength		5
#define		TimeSequenceLength		15
#define		ISTXT					('.'+('t'<<8)+('x'<<16)+('t'<<24))

#define		ISGET					('G'+('E'<<8)+('T'<<16)+(' '<<24))
#define		ISPOST					('P'+('O'<<8)+('S'<<16)+('T'<<24))
#define		ISCONNECT				('C'+('O'<<8)+('N'<<16)+('N'<<24))
#define		ISCRCR					(0xd+(0xa<<8)+(0xd<<16)+(0xa<<24))

#define		ISBR					('b'+('r'<<8))
#define		ISSPACE					('n'+('b'<<8)+('s'<<16)+('p'<<24))
#define		ISSCRIPTBEGIN			('s'+('c'<<8)+('r'<<16)+('i'<<24))
#define		ISSCRIPTEND				('/'+('s'<<8)+('c'<<16)+('r'<<24))
#define		DISPLAYCR				('\\'+('r'<<8)+('\\'<<16)+('n'<<24))


#define		ISHost					('H'+('o'<<8)+('s'<<16)+('t'<<24))
#define		ISConnection			('C'+('o'<<8)+('n'<<16)+('n'<<24))
#define		ISProxy					('P'+('r'<<8)+('o'<<16)+('x'<<24))
#define		ISVia					('V'+('i'<<8)+('a'<<16)+(':'<<24))

#define		ISHTTP					('h'+('t'<<8)+('t'<<16)+('p'<<24))
#define		ISDIV					(':'+('/'<<8)+('/'<<16))



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Copy the PROTOCOL number function to PROTOCOL function array																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
TemplateBase long RegisterProtocol(_BASE* pProtocol, long nProtocol, long flag)													//
{																																	//
	pProtocol->ProtocolNumber = nProtocol;																							//
	pProtocol->ProtocolFlag = flag;																									//
	CIOCP::fProtocolFunction[nProtocol].fCreateNew = (ProtocolNew)&_BASE::CreateNew;												//
	CIOCP::fProtocolFunction[nProtocol].fCreateRemote = (ProtocolNew)&_BASE::CreateRemote;											//
	CIOCP::fProtocolFunction[nProtocol].fPostAccept = (ProtocolHandle)&_BASE::PostAccept;											//
	CIOCP::fProtocolFunction[nProtocol].fPostConnect = (ProtocolHandle)&_BASE::PostConnect;											//
	CIOCP::fProtocolFunction[nProtocol].fPostSend = (ProtocolAction)&_BASE::PostSend;												//
	CIOCP::fProtocolFunction[nProtocol].fPostReceive = (ProtocolAction)&_BASE::PostReceive;											//
	CIOCP::fProtocolFunction[nProtocol].fPostClose = (ProtocolHandle)&_BASE::PostClose;												//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Copy the APPLICATION number function to APPLICATION function array																//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
TemplateBase long RegisterApplication(_BASE* pApplication, long nApplication, long flag)											//
{																																	//
	pApplication->ApplicationNumber = nApplication;																					//
	pApplication->ApplicationFlag = flag;																							//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnAccept] = (ApplicationHandle)&_BASE::OnAccept;							//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnConnect] = (ApplicationHandle)&_BASE::OnConnect;						//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnClientRead] = (ApplicationHandle)&_BASE::OnClientRead;					//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnClientWrite] = (ApplicationHandle)&_BASE::OnClientWrite;				//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnServerRead] = (ApplicationHandle)&_BASE::OnServerRead;					//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnServerWrite] = (ApplicationHandle)&_BASE::OnServerWrite;				//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnClose] = (ApplicationHandle)&_BASE::OnClose;							//
	CIOCP::fApplicationFunction[nApplication].fOnFunction[fOnPassby] = (ApplicationHandle)&_BASE::OnPassby;
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IOCP is the most complex module in Windows, typical it is used in SOCKET only, assorted with TCP SERVER.						//
//	But now, many function has migrate to CProtocol and CApplication, IOCP only do WorkThread to GetQueuedCompletionStatus,			//
//	then call APPLICATION function array.																							//
//	This is NOT a good tutorial for IOCP, this is a high efficient and high compatible platform.									//
//	LIFECYCLE: declare -> InitProcess(threadNumber) -> ExitProcess() -> exit														//
//	Create by CResources::ResourceArray[MAX_RESOURCE] declare																		//
//	Destroy by CResources exit																										//
//	IOCP will terminate will the thread it created, before it exit																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CIOCP : public CListItem																										//
{																																	//
public:																																//
	HANDLE				mIOCP;									//	The IOCP HANDLE													//
	MYINT				mWorkThreadNumber;						//	The Number of thread attach to IOCP, it can changed dynamics	//
	HANDLE				mWorkThread[MAX_THREAD];				//	The thread are here, the MAX is 64, and NO any more				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	These Function pointer is used for xxEx in IOCP																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	static LPFN_ACCEPTEX		m_lpfnAcceptEx;																						//
	static LPFN_CONNECTEX		m_lpfnConnectEx;																					//
	static LPFN_DISCONNECTEX	m_lpfnDisconnectEx;																					//
	static LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptExSockaddrs;																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PROTOCOL and APPLICATION virtual function array, IOCP call these function depend on PROTOCOL and NOperation.					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	static ProtocolHandles		fProtocolFunction[MAX_PROTOCOL];																	//
	static ApplicationHandles	fApplicationFunction[MAX_APPLICATION];																//
																																	//
	static sockaddr_in tempaddr;								// this is for bind client socket									//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:																																//
	CIOCP() : CListItem() {}																										//
	BOOL InitProcess(long threadNumber);																							//
//	BOOL MainProcess(void);																											//
	BOOL ExitProcess(void);																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	To use 'this', so have these two function																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	static unsigned int __stdcall WorkThread(LPVOID Param)																			//
	{																																//
		CIOCP *pthis = (CIOCP*)Param;																								//
		return pthis->WorkThread();																									//
	}																																//
	unsigned int WorkThread(void);																									//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Set the TIMEOUT count to init																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
void inline ReflushTimeout(CContextItem* mContext, long timeout = 0)																//
{																																	//
	if (!timeout)
	{
		timeout = mContext->ContextType->TimeoutInit;																				//
																																	//
		if ( (mContext->countDown < timeout) && (mContext->countDown > TIMEOUT_QUIT) )												//
			mContext->countDown = timeout;																							//
	}
//	Now add this line for TIMEOUT_TCPCONNECT. it should set to a small value, and set back to TimeoutInit after received	//	Apr. 28 '14
	else mContext->countDown = timeout;																								//
	return;																															//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	One of the MOST important base CLASS, another is CApplication																	//
//	PROTOCOL means how the DATA in and out, Such as TCP, UDP, FILE, DATABASE														//
//	LIFECYCLE: new -> SetAction() -> RegisterProtocol() -> delete																	//
//	Create by CService::InitProcess() and CSystemApplication::InitProcess()															//
//	Destroy by CService::ExitProcess() and CSystemAplication::ExitProcess()															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CProtocol																														//
{																																	//
public:																																//
	static CResources*	Resoucce;								//	pointer to the one and only CResources							//
	long				ProtocolNumber;							//	The number define as PROTOCOL_xx								//
	long				ProtocolFlag;							//	The flag define as FLAG_xx										//
	CIOCP*				ProtocolIOCP;							//	every instance of PROTOCOL must attach to a IOCP				//
	CMemoryListUsed*	ContextInfo;							//	Where to get ContextInfo, from a  MEMORYLIST					//
	int					CompareParaStart;						//	The start offset of the Keyword in the item for the PROTOCOL	//
	int					CompareParaLength;						//	The length of the Keyword in the item for the PROTOCOL			//
	BOOL				(*MyCloseHandle)(HANDLE);				//	for socket use closesocket, for other use CloseHandle			//
// 	CInterProtocol*		EncapProtocol;							//
																//////////////////////////////////////////////////////////////////////
	CProtocol()																														//
	{																																//
		ProtocolNumber = 0;																											//
		ProtocolFlag = 0;																											//
		ProtocolIOCP = NULL;																										//
		ContextInfo = NULL;																											//
		CompareParaStart = 0;																										//
		CompareParaLength = 0;																										//
// 		EncapProtocol = NULL;
		MyCloseHandle = CloseOtherHandle;																												//
	}																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Get the IOCP and MEMORYLIST pointer by name																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	BOOL SetAction(CResources* res, char* iocpname, char* contextname)																//
	{																																//
		Resoucce = res;																												//
		if (iocpname) ProtocolIOCP = (CIOCP*)Resoucce->GetResource(iocpname)->GetOnlyOneList();										//
		else ProtocolIOCP = NULL;
		if (contextname) ContextInfo = Resoucce->GetResource(contextname);															//
		return 0;																													//
	}																																//
	inline void ZeroProtocolContext(CContextItem* mContext)
	{
		mContext->PProtocol = this;																								//
		mContext->ContextType = ContextInfo;																					//
		mContext->OverlapOffset	= 0;																							//
		mContext->BodyRemain = 0;
		mContext->MoreBuffer = NULL;																							//
		mContext->CliBuffer = NULL;							
		mContext->FirstContent = NULL;
// 			mContext->BufferOffset = 0;																								//
		mContext->ListFlag = FLAG_IS_CONTEXT | ProtocolFlag;	//		testuse should add ???
		mContext->EncapContext = 0;
		mContext->DecapContext = 0;
		mContext->LogFileContext = 0;
		mContext->ContentMode = CONTENT_MODE_HEAD;
		mContext->ServerName[0] = 0;
		mContext->PeerProtocol = NULL;
		mContext->OldPeer = NULL;

		memset(mContext+1, 0, mContext->ContextType->BufferSize);	//	should do ?
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Get one CONTEXT from MEMORYLIST and init																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	inline CContextItem* GetProtocolContext()																						//
	{																																//
		CContextItem* mContext = (CContextItem*)ContextInfo->GetContext();															//
		if (mContext)																												//
		{																															//
			ZeroProtocolContext(mContext);
#ifdef	DEBUG_CONTEXT
			printf("Get :%08x,%s,%4d\r\n", mContext, mContext->ContextType->ListName, mContext->ContextType->GetFreeNumber());
#endif	DEBUG_CONTEXT
		}																															//
		return mContext;																											//
	};																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Get one CONTEXT from MEMORYLIST and init value with given item																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	inline CContextItem* GetDuplicateContext(CContextItem* oldContext)																//
	{																																//
		CContextItem* mContext = (CContextItem*)ContextInfo->GetContext();															//
		if (mContext)																												//
		{																															//
			mContext->GU.CS = oldContext->GU.CS;																					//
			memcpy(mContext+1, oldContext+1, mContext->ContextType->BufferSize);													//
			mContext->ListFlag = oldContext->ListFlag;																				//
#ifdef	DEBUG_CONTEXT
			printf("GetDup :%08x,%s,%4d\r\n", mContext, mContext->ContextType->ListName, mContext->ContextType->GetFreeNumber());
#endif	DEBUG_CONTEXT
		}																															//
		return mContext;																											//
	};																																//

	inline CContextItem* DuplicateContextAddition(CContextItem* mContext, CContextItem* oldContext)									//
	{																																//
		memcpy(mContext+1, oldContext+1, mContext->ContextType->BufferSize);														//
		return mContext;																											//
	};																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Free the CONTEXT back to MEMORYLIST, and reInit the value, and determined whether close the CONNECT								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	CContextItem* ClearProtocolContext(CContextItem* mContext, BOOL forceclose = TRUE, BOOL clearcontext = FALSE);
	CContextItem* FreeProtocolContext(CContextItem* mContext, BOOL forceclose = TRUE)												//
	{
		if (!mContext) return 0;
		ClearProtocolContext(mContext, forceclose);
#ifdef	DEBUG_CONTEXT
		printf("Free :%08x,%s,%4d\r\n", mContext, mContext->ContextType->ListName, mContext->ContextType->GetFreeNumber());
#endif	DEBUG_CONTEXT
		return (CContextItem*)mContext->ContextType->FreeContext(mContext);															//

	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	These function just like Visual function																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
	long	CreateNew(CContextItem* mContext, void* para, long size) {return NULL;};												//
	long	CreateRemote(CContextItem* mContext, void* para, long size) {return NULL;};											//
//	CContextItem* CreateConnect(CProtocol* pProtocol, void* para, long size) {return NULL;};										//
	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op) {return 0;};								//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op) {return 0;};								//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide) {return 0;};					//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide) {return 0;};					//
	long	PostClose(CContextItem* mContext, CListItem* &mBuffer, long size, long op)												//
		{return FreeProtocolContext(mContext, !(mContext->ListFlag & FLAG_LATER_CLOSE)) ? 0 : 1;};									//

	__int64 virtual	GetContextLength(CContextItem* mContext, CListItem* mBuffer) { return 0; };
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Of course, CNoneProtocol and CProtocol can make into one. The same as CNoneApplication and CApplication							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CNoneProtocol : public CProtocol																								//
{																																	//
public:																																//
	CNoneProtocol() : CProtocol() {};																								//
	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CIPProtocol : public CProtocol																								//
{																																	//
protected:																															//
	long BindLocalSocket(CContextItem* mContext, CProtocol* pProtocol, sockaddr_in* addr);											//

	long BindLocalSocket(CContextItem* mContext, CProtocol* pProtocol, sockaddr_in* addr, int* isop);											//

};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CTCPProtocol : public CIPProtocol																								//
{																																	//
public:
	CListItem			CliListStart;
	volatile MYINT		CliListInProcess;
public:																																//
	CTCPProtocol() : CIPProtocol()																									//
	{																																//
		MyCloseHandle = CloseSocket;																								//
		CliListInProcess = MARK_NOT_IN_PROCESS;
	}																																//
	long	CreateNew(CContextItem* mContext, void* para, long size);																//
	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};																																	//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CUDPProtocol : public CIPProtocol																								//
{																																	//
public:																																//
	CUDPProtocol() : CIPProtocol()																									//
	{																																//
		CUDPContext tryContext;																										//
		MyCloseHandle = CloseSocket;																								//
		CompareParaStart = (char*)(&(tryContext.addrServer))-(char*)(&tryContext);													//
		CompareParaLength = 8;									//	short sin_family + u_short sin_port + struct in_addr sin_addr	//
	};															//////////////////////////////////////////////////////////////////////
	long	CreateNew(CContextItem* mContext, void* para, long size);																//
	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSingleUDPProtocol : public CIPProtocol																								//
{																																	//
public:																																//
	CSingleUDPProtocol() : CIPProtocol()																									//
	{																																//
		MyCloseHandle = CloseSocket;																								//
	};															//////////////////////////////////////////////////////////////////////
	long	CreateNew(CContextItem* mContext, void* para, long size);																//
	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WINPCAP_FUNCTION
struct	pcap_pkthdr;
struct	pcap_if;
struct	pcap;

void WinPCapHandle(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

class CWinPCapProtocol : public CProtocol																								//
{																																	//
public:																																//
	volatile MYINT		WinPCapInit;
	pcap_if *AllDevices;

	CWinPCapProtocol() : CProtocol()																									//
	{																																//
		WinPCapInit = MARK_NOT_IN_PROCESS;
		MyCloseHandle = CloseNullHandle;
	};															//////////////////////////////////////////////////////////////////////
	long	InitWinPCap();
	long	FreeWinPCap();
	long	CreateNew(CContextItem* mContext, void* para, long size);																//
// 	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
// 	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
// 	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
// 	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
// 	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};																																	//
#endif WINPCAP_FUNCTION

// class CTunnelProtocol : public CIPProtocol																							//
// {																																	//
// public:
// 	CListItem			CliListStart;
// 	volatile MYINT		CliListInProcess;
// public:																																//
// 	CTCPProtocol() : CIPProtocol()																									//
// 	{																																//
// 		MyCloseHandle = CloseSocket;																								//
// 		CliListInProcess = MARK_NOT_IN_PROCESS;
// 	}																																//
// 	long	CreateNew(CContextItem* mContext, void* para, long size);																//
// 	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
// 	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
// 	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
// 	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
// 	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
// };																																	//



__int64 GetFileContextLength(CContextItem* mContext);

class CFileProtocol : public CProtocol																								//
{																																	//
public:																																//
	CFileProtocol() : CProtocol() {};																								//
	long	CreateNew(CContextItem* mContext, void* para, long size);																//
	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
	long	PostAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//

	__int64 virtual	GetContextLength(CContextItem* mContext, CListItem* mBuffer);
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFileReadProtocol : public CProtocol																							//
{																																	//
public:																																//
	CFileReadProtocol() : CProtocol() {};																							//
	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//

	__int64 virtual	GetContextLength(CContextItem* mContext, CListItem* mBuffer);
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CDirectoryProtocol : public CFileProtocol																						//
{																																	//
private:
	ProcessHandle GetFilename;
public:																																//
	CDirectoryProtocol(ProcessHandle phandle) : CFileProtocol() {GetFilename = phandle;};											//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CDirectoryReadProtocol : public CFileReadProtocol																				//
{																																	//
private:
	ProcessHandle GetFilename;
public:																																//
	CDirectoryReadProtocol(ProcessHandle phandle) : CFileReadProtocol() {GetFilename = phandle;};									//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);											//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CListItem* ProcessClientCommand( CContextItem* mContext, CListItem* &mBuffer, long size );											//
CListItem* ProcessServerCommand( CContextItem* mContext, CListItem* &mBuffer, long size );	

long GetURL2Filename(CContextItem* mContext, CContextItem* modelContext, CListItem* &mBuffer, long size);
long GetSequence2Filename(CContextItem* mContext, CContextItem* modelContext, CListItem* &mBuffer, long size);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      