
#include "stdafx.h"
#include "SSLApplication.h"

extern CService theService;
extern CListItem*				isNULL;

extern unsigned char ASCII_DEC[ASCII_NUMBER];
extern unsigned char ASCII_HEX[ASCII_NUMBER];

extern CMemoryListUsed*	TempUseMemory;							//	for program memory use, avoid use malloc				//

extern char MAIN404PAGE[MAX_PATH];

ContentPadConfig* configInfo;

#ifdef	FORWARDLOOP_FUNCTION
	ForwardLoopConfig* pForwardLoopConfig;
#endif	FORWARDLOOP_FUNCTION

//long	SYSTEM_BUFFER_SIZE	= 3;		//	only used for fileBuffer & configBuffer(for ContentPad), 
long	SYSTEM_BUFFER_SIZE	= 4;		//	sim-CLIENTpad is add	// May 17 '14

long CSystemApplication::InitProcess(void)
{
	CIOCP* classIOCP;
// 	CContextItem* retContext;
	long ret_err = 0x01;
	CListItem* configBuffer;

	long ret;
// 	int i;
// 	ContentForward* fInfo;
// 	sockaddr_in nowaddr;

	char exename[MAX_PATH];
	long exenamelength;
	HANDLE handle;
	__int64 configfilesize;
	CListItem* fileBuffer, *cliBuffer = NULL;
	DWORD fileread;
	char* filestart;
	char* referstart, *referend;					//	for wafarefer info for  startup					//	May 08 '14
	char* tpointer = 0;
	CHTTPContext usedContext;

	__TRY

	ret_err = 0x10;
	//	move to CService
#ifdef	FORWARDLOOP_FUNCTION
	CListItem* forwardBuffer;

	SYSTEM_BUFFER_SIZE += 1;				// one for forwardloopconfig, 
#endif	FORWARDLOOP_FUNCTION

#ifdef	CONFIRMDNS_FUNCTION
	SYSTEM_BUFFER_SIZE += 2;				//	for IPTable & MACTable in DNS
#endif	CONFIRMDNS_FUNCTION

	if (!SystemAddResource<CLaregBuffer>("TEMPUSE", SYSTEM_BUFFER_SIZE, PAGE_SIZE, TRUE, 0)) break;
	TempUseMemory = Resoucce->GetResource("TEMPUSE");															//
	memset(&(usedContext.GU), 0, sizeof(usedContext.GU));
// 	ZeroProtocolContext(&usedContext);

	ret_err = 0x20;
	fileBuffer = TempUseMemory->GetContext();
	if (!fileBuffer) break;

	ret_err = 0x30;
	if (TempUseMemory->BufferSize < sizeof(ContentPadConfig)+8) break;
	configBuffer = TempUseMemory->GetContext();
	if (!configBuffer) break;
	configInfo = (ContentPadConfig*)(configBuffer+1);

#ifdef	FORWARDLOOP_FUNCTION
	ret_err = 0x35;
	if (TempUseMemory->BufferSize < sizeof(ForwardLoopConfig)+8) break;
	forwardBuffer = TempUseMemory->GetContext();
	if (!forwardBuffer) break;
	pForwardLoopConfig = (ForwardLoopConfig*)(forwardBuffer+1);
#endif	FORWARDLOOP_FUNCTION

	ret_err = 0x40;
	GetModuleFileName(NULL, exename, MAX_PATH);
	exenamelength = strlen(exename);
	exename[exenamelength-3] = 'j';
	exename[exenamelength-2] = 's';
	exename[exenamelength-1] = 'o';
	exename[exenamelength] = 'n';
	exename[exenamelength+1] = 0;
	handle = CreateFile(exename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);																			//
	if (handle == INVALID_HANDLE_VALUE)	 
		DEBUG_MESSAGE_FILE_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2);

	ret_err = 0x50;
	if (!GetFileSizeEx(handle, (PLARGE_INTEGER)&configfilesize ) ) break;
	if (TempUseMemory->BufferSize < (long)configfilesize) break;

	ret_err = 0x60;
	ret = ReadFile(handle, REAL_BUFFER(fileBuffer), TempUseMemory->BufferSize, &fileread, NULL);										//
	if (!ret) DEBUG_MESSAGE_FILE_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L3);				//
	CloseHandle(handle);

	ret_err = 0x70;
	filestart = REAL_BUFFER(fileBuffer);
	fileBuffer->HeadInfo = (ContentPad*)(filestart + TempUseMemory->BufferSize);
	memset(fileBuffer->HeadInfo, 0, SIZEOFCONTENT);

	usedContext.FirstContent = 0;
	AddtoContentList(&usedContext, fileBuffer, CONTENT_NAME_FILE);
	fileBuffer->NProcessSize = (long)configfilesize;
//	ProcessWafaPage(&usedContext, fileBuffer, tpointer, filestart, filestart+(long)configfilesize, ProcessConfigVars);

	referstart = memstr(filestart, (long)configfilesize, NASZ(WAFA_BODY_START));
	if (!referstart) break;
	referend = memstr(referstart, filestart+(long)configfilesize-referstart, NASZ(WAFA_BODY_END));
	if (!referend) break;
	ProcessWafaPage(&usedContext, fileBuffer, tpointer, referstart, referend+sizeof(WAFA_BODY_END), ProcessConfigVars);

	ret_err = 0x80;
	if (!SystemAddResource<CNormalBuffer>("BUFFER", configInfo->ContentMemoryBuffer, PAGE_SIZE, TRUE, 0)) break;
	if (!SystemAddResource<CHTTPContext>("HTTP", configInfo->ContentMemoryHttp, CACHE_SIZE, FALSE, TIMEOUT_TCP)) break;
	if (!SystemAddResource<CFileContext>("FILE", configInfo->ContentMemoryFile, CACHE_SIZE, FALSE, TIMEOUT_FILE)) break;

	ret_err = 0x90;
	if (!SystemAddResource<CIOCP>("IOCP", 1, sizeof(INT), TRUE, 0)) break;
	classIOCP = (CIOCP*)SystemGetResource("IOCP")->GetOnlyOneList();
	if ( classIOCP->InitProcess(configInfo->ContentThread) ) break;			// should add return control

	ret_err = 0xa0;
	pTCP = new CTCPProtocol();
	pTCP->SetAction(Resoucce, "IOCP", "HTTP");
	RegisterProtocol(pTCP, PROTOCOL_TCP, 0);

	pFile = new CFileProtocol();
	pFile->SetAction(Resoucce, "IOCP", "FILE");
	RegisterProtocol(pFile, PROTOCOL_FILE, FLAG_ADD_OVERLAP);

	pFileRead = new CFileReadProtocol();
	pFileRead->SetAction(Resoucce, "IOCP", "FILE");
	RegisterProtocol(pFileRead, PROTOCOL_FILE_READ, FLAG_ADD_OVERLAP);

#define IOCPUDP "IOCP"
	pUDPSingle = new CSingleUDPProtocol();
	pUDPSingle->SetAction(Resoucce, IOCPUDP, "HTTP");
	RegisterProtocol(pUDPSingle, PROTOCOL_SINGLEUDP, FLAG_SINGLE);

	pUDP = new CUDPProtocol();
	pUDP->SetAction(Resoucce, IOCPUDP, "HTTP");
	RegisterProtocol(pUDP, PROTOCOL_UDP, FLAG_MULTI_INPUT);

	pInter = new CInterProtocol();
	pInter->SetAction(Resoucce, NULL, "HTTP");
	RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 

	pSSLSerMode = new CSSLServerApplication();
	pSSLSerMode->SetAction(Resoucce, "BUFFER");
	RegisterApplication(pSSLSerMode, APPLICATION_SSLSERVER, FLAG_PROCESS_ACCEPT );

#ifdef	CONTENTPAD_APPLICATION
	pContentPad = new CContentPadServer();
	pContentPad->SetAction(Resoucce, "BUFFER");
	RegisterApplication(pContentPad, APPLICATION_CONTENTPAD, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_MULTI_PROTOCOL );
	if (pContentPad->BufferInfo->BufferSize < 4*SIZEOFCONTENT) break;
#endif	CONTENTPAD_APPLICATION

#ifdef	CONFIRMDNS_FUNCTION
	pDNS = new CDNSApplication();
	pDNS->SetAction(Resoucce, "BUFFER");
	RegisterApplication(pDNS, APPLICATION_DNS, FLAG_DUPLEX);
#endif	CONFIRMDNS_FUNCTION


#ifdef	PROXY_FUNCTION
	pDir = new CDirectoryProtocol(GetSequence2Filename);
	pDir->SetAction(Resoucce, "IOCP", "HTTP");
	RegisterProtocol(pDir, PROTOCOL_DIRECTORY, FLAG_ADD_OVERLAP | FLAG_PROCESS_FILE | FLAG_PASSBY);

	pPassby = new CPassbyApplication();
	pPassby->SetAction(Resoucce, "BUFFER");
	RegisterApplication(pPassby, APPLICATION_PASSBY, FLAG_PROCESS_ACCEPT | FLAG_LATER_CONNECT);

	pProxy = new CProxyApplication();
	pProxy->SetAction(Resoucce, "BUFFER");
	RegisterApplication(pProxy, APPLICATION_PROXY, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_REMOVE_HTTPHEAD);
#endif	PROXY_FUNCTION

	ret_err = 0xd0;
	char* createstart, * createend;					//	for wafadefine info for create application		//	May 08 '14

	createstart = memstr(filestart, long(configfilesize), NASZ(WAFA_CREATE_START));
	if (createstart)
	{
		ContentPad* pad;
		char *wafastart, *wafaend;
		createend = memstr(createstart+sizeof(WAFA_CREATE_START), filestart+long(configfilesize)-createstart, NASZ(WAFA_CREATE_END));
		if (!createend) break;
		cliBuffer = TempUseMemory->GetContext();
		if (!cliBuffer) break;
		cliBuffer->HeadInfo = (ContentPad*)(REAL_BUFFER(cliBuffer) + TempUseMemory->BufferSize);
		memset(cliBuffer->HeadInfo, 0, SIZEOFCONTENT);
		usedContext.PApplication = pContentPad;
		usedContext.PProtocol = pTCP;

		AddtoContentList(&usedContext, cliBuffer, CONTENT_NAME_CLIENT);
		pad = cliBuffer->HeadInfo;
		pad->checkStart = createstart+sizeof(WAFA_CREATE_START);
		pad->checkEnd = createend;
		fileBuffer->HeadInfo->inConcurrency = CONCURRENCY_PAUSE;
		cliBuffer->HeadInfo->inConcurrency = 0;						//	following the concurrency order
		usedContext.PPeer = 0;
		do 
		{
			PrepareDefine(&usedContext, cliBuffer, pad, 0, wafastart, wafaend);
		} while (wafastart && wafaend);
	}
	if (fileBuffer) TempUseMemory->FreeContext(fileBuffer);

	__CATCH(MODULE_APPLICATION, "CSystemApplication - InitProcess")
}


#ifdef TEST_APPLICATION

long CSystemApplication::InitProcess(void)
{
	CIOCP* classIOCP;
	CContextItem* retContext = 0;
	CContextItem* retContextNest = 0;
	CContextItem* retContextNest2 = 0;
	CContextItem* retContextSerNest2 = 0;
	CContextItem* retContextSerNest = 0;
	CContextItem* retContextSer = 0;
	CContextItem* retContextFor = 0;

	while (TRUE)
	{
		sockaddr_in addrHttp;
		addrHttp.sin_family = AF_INET;    
		addrHttp.sin_port = htons(80);    
		addrHttp.sin_addr.S_un.S_addr = inet_addr("192.168.23.128");

		sockaddr_in addrHttps;
		addrHttps.sin_family = AF_INET;    
		addrHttps.sin_port = htons(443);    
		addrHttps.sin_addr.S_un.S_addr = inet_addr("192.168.23.128");

		sockaddr_in addrListen;
		addrListen.sin_family = AF_INET;    
		addrListen.sin_port = htons(1166);    
		addrListen.sin_addr.S_un.S_addr = inet_addr("192.168.23.128");

		sockaddr_in addrHttpPassby;
		addrHttpPassby.sin_family = AF_INET;    
		addrHttpPassby.sin_port = htons(1177);    
		addrHttpPassby.sin_addr.S_un.S_addr = inet_addr("192.168.23.128");

		char logFile[] = "c:\\log\\";

		if (!SystemAddResource<CNormalBuffer>("TEMPUSE", 10, PAGE_SIZE, TRUE, 0)) break;
		TempUseMemory = Resoucce->GetResource("TEMPUSE");															//
		if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 200, PAGE_SIZE, TRUE, 0)) break;
		if (!SystemAddResource<CHTTPContext>("HTTP", 300, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;
		if (!SystemAddResource<CIOCP>("IOCP", 1, sizeof(INT), TRUE, 0)) break;
		classIOCP = (CIOCP*)SystemGetResource("IOCP")->GetOnlyOneList();
		classIOCP->InitProcess(1);			// should add return control

		pTCP = new CTCPProtocol();
		pTCP->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pTCP, PROTOCOL_TCP, 0);

		pFile = new CFileProtocol();
		pFile->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pFile, PROTOCOL_FILE, FLAG_ADD_OVERLAP | FLAG_PROCESS_FILE | FLAG_PASSBY);

		pDir = new CDirectoryProtocol(GetURL2Filename);
		pDir->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pDir, PROTOCOL_DIRECTORY, FLAG_ADD_OVERLAP | FLAG_PROCESS_FILE | FLAG_PASSBY);

		pInter = new CInterProtocol();
		pInter->SetAction(Resoucce, NULL, "HTTP");
		RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 

		pInterNest = new CInterProtocol();
		pInterNest->SetAction(Resoucce, NULL, "HTTP");
		RegisterProtocol(pInterNest, PROTOCOL_INTER, 0);		// set NULL in SetAction, 

		pSSLSerMode = new CSSLServerApplication();
		pSSLSerMode->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pSSLSerMode, APPLICATION_SSLSERVER, FLAG_PROCESS_ACCEPT);

		pPassby = new CPassbyApplication();
		pPassby->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pPassby, APPLICATION_PASSBY, FLAG_PROCESS_ACCEPT | FLAG_LATER_CONNECT);

		pForwardDup = new CForwardApplication(NULL, 0, NULL, 0);
		pForwardDup->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pForwardDup, APPLICATION_FORWARD, FLAG_DUPLEX);

		retContext = CreateApplication(pSSLSerMode, pTCP, (void*)&addrListen, sizeof(sockaddr_in), NULL, 0, 0);
		retContextNest = CreateApplication(pPassby, pInterNest, retContext, sizeof(CContextItem*), pDir/*FLAG_PASSBY*/, (void*)logFile, sizeof(logFile));
		retContextFor = CreateApplication(pForwardDup, pInter, retContextNest, sizeof(CContextItem*), pTCP, (void*)&addrHttp, sizeof(sockaddr_in));

//	Tested OK, for two
//		char logFile[] = "c:\\log\\test.txt";
// 		retContext = CreateApplication(pPassby, pTCP, (void*)&addrListen, sizeof(sockaddr_in), pFile, (void*)logFile, sizeof(logFile));
// 		retContext = CreateApplication(pPassby, pTCP, (void*)&addrListen, sizeof(sockaddr_in), pTCP, (void*)&addrHttpPassby, sizeof(sockaddr_in));
//		retContextFor = CreateApplication(pForwardDup, pInter, retContext, sizeof(CContextItem*), pTCP, (void*)&addrHttp, sizeof(sockaddr_in));

// 		pPrint = new CPrintApplication();
// 		pPrint->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pPrint, APPLICATION_PRINT, 0);

// 		pInterNest = new CInterProtocol();
// 		pInterNest->SetAction(Resoucce, NULL, "HTTP");
// 		RegisterProtocol(pInterNest, PROTOCOL_INTER, 0);		// set NULL in SetAction, 
// 
// 		pInterNest2 = new CInterProtocol();
// 		pInterNest2->SetAction(Resoucce, NULL, "HTTP");
// 		RegisterProtocol(pInterNest2, PROTOCOL_INTER, 0);		// set NULL in SetAction, 
// 
// 		pInterSerNest2 = new CInterProtocol();
// 		pInterSerNest2->SetAction(Resoucce, NULL, "HTTP");
// 		RegisterProtocol(pInterSerNest2, PROTOCOL_INTER, 0);		// set NULL in SetAction, 
// 
// 		pInterSerNest = new CInterProtocol();
// 		pInterSerNest->SetAction(Resoucce, NULL, "HTTP");
// 		RegisterProtocol(pInterSerNest, PROTOCOL_INTER, 0);		// set NULL in SetAction, 
// 
// 		pInterSer = new CInterProtocol();
// 		pInterSer->SetAction(Resoucce, NULL, "HTTP");
// 		RegisterProtocol(pInterSer, PROTOCOL_INTER, 0);			// set NULL in SetAction, 

// 		pInterApp = new CInterApplication();
// 		pInterApp->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pInterApp, APPLICATION_INTER, FLAG_PROCESS_ACCEPT );
// 
// 		pInterAppNest = new CInterApplication();
// 		pInterAppNest->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pInterAppNest, APPLICATION_INTER, FLAG_PROCESS_ACCEPT );
// 
// 		pInterAppNest2 = new CInterApplication();
// 		pInterAppNest2->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pInterAppNest2, APPLICATION_INTER, FLAG_PROCESS_ACCEPT );
// 
// 		pInterAppSerNest2 = new CInterApplication();
// 		pInterAppSerNest2->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pInterAppSerNest2, APPLICATION_INTER, FLAG_PROCESS_ACCEPT);
// 
// 		pInterAppSerNest = new CInterApplication();
// 		pInterAppSerNest->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pInterAppSerNest, APPLICATION_INTER, FLAG_PROCESS_ACCEPT);
// 
// 		pInterAppSer = new CInterApplication();
// 		pInterAppSer->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pInterAppSer, APPLICATION_INTER, FLAG_PROCESS_ACCEPT);
// 
// 		pSSLSerMode = new CSSLServerApplication();
// 		pSSLSerMode->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pSSLSerMode, APPLICATION_SSLSERVER, FLAG_PROCESS_ACCEPT);
// 
// 		pSSLCliMode = new CSSLClientApplication();	// client ssl should set FLAG_LATER_CONNECT
// 		pSSLCliMode->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pSSLCliMode, APPLICATION_SSLCLIENT, FLAG_PROCESS_ACCEPT | FLAG_LATER_CONNECT );

//	Tested OK
// 		retContext = CreateApplication(pInterApp, pTCP, (void*)&addrListen, sizeof(sockaddr_in), NULL, NULL, 0);
// 		retContextNest = CreateApplication(pInterAppNest, pInterNest, retContext, sizeof(CContextItem*), NULL, NULL, 0);
// 		retContextNest2 = CreateApplication(pInterAppNest2, pInterNest2, retContextNest, sizeof(CContextItem*), NULL, NULL, 0);
// 		retContextSer = CreateApplication(pInterAppSer, NULL, NULL, 0, pTCP, (void*)&addrHttp, sizeof(sockaddr_in));
// 		retContextSerNest = CreateApplication(pInterAppSerNest, NULL, NULL, 0, pInterSerNest, retContextSer, sizeof(CContextItem*));
// 		retContextSerNest2 = CreateApplication(pInterAppSerNest2, NULL, NULL, 0, pInterSerNest2, retContextSerNest, sizeof(CContextItem*));
// 		retContextFor = CreateApplication(pForwardDup, pInter, retContextNest2, sizeof(CContextItem*), pInterSer, retContextSerNest2, sizeof(CContextItem*));
		
//	Tested OK
// 		retContext = CreateApplication(pInterApp, pTCP, (void*)&addrListen, sizeof(sockaddr_in), NULL, NULL, 0);
// 		retContextNest = CreateApplication(pSSLSerMode, pInterNest, retContext, sizeof(CContextItem*), NULL, NULL, 0);
// 		retContextNest2 = CreateApplication(pInterAppNest2, pInterNest2, retContextNest, sizeof(CContextItem*), NULL, NULL, 0);
// 		retContextSer = CreateApplication(pInterAppSer, NULL, NULL, 0, pTCP, (void*)&addrHttps, sizeof(sockaddr_in));
// 		retContextSerNest = CreateApplication(pSSLCliMode, NULL, NULL, 0, pInterSerNest, retContextSer, sizeof(CContextItem*));
// 		retContextSerNest2 = CreateApplication(pInterAppSerNest2, NULL, NULL, 0, pInterSerNest2, retContextSerNest, sizeof(CContextItem*));
// 		retContextFor = CreateApplication(pForwardDup, pInter, retContextNest2, sizeof(CContextItem*), pInterSer, retContextSerNest2, sizeof(CContextItem*));
		break;
	}
	return 0;
}
#endif TEST_APPLICATION                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 