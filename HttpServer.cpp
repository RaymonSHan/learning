
#include "stdafx.h"
#include "SSLApplication.h"

extern CService theService;

// #if ( defined(USE_ARGC)|| defined(USE_PATH) )
extern sockaddr_in AddrListen;
extern sockaddr_in AddrForward;
// #endif

#ifdef FORWARDLOOP_FUNCTION
extern	ForwardLoopConfig	*pForwardLoopConfig;
#endif FORWARDLOOP_FUNCTION

// #ifdef USE_PATH
extern char LogFile[MAX_PATH];
// #endif USE_PATH

long CTelnetApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	return NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, OP_SERVER_WRITE, opSide);
}

long CSystemApplication::ExitProcess(void)
{
#ifdef SIMSERVER_APPLICATION
	CloseApplication(pSim);
	SystemRemoveAllResource();			// have not tested	// testuse
	if (pSim) delete pSim;
#endif SIMSERVER_APPLICATION

#ifdef CONTENTPAD_APPLICATION
	if (pContentPad) delete pContentPad;
#endif CONTENTPAD_APPLICATION

#ifdef	CONFIRMDNS_FUNCTION
	if (pDNS) delete pDNS;
#endif	CONFIRMDNS_FUNCTION

#ifdef PROXYSERVER_APPLICATION
	if (pProxySer) delete pProxySer;
#endif PROXYSERVER_APPLICATION

#ifdef FORWARDLOOP_FUNCTION
	if (pForwardLoopConfig)
	{
		for (int i=0; i<pForwardLoopConfig->ContentListenNumber; i++)
			if (pForwardLoopConfig->pForwardLoop[i]) delete pForwardLoopConfig->pForwardLoop[i];
	}
#endif FORWARDLOOP_FUNCTION

#ifdef	PROXY_FUNCTION
	if (pDir) delete pDir;
	if (pPassby) delete pPassby;
	if (pProxy) delete pProxy;
#endif	PROXY_FUNCTION

	if (pSSLSerMode) delete pSSLSerMode;
	if (pInter) delete pInter;
// 	if (pTunnelPro) delete pTunnelPro;
// 	if (pTunnel) delete pTunnel;

// 	if (pWeb) delete pWeb;

// 	if (pSSLCliMode) delete pSSLCliMode;

// 	delete pWeb;
	if (pUDP) delete pUDP;
	if (pUDPSingle) delete pUDPSingle;
	if (pFileRead) delete pFileRead;
	if (pFile) delete pFile;
	if (pTCP) delete pTCP;

	return 0;
}

long SetBufferText(CListItem* mBuffer, char* str)
{
	long len = strlen(str);
	memcpy ( (char*)(mBuffer+1), str, len );
	return len;
}

//	MAY NOT included
long CSystemApplication::OnAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	long len;

	len = SetBufferText(mBuffer, "This is RaymonIOCP Server\r\n");
	NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, len, OP_SERVER_WRITE, opSide);
	return 1;
}

long CSystemApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	long len;

	len = AnalysisCommand(mContext, mBuffer, size);

	if ( len>0 ) NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, len, OP_SERVER_WRITE, OPSIDE_CLIENT);
	//	two op is different, attention
	if ( len==0 ) NoneProFunc(mContext->PProtocol, fPostReceive)(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, 0);
	return 0;
}

long CSystemApplication::AnalysisCommand(CContextItem* mContext, CListItem* &mBuffer, long size)
{
	char* bpointer = (char*)(mBuffer+1);								// maybe here should change
	char* lastpointer = bpointer + size;
	long* lpointer;
	long ret_err = 0x01;
	long ret = 0;
	char* tpointer;
	long len = 0;

	sockaddr_in addrEcho;
	addrEcho.sin_family = AF_INET;    
	addrEcho.sin_port = htons(1155);    
	addrEcho.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	sockaddr_in addrFwd;
	addrFwd.sin_family = AF_INET;    
	addrFwd.sin_port = htons(1166);    
	addrFwd.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	sockaddr_in addrHttp;
	addrHttp.sin_family = AF_INET;    
	addrHttp.sin_port = htons(80);    
	addrHttp.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

	sockaddr_in addrMSTSC;
	addrMSTSC.sin_family = AF_INET;    
	addrMSTSC.sin_port = htons(3389);    
	addrMSTSC.sin_addr.S_un.S_addr = inet_addr("192.168.23.129");

	sockaddr_in addrUDP;
	addrUDP.sin_family = AF_INET;    
	addrUDP.sin_port = htons(53);    
	addrUDP.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

	sockaddr_in addrDNS;
	addrDNS.sin_family = AF_INET;    
	addrDNS.sin_port = htons(53);    
	//		addrDNS.sin_addr.S_un.S_addr = inet_addr("192.168.1.1");
	addrDNS.sin_addr.S_un.S_addr = inet_addr("10.32.0.20");

	char filename[] = "c:\\ab.txt";
	char filenameto[] = "c:\\ac.txt";
	char httpFile[] = "c:\\Inetpub\\wwwroot";

	// the following line to prevent bad input	
	*lastpointer = 0;
	*(lastpointer+1) = 1;

	tpointer = bpointer;
	do 
	{
		if (*bpointer == '\r' ) 
		{
			*bpointer=0;
			tpointer = bpointer+1;
		}
		bpointer++;
	} while (bpointer<lastpointer);

	bpointer = (char*)(mBuffer+1);
	do 
	{
		while ( *bpointer == 0 ) bpointer++;

		lpointer = (long*)bpointer;
		switch (*lpointer & ~0x20202020)				// To UpCase
		{
		case 'TIUQ' :
			bpointer += strlen("Quit");
//			while ( *bpointer == 0 || *bpointer == ' ' ) bpointer++;
//			len = SetBufferText(mBuffer, "To quit\r\n");

			theService.SetExit();
			len = -1;						// to quit
			ret_err = 0x100;
			break;
		case 'LLEH' :
			bpointer += strlen("Hello");
			len = SetBufferText(mBuffer, "Hello\r\n");
			ret_err = 0x10;
			break;
		case 'RATS' :
			bpointer += strlen("Start");
// 			ret = CreateApplication(pWeb, pTCP, (void*)&addrHttp, sizeof(sockaddr_in), NULL, NULL, 0, 12 );
// 			ret = pWeb->AddProtocolPeer(pFileRead, (void*)httpFile, sizeof(httpFile), "\\tree");

			ret_err = 0x10;
			break;
		case 'POTS' :
			bpointer += strlen("Stop");
//			ret = CloseApplication(pForwardDup);
//			ret = CloseApplication(pWeb);
			ret_err = 0x10;
			break;
		}
		bpointer = strchr(bpointer, ' ');
		if (!bpointer) break;
		bpointer++;
		if (*bpointer == '\n' ) bpointer++;
	} while (bpointer<lastpointer);

	if ( ret_err == 0x01)
		len = SetBufferText(mBuffer, "Bad command\r\n");


	return len;
}




