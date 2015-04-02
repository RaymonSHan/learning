
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char HTTPSERVERVERSION[]	= "Server: Raymon IOCP-HTTP/1.01";
char HTTPFILETEXT[]			= "Content-Type: text/html";
char HTTPFILEAPPLICATION[]	= "Content-Type: */*";
char HTTP200HEAD[]			= "HTTP/1.0 200 OK\r\n%s\r\n%s\r\nContent-Length: %d\r\nAccept-Ranges: bytes\r\nCache-Control: no-cache\r\nConnection: Keep-Alive\r\n\r\n";
char HTTP206HEAD[]			= "HTTP/1.0 206 Partial Content\r\n%s\r\n%s\r\nContent-Length: %d\r\nContent-Range: bytes %d-%d/%d\r\nAccept-Ranges: bytes\r\nConnection: Keep-Alive\r\n\r\n";
char HTTP302HEAD[]			= "HTTP/1.1 302 Found\r\n%s\r\n%s\r\nLocation: http://%s%s\r\nContent-Length: %d\r\nAccept-Ranges: bytes\r\nCache-Control: no-cache\r\nConnection: Keep-Alive\r\n\r\n";
char HTTP201HEAD[]			= "HTTP/1.1 201 Created\r\n%s\r\n%s\r\nLocation: http://%s%s\r\nContent-Length: %d\r\nAccept-Ranges: bytes\r\nCache-Control: no-cache\r\nConnection: Keep-Alive\r\n\r\n";
char HTTPC00HEAD[]			= "HTTP/1.1 200 OK\r\n%s\r\n%s\r\n%s\r\nConnection: Keep-Alive\r\n\r\n";

char P3P[] = "P3P: CP=\"CAO DSP COR CUR ADM DEV TAI PSA PSD IVAi IVDi CONi TELo OTPi OUR DELi SAMi OTRi UNRi PUBi IND PHY ONL UNI PUR FIN COM NAV INT DEM CNT STA POL HEA PRE GOV\"";
char Cookie[] = "Set-Cookie: USER=Raymon&Session=abce1234";
char HTTP404HEAD[]			= "HTTP/1.0 404 Not Found\r\n%s\r\n%s\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n\r\n";
char HTTP404FILE[]			= "c:\\Inetpub\\wwwroot\\404.htm";

extern CListItem*				isNULL;


// struct ProtocolId
// {
// 	CContextItem*		peerContext;
// 	char				peerKeyword[NORMAL_CHAR];
// 	long				peerKeywordLen;
// 	char				peerHost[NORMAL_CHAR];							//	http request host name
// 	long				peerHostLen;
// 	long				peerPower;										//	less than 0 means invalid
// 	long				peerPowerInit;									//	0 for not load share
// 	ProtocolId*			peerNextShare;									//	for little faster
// };

CContextItem* CMultiApplication::AddProtocolPeer(CProtocol *pProSer, void* paraSer, long sizeSer, char* keySer, char* host, long share)
{
	CContextItem *serContext = NULL;																							//
	MYINT nowNum;
	long ret_err = 0x01;																											//
	long ret;																														//
	int i;
	ProtocolId* nowPeer;
	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;
		if (PeerNumber >= MAX_SERVER_PEER) break;

		ret_err = 0x20;																												//
		ApplicationFlag |= FLAG_MULTI_PROTOCOL;
		serContext = pProSer->GetProtocolContext();																				//
		if (!serContext) break;																									//

		ret_err = 0x30;
		ret = ProFunc(pProSer, fCreateRemote)(serContext, paraSer, sizeSer);													//
		serContext->PApplication = this;						//	ATTENTION for ReturnApplication						//

		ret_err = 0x40;
		nowNum = InterInc(&PeerNumber) - 1;
		nowPeer = &ServerPeer[nowNum];
		nowPeer->peerContext = serContext;
		if (host)
		{
			nowPeer->peerHostLen = strlen(host);
			memcpy(nowPeer->peerHost, host, nowPeer->peerHostLen+1);
		}

		ret_err = 0x50;
		if (keySer)
		{
			nowPeer->peerKeywordLen = strlen(keySer);
			memcpy(nowPeer->peerKeyword, keySer, nowPeer->peerKeywordLen+1);

			for (i=nowNum-1; i>=0; i--)		//	found the last one for link pointer
			{
				if (!memcmp(keySer, ServerPeer[i].peerKeyword, ServerPeer[i].peerKeywordLen+1))
				{
// 					if (!ServerPeer[i].peerPowerInit) ServerPeer[i].peerPowerInit++;
// 					if (!share) nowPeer->peerPowerInit++;
					ServerPeer[i].peerNextShare = nowPeer;
					break;
				}
			}
			for (i=0; i<=nowNum; i++)		//	found the first one for add total share
			{
				if (!memcmp(keySer, ServerPeer[i].peerKeyword, ServerPeer[i].peerKeywordLen+1))
				{
					nowPeer->peerPowerInit = share;
					ServerPeer[i].peerPowerTotal += share;
					break;
				}
			}
		}

		break;
	}
	return serContext;
}

CContextItem* CMultiApplication::PreparePeer(CContextItem* mContext, CListItem* mBuffer, bool isCopy)
{
	MYINT i;
	long len;
	CContextItem* serContext;
 	CHTTPContext* httpContext = (CHTTPContext*)(mContext/*->PPeer*/);
	CContextItem* retContext;

	URLInfo* urlinfo = &(httpContext->urlInfo);
	char* keySer = urlinfo->getURL;

	for ( i=0; i<PeerNumber; i++ )
	{
		len = strlen(ServerPeer[i].peerKeyword);
		if ( !strncmp (ServerPeer[i].peerKeyword, keySer, len ) ) break;
	}

	if (i==PeerNumber)	return NULL; //break;
	if (isCopy)
	{
		strcpy_s(keySer, MAX_PATH, keySer+len);
		if (urlinfo->getLength[MODE_GET] != VALUE_NOT_FOUND) urlinfo->getLength[MODE_GET] -= len;
		if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND) urlinfo->getLength[MODE_POST] -= len;
	}
	serContext = ServerPeer[i].peerContext;

	if (serContext)
	{
		retContext = (serContext->PProtocol->GetDuplicateContext)(serContext);							//
		return retContext;
	}
	else return NULL;
}

CContextItem* CMultiApplication::PreparePeer(CContextItem* mContext, char* keySer)
{
	MYINT i;
	CContextItem* serContext = NULL, *retContext = NULL;
	ProtocolId* nowPeer;
	double remain = 0;

	while(TRUE)
	{
		if (keySer)
		{
			for (i=0; i<PeerNumber; i++)
			{
				if (!memcmp(keySer, ServerPeer[i].peerKeyword, ServerPeer[i].peerKeywordLen+1)) break;
			}
			if (i==PeerNumber) break;				//	NOT found server name
		}
		else i=0;

		nowPeer = &ServerPeer[i];
		if (nowPeer->peerNextShare) 
		{
			remain = ((double)rand())/RAND_MAX*nowPeer->peerPowerTotal;
#ifdef	DEBUG_PEER
			printf("remain:%d;", int(remain));
#endif	DEBUG_PEER

			while ((remain > nowPeer->peerPowerInit) && nowPeer->peerNextShare)
			{
				remain -= nowPeer->peerPowerInit;
				nowPeer = nowPeer->peerNextShare;
			}
#ifdef	DEBUG_PEER
			printf("Host:%s\r\n",nowPeer->peerHost);
#endif	DEBUG_PEER

		}
		serContext = nowPeer->peerContext;

		if (serContext) 
		{
			retContext = (serContext->PProtocol->GetDuplicateContext)(serContext);							//
			if (retContext) retContext->PeerProtocol = nowPeer;
		}
		
		break;
	}
	return retContext;
}

long CMultiApplication::FreeServerContext()
{
	MYINT i;
	for ( i=PeerNumber-1; i>=0; i--)
	{
		ServerPeer[i].peerContext->PProtocol->FreeProtocolContext(ServerPeer[i].peerContext);
	}
	PeerNumber = 0;
	return 0;
}

unsigned char HttpURLMap[ASCII_NUMBER] = {
	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	IS_END,	0x00,	0x00,	IS_END,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	IS_SPC,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	IS_KEY,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	IS_KEY,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
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
KeyParaTran HttpURLPara[] = { SNASZ("GET "), SNASZ("POST "), sNASZ("Host:"), sNASZ("Connection:"), dNASZ("Content-Length:"),
	dNASZ("Range: bytes="), sNASZ("Cookie:"), sNASZ("boundary="), {NULL} };

unsigned char HttpResMap[ASCII_NUMBER] = {
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

//	Add dNASZ("HTTP/1.0") for I found some server return this	//	Apr. 29 '14
KeyParaTran HttpResPara[] = { dNASZ("HTTP/1.1"), dNASZ("Content-Length:"), sNASZ("Connection:"), sNASZ("Set-Cookie:"), sNASZ("Content-Type:"), dNASZ("HTTP/1.0"), {NULL} };
//Content-Type: text/html; charset=UTF-8
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CWebApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	CContextItem *serContext = 0;
//	CFileContext *fileContext;
	__int64	contextLength;
	long messagelength = 0;
	long ret_err = 0x01;																											//
	CHTTPContext* httpContext = (CHTTPContext*)mContext;
	URLInfo* urlinfo = &(httpContext->urlInfo);

	while (TRUE)
	{
 		ret_err = 0x10;
//		if (AnalysisHead(mContext, mBuffer, size)) break;

		while (TRUE)
		{
			ret_err = 0x11;
			if (mContext->PPeer != mContext) serContext = mContext->PPeer;		// accept only do weblogin 
			else serContext = PreparePeer(mContext, mBuffer);

			ret_err = 0x12;
			if (serContext)
			{
				mContext->PPeer = serContext;			// normally add this just after accept
				serContext->PPeer = mContext;			// add this before connect

				ret_err = 0x13;
				if ( !(serContext->PProtocol->ProtocolFlag & FLAG_PROCESS_URL) )
				{
					strcpy_s( (char*)(mBuffer+1), MAX_URL, urlinfo->getURL );
					urlinfo->httpRedirect = FALSE;
				}
			}

			if ( !serContext || NoneProFunc(serContext->PProtocol, fPostConnect)(serContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT) )
			{
				// redirect to 404 by httpRedirect
				strcpy_s(urlinfo->getURL, MAX_URL, "/tree/tree.html");	//	for file PostConnect use
				urlinfo->httpResult = HTTP404;
				urlinfo->httpRedirect = TRUE;
				mContext->PPeer->ListFlag |= FLAG_KEEPALIVE;
			}

			ret_err = 0x14;
			if ( !urlinfo->httpRedirect ) break;

			ret_err = 0x15;
			mContext->PPeer = mContext;
			if (serContext)
			{
				serContext->PPeer = NULL;
				if ( ProFunc(serContext->PProtocol, fPostClose)(serContext, isNULL, 0, 0) ) break;								//
			}
		}
		if ( ret_err != 0x14 ) break;

		ret_err = 0x20;
		if ( !(serContext->PProtocol->ProtocolFlag & FLAG_PROCESS_URL) )
			urlinfo->httpResult = (mContext->OverlapOffset) ? HTTP206 : HTTP200;

		ret_err = 0x30;
		if (urlinfo->httpResult == HTTPC00) contextLength = 0;
		else
		{
			contextLength = (serContext->PProtocol->GetContextLength)(serContext, mBuffer);
			if (!contextLength) break;
		}
		messagelength = PrepareHttpRelay(serContext, mBuffer, contextLength);

		ret_err = 0x40;
		if (NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, messagelength, OP_CLIENT_WRITE, opSide)) break;

		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in WebApplicaton - OnClientRead, 0x%x\r\n", ret_err);			//
	}																																//
	return ret_err;																													//

}																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CWebApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	return SendToPeer(mContext, mBuffer, size, OP_CLIENT_WRITE);																	//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long CWebApplication::PrepareHttpRelay(CContextItem* mContext, CListItem* mBuffer, __int64 fileLength)
{
	CHTTPContext* httpContext = (CHTTPContext*)(mContext->PPeer);
	URLInfo* urlinfo = &(httpContext->urlInfo);
	char* filetype;
	long messagelength;
//	if ( IN_EXT_NAME(mContext->fileName, ".txt", ".htm", ".html", NULL) ) filetype = HTTPFILETEXT;
//	else filetype = HTTPFILEAPPLICATION;
	filetype = HTTPFILETEXT;

	switch (urlinfo->httpResult)
	{
	case HTTP200:
		sprintf_s( ((char*)(mBuffer+1)), MAX_URL, HTTP200HEAD, filetype, HTTPSERVERVERSION, fileLength);
		break;
	case HTTP201:
		sprintf_s( (char*)(mBuffer+1), MAX_PATH, HTTP201HEAD, filetype, HTTPSERVERVERSION, urlinfo->getSite, urlinfo->getURL, fileLength);
		break;
	case HTTP206:
		sprintf_s( ((char*)(mBuffer+1)), MAX_URL, HTTP206HEAD, HTTPFILEAPPLICATION, HTTPSERVERVERSION,
			fileLength-mContext->OverlapOffset, mContext->OverlapOffset, fileLength-1, fileLength);	
		break;
	case HTTP302:
		sprintf_s( (char*)(mBuffer+1), MAX_PATH, HTTP302HEAD, filetype, HTTPSERVERVERSION, urlinfo->getSite, urlinfo->getURL, fileLength);
		break;
	case HTTP404:
		sprintf_s( ((char*)(mBuffer+1)), MAX_URL, HTTP404HEAD, HTTPFILETEXT, HTTPSERVERVERSION);
		break;
	case HTTPC00:
		sprintf_s( ((char*)(mBuffer+1)), MAX_URL, HTTPC00HEAD, HTTPSERVERVERSION, P3P, Cookie);
		break;
	}
	messagelength = strlen((char*)(mBuffer+1));

	if (urlinfo->sendTogether)
	{
		sprintf_s( ((char*)(mBuffer+1))+messagelength, mBuffer->BufferType->BufferSize-messagelength, (char*)(mContext->BHandle), urlinfo->getURL );
		messagelength += (long)fileLength;
	}
	return messagelength;
}

// in mBuffer+1, HTML
// out mBuffer+1, URLInfo
long CWebApplication::AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int op)
{
	if (op == OP_CLIENT_READ)
		return AnalysisHttpURL(mContext, mBuffer, size);
	return 0;
}

long AnalysisHttpURL(CContextItem* mContext, CListItem* mBuffer, long size)
{
	long ret_err = 0x01;
	CHTTPContext* httpContext = (CHTTPContext*)mContext;
	URLInfo* urlinfo = &(httpContext->urlInfo);

	urlinfo->httpResult = 0;
	urlinfo->httpRedirect = 0;
	urlinfo->sendTogether = 0;

	memset(urlinfo, 0, sizeof(URLInfo));
	mContext->TransferEncoding = ENCODING_LENGTH;

	while (TRUE)
	{
		ret_err = 0x10;
		SET_PARA_STR(urlinfo, 0, getURL)
		SET_PARA_STR(urlinfo, 1, getURL)
		SET_PARA_STR(urlinfo, 2, getSite)
		SET_PARA_STR(urlinfo, 3, getKeepalive)
		SET_PARA_TYPE(urlinfo, 4, getBodyRemain, __int64)

		urlinfo->valPlace[5].pValue = &(mContext->OverlapOffset);
		urlinfo->valPlace[5].pValueLength = &urlinfo->getLength[5];
		urlinfo->getLength[5] = sizeof(__int64);

		SET_PARA_STR(urlinfo, 6, getCookie)
		SET_PARA_STR(urlinfo, 7, getBoundary)

		ret_err = 0x20;
		if ( CommandLineParaTran( (char*)(mBuffer+1), size, NULL, NULL, HttpURLPara, urlinfo->valPlace, HttpURLMap) ) break;	//

		if (urlinfo->getLength[7] != VALUE_NOT_FOUND)
		{
			urlinfo->getBoundary[urlinfo->getLength[7]] = 0;
		}	
		if  (urlinfo->getLength[4]==VALUE_NOT_FOUND)
		{
			char* coding  = memstr((char*)(mBuffer+1), size, "\r\nTransfer-Encoding:", 20);
			if ( coding && memstr(coding+20, 20, "chunked", 7) ) mContext->TransferEncoding = ENCODING_CHUNKED;
			mContext->BodyRemain = VALUE_NOT_FOUND;
		}
		else
			mContext->BodyRemain = urlinfo->getBodyRemain;

		ret_err = 0x30;
		if ( urlinfo->getLength[0]==VALUE_NOT_FOUND && urlinfo->getLength[1]==VALUE_NOT_FOUND ) break;

		ret_err = 0x40;
		if (urlinfo->getURL[strlen(urlinfo->getURL)-1] == '/')
		{
#ifndef SIMSERVER_APPLICATION
			strcat_s(urlinfo->getURL, MAX_PATH, "index.html");
//	the following two line is out of the #ifndef, now I move into it. surely ?
			if (urlinfo->getLength[MODE_GET] != VALUE_NOT_FOUND) urlinfo->getLength[MODE_GET] += 10;
			if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND) urlinfo->getLength[MODE_POST] += 10;
#endif SIMSERVER_APPLICATION
		}
		urlinfo->sendTogether = FALSE;

		ret_err = 0x50;
//		if ( !strncmp(urlinfo->getKeepalive, "Keep-Alive", NORMAL_CHAR)) mContext->ListFlag |= FLAG_KEEPALIVE;			// I should close client, no keep-alive.	// Nov. 24 '14

		if (urlinfo->getLength[6] != VALUE_NOT_FOUND)
		{
			urlinfo->getCookie[urlinfo->getLength[6]] = 0;
// printf("In Get Cookie, %s, %x\r\n", urlinfo->getCookie, mContext);
		}


		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in ::AnalysisHttpURL, 0x%x\r\n  ", ret_err);						//
	}																																//
	return ret_err;																													//
}

//	Here use mContext->OverlapOffset for HTTP result;
//	use get cookie for set cookie
long AnalysisHttpRespond(CContextItem* mContext, CListItem* mBuffer, long size)
{
	long ret_err = 0x01;
	CHTTPContext* httpContext = (CHTTPContext*)mContext;
	URLInfo* urlinfo = &(httpContext->urlInfo);

	memset(urlinfo, 0, sizeof(URLInfo));
	mContext->TransferEncoding = ENCODING_LENGTH;

	__TRY

	ret_err = 0x10;

//	I change this for translate the result into following; have NOT hard test	//	Apr. 28 '14
// 	SET_PARA_OTHER_TYPE(urlinfo, 0, mContext->OverlapOffset, __int64)
	SET_PARA_TYPE(urlinfo, 0, httpResult, long)
	SET_PARA_TYPE(urlinfo, 5, httpResult, long)			// Apr. 29 '14 for some server return HTTP/1.0

	SET_PARA_TYPE(urlinfo, 1, getBodyRemain, __int64)
	SET_PARA_STR(urlinfo, 2, getKeepalive)
	SET_PARA_STR(urlinfo, 3, getCookie)
	SET_PARA_STR(urlinfo, 4, getCharset)
	if ( CommandLineParaTran( (char*)(mBuffer+1), size, NULL, NULL, HttpResPara, urlinfo->valPlace, HttpResMap) ) break;	//

	ret_err = 0x20;
	if  (urlinfo->getLength[1]==VALUE_NOT_FOUND)
	{
		char* coding  = memstr((char*)(mBuffer+1), size, "\r\nTransfer-Encoding:", 20);
		if ( coding && memstr(coding+20, 20, "chunked", 7) ) mContext->TransferEncoding = ENCODING_CHUNKED;
		else mContext->BodyRemain = VALUE_NOT_FOUND;
	}
	else
		mContext->BodyRemain = urlinfo->getBodyRemain;

	ret_err = 0x50;
	if ( !strncmp(urlinfo->getKeepalive, "Keep-Alive", NORMAL_CHAR)) 
		mContext->ListFlag |= FLAG_KEEPALIVE;

	if (urlinfo->getLength[3]!=VALUE_NOT_FOUND)
	{
		urlinfo->getCookie[urlinfo->getLength[3]] = 0;
// printf("In Set Cookie, %s, %x\r\n", urlinfo->getCookie, mContext);
	}

	__CATCH(MODULE_PROTOCOL, "AnalysisHttpRespond")
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     