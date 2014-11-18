
#include "stdafx.h"

extern CListItem*				isNULL;

#ifdef	FORWARDLOOP_FUNCTION

extern	ForwardLoopConfig	*pForwardLoopConfig;

// long CForwardLoopApplication::OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
// {
// 	return 0;
// }

long CForwardLoopApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	CForwardLoopApplication* pApp = (CForwardLoopApplication*)mContext->PApplication;
	CContextItem* retContext;
	long ret_err = 0x01;
	long ret = 0;
	char* hostname;
	long hostlen;

	__TRY
	
	ret_err = 0x10;
	if (mContext->PPeer != mContext)
	{
		ret = NoneAppFunc(mContext->PApplication, fOnClose)(mContext->PPeer, isNULL, FLAG_GRACE_CLOSE, opSide);
	}
	retContext = PreparePeer(mContext, (char*)NULL);
	if (!retContext) break;

	mContext->PPeer = retContext;
	retContext->PPeer = mContext;

	hostname = retContext->PeerProtocol->peerHost;
	hostlen = retContext->PeerProtocol->peerHostLen;
	if ((*((long*)hostname) | 0x40404040 ) == ISHTTP )
	{
		hostname += 7;
		hostlen -= 7;
	}
	ReplaceHttpRequest(mBuffer, "Host:", hostname, hostlen);
	ret = NoneProFunc(retContext->PProtocol, fPostConnect)(retContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT);
// 	}
// 	else
// 	{
// #ifdef	DEBUG_PEER
// 		printf("Same Peer\r\n");
// #endif	DEBUG_PEER
// 		retContext = mContext->PPeer;
// 		ReplaceHttpRequest(mBuffer, "Host:", retContext->PeerProtocol->peerHost, retContext->PeerProtocol->peerHostLen);
// 		ret = SendToPeer(mContext, mBuffer, size, OP_SERVER_WRITE);
// 	}

// 	if ((mContext->PPeer == mContext) && (pApp->ApplicationFlag & FLAG_MULTI_PROTOCOL))
// 	{
// 		retContext = PreparePeer(mContext, (char*)NULL);
// 		if (!retContext) break;
// 
// 		mContext->PPeer = retContext;
// 		retContext->PPeer = mContext;
// 		ReplaceHttpRequest(mBuffer, "Host:", retContext->PeerProtocol->peerHost, retContext->PeerProtocol->peerHostLen);
// 		ret = NoneProFunc(retContext->PProtocol, fPostConnect)(retContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT);
// 	}
// 	else
// 	{
// #ifdef	DEBUG_PEER
// 		printf("Same Peer\r\n");
// #endif	DEBUG_PEER
// 		retContext = mContext->PPeer;
// 		ReplaceHttpRequest(mBuffer, "Host:", retContext->PeerProtocol->peerHost, retContext->PeerProtocol->peerHostLen);
// 		ret = SendToPeer(mContext, mBuffer, size, OP_SERVER_WRITE);
// 	}
	if (ret) break;
	__CATCH(MODULE_APPLICATION, "CForwardLoopApplication::OnClientRead")
}

long CForwardLoopApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
#ifdef	DEBUG_PEER
	char buffer[SMALL_CHAR];
	if (!memcmp(REAL_BUFFER(mBuffer), NASZ("HTTP/1.")))
	{
		memcpy(buffer, REAL_BUFFER(mBuffer), 12);
		buffer[12]=0;
		printf("Return %12s\r\n",buffer);
	}
#endif	DEBUG_PEER
	return SendToPeer(mContext, mBuffer, size, OP_CLIENT_WRITE);																	//
}

long			ContentListenNumber;
CHARS			mForwardLoopName[MAX_FORWARDLOOP_LISTEN];
CForwardLoopApplication*	pForwardLoop[MAX_FORWARDLOOP_LISTEN];


long GetApplicationByName(char* name, CForwardLoopApplication* &pApp)
{
	int i;
	for (i=0; i<pForwardLoopConfig->ContentListenNumber; i++)
	{
		if (!strncmp(pForwardLoopConfig->mForwardLoopName[i], name, MAX_PATH))
		{
			pApp = pForwardLoopConfig->pForwardLoop[i];
			return 0;
		}
	}
	pApp = 0;
	return 0;
}
#endif	FORWARDLOOP_FUNCTION
