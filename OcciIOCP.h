
#define SystemAddResource		theService.ServiceResources.AddResource
#define SystemGetResource		theService.ServiceResources.GetResource
#define SystemRemoveAllResource	theService.ServiceResources.RemoveAllResource

class CTelnetApplication : public CApplication
{
public:
	CTelnetApplication() : CApplication("\r\n", 2, NULL, 0) {};
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
};

class CSystemApplication : public CTelnetApplication
{
public:
	CTCPProtocol* pTCP;
// 	CTCPProtocol* pTCP2;
	CSingleUDPProtocol* pUDPSingle;
	CUDPProtocol* pUDP;
	CFileProtocol* pFile;
  	CFileReadProtocol* pFileRead;
	CDirectoryProtocol* pDir;
	CDirectoryReadProtocol* pDirRead;
// 	COciProtocol* pOci;

// 	CEchoApplication* pEcho;
// 	CForwardApplication* pForward;
	CForwardApplication* pForwardDup;
	CDNSApplication* pDNS;

	CTunnelProtocol* pTunnelPro;
	CTunnelApplication* pTunnel;

// #ifdef FORWARDLOOP_FUNCTION
// 	CForwardLoopApplication* pForwardLoop;
// #endif FORWARDLOOP_FUNCTION
	CWebApplication* pWeb;
//	CInterApplication* pSSL;

// #ifdef WITH_HTTPS
	CInterProtocol* pInter;
	CInterProtocol* pInterNest;
	CInterProtocol* pInterNest2;
	CInterProtocol* pInterSerNest2;
	CInterProtocol* pInterSerNest;
	CInterProtocol* pInterSer;
	CInterApplication* pInterApp;
	CInterApplication* pInterAppNest;
	CInterApplication* pInterAppNest2;
	CInterApplication* pInterAppSerNest2;
	CInterApplication* pInterAppSerNest;
	CInterApplication* pInterAppSer;
	CSSLServerApplication* pSSLSerMode;
	CSSLClientApplication* pSSLCliMode;

	CPassbyApplication* pPassby;
	CPrintApplication* pPrint;
// #endif WITH_HTTPS

// 	COracleApplication* pOracle;
//	CTelnetApplication* pTelnet;

#ifdef SIMSERVER_APPLICATION
	CSimServer* pSim;
#endif SIMSERVER_APPLICATION

#ifdef CONTENTPAD_APPLICATION
	CContentPadServer* pContentPad;
#endif CONTENTPAD_APPLICATION

#ifdef PROXY_FUNCTION
	CProxyApplication *pProxy;
#endif PROXY_FUNCTION

#ifdef PROXYSERVER_APPLICATION
	CProxyServerApplication *pProxySer;
#endif PROXYSERVER_APPLICATION

public:
	CSystemApplication() : CTelnetApplication() {};
	long	InitProcess(void);
	long	ExitProcess(void);
	long	OnAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);

private:
	long AnalysisCommand(CContextItem* mContext, CListItem* &mBuffer, long size);
};