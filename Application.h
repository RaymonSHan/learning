
#define HTTP200						200
#define HTTP201						201
#define HTTP206						206
#define HTTP302						302
#define HTTP404						404
#define HTTPC00						0xc00

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	SHOULD NOT change, for the order of HttpURLPara
#define MODE_GET							0x0000
#define MODE_POST							0x0001
#define MODE_CONNECT						0x0002
// #define MODE_HOST							0x0002
// #define MODE_CONNECTION						0x0003
// #define MODE_LENGTH							0x0004
// #define MODE_RANGE							0x0005
// #define MODE_COOKIE							0x0006

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	This is another important base class																							//
//	APPLICATION means how the DATA means Such as ECHO, FORWARD, HTTP																//
//	LIFECYCLE: new -> SetAction() -> RegisterApplication() -> delete																//
//	Create by CService::InitProcess() and CSystemApplication::InitProcess()															//
//	Destroy by CService::ExitProcess() and CSystemAplication::ExitProcess()															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CApplication																													//
{																																	//
public:																																//
	static CResources*	Resoucce;								//	pointer to the one and only CResources							//
	long				ApplicationNumber;						//	The number define as APPLICATION_xx								//
	long				ApplicationFlag;						//	The flag define as FLAG_xx										//
	CMemoryListUsed*	BufferInfo;								//	Where to get BufferInfo, from a MEMORYLIST						//
	char				CliCommandEnd[COMMAND_END_SIZE];		//	Client side commandEnd											//
	long				CliCommandEndSize;						//	Size of client side commandEnd									//
	char				SerCommandEnd[COMMAND_END_SIZE];		//	Server side commandEnd											//
	long				SerCommandEndSize;						//	Size of Server side commandEnd									//
	CContextItem*		FirstContext;							//	The CONTEXT for ACCEPT, used for Close APPLICATION				//
	CContextItem*		FirstServerContext;						//	for serContext with peer
	// 	CContextItem*		FirstServerContext;						//	for InterProtocol use
	// 	CInterApplication*	EncapApplication;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CApplication(char* clicomend, long cliendsize, char* sercomend, long serendsize)												//
	{																																//
		ApplicationNumber = 0;																										//
		ApplicationFlag = 0;																										//
		BufferInfo = NULL;																											//
		FirstContext = FirstServerContext = NULL;																										//
		if (cliendsize) memcpy_s(CliCommandEnd, COMMAND_END_SIZE, clicomend, cliendsize);											//
		CliCommandEndSize = cliendsize;																								//
		if (serendsize) memcpy_s(SerCommandEnd, COMMAND_END_SIZE, sercomend, serendsize);											//
		SerCommandEndSize = serendsize;																								//
	}																																//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BOOL SetAction(CResources* res, char* buffername)																				//
	{																																//
		Resoucce = res;																												//
		if (buffername) BufferInfo = Resoucce->GetResource(buffername);																//
		return 0;																													//
	}																																//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void ClearApplicationBuffer(CListItem* mBuffer)
	{
		mBuffer->BufferType = BufferInfo;																						//
		mBuffer->NOperation = 0;																								//
		mBuffer->NProcessSize = BufferInfo->BufferSize;																			//
		mBuffer->CliHandle = 0;
		mBuffer->OperationSide = 0;

		if (BufferInfo->BufferSize > SIZEOFCONTENT*4)		// used for contentpad only
		{
			mBuffer->HeadInfo = (ContentPad*)(((char*)(mBuffer+1)) + BufferInfo->BufferSize + PAGE_SIZE);
			memset(mBuffer->HeadInfo, 0, SIZEOFCONTENT);
		}
	}

	CListItem* GetApplicationBuffer()																						//
	{																																//
		CListItem* mBuffer = BufferInfo->GetContext();																				//
		if (mBuffer)																												//
		{																															//
			ClearApplicationBuffer(mBuffer);
		}																															//
#ifdef	DEBUG_BUFFER
		printf("Get :%08x,%s%4d\r\n", mBuffer, mBuffer->BufferType->ListName, mBuffer->BufferType->GetFreeNumber());
#endif
		return mBuffer;																												//
	};																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CListItem* FreeApplicationBuffer(CListItem* mBuffer)																			//
	{																																//
		if (!mBuffer) return NULL;

		CListItem* next;
		mBuffer->CliHandle = 0;									//	For CliBuffer use												//

		next = mBuffer->HeadInfo->nextContent;
		if (next)
		{
			mBuffer->HeadInfo->nextContent = 0;
			FreeApplicationBuffer(next);
		}

#ifdef	DEBUG_BUFFER
		printf("Free:%08x,%s%4d\r\n", mBuffer, mBuffer->BufferType->ListName, mBuffer->BufferType->GetFreeNumber());
#endif
		return mBuffer->BufferType->FreeContext(mBuffer);		//																	//
	}															//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	long	OnAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide) {return 0;};								//
	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
	{
		return NoneProFunc(mContext->PPeer->PProtocol, fPostReceive)																//
			(mContext->PPeer, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide);									//
	}
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide) {return 0;};							//
	long	OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
	{
		return NoneProFunc(mContext->PPeer->PProtocol, fPostReceive)																//
			(mContext->PPeer, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, opSide);
	}
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide) {return 0;};							//
	long	OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
	{
		return NoneProFunc(mContext->PPeer->PProtocol, fPostReceive)																//
			(mContext->PPeer, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide);
	}
	long	OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
	{
		if (mBuffer) return FreeApplicationBuffer(mBuffer) ? 0 : 1;
		else return 0;						//	Add for Concurrency	//	Apr. 30 '14
	}																																//
	long	OnPassby(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
	{
		return FreeApplicationBuffer(mBuffer) ? 0 : 1;
	}

	virtual long AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op) {return 0;};							//
	virtual CApplication* ReturnApplication(CContextItem* mContext, CListItem* &mBuffer, long size) {return this;};
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Of course, NoneApplication and CApplication can make into one																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class CNoneApplication : public CApplication																						//
{																																	//
public:																																//
	CNoneApplication() : CApplication(NULL, 0, NULL, 0) { };																		//
	~CNoneApplication()	{ }																											//
	long	OnAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);											//
	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);									//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);									//
	long	OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);											//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Yes, as do so, the real APPLICATION is quiet easy. There only one line for ECHO, and two line for FORWARD						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CEchoApplication : public CApplication																						//
{																																	//
public:																																//
	CEchoApplication(char* clicomend, long cliendsize) : CApplication(clicomend, cliendsize, NULL, 0) {};							//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPrintApplication : public CApplication																						//
{																																	//
public:																																//
	CPrintApplication(char* clicomend = NULL, long cliendsize = 0) : CApplication(clicomend, cliendsize, NULL, 0) {};				//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CForwardApplication : public CApplication																						//
{																																	//
public:																																//
	CForwardApplication(char* clicomend, long cliendsize, char* sercomend, long serendsize) :										//
		CApplication(clicomend, cliendsize, sercomend, serendsize) {};																	//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};																																	//

#define		MAX_IP_NUMBER		65536
#define		MAX_MAC_NUMBER		65536
#define		DNS_MAC_FLUSH_TIME	1800

typedef struct IPStateList
{
	ULONG	accessTime;
}IPStateList, *pIPStateList;

typedef struct MACStateList
{
	__int64	macAddress;
}MACStateList, *pMACStateList;

typedef struct IPStateTable
{
// 	long	ipTableNumber;
// 	long	rev;
	IPStateList	IPState[MAX_IP_NUMBER];
}IPStateTable, *pIPStateTable;

typedef struct MACStateTable
{
	long	macTableNumber;
	long	rev;
	MACStateList MACState[MAX_MAC_NUMBER];
}MACStateTable, *pMACStateTable;

long SetDNS (CListItem* mBuffer, DWORD addr);
long SetOneIPMac (ContentPad* resultPad, long keyFrom);

class CDNSApplication : public CApplication																						//
{																																	//
private:
	long	isForward;
	CListItem* DNSMACList;
	CListItem* DNSIPList;

	IPStateTable	*pIPTable;
	MACStateTable	*pMACTable;

public:																																//
	CDNSApplication() :	CApplication(NULL, 0, NULL, 0) {BuildList();};
	~CDNSApplication();
	long	BuildList();
	long	InMACList(__int64 mac);
	long	InIPList(IPAddr ipaddr);
	long	AddIPMACList(IPAddr ipaddr, __int64 mac);

	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};																																	//



typedef struct ProtocolId
{
	CContextItem*		peerContext;
	char				peerKeyword[NORMAL_CHAR];
	long				peerKeywordLen;
	char				peerHost[NORMAL_CHAR];							//	http request host name
	long				peerHostLen;
	long				peerPowerTotal;									//	Total power for one load share group
	long				peerPowerInit;									//	0 for not load share, less then 0 for invalid
	ProtocolId*			peerNextShare;									//	for little faster
}ProtocolId;

class CMultiApplication : public CApplication																						//
{																																	//
protected:
	long				PeerNumber;
	ProtocolId			ServerPeer[MAX_SERVER_PEER];
public:																																//
	CMultiApplication(char* clicomend, long cliendsize, char* sercomend, long serendsize)	
		: CApplication(clicomend, cliendsize, sercomend, serendsize) 
	{ 
		PeerNumber = 0;
		ZeroMemory(ServerPeer, sizeof(ServerPeer));
	};											//
	~CMultiApplication() { };																										//

	CContextItem*	AddProtocolPeer(CProtocol *pProSer, void* paraSer, long sizeSer, char* keySer, char* host=NULL, long share=0);
	long	FreeServerContext();
	CContextItem* PreparePeer(CContextItem* mContext, CListItem* mBuffer, bool isCopy=TRUE);		// create the really peer
	CContextItem* PreparePeer(CContextItem* mContext, char* keySer);
};																																	//

long AnalysisHttpURL(CContextItem* mContext, CListItem* mBuffer, long size);
long AnalysisHttpRespond(CContextItem* mContext, CListItem* mBuffer, long size);

long AnalysisProxyURL(CContextItem* mContext, CListItem* &mBuffer, long size);
long AnalysisProxyRespone(CContextItem* mContext, CListItem* &mBuffer, long size);

class CWebApplication : public CMultiApplication																					//
{																																	//
protected:
public:																																//
	CWebApplication() : CMultiApplication ("\r\n\r\n", 4, NULL, 0) {};																//
	~CWebApplication() { };																											//

	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op);	// now, for web only

	long	PrepareHttpRelay(CContextItem* mContext, CListItem* mBuffer, __int64 fileLength);
};																																	//


class CProxyApplication : public CApplication
{
public:
	CProxyApplication() : CApplication("\r\n\r\n", 4, "\r\n\r\n", 4) {};															//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//

	long	AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op);
};

class CProxyServerApplication : public CApplication
{
public:
	CProxyServerApplication() : CApplication("\r\n\r\n", 4, NULL, 0) {};															//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//

	long	AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Associate the PROTOCOL and APPLICATION & Close APPLICATION																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CContextItem* CreateApplication( CApplication *pApp, CProtocol *pProCli, void* paraCli, long sizeCli,								//
								CProtocol *pProSer, void* paraSer, long sizeSer, long acceptNumber = 1, char * keySer = NULL );															//
long CloseApplication( CApplication *pApp );																						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Process the PACKET to COMMAND																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CListItem* ProcessCommand( CContextItem* mContext, CListItem* mBuffer, long size, char* comEnd, long endSize );					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Some useful function, declare here																								//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long SendToPeer(CContextItem* mContext, CListItem* &mBuffer, long size, long op);													//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class CSimServer : public CWebApplication
{
protected:
	// 	char	DefaultSite[MAX_PATH];
public:																																//
	CSimServer() : CWebApplication () {};																							//
	~CSimServer() {};																												//

	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};
