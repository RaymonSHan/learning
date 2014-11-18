
#include "stdafx.h"
#include "SSLApplication.h"			// for ssl and oci have type collision, ssl only include when necessary

extern unsigned char FileNameTran[ASCII_NUMBER];

unsigned char ProxyURLMap[ASCII_NUMBER] = {
	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	IS_END,	0x00,	0x00,	IS_END,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	IS_SPC,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	IS_KEY,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
};

KeyParaTran ProxyURLPara[] = { SNASZ("GET "), SNASZ("POST "), sNASZ("Host:"), sNASZ("Connection:"), sNASZ("Proxy-Connection:"),
	{NULL} };

unsigned char ProxyResMap[ASCII_NUMBER] = {
	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	IS_END,	0x00,	0x00,	IS_END,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	IS_PRE,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
};
KeyParaTran ProxyResPara[] = { {"HTTP/1.1",8,"d"}, {"Content-Length:",15,"d"}, {"Connection:",11,"s"}, {"Set-Cookie:",11,"s"}, {NULL} };
//	ATTENTION, Set-Cookie may have multi-line, now I only record the last line.


extern CListItem*				isNULL;


extern CMemoryListUsed*	TempUseMemory;							//	for program memory use, avoid use malloc				//
extern CService theService;
extern sockaddr_in AddrListen;
extern sockaddr_in AddrForward;
extern char LogFile[MAX_PATH];

#ifdef FORWARDLOG_APPLICATION

long CSystemApplication::InitProcess(void)
{
	CIOCP* classIOCP;
	CContextItem* retContext;
	// 	CContextItem* retContextNest;
	CContextItem* retContextProxy;

#if ( !defined(USE_ARGC) && !defined(USE_PATH) )
	AddrListen.sin_family = AF_INET;    
	AddrListen.sin_port = htons(9898);    
	AddrListen.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

	AddrForward.sin_family = AF_INET;  
	AddrForward.sin_port = htons(9898);    
  	AddrForward.sin_addr.S_un.S_addr = inet_addr("10.4.220.62");		// ghgc

// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("58.211.1.101");		// wxis
// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("10.25.0.191");		// citicerp
// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("10.25.1.11");		// citicoa

// 	AddrForward.sin_port = htons(8888);    
// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("58.210.227.26");		// sz account

// 	AddrForward.sin_port = htons(80);    
// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("218.94.38.179");		// js account

	strncpy_s(LogFile, "c:\\gov\\", MAX_PATH);
#endif ( !defined(USE_ARGC) && !defined(USE_PATH) )

	while (TRUE)
	{
		if (!SystemAddResource<CNormalBuffer>("TEMPUSE", 10, PAGE_SIZE, TRUE, 0)) break;
		TempUseMemory = Resoucce->GetResource("TEMPUSE");															//

		if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 200, PAGE_SIZE, TRUE, 0)) break;
		if (!SystemAddResource<CHTTPContext>("HTTP", 100, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;
		if (!SystemAddResource<CFileContext>("FILE", 400, CACHE_SIZE, TRUE, INFINITE)) break;
		if (!SystemAddResource<CIOCP>("IOCP", 1, sizeof(INT), TRUE, 0)) break;
		classIOCP = (CIOCP*)SystemGetResource("IOCP")->GetOnlyOneList();
		classIOCP->InitProcess(1);			// should add return control

		pTCP = new CTCPProtocol();
		pTCP->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pTCP, PROTOCOL_TCP, 0);

		pFile = new CFileProtocol();
		pFile->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pFile, PROTOCOL_FILE, 0);

		pDir = new CDirectoryProtocol(GetSequence2Filename);
		pDir->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pDir, PROTOCOL_DIRECTORY, FLAG_ADD_OVERLAP | FLAG_PROCESS_FILE | FLAG_PASSBY);

		pInter = new CInterProtocol();
		pInter->SetAction(Resoucce, NULL, "HTTP");
		RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 

		pInterNest = new CInterProtocol();
		pInterNest->SetAction(Resoucce, NULL, "HTTP");
		RegisterProtocol(pInterNest, PROTOCOL_INTER, 0);		// set NULL in SetAction, 

		// 		pSSLSerMode = new CSSLServerApplication();
		// 		pSSLSerMode->SetAction(Resoucce, "FOR_BUFFER");
		// 		RegisterApplication(pSSLSerMode, APPLICATION_SSLSERVER, FLAG_PROCESS_ACCEPT);

		pPassby = new CPassbyApplication();
		pPassby->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pPassby, APPLICATION_PASSBY, FLAG_PROCESS_ACCEPT | FLAG_LATER_CONNECT);

// 		pProxy = new CProxyApplication();
// 		pProxy->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pProxy, APPLICATION_PROXY, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_REMOVE_HTTPHEAD);

// 		retContext = CreateApplication(pPassby, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pDir, LogFile, strlen(LogFile)+1);
// 		retContextProxy = CreateApplication(pProxy, pInter, retContext, sizeof(CContextItem), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);

// 		retContextProxy = CreateApplication(pProxy, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);

		pForwardDup = new CForwardApplication(NULL, 0, NULL, 0);
		pForwardDup->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pForwardDup, APPLICATION_FORWARD, FLAG_DUPLEX);

		retContext = CreateApplication(pPassby, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pDir, LogFile, strlen(LogFile)+1);
		retContextProxy = CreateApplication(pForwardDup, pInter, retContext, sizeof(CContextItem), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);

// 		retContext = CreateApplication(pForwardDup, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);
		break;
	}
	return 0;
}

#endif FORWARDLOG_APPLICATION

#if (defined PROXY_APPLICATION || defined PROXYSERVER_APPLICATION)

long CSystemApplication::InitProcess(void)
{
	CIOCP* classIOCP;
	CContextItem* retContext;
// 	CContextItem* retContextNest;
	CContextItem* retContextProxy;

#if ( !defined(USE_ARGC) && !defined(USE_PATH) )
	AddrListen.sin_family = AF_INET;    
	AddrListen.sin_port = htons(8888);    
	AddrListen.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

	AddrForward.sin_family = AF_INET;  
	AddrForward.sin_port = htons(0);    
	AddrForward.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

	strncpy_s(LogFile, "Z:\\Develop\\citic\\", MAX_PATH);
#endif ( !defined(USE_ARGC) && !defined(USE_PATH) )

	while (TRUE)
	{
		if (!SystemAddResource<CNormalBuffer>("TEMPUSE", 10, PAGE_SIZE, TRUE, 0)) break;
		TempUseMemory = Resoucce->GetResource("TEMPUSE");															//

		if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 200, PAGE_SIZE, TRUE, 0)) break;
		if (!SystemAddResource<CHTTPContext>("HTTP", 100, CACHE_SIZE, TRUE, TIMEOUT_TCP)) break;
		if (!SystemAddResource<CFileContext>("FILE", 400, CACHE_SIZE, TRUE, INFINITE)) break;
		if (!SystemAddResource<CIOCP>("IOCP", 1, sizeof(INT), TRUE, 0)) break;
		classIOCP = (CIOCP*)SystemGetResource("IOCP")->GetOnlyOneList();
		classIOCP->InitProcess(1);			// should add return control

		pTCP = new CTCPProtocol();
		pTCP->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pTCP, PROTOCOL_TCP, 0);

		pFile = new CFileProtocol();
		pFile->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pFile, PROTOCOL_FILE, 0);

#ifdef PROXY_APPLICATION
		pDir = new CDirectoryProtocol(GetSequence2Filename);
		pDir->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pDir, PROTOCOL_DIRECTORY, FLAG_ADD_OVERLAP | FLAG_PROCESS_FILE | FLAG_PASSBY);

		pInter = new CInterProtocol();
		pInter->SetAction(Resoucce, NULL, "HTTP");
		RegisterProtocol(pInter, PROTOCOL_INTER, 0);			// set NULL in SetAction, 

		pInterNest = new CInterProtocol();
		pInterNest->SetAction(Resoucce, NULL, "HTTP");
		RegisterProtocol(pInterNest, PROTOCOL_INTER, 0);		// set NULL in SetAction, 

// 		pSSLSerMode = new CSSLServerApplication();
// 		pSSLSerMode->SetAction(Resoucce, "FOR_BUFFER");
// 		RegisterApplication(pSSLSerMode, APPLICATION_SSLSERVER, FLAG_PROCESS_ACCEPT);

		pPassby = new CPassbyApplication();
		pPassby->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pPassby, APPLICATION_PASSBY, FLAG_PROCESS_ACCEPT | FLAG_LATER_CONNECT);

		pProxy = new CProxyApplication();
		pProxy->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pProxy, APPLICATION_PROXY, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_REMOVE_HTTPHEAD);

		retContext = CreateApplication(pPassby, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pDir, LogFile, strlen(LogFile)+1);
		retContextProxy = CreateApplication(pProxy, pInter, retContext, sizeof(CContextItem), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);

// 		retContextProxy = CreateApplication(pProxy, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);
#endif PROXY_APPLICATION

#ifdef PROXYSERVER_APPLICATION
		pDirRead = new CDirectoryReadProtocol(GetURL2Filename);
		pDirRead->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pDirRead, PROTOCOL_DIRECTORY_READ, FLAG_ADD_OVERLAP | FLAG_PROCESS_FILE);

		pProxySer = new CProxyServerApplication();
		pProxySer->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pProxySer, APPLICATION_PROXYSIM, FLAG_DUPLEX | FLAG_LATER_CONNECT);

		retContext = CreateApplication(pProxySer, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), pDirRead, LogFile, strlen(LogFile)+1);
#endif PROXYSERVER_APPLICATION


		break;
	}
	return 0;
}

#endif (defined PROXY_APPLICATION || defined PROXYSERVER_APPLICATION)

long CProxyApplication::AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op)
{
	if (op == OP_CLIENT_READ) return AnalysisProxyURL(mContext, mBuffer, size);
	if (op == OP_SERVER_READ) return AnalysisProxyRespone(mContext, mBuffer, size);
	return 0;
}

long AnalysisProxyURL(CContextItem* mContext, CListItem* &mBuffer, long size)
{
	CHTTPContext* httpContext = (CHTTPContext*)mContext;
	URLInfo* urlinfo = &(httpContext->urlInfo);
	char* sStart = (char*)(mBuffer+1);
	char* sEnd = sStart+size;
	CListItem* newBuffer;
	char* tStart;
	char* lStart;
	char* lEnd;
	long key;
	long urllength;
	long keepalivelength;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x10;
	if ( *((long*)(sEnd-4)) != ISCRCR )
		break;

	ret_err = 0x20;
	memset(urlinfo, 0, sizeof(URLInfo));
	SET_PARA_STR(urlinfo, 0, getURL)
	SET_PARA_STR(urlinfo, 1, getURL)
	SET_PARA_STR(urlinfo, 2, getSite)
	SET_PARA_STR(urlinfo, 3, getKeepalive)
	SET_PARA_STR(urlinfo, 4, getKeepalive)
	if (CommandLineParaTran( (char*)(mBuffer+1), size, NULL, NULL, ProxyURLPara, urlinfo->valPlace, ProxyURLMap)) break;	//

	ret_err = 0x30;
	if (urlinfo->getLength[MODE_GET]==VALUE_NOT_FOUND && urlinfo->getLength[MODE_POST]==VALUE_NOT_FOUND) break;

	ret_err = 0x40;
	newBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!newBuffer) break;
	tStart = (char*)(newBuffer+1);

	ret_err = 0x50;
	urllength = max(urlinfo->getLength[MODE_GET], urlinfo->getLength[MODE_POST]);
	lStart = memstr(urlinfo->getURL, urllength, "://", 3);
//	if (!lStart) break;
// 	lEnd = memstr(lStart+3, urllength-(lStart-urlinfo->getURL)-3, "/", 1);
// 	if (!lEnd) break;
	if (!lStart) 
	{
		lStart = urlinfo->getURL;				//	for FileTranslate use
		lEnd = lStart;
	}
	else
	{
		lEnd = memstr(lStart+3, urllength-(lStart-urlinfo->getURL)-3, "/", 1);
		if (!lEnd) break;
	}

	ret_err = 0x60;
	if (urlinfo->getLength[MODE_GET] != VALUE_NOT_FOUND) tStart = AddString(tStart, "GET ", 4);
	if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND) tStart = AddString(tStart, "POST ", 5);

	if (mContext->PApplication->ApplicationFlag & FLAG_REMOVE_HTTPHEAD) tStart = AddString(tStart, lEnd, urllength-(lEnd-urlinfo->getURL));
	else tStart = AddString(tStart, urlinfo->getURL, urllength);
	tStart = AddString(tStart, " HTTP/1.1\r\n", 11);

	ret_err = 0x70;
	if (lEnd-lStart > 3)			//	??	//	May 19 '14
//	if (lStart-lEnd > 3)
	{
		tStart = AddString(tStart, "Host: ", 6);
		tStart = AddString(tStart, lStart+3, lEnd-lStart-3);
		tStart = AddString(tStart, "\r\n", 2);
		memcpy(urlinfo->getSite, lStart+3, lEnd-lStart-3);			//	Set Host for further use
		urlinfo->getLength[2] = lEnd-lStart-3;
	}

	lEnd = memstr(sStart, sEnd-sStart, "\r\n", 2);
	if (!lEnd) break;
	lEnd += 2;


	ret_err = 0x90;
	while (sStart < sEnd-2)
	{
		sStart = lEnd;
		lEnd = memstr(sStart, sEnd-sStart, "\r\n", 2) + 2;
		key = *((long*)sStart);
		if (key==ISHost || key==ISConnection || key==ISProxy || key==ISVia) continue;		// re-add Host in May 19 '14, is it a bug ?
//		if (/*key==ISHost ||*/ key==ISConnection || key==ISProxy || key==ISVia) continue;	// remove HOST in Dec. 09 '13
		else if (memcmp(sStart, NASZ("Accept-Encoding:"))) continue;
		else 
		{
			memcpy(tStart, sStart, lEnd-sStart);
			tStart += (lEnd-sStart);
		}
	}

	ret_err = 0x80;
	keepalivelength = max(urlinfo->getLength[3], urlinfo->getLength[4]);
	if (keepalivelength != VALUE_NOT_FOUND)
	{
		tStart = AddString(tStart, "Connection: ", 12);
		tStart = AddString(tStart, urlinfo->getKeepalive, keepalivelength);
		tStart = AddString(tStart, "\r\n", 2);
	}



// 	ret_err = 0x80;
// 	keepalivelength = max(urlinfo->getLength[3], urlinfo->getLength[4]);
// 	if (keepalivelength != VALUE_NOT_FOUND)
// 	{
// 		tStart = AddString(tStart, "Connection: ", 12);
// 		tStart = AddString(tStart, urlinfo->getKeepalive, keepalivelength);
// 		tStart = AddString(tStart, "\r\n", 2);
// 	}
// 
// 	ret_err = 0x90;
// 	while (sStart < sEnd-2)
// 	{
// 		sStart = lEnd;
// 		lEnd = memstr(sStart, sEnd-sStart, "\r\n", 2) + 2;
// 		key = *((long*)sStart);
// 		if (key==ISHost || key==ISConnection || key==ISProxy || key==ISVia) continue;
// 		else 
// 		{
// 			memcpy(tStart, sStart, lEnd-sStart);
// 			tStart += (lEnd-sStart);
// 		}
// 	}

	ret_err = 0xa0;
	newBuffer->NProcessSize = tStart - (char*)(newBuffer+1);
	mContext->PApplication->FreeApplicationBuffer(mBuffer);
	mBuffer = newBuffer;
// 	mContext->ContentMode = CONTENT_MODE_LENGTH;				//	but not set TransferEncoding, for do not control the length
	mContext->ContentMode = CONTENT_MODE_AFTERHEAD;				//	but not set TransferEncoding, for do not control the length

	__CATCH_BEGIN(MODULE_PROTOCOL, "AnalysisProxyURL")
	RET_BETWEEN(0x50, 0x90)  mContext->PApplication->FreeApplicationBuffer(mBuffer);
	__CATCH_END
}

long AnalysisProxyRespone(CContextItem* mContext, CListItem* &mBuffer, long size)
{
	char* sStart = (char*)(mBuffer+1);
	char* sEnd = sStart+size;
	CListItem* newBuffer;
	char* tStart;
	char* lEnd;
	char* conn;
	long key;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x10;
	if ( *((long*)(sEnd-4)) != ISCRCR ) break;
// 	*(sStart+7) = '0';				// change to HTTP/1.0

	ret_err = 0x20;
	newBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!newBuffer) break;
	tStart = (char*)(newBuffer+1);

	ret_err = 0x30;
	lEnd = sStart;
	while (sStart < sEnd-2)
	{
		sStart = lEnd;
		lEnd = memstr(sStart, sEnd-sStart, "\r\n", 2) + 2;
		key = *((long*)sStart);
		if(key==ISProxy || key==ISVia) continue;
		else
		{
			memcpy(tStart, sStart, lEnd-sStart);
			tStart += (lEnd-sStart);
			if (key==ISConnection)
			{
				conn = memstr(sStart, lEnd-sStart, ":", 1);
				if (!conn) break;
				tStart = AddString(tStart, "Proxy-Connection", 16);
				memcpy(tStart, conn, lEnd-conn);
				tStart += (lEnd-conn);
			}
		}
	}

	ret_err = 0x40;
	newBuffer->NProcessSize = tStart - (char*)(newBuffer+1);
	mContext->PApplication->FreeApplicationBuffer(mBuffer);
	mBuffer = newBuffer;
	// 	mContext->ContentMode = CONTENT_MODE_LENGTH;				//	but not set TransferEncoding, for do not control the length
	mContext->ContentMode = CONTENT_MODE_AFTERHEAD;				//	but not set TransferEncoding, for do not control the length

	__CATCH_BEGIN(MODULE_PROTOCOL, "AnalysisProxyURL")
		RET_BETWEEN(0x30, 0x30)  mContext->PApplication->FreeApplicationBuffer(mBuffer);
	__CATCH_END
}


long CProxyApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	CContextItem* peerContext = mContext->PPeer;
//	CContextItem* serContext;
	URLInfo* urlinfo = &((CHTTPContext*)mContext)->urlInfo;
	CTCPContext* tcpContext;
	struct addrinfo *aiList = NULL;

// 	sockaddr_in* remoteSer;
// 	hostent* hostInfo;
// 	char* portPlace;
// 	WORD	portNumber;
// 	unsigned long addrNumber;

	long ret_err = 0x01;
//	long ret;

	__TRY
	ret_err = 0x10;
	if (mContext->ContentMode == CONTENT_MODE_LENGTH)
	{
		if (peerContext)
			return NoneProFunc(peerContext->PProtocol, fPostSend)(peerContext, mBuffer, mBuffer->NProcessSize, OP_SERVER_WRITE, opSide);
		else break;
	}
	MyGetInfoAddr(urlinfo->getURL+7, max(urlinfo->getLength[0], urlinfo->getLength[1])-7, aiList);

// 	ret_err = 0x20;
// 	if (peerContext->PPeer && memcmp(&tcpContext->addrServer, aiList->ai_addr, sizeof(sockaddr)))
// 	{
// 		serContext = peerContext->PProtocol->GetDuplicateContext(peerContext);											//
// 		if (!serContext) break;
// 		mContext->PPeer = serContext;
// 		serContext->PPeer = 0;
// 
// 		ret_err = 0x40;
// 		if (mContext != peerContext)						//	after GRACE_CLOSE mContext == peerContext, this condition SHOULD do in every APPLICATION
// 		{
// 			peerContext->PPeer = peerContext;
// 			peerContext->MoreBuffer = 0;
// 			peerContext->LogFileContext = 0;						//	two lines add Jul 01 '13
// 			ret = NoneAppFunc(mContext->PApplication, fOnClose)(peerContext, isNULL, FLAG_GRACE_CLOSE, opSide);
// 			if (ret) break;
// 		}
// 		else
// 		{
// 			serContext->MoreBuffer = 0;							//	add in Jun 02 '13
// 			serContext->LogFileContext = 0;
// 		}
// 		peerContext = serContext;
// 	}

	if (!peerContext->PPeer)
	{
		ret_err = 0x50;
		tcpContext = (CTCPContext*)peerContext;
		memcpy( &tcpContext->addrServer, aiList->ai_addr, sizeof(sockaddr) );

		ret_err = 0x60;
		peerContext->PPeer = mContext;
		if ( NoneProFunc(peerContext->PProtocol, fPostConnect)(peerContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT) ) break;		//
	}
	else
	{
		ret_err = 0x70;
		if ( NoneProFunc(peerContext->PProtocol, fPostSend)(peerContext, mBuffer, mBuffer->NProcessSize, OP_SERVER_WRITE, opSide) ) break;		//
	}

	__CATCH(MODULE_APPLICATION, "CProxyApplication - OnClientRead")
}


// long CProxyApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
// {
// 	CContextItem* peerContext = mContext->PPeer;
// 	CContextItem* serContext;
// 	URLInfo* urlinfo = &((CHTTPContext*)mContext)->urlInfo;
// 	sockaddr_in* remoteSer;
// 	hostent* hostInfo;
// 	char* portPlace;
// 	WORD	portNumber;
// 	unsigned long addrNumber;
// 
// 	long ret_err = 0x01;
// 	long ret;
// 
// 	__TRY
// 	
// 	if (mContext->ContentMode == CONTENT_MODE_LENGTH)
// 	{
// 		if (peerContext)
// 			return NoneProFunc(peerContext->PProtocol, fPostSend)(peerContext, mBuffer, mBuffer->NProcessSize, OP_SERVER_WRITE, opSide);
// 		else break;
// 	}
// 
// 	ret_err = 0x10;
// 	portPlace = (char*)memchr(urlinfo->getURL+6, ':', max(urlinfo->getLength[0], urlinfo->getLength[1])-6);		//	avoid http://
// 	if (portPlace)
// 	{
// // 		*portPlace = 0;		//	why? I forget
// 		portNumber = (WORD)GetInt(portPlace+1, 10);			// large enough, for the end is 0;
// 	}
// 	else portNumber = 80;
// 
// 	char siteWithoutPort[SMALL_CHAR+8];
// 	memcpy(siteWithoutPort, urlinfo->getSite, urlinfo->getLength[2]);
// 	siteWithoutPort[urlinfo->getLength[2]] = 0;
// 	char* portstart = (char*)memchr(siteWithoutPort, ':', urlinfo->getLength[2]);
// 	if (portstart) *portstart = 0;
// 
// //	addrNumber = inet_addr(urlinfo->getSite);
// 	addrNumber = inet_addr(siteWithoutPort);
// 	if (addrNumber == INADDR_NONE)
// 	{
// //		hostInfo = gethostbyname(urlinfo->getSite);
// 		hostInfo = gethostbyname(siteWithoutPort);
// 		if (!hostInfo) return 1;
// 		addrNumber = *(u_long *) hostInfo->h_addr_list[0];
// 	}
// 	remoteSer = &(((CHTTPContext*)peerContext)->addrServer);
// 
// 	ret_err = 0x20;
// 	if (peerContext->PPeer && (addrNumber!=remoteSer->sin_addr.S_un.S_addr || htons(portNumber)!=remoteSer->sin_port || closeHTTP ))
// 	{
// 		serContext = peerContext->PProtocol->GetDuplicateContext(peerContext);											//
// 		if (!serContext) break;
// 		mContext->PPeer = serContext;
// 		serContext->PPeer = 0;
// 
// 		ret_err = 0x40;
// 		if (mContext != peerContext)						//	after GRACE_CLOSE mContext == peerContext, this condition SHOULD do in every APPLICATION
// 		{
// 			peerContext->PPeer = peerContext;
// 			peerContext->MoreBuffer = 0;
// 			peerContext->LogFileContext = 0;						//	two lines add Jul 01 '13
// 			ret = NoneAppFunc(mContext->PApplication, fOnClose)(peerContext, isNULL, FLAG_GRACE_CLOSE, opSide);
// 			if (ret) break;
// 		}
// 		else
// 		{
// 			serContext->MoreBuffer = 0;							//	add in Jun 02 '13
// 			serContext->LogFileContext = 0;
// 		}
// 		peerContext = serContext;
// 		remoteSer = &(((CHTTPContext*)peerContext)->addrServer);
// 	}
// 
// 	if (!peerContext->PPeer)
// 	{
// 		ret_err = 0x50;
// 		remoteSer->sin_family = AF_INET;    
// 		remoteSer->sin_port = htons(portNumber);    
// 		remoteSer->sin_addr.S_un.S_addr = addrNumber;
// 
// 		ret_err = 0x60;
// 		peerContext->PPeer = mContext;
// 		if ( NoneProFunc(peerContext->PProtocol, fPostConnect)(peerContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT) ) break;		//
// 	}
// 	else
// 	{
// 		ret_err = 0x70;
// 		if ( NoneProFunc(peerContext->PProtocol, fPostSend)(peerContext, mBuffer, mBuffer->NProcessSize, OP_SERVER_WRITE, opSide) ) break;		//
// 	}
// 
// 	__CATCH(MODULE_APPLICATION, "CProxyApplication - OnClientRead")
// }

long CProxyApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	return SendToPeer(mContext, mBuffer, size, OP_CLIENT_WRITE);																	//
}

long CProxyServerApplication::AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op)
{
	if (op == OP_CLIENT_READ) return AnalysisProxyURL(mContext, mBuffer, size);
	return 0;
}

long CProxyServerApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)					//
{
	URLInfo* urlinfo = &((CHTTPContext*)mContext)->urlInfo;
	long ret_err = 0x01;
	long ret = 0;

	__TRY

	ret_err = 0x10;
	if (!mContext->PPeer || mContext->PPeer == mContext) break;

	if (urlinfo->getLength[MODE_GET]==VALUE_NOT_FOUND && urlinfo->getLength[MODE_POST]==VALUE_NOT_FOUND)
	{
		ret_err = 0x20;
		return NoneProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, 0);
	}
	else
	{
		ret_err = 0x30;
		mContext->PPeer->PPeer = mContext;
		return NoneProFunc(mContext->PPeer->PProtocol, fPostConnect)(mContext->PPeer, mBuffer, size, OP_CONNECT);
	}

	__CATCH(MODULE_APPLICATION, "CProxyServerApplication - OnClientRead")
}

long CProxyServerApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)					//
{
	long ret_err = 0x01;
	char* sStart, *tStart;
	long copysize;

	__TRY

	ret_err = 0x10;
	if (!mContext->PPeer || mContext->PPeer == mContext) break;

	ret_err = 0x20;
	tStart = (char*)(mBuffer+1);
	sStart = memstr(tStart, size, "\r\n\r\n", 4);
	if (!sStart) break;
	sStart = memstr(sStart, size - (sStart-tStart), "HTTP", 4);
	if (!sStart) break;
// 	sStart += 4;

	ret_err = 0x30;
	copysize = size - (sStart-tStart);
	memcpy(tStart, sStart, copysize);
	return NoneProFunc(mContext->PPeer->PProtocol, fPostSend)(mContext->PPeer, mBuffer, copysize, OP_CLIENT_WRITE, 0);

	__CATCH(MODULE_APPLICATION, "CProxyServerApplication - OnServerRead")
}


#ifdef SIMSERVER_APPLICATION

char MAINPATH[MAX_PATH]	= "c:\\log\\\0";

long CSystemApplication::InitProcess(void)
{
	CIOCP* classIOCP;
	CContextItem* retContext;
	// 	long ret;


#ifdef USE_SERVICE
	AddrListen.sin_family = AF_INET;    
	AddrListen.sin_port = htons(7000);    
	AddrListen.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

	// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("192.168.1.197");
	// 	AddrForward.sin_addr.S_un.S_addr = inet_addr("10.32.209.197");
#endif USE_SERVICE

#ifdef USE_PATH
	strcpy_s(MAINPATH, MAX_PATH, LogFile);
#endif USE_PATH

	while (TRUE)
	{
		if (!SystemAddResource<CNormalBuffer>("TEMPUSE", 10, PAGE_SIZE, TRUE, 0)) break;
		TempUseMemory = Resoucce->GetResource("TEMPUSE");															//

		if (!SystemAddResource<CNormalBuffer>("FOR_BUFFER", 200, PAGE_SIZE, TRUE, 0)) break;
		if (!SystemAddResource<CHTTPContext>("HTTP", 100, CACHE_SIZE, TRUE, INFINITE)) break;
		if (!SystemAddResource<CFileContext>("FILE", 100, CACHE_SIZE, TRUE, INFINITE)) break;

		if (!SystemAddResource<CIOCP>("IOCP", 1, sizeof(INT), TRUE, 0)) break;
		classIOCP = (CIOCP*)SystemGetResource("IOCP")->GetOnlyOneList();
		classIOCP->InitProcess(1);			// should add return control

		pTCP = new CTCPProtocol();
		pTCP->SetAction(Resoucce, "IOCP", "HTTP");
		RegisterProtocol(pTCP, PROTOCOL_TCP, 0);

		pFile = new CFileProtocol();
		pFile->SetAction(Resoucce, "IOCP", "FILE");
		RegisterProtocol(pFile, PROTOCOL_FILE, FLAG_ADD_OVERLAP);

		pFileRead = new CFileReadProtocol();
		pFileRead->SetAction(Resoucce, "IOCP", "FILE");
		RegisterProtocol(pFileRead, PROTOCOL_FILE_READ, FLAG_ADD_OVERLAP);

		// 		pWeb = new CWebApplication();
		// 		pWeb->SetAction(Resoucce, "FOR_BUFFER");
		// 		RegisterApplication(pWeb, APPLICATION_WEB, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_MULTI_PROTOCOL);

		pSim = new CSimServer();
		pSim->SetAction(Resoucce, "FOR_BUFFER");
		RegisterApplication(pSim, APPLICATION_SIM, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_MULTI_PROTOCOL);

		retContext = CreateApplication(pSim, pTCP, (void*)&AddrListen, sizeof(sockaddr_in), NULL, NULL, 0);
//		retContext = pSim->AddProtocolPeer(pFileRead, (void*)MAINPATH, strlen(MAINPATH), "/web/");
		retContext = pSim->AddProtocolPeer(pFileRead, (void*)MAINPATH, strlen(MAINPATH), "/");
		break;
	}
	return 0;
}


// extern unsigned char FileNameTran[ASCII_NUMBER];
// extern char MAINPATH[];

long CSimServer::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	CContextItem *serContext = 0;
	// 	//	CFileContext *fileContext;
	__int64	contextLength;
	// 	long messagelength = 0;
	long ret_err = 0x01;																											//
	CHTTPContext* httpContext = (CHTTPContext*)mContext;
	URLInfo* urlinfo = &(httpContext->urlInfo);
	char* urlstart = urlinfo->getURL;
	unsigned char nowchar;
	char wildFilename[MAX_PATH];
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	if (mContext->ContentMode == CONTENT_MODE_LENGTH)		// for post body, ignore the input
	{
		if ( mContext->BodyRemain == 0 )
			mContext->ContentMode = CONTENT_MODE_HEAD;
		// the old is mContext->BodyRemain = 0; mContext->ContentMode = CONTENT_MODE_HEAD;

		NoneProFunc(mContext->PProtocol, fPostReceive)
			(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide);
		return 0;
	}

	while (TRUE)
	{
		ret_err = 0x10;
		if ( (mContext->PPeer != mContext) && (mContext->PPeer) ) serContext = mContext->PPeer;		// accept only do weblogin 
		else serContext = PreparePeer(mContext, mBuffer, FALSE);		// Do NOT remove the ahead '/'

		ret_err = 0x20;
		if (serContext)
		{
			mContext->PPeer = serContext;			// normally add this just after accept
			serContext->PPeer = mContext;			// add this before connect

			do 
			{
				nowchar = (unsigned char)*urlstart;
				*urlstart = FileNameTran[nowchar];
				urlstart++;
			}
			while (*urlstart);
		}

		urlstart = urlinfo->getURL;

		int urllength = strlen((char*)urlinfo->getURL);
		if (urllength > 100) urlstart+=30;	//??

		ret_err = 0x30;
		strncpy_s(wildFilename, MAINPATH, MAX_PATH);
		if ( urlinfo->getLength[1]!=VALUE_NOT_FOUND ) strncat_s(wildFilename, "P", MAX_PATH);
		strncat_s(wildFilename, "\?\?\?\?\?", MAX_PATH);
		strncat_s(wildFilename, urlstart, MAX_PATH);
		if ( urllength > 80 ) 
		{
			wildFilename[70] = '*';
			wildFilename[71] = 0;
		}
		strncat_s(wildFilename, ".log", MAX_PATH);

		//	add here for preventCache and so on.

		hFind = FindFirstFile(wildFilename, &findFileData);
		if (hFind ==INVALID_HANDLE_VALUE)
		{
			DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Do not find file:%s, \r\n", wildFilename);			//
			break;
		}

		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_STATUS, "Open file:%s, \r\n", findFileData.cFileName );			//

		ret_err = 0x40;
		if ( !(serContext->PProtocol->ProtocolFlag & FLAG_PROCESS_URL) )
		{
			strcpy_s( (char*)(mBuffer+1), MAX_URL, findFileData.cFileName );
			urlinfo->httpRedirect = FALSE;
		}

		ret_err = 0x50;
		if ( !serContext || NoneProFunc(serContext->PProtocol, fPostConnect)(serContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT) )
			break;

		ret_err = 0x60;
		FindClose(hFind);

		contextLength = (serContext->PProtocol->GetContextLength)(serContext, mBuffer);
		if ( (contextLength==0) || (contextLength>mBuffer->BufferType->BufferSize) ) break;

		ret_err = 0x70;
		if (NoneProFunc(serContext->PProtocol, fPostReceive)(serContext, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, opSide) )
			break;


		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in CSimServer - OnClientRead, 0x%x\r\n", ret_err);			//
	}																																//
	return ret_err;																													//
}

char CLIENT_CONTEXT[]	= "(Client:Context:";
char SERVER_CONTEXT[]	= "(Server:Context:";
char CONTEXT_END[]		= ")\r\n\r\n\r\n";
int  CONTEXT_LENGTH		= 16;
int  CONTEXT_END_LEN	= 7;

long CSimServer::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	long ret_err = 0x01;																											//
	char *sPointer = (char*)(mBuffer+1), *sStart;
	char *tPointer = sPointer;
	char *sEnd = sPointer + size;
	long copySize = 0, totalCopySize = 0;

	while (TRUE)
	{
		ret_err = 0x10;
		do 
		{
			sStart = memstr(sPointer+1, sEnd-sPointer, CLIENT_CONTEXT, CONTEXT_LENGTH);
			if (sStart) sPointer = sStart;
		} while (sStart);
		sPointer = memstr(sPointer, 100, CONTEXT_END, CONTEXT_END_LEN);	
		if (!sPointer) break;
		sPointer += CONTEXT_END_LEN;							// sPointer after the last Client:Context

		while (sPointer < sEnd)
		{
			sStart = memstr(sPointer, sEnd-sPointer, SERVER_CONTEXT, CONTEXT_LENGTH);
			if (sStart)
			{
				copySize = sStart - sPointer;
				memcpy(tPointer, sPointer, copySize);
				tPointer  += copySize;
				totalCopySize += copySize;
				sPointer = memstr(sStart, 100, CONTEXT_END, CONTEXT_END_LEN);
				if (!sPointer) break;
				sPointer += CONTEXT_END_LEN;
			}
		}

		ret_err = 0x20;
		mBuffer->NProcessSize = totalCopySize;
		if ( SendToPeer(mContext, mBuffer, totalCopySize, OP_CLIENT_WRITE) ) break;

		// 		return ( NoneAppFunc(mContext->PApplication, fOnClose)(mContext, mBuffer, FLAG_KEEPALIVE, opSide) );				//


		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in CSimServer - OnServerRead, 0x%x\r\n", ret_err);			//
	}																																//
	return ret_err;																													//
}


#endif SIMSERVER_APPLICATION                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   