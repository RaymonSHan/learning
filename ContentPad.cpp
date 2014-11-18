

//	A BUG remember, just before <wafastart>, it MUST be define, not refer

#include "stdafx.h"
#include "SSLApplication.h"
// #include "ContentPad.h"

extern CMemoryListUsed*	TempUseMemory;							//	for program memory use, avoid use malloc				//
extern CService theService;
extern CListItem*				isNULL;

extern unsigned char ASCII_DEC[ASCII_NUMBER];
extern unsigned char ASCII_HEX[ASCII_NUMBER];
extern unsigned char TRAN_POST[ASCII_NUMBER];

#define WAFA_CHARSET_ORDER		2
#define WAFA_CHARSET			configInfo->ContentVar.AdditionArray[WAFA_CHARSET_ORDER]
int		sizeofCharset = 0;

#ifdef DEBUG_WAFA
#define MAX_DEBUG_STRING	10240-10
char wafadebug[MAX_DEBUG_STRING+10];
#endif DEBUG_WAFA


// #if ( defined(CONTENTPAD_APPLICATION) )
char MAIN404PAGE[MAX_PATH];
extern ContentPadConfig* configInfo;


long CContentPadServer::AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int opSide)
{
	if (opSide == OP_CLIENT_READ)
		return AnalysisHttpURL(mContext, mBuffer, size);
	if (opSide == OP_SERVER_READ && mContext->PProtocol->ProtocolNumber == PROTOCOL_TCP)
		return AnalysisHttpRespond(mContext, mBuffer, size);
	return 0;
}

long CContentPadServer::OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
#ifdef	DEBUG_CONCURRENCY
	printf("In OnConnect\r\n");
#endif	DEBUG_CONCURRENCY
	mContext->PApplication->FreeApplicationBuffer(mBuffer);
	return 0;
}

long CContentPadServer::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	CContextItem *serContext = 0;
	CListItem *fileBuffer, *headBuffer;

	long messagelength = 0;
	long ret_err = 0x01;																											//
	__int64 fileLength;
	CHTTPContext* httpContext = (CHTTPContext*)mContext;
	URLInfo* urlinfo = &(httpContext->urlInfo);
	long ret;
	char* userPara = 0;
	long userParaLength = 0;
	int urllength = 0;
	int filenamelength = 0;
	CListItem* usedBuffer = mBuffer;

	ContentPad* clientPad = usedBuffer->HeadInfo;
	long redirectloop = 0;

	__TRY

	ret_err = 0x10;
	memset(clientPad, 0 , sizeof(ContentPad));

	usedBuffer = ProcessFullCommand(mContext, usedBuffer, size);
	if (usedBuffer->NProcessSize < 0) break;

	if (usedBuffer->NProcessSize == 0)
	{
		ret_err = 0x11;
		if (mContext->ContentMode == CONTENT_MODE_LENGTH)
		{
			headBuffer = mContext->PApplication->GetApplicationBuffer();
			if (!headBuffer) break;
			AddtoContentList(mContext, usedBuffer, CONTENT_NAME_CLIENTHEAD);
			usedBuffer = headBuffer;
		}

		ret_err = 0x12;
		ret = NoneProFunc(mContext->PProtocol, fPostReceive)
			(mContext, usedBuffer, usedBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide);
		if (!ret) ret_err = 0;
		break;
	}

	ret_err = 0x20;
	fileBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!fileBuffer) break;
	AddtoContentList(mContext, usedBuffer, CONTENT_NAME_CLIENT);				//	after PreparePeer, the mContext will NOT change again
	mBuffer = 0;

//	add here for use %d97 for boundary at any time // Sept. 28 '13
	ContentPad* cliPad;
	cliPad = usedBuffer->HeadInfo;
	cliPad->resultKey[HEADINFO_BOUNDARY] = &(urlinfo->getBoundary[0]);
	//	here 7 is the order of AnalysisHttpURL
	cliPad->getLength[HEADINFO_BOUNDARY] = urlinfo->getLength[7];				// the only fast way to finish multipart boundary

	while (TRUE)
	{
		ret_err = 0x30;
		if (mContext->PPeer != mContext) serContext = mContext->PPeer;
		else serContext = PreparePeer(mContext, fileBuffer);			// should change	//	May 08 '14,	Attention AddContentList

		ret_err = 0x40;
		if (serContext)
		{
			mContext->PPeer = serContext;			// normally add this just after accept
			serContext->PPeer = mContext;			// add this before connect

			if (urlinfo->getLength[MODE_GET] != VALUE_NOT_FOUND) urllength = urlinfo->getLength[MODE_GET];
			else if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND) urllength = urlinfo->getLength[MODE_POST];

			if (urllength)
			{
				userPara = (char*)memchr(urlinfo->getURL, '?', urllength);
				if (userPara)
				{
					userParaLength = urllength - ( userPara - urlinfo->getURL );
					GetClientPara(mContext, usedBuffer, urlinfo->getURL, urllength, 0);
					filenamelength = userPara - urlinfo->getURL;
				}
				else filenamelength = urllength;
memcpy( (char*)(fileBuffer+1), urlinfo->getURL, filenamelength );			// unkonw reason, have to add this, Sept 2 '13
			}

			ret_err = 0x50;
			if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND )
			{
				userPara = (char*)(usedBuffer+1);
				userParaLength = usedBuffer->NProcessSize;
				GetClientPara(mContext, usedBuffer, userPara, userParaLength, POST_PARAMETER_START);
			}

			*(((char*)(fileBuffer+1))+filenamelength) = 0;
			fileBuffer->NProcessSize = filenamelength;
			urlinfo->httpRedirect = FALSE;
		}

		ret_err = 0x60;
		if ( !serContext || NoneProFunc(serContext->PProtocol, fPostConnect)(serContext, fileBuffer, fileBuffer->NProcessSize, OP_CONNECT) )
		{
			// 			if (!strncmp(urlinfo->getURL, MAIN404PAGE, strlen(MAIN404PAGE))) break;		// do not found 404 file
			// redirect to 404 by httpRedirect
			strcpy_s(urlinfo->getURL, MAX_URL, MAIN404PAGE);	//	for file PostConnect use
// should copy to buffer + 1 ????
			urlinfo->httpResult = HTTP404;
			urlinfo->httpRedirect = TRUE;
			filenamelength = strlen(MAIN404PAGE);

			//	have error when 404 not found, not free filecontext
			if (urlinfo->getLength[MODE_GET] != VALUE_NOT_FOUND) urlinfo->getLength[MODE_GET] = filenamelength;
			else if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND) urlinfo->getLength[MODE_POST] = filenamelength;

			mContext->PPeer->ListFlag |= FLAG_KEEPALIVE;
			redirectloop++;
			if (redirectloop>3) break;
		}

		ret_err = 0;
		if ( !urlinfo->httpRedirect ) break;

		ret_err = 0x70;
		mContext->PPeer = mContext;
		if (serContext)
		{
			serContext->PPeer = NULL;
			if ( ProFunc(serContext->PProtocol, fPostClose)(serContext, isNULL, 0, 0) ) break;										//
		}
	}
	if ( ret_err ) break;

	ret_err = 0x80;
	fileLength = (serContext->PProtocol->GetContextLength)(serContext, fileBuffer);
	if (!fileLength)
		break;

	ret_err = 0x90;
	serContext->ContentMode = CONTENT_MODE_LENGTH;			//	For file read, without HTTP head
	ret = NoneProFunc(serContext->PProtocol, fPostReceive)
		(serContext, fileBuffer, fileBuffer->BufferType->BufferSize, OP_SERVER_READ, 0);
	if (ret) break;

	__CATCH_BEGIN(MODULE_PROTOCOL, "CContentPadServer - OnClientRead")
		RET_BETWEEN(0x30, 0x90) mContext->PApplication->FreeApplicationBuffer(fileBuffer);
	__CATCH_END
}																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//

long CContentPadServer::OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
// 	ContentPad* pad = mBuffer->HeadInfo;

	mBuffer->NProcessSize = 0;

	int protocolnumber = mContext->PPeer->PProtocol->ProtocolNumber;

//	Now mContent->PPeer may equal mContext for TCP		//	May 25 '14
	if ( (mContext->PPeer != mContext) && (protocolnumber == PROTOCOL_FILE) )
// 	if (mContext->PPeer != mContext)									//	for FILE
	{
		return ( NoneProFunc(mContext->PPeer->PProtocol, fPostReceive)																//
			(mContext->PPeer, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, opSide) );									//
	}
	else
	{
//	After ClientWrite, should close client		//	May 25 '14
		return NoneAppFunc(mContext->PApplication, fOnClose)(mContext, mBuffer, 0, opSide);
// 		return ( NoneProFunc(mContext->PProtocol, fPostReceive)																		//
// 			(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide) );											//
	}
}

//	mContext is SERVER side
long CContentPadServer::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	int nProtocol = mContext->PProtocol->ProtocolNumber;
	CContextItem* cliContext = mContext->PPeer;
	long ret_err = 0x01;
	long ret, retclose = 0;
	ContentPad* clientPad = mBuffer->HeadInfo;
	CListItem* headBuffer;
	char* tpointer;
	URLInfo* urlinfo = &(((CHTTPContext*)mContext)->urlInfo);
	CListItem* usedBuffer;

	__TRY

// 	printf("		In OnServerRead %x, peer:%x\r\n", mContext, mContext->PPeer);
	ret_err = 0x10;
	if (!cliContext) break;
#ifdef	DEBUG_CONCURRENCY
	printf("in server read, cliContext is %x\r\n", cliContext);
// 	printf("ServerRead buffer:%d, name:%s, context:%s\r\n", mBuffer->NProcessSize, mContext->ServerName, REAL_BUFFER(mBuffer));

#endif	DEBUG_CONCURRENCY

	memset(clientPad, 0 , sizeof(ContentPad));

	if (nProtocol==PROTOCOL_FILE_READ)
	{
		if (mContext->OverlapOffset > mBuffer->BufferType->BufferSize)		//	file large than buffer
		{
			ret = NoneProFunc(cliContext->PProtocol, fPostSend)
				(cliContext, mBuffer, size, OP_CLIENT_WRITE, opSide);
			if (!ret) ret_err = 0;
			break;
		}
// now remark it again May 22 '14
// // /*			//	change Peer link test, replace this lines, and test		// Oct. 24 '13
// 		ret_err = 0x20;
// //	cliContext will open until send back to client, but fileContext will close soon, just after mBuffer is record
// //	but now, cliContext->PPeer is WafaDefine HOST, is not mContext.			add in Aug 23 '13
// 		cliContext->PPeer = cliContext;							//	Add this two line		//	Jun 21 '13
// 		mContext->PPeer = mContext;								//	Add this two line		//	Jun 21 '13
// //	SHOULD DEBGU	// In my memory, it should be cliContext->PPeer = mContext; mContext->PPeer = mContext	// Oct. 22 '13
// //	or means the above two lines should move after OnHttpFileRead, and before fOnClose ?? Then the following is not need	// Oct. 22 '13

//	for fileLength which get in CContentPadServer::OnClientRead is lost for use CLIENT context,
//	The length is useful for Normal file return, via HTTP		//	Add Oct 22 '13
		((CFileContext*)cliContext)->fileLength = ((CFileContext*)mContext)->fileLength;
		((CFileContext*)cliContext)->fileNameLength = ((CFileContext*)mContext)->fileNameLength;

//	Restore PPeer link and close fileContext
//	for Encap, Decap, the NoneAppFunc::fOnClise will not close mContext while (peerContext->PPeer != mContext)
		ret = OnHttpFileRead(cliContext, mBuffer, size);
		if (ret) break;
// */

//	following is alternative, have not heavy test, But I have received release twice		// Oct. 24 '13
// 		ret_err = 0x20;
// 		cliContext->PPeer = cliContext;
// 		ret = OnHttpFileRead(cliContext, mBuffer, size);
// 		if (ret) break;
// 		mContext->PPeer = mContext;
//	above is alternative


//	It is a ERROR, for close the fileContext while read normal file, such as attachment		//	Oct. 22 '12
		ret = NoneAppFunc(mContext->PApplication, fOnClose)(mContext, isNULL, FLAG_GRACE_CLOSE, opSide);
		if (ret) break;
	}
	else if (nProtocol==PROTOCOL_TCP || nProtocol==PROTOCOL_TCP_POOL)
	{
#ifdef	DEBUG_NTLM
		char* lineend = (char*)memchr(REAL_BUFFER(mBuffer), 0xd, 100);
		if (lineend) 
		{
			*lineend = 0;
			printf("%s\r\n", REAL_BUFFER(mBuffer));
			*lineend = 0xd;
		}
#endif	DEBUG_NTLM

		ret_err = 0x30;
		mBuffer = ProcessFullCommand(mContext, mBuffer, size);
// 		if (mBuffer->NProcessSize < 0) break;
		if (mBuffer->NProcessSize & 0x10000000) 
			break;

		if (mBuffer->NProcessSize == 0)
		{
			ret_err = 0x40;
// 			if (mContext->ContentMode == CONTENT_MODE_LENGTH)
// 			{
// 				tpointer = memstr(REAL_BUFFER(mBuffer), mBuffer->BufferType->BufferSize, NASZ("\r\n\r\n"));
// 				mBuffer->NProcessSize = tpointer - REAL_BUFFER(mBuffer);				//	for SHEAD use, should set NProcessSize, it is a lazy way.
// 
// 				//	Here means, this is POST head. add POST head to content link
// 				headBuffer = mContext->PApplication->GetApplicationBuffer();
// 				if (!headBuffer) break;
// // 				AddtoContentList(mContext, mBuffer, CONTENT_NAME_SERVERHAED);
// 
//  				AddtoContentList(cliContext, mBuffer, CONTENT_NAME_SERVERHAED);		//	should add to client side Sept 09 '13
// 
// 				mBuffer = headBuffer;
// 			}
			if (mContext->ContentMode == CONTENT_MODE_LENGTH)
			{
				tpointer = memstr(REAL_BUFFER(mBuffer), mBuffer->BufferType->BufferSize, NASZ("\r\n\r\n"));
				if (tpointer)
				{
					mBuffer->NProcessSize = tpointer - REAL_BUFFER(mBuffer);				//	for SHEAD use, should set NProcessSize, it is a lazy way.

					//	Here means, this is POST head. add POST head to content link
					headBuffer = mContext->PApplication->GetApplicationBuffer();
					if (!headBuffer) break;
 					AddtoContentList(cliContext, mBuffer, CONTENT_NAME_SERVERHAED);		//	should add to client side Sept 09 '13
					mBuffer = headBuffer;
				}
			}
//	Attention, while Server reply only head, without body, the tpointer will be 0 for sometime.		// Dec. 11 '13
//	At that time, use the really contentname, inside of SHEAD

			ret_err = 0x41;
			ret = NoneProFunc(mContext->PProtocol, fPostReceive)
				(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, opSide);
			if (!ret) ret_err = 0;
			break;
		}
//	re-remove it for keep-alive, May 22 '14
// 		cliContext->PPeer = cliContext;							//	Add this two line		//	Jun 21 '13
// 		mContext->PPeer = mContext;								//	Add this two line		//	Jun 21 '13
		//	Restore PPeer link and close fileContext
		//	for Encap, Decap, the NoneAppFunc, fOnClise will not close mContext while (peerContext->PPeer != mContext)

		ret_err = 0x50;
		ret = TranCookieInfo(cliContext, mContext, 3, 3);		//	all for server side, respond order, for cookie
		ret = TranCharsetInfo(cliContext, mContext, 4, 4);		//	for CHARSET

#ifdef	DEBUG_CONCURRENCY
// 		printf("ServerRead buffer2:%d, name:%s, context:%s\r\n", mBuffer->NProcessSize, mContext->ServerName, REAL_BUFFER(mBuffer));
#endif	DEBUG_CONCURRENCY

//	A lazy way for fast finish GHGC
// 		if (urlinfo->httpResult == HTTP200)						//	Add for control other respond than 200	//	Apr. 28 '14
		{
			AddtoContentList(cliContext, mBuffer, mContext->ServerName);		//	for concurrency use, server side pad name saved in server context Apr. 26 '14
			usedBuffer = NULL;
		}
// 		else
// 			usedBuffer = mBuffer;

		ret = OnRemoteServerRead(cliContext, mBuffer, size);	//	if this fail, should close mContext, then return error	// Oct. 27 '13 by ConfirmDNS report
// 		if (ret) break;											//	if Above falil, should close mContext
		// 		ret_err = 0x60;
//	add this condition for not close keep-alive context
		if (mContext->PPeer == mContext)		//	Add this line in May 22 '14
		{
			printf ("in serverread, to close old peer\r\n");
			retclose = NoneAppFunc(mContext->PApplication, fOnClose)(mContext, usedBuffer, FLAG_GRACE_CLOSE, opSide);
		}
		if (ret || retclose) break;
	}

	__CATCH(MODULE_APPLICATION, "CContentPadServer - OnServerRead")
}																																	//

long CContentPadServer::OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	//	In fact it will not be run
	mBuffer->NProcessSize = 0;
	return ( NoneProFunc(mContext->PProtocol, fPostReceive)																			//
		(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, opSide) );														//
}


long CContentPadServer::OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)
{
	CListItem* usedBuffer = mBuffer;
	CContextItem* peerContext = mContext->PPeer;
//	The ONLY thing to handle timeout for concurrency is this funciton		//	Apr. 28 '14
// 	OnRemoteServerRead(mContext->PPeer, isNULL, 0);
// 	if (size) OnRemoteServerRead(mContext->PPeer, isNULL, 0);				//	Apr. 29 '14

	printf("In ContentPad close mContext:%x, peer:%x\r\n", mContext, mContext->PPeer);
	if (mBuffer->NOperation == OP_CONNECT)
	{
		OnRemoteServerRead(mContext->PPeer, mBuffer, 0);		//	Apr. 30 '14
		FreeApplicationBuffer(usedBuffer);
		if (peerContext) peerContext->PPeer = peerContext;
		mContext->PPeer = mContext;
		return 0;
// 		if (mBuffer) usedBuffer = mBuffer;
// 		return CApplication::OnClose(mContext, isNULL, FLAG_GRACE_CLOSE, opSide);
	}
// 	else
// 		return CApplication::OnClose(mContext, mBuffer, size, opSide);
		return CApplication::OnClose(mContext, mBuffer, FLAG_GRACE_CLOSE, opSide);

}

//	mContext is CLIENT context
//	from "<!--wafadefine" to "/wafadefine-->"
long CContentPadServer::OnHttpFileRead(CContextItem* mContext, CListItem* &mBuffer, long size)										//
{
	long ret_err = 0x01;																											//
	long ret;

	//	mContext is CLIENT context
	char* bufferStart = REAL_BUFFER(mBuffer);
	char* bufferEnd  = bufferStart + mBuffer->NProcessSize;
	char* searchEnd = 0;
	char* wafaStart = 0, *wafaEnd = 0;
	ContentPad* cliPad;
	URLInfo* urlinfo = &((CHTTPContext*)mContext)->urlInfo;
	CListItem* usedBuffer = mBuffer;//, *cliContent;

	__TRY

	if (urlinfo->getLength[MODE_POST] != VALUE_NOT_FOUND) 
		bufferStart = memstr(bufferStart, mBuffer->NProcessSize, WAFA_POST_MARK, 15);
	if (!bufferStart) bufferStart = REAL_BUFFER(mBuffer);

	AddtoContentList(mContext, usedBuffer, CONTENT_NAME_FILE);
	mBuffer = 0;												//	add content "FILE"
	cliPad = GetPadByName(mContext, CONTENT_NAME_CLIENT);

	ret_err = 0x10;
	searchEnd = memstr(bufferStart, bufferEnd - bufferStart, NASZ(WAFA_BODY_START));
	if (searchEnd)
	{
		ret_err = 0x20;
		searchEnd += (sizeof(WAFA_BODY_START)-1);
		cliPad->checkStart = bufferStart;
		cliPad->checkEnd = searchEnd;								// any search will stop here, even NO search

//	The full concurrency should send request here, Instead of OnConnect					//	Apr. 28 '14
//		ret = PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
// 		do 
// 		{
		ret = PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
// 			if (ret) break;
// 		} while (wafaStart && wafaEnd && usedBuffer->HeadInfo->inConcurrency == CONCURRENCY_GOON);	// here usedBuffer is fileContent
		if (ret) break;

	}
	else
	{
		cliPad->checkEnd = 0;										//	This is the MARK for normal file
//  		PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
		ret = ProcessNormalReplay(mContext, usedBuffer);
		if (ret) break;
	}

// 	ret_err = 0x30;
// 	if (!searchEnd || !wafaStart || !wafaEnd) PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);

	__CATCH(MODULE_APPLICATION, "CContentPadServer - OnHttpFileRead")
}

//	mContext is CLIENT context
long CContentPadServer::OnRemoteServerRead(CContextItem* mContext, CListItem* &mBuffer, long size)									//
{
	CListItem* cliContent, *fileContent;
	ContentPad* cliPad;
	char* wafaStart, *wafaEnd;
	long ret;
	long ret_err = 0x01;
	CListItem* usedBuffer = mBuffer;
	MYINT nowCon;

// 	char iiipath[100]="c:\\log\\";
// char iiibuf[20];
// static int iii = 100;
// itoa(iii, iiibuf, 10);
// strcat(iiipath, iiibuf);
// char iscr[]= "\r\n\r\n";
// FILE* fi;
// fi = fopen(strcat(iiipath, iiibuf), "w+b");
// CListItem* shead = GetContentByName(mContext, CONTENT_NAME_SERVERHAED);
// // if (!cliContent) break;
// fwrite(REAL_BUFFER(shead), 1, shead->NProcessSize, fi);
// fwrite(iscr, 1, 4, fi);
// fwrite(REAL_BUFFER(mBuffer), 1, mBuffer->NProcessSize, fi);
// fclose(fi);
// iii++;

	__TRY
	ret_err = 0x10;
	cliContent = GetContentByName(mContext, CONTENT_NAME_CLIENT);
	if (!cliContent) 
		break;

	ret_err = 0x20;
	cliPad = cliContent->HeadInfo;

//	Change this line to OnServerRead for concurrency, but it may cause bug about buffer release when socket error happen.	//	Apr. 26 '14
// 	AddtoContentList(mContext, usedBuffer, cliContent->HeadInfo->serverName);
	if (mBuffer) mBuffer = 0;

//////	GREAT change for concurrency		// Apr. 26 '14
//	// 	PREPARE_DEFINE(FALSE)
// 	ret = PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
// 	if (ret) break;				//	should ? Sept. 18 '13
// 
// 	ret_err = 0x30;
// 	if (!wafaStart || !wafaEnd) PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);

	if (usedBuffer) usedBuffer->HeadInfo->inConcurrency = CONCURRENCY_OK;				//	Mark self is OK
	nowCon = InterDec(&cliPad->inConcurrency);
#ifdef	DEBUG_CONCURRENCY
	if (usedBuffer) printf("Get Server Read: %s, nowCon: %d\r\n", usedBuffer->HeadInfo->nameFor, nowCon);
#endif	DEBUG_CONCURRENCY

	if (!nowCon)											// It seemed all request returned
	{
		fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
		if (!fileContent) break;
		if (fileContent->HeadInfo->inConcurrency == CONCURRENCY_PAUSE)		//	Yes, all returned		// why filepad->inConcurrency is not pause ?
		{
#ifdef	DEBUG_CONCURRENCY
			DisplayContentByName(mContext);
			printf("All Finish, do prepare again. \r\n");
#endif	DEBUG_CONCURRENCY

// 			ret = PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
// 			do 
// 			{
			ret = PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);

// 			ret = PrepareDefine(mContext, fileContent, cliPad, 0, wafaStart, wafaEnd);
// 				ret = PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
// 				if (ret) break;
// 			} while (wafaStart && wafaEnd && fileContent->HeadInfo->inConcurrency == CONCURRENCY_GOON);
			if (ret) break;				//	should ? Sept. 18 '13

// 			ret_err = 0x30;		//	should do such thing	//	Apr. 29 '14
// 			if ( (!wafaStart || !wafaEnd) && !cliPad->inConcurrency )
// 				PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
		}
	}

	__CATCH(MODULE_APPLICATION, "CContentPadServer - OnRemoteServerRead")
}

long ProcessConfigVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror)
{
// 	ContentForward* fInfo;
	ConfigAdditionArray* wAddition;
// 	char listenaddr [MAX_PATH];
	int i;

	//	config wafa, should NOT with error control, it MUST NOT have error.
	if (varerror) return 0;
	if (!configInfo) return 0;

	if (hInfo->resultKey[40] != 0)
	{
		i = 0;
		wAddition = &(configInfo->ContentVar);
		while (hInfo->getLength[i+40] != 0)
		{
			COPY_PAD_CHARS(wAddition, i, hInfo, i+40);
			i++;
		}
		hInfo->resultKey[40] = 0;
		sizeofCharset = strlen(wAddition->AdditionArray[WAFA_CHARSET_ORDER]);
	}
	else if (hInfo->resultKey[30] != 0)
	{
		i = 0;
		wAddition = &(configInfo->ContendArray)[configInfo->ContentAdditionArrayNumber];
		while (hInfo->getLength[i+30] != 0)
		{
			COPY_PAD_CHARS(wAddition, i, hInfo, i+30);
			i++;
		}
		configInfo->ContentAdditionArrayNumber ++;
//	No error control here, only display a message	// Nov. 15 '13
		if (configInfo->ContentAdditionArrayNumber > MAX_ADDITION_ARRAY)
			printf("ERROR!, max user reach!!, unexpect error may happen\r\n");
		hInfo->resultKey[30] = 0;
	}
// 	else if (hInfo->resultKey[20] != 0)
// 	{
// 		fInfo = &(configInfo->cfc)[configInfo->ContentForwardNumber];
// 		if (!memcmp(hInfo->resultKey[20], NASZ("fileread"))) fInfo->ContentForwardProtocol = PROTOCOL_FILE_READ;
// 		else if (!memcmp(hInfo->resultKey[20], NASZ("https"))) fInfo->ContentForwardProtocol = PROTOCOL_INTER;
// 		else if (!memcmp(hInfo->resultKey[20], NASZ("http"))) fInfo->ContentForwardProtocol = PROTOCOL_TCP;
// 		else if (!memcmp(hInfo->resultKey[20], NASZ("oci"))) fInfo->ContentForwardProtocol = PROTOCOL_OCI;
// 		else if (!memcmp(hInfo->resultKey[20], NASZ("udp"))) fInfo->ContentForwardProtocol = PROTOCOL_UDP;
// 		else if (!memcmp(hInfo->resultKey[20], NASZ("dns"))) fInfo->ContentForwardProtocol = PROTOCOL_SINGLEUDP;
// 		else fInfo->ContentForwardProtocol = 0;
// 
// 		COPY_PAD_STRING(fInfo->ContentForwardHost, hInfo, 21);
// 		COPY_PAD_STRING(fInfo->ContentForwardName, hInfo, 22);
// 		COPY_PAD_STRING(fInfo->ContentForwardAddress, hInfo, 23);
// 		COPY_PAD_INT(fInfo->ContentForwardPort, hInfo, 24);
// 		COPY_PAD_INT(fInfo->ContentForwardAcceptNumber, hInfo, 25);
// 
// 		configInfo->ContentForwardNumber ++;
// 		hInfo->resultKey[20] = 0;
// 	}
// 	else if (hInfo->resultKey[10] != 0)
// 	{
// 		if (!memcmp(hInfo->resultKey[10], "https", 5)) configInfo->ContentListenProtocol = PROTOCOL_INTER;
// 		else if (!memcmp(hInfo->resultKey[10], "http", 4)) configInfo->ContentListenProtocol = PROTOCOL_TCP;
// 		else configInfo->ContentListenProtocol = PROTOCOL_TCP;
// 
// 		configInfo->ContentListenAddress.sin_family = AF_INET;
// 		memcpy(listenaddr, hInfo->resultKey[11], hInfo->getLength[11]);
// 		listenaddr[hInfo->getLength[11]] = 0;
// 		configInfo->ContentListenAddress.sin_addr.S_un.S_addr = inet_addr(listenaddr);
// 		configInfo->ContentListenAddress.sin_port = htons((WORD)(GetInt(hInfo->resultKey[12], hInfo->getLength[12])));
// 		configInfo->ContentListenPostAccept = GetInt(hInfo->resultKey[13], hInfo->getLength[13]);
// 		hInfo->resultKey[10] = 0;
// 	}
	else if (hInfo->resultKey[0] != 0)
	{
		COPY_PAD_INT(configInfo->ContentMemoryHttp, hInfo, 0);
		COPY_PAD_INT(configInfo->ContentMemoryFile, hInfo, 1);
		COPY_PAD_INT(configInfo->ContentMemoryBuffer, hInfo, 2);
		COPY_PAD_INT(configInfo->ContentThread, hInfo, 3);
		hInfo->resultKey[0] = 0;
	}

	return 0;
}

CListItem* ProcessFullCommand(CContextItem* mContext, CListItem* mBuffer, long size)
{
	CContextItem* messContext;
	CListItem *newBuffer = mBuffer;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x10;
	if (mContext->ContentMode == CONTENT_MODE_HEAD)
	{
		//	this command have only head, without body
		//	and NProcessSize is NOT 0
	}

	ret_err = 0x20;
	if (mContext->ContentMode == CONTENT_MODE_LENGTH)
	{
		messContext = mContext->MessageContext;

		ret_err = 0x30;
		if (mContext->TransferEncoding == ENCODING_LENGTH)
		{
			messContext->BodyRemain = mContext->BodyRemain;		// when BodyRemain is 0, means the packet is over
			newBuffer = ProcessMoreCommand(messContext, mBuffer, size);
		}
		else if (mContext->TransferEncoding == ENCODING_CHUNKED)
		{
			newBuffer = ProcessChunkedCommand(messContext, mBuffer, size);
		}

		if (newBuffer->NProcessSize > 0)
		{
			mContext->ContentMode = CONTENT_MODE_HEAD;
			mContext->PProtocol->FreeProtocolContext(messContext);
			mContext->MessageContext = 0;
		}
	}

	ret_err = 0x40;
	if (mContext->ContentMode == CONTENT_MODE_AFTERHEAD)
	{
		//	head is over, should receive body
		mContext->ContentMode = CONTENT_MODE_LENGTH;
		newBuffer->NProcessSize = 0;

		ret_err = 0x50;
		if (!mContext->MessageContext)
		{
			mContext->MessageContext = mContext->PProtocol->GetProtocolContext();
			if (!mContext->MessageContext)
			{
				newBuffer->NProcessSize = BUFFER_TOO_LARGE;
				break;
			}
		}
		messContext = mContext->MessageContext;
		messContext->PPeer = messContext;

		ret_err = 0x60;
		if (!messContext->MoreBuffer)
		{
			messContext->MoreBuffer = mContext->PApplication->GetApplicationBuffer();
			if (!messContext->MoreBuffer)
			{
				newBuffer->NProcessSize = BUFFER_TOO_LARGE;
				break;
			}
		}
		messContext->MoreBuffer->NProcessSize = 0;
	}

	__CATCH_BEGIN(MODULE_APPLICATION, "ProcessFullCommand")
	__CATCH_END_
	return newBuffer;																												//
}																																	//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	mContext is CLIENT side, mBuffer is data from OnServerRead																		//
//	wafaStart point to the begin of WAFA_DEFINE_START : "<!--wafadefine"															//
//	wafaEnd point to the end of WAFA_DEFINE_END : "/wafadefine-->"																//
//	lastStart is the end of last wafadefine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	change declare May 01 '14 for change checkstart in concurrency
// long ProcessWafaDefineRefer(CContextItem* mContext, CListItem* &mBuffer, char* wafaStart, char* wafaEnd, char* lastStart)
long ProcessWafaDefineRefer(CContextItem* mContext, CListItem* &mBuffer, ContentPad* pad, char* wafaStart, char* wafaEnd)
{
	long ret = 0;
	char *referStart, *nextRefer;
	char* isCharNull = 0;
	long ret_err = 0x01;
	CListItem* fileContent, *cliContent;
	char* lastStart = pad->checkStart;
	char* referend;

	__TRY
	ret_err = 0x20;
	fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
	if (!fileContent) break;
	ret_err = 0x30;
	cliContent = GetContentByName(mContext, CONTENT_NAME_CLIENT);
	if (!cliContent) break;

	referend = wafaStart ? wafaStart : pad->checkEnd;
	ret_err = 0x10;
	referStart = memstr(lastStart, referend-lastStart, NASZ(WAFA_REFER_START));
	while (referStart)
	{
		nextRefer = memstr(referStart+sizeof(WAFA_REFER_START), referend-referStart, NASZ(WAFA_REFER_START));
		if (!nextRefer) nextRefer = referend;

		ret_err = 0x40;
		if (cliContent->HeadInfo->inConcurrency)						//	If no request has been send, continued			//	May 02 '14
		{
#ifdef	DEBUG_CONCURRENCY
			printf("Find Refer, pause concurrency by definereder. \r\n");
#endif	DEBUG_CONCURRENCY
			fileContent->HeadInfo->inConcurrency = CONCURRENCY_PAUSE;	//	any refer pause concurrency	//	Apr. 26 '14
			return 0;													//	This means not all respond have been returned	//	May 01 '14
		}
// 		fileContent->HeadInfo->inConcurrency = CONCURRENCY_PAUSE;		//	any refer pause concurrency	//	Apr. 26 '14
// 		if (cliContent->HeadInfo->inConcurrency) return 0;				//	This means not all respond have been returned	//	May 01 '14

		//	process <wafarefer> to <wafarefer> | WAFA_BODY_END, step by </wafarefer>
		ret_err = 0x50;
		ret = ProcessWafaRefer(mContext, mBuffer, isCharNull, referStart, nextRefer, ProcessWafaDefineInRefer);	// after done, nowfile will pointer nextRefer
		if (ret || nextRefer == referend) break;			// error or end

		referStart = nextRefer;								// to solve two or more wafarefer between wafadefine.	// two hours work for this in Sept. 28 '13
	}
	if (ret) break;
	ret_err = 0x60;
	if (wafaStart)											//	Add this for refer just before wafastart	//	May 01 '14
		ret = ProcessWafaDefine(mContext, mBuffer, wafaStart, wafaEnd);	
	else
		fileContent->HeadInfo->inConcurrency = CONCURRENCY_PAUSE;		//	end of define pause concurrency	//	May 01 '14

//	Change into here from PrepareDefine		//	May 01 '14
	if ((lastStart == pad->checkStart) && wafaEnd)		// for nested call by RESTART, if the nest have changed, here do NOT change Sept 12 '13
	{
		pad->checkStart = wafaEnd;
// 		oldCheckStart = pad->checkStart;
	}
	__CATCH(MODULE_APPLICATION, "ProcessWafaDefineRefer")
}

long ProcessWafaDefineInRefer(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror)
{
	return 0;
}

unsigned char WafaDefineMap[ASCII_NUMBER] = {
// 	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	IS_END,	0x00,	0x00,	IS_END,	0x00,	0x00,
 	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	IS_SPC,	0x00,	IS_END,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	IS_KEY,	0x00,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	IS_KEY,	0x00,	0x00,	IS_KEY,	0x00,	IS_KEY,	IS_KEY,	0x00,
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
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};

KeyParaTran WafaDefinePara[] = { sNASZ("Action=\""), sNASZ("Method=\""), sNASZ("Host=\""), sNASZ("Name=\""), sNASZ("Key=\""),
		sNASZ("Referer=\""), sNASZ("Cookie=\""), sNASZ("ReferName=\""), {NULL} };

//	is it can be called BUG?
long abcfunction(void)
{
	int abc = 0;
	while(TRUE)
	{
		abc = 1;			// the asm file said, it is the abc in while loop; but it will compile error without main abc
		int abc = -1;
		abc = 2;
	}
	ASSERT(abc == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	mContext is CLIENT side, mBuffer is data from OnServerRead																		//
//	defineStart point to the begin of WAFA_DEFINE_START : "<!--wafadefine"															//
//	defineEnd point to the end of WAFA_DEFINE_END : "/wafadefine-->"																//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long ProcessWafaDefine(CContextItem* mContext, CListItem* &mBuffer, char* defineStart, char* defineEnd)		
{
	long ret_err = 0x01;																											//
	ContentPad *cliPad, *bufPad = mBuffer->HeadInfo;
	long ret;
	char* tpointer, *lengthPlace, *headPlace, *namefor;
	CContextItem* oldSerContext;

// if (mContext->PPeer == mContext) oldSerContext = 0;
// if (mContext->PPeer && mContext->PPeer->PProtocol->ProtocolNumber == PROTOCOL_FILE_READ) 
// 	oldSerContext = mContext->OldPeer;
// if (mContext->PPeer && mContext->PPeer->PProtocol->ProtocolNumber == PROTOCOL_TCP) 
// 	oldSerContext = mContext->PPeer;

	oldSerContext = (mContext == mContext->PPeer) ? 0 : mContext->PPeer;
	CContextItem* serContext;
	CListItem* referContent, *cliContent, *fileContent;
	CListItem* serBuffer, *usedBuffer = mBuffer;
	URLInfo* urlinfo = &((CHTTPContext*)mContext)->urlInfo;
// 	ContentForward* cf;
	long	methodNum = 0;		//0 for GET, 1 for POST, 2 for MODEL
//	now GET/POST for TCP only, MODEL for FILEREAD only

	char* wafaStart = 0, *wafaEnd = 0;
	char* bufferStart, *searchEnd;
	WafaCreateFunciont createFunction = NULL;

#ifdef DEBUG_WAFA
	memcpy (wafadebug, defineStart, min(defineEnd-defineStart, MAX_DEBUG_STRING));
	wafadebug[min(defineEnd-defineStart, MAX_DEBUG_STRING)] = 0;
	printf("  For define:%s\r\n", wafadebug);
#endif DEBUG_WAFA

	__TRY

	ret_err = 0x10;
	SET_PARA_STR(bufPad, PARA_DEF_ACTION, serverAction)
	SET_PARA_STR(bufPad, PARA_DEF_METHOD, serverMethod)
	SET_PARA_STR(bufPad, PARA_DEF_HOST, serverHost)
	SET_PARA_STR(bufPad, PARA_DEF_NAME, serverName)
	SET_PARA_STR(bufPad, PARA_DEF_KEY, serverKey)
	SET_PARA_STR(bufPad, PARA_DEF_REFERER, serverReferer)
	SET_PARA_STR(bufPad, PARA_DEF_COOKIE, serverCookie)
	SET_PARA_STR(bufPad, PARA_DEF_REFERNAME, serverReferName)
	if ( CommandLineParaTran( defineStart, defineEnd-defineStart, NULL, NULL, WafaDefinePara, bufPad->valPlace, WafaDefineMap) ) break;	//


	ret_err = 0x20;
	if ( bufPad->getLength[PARA_DEF_REFERNAME] != VALUE_NOT_FOUND) 
		referContent = GetContentByName(mContext, bufPad->serverReferName);
	else
		referContent = GetContentByName (mContext, CONTENT_NAME_CLIENT);
	if (!referContent) break;	
	
	ret_err = 0x30;
	serBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!serBuffer) break;
	tpointer = REAL_BUFFER(serBuffer);

	ret_err = 0x40;
	cliContent = GetContentByName(mContext, CONTENT_NAME_CLIENT);
	if (!cliContent) break;
	cliPad = cliContent->HeadInfo;
// 	namefor = cliContent->HeadInfo->serverName;					// temp save the NAME	Aug 26 '13	//	change to below Apr. 26 '14

	ret_err = 0x50;
	fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
	if (!fileContent) break;

	ret_err = 0x60;
	if (!memcmp(bufPad->serverMethod, NASZ("RESTART")))
	{
//	for different module may use same addr		//	May 26 '14
// 		mContext->PPeer = mContext;
// 		if (oldSerContext) oldSerContext->PPeer = oldSerContext;

		long indexnumber, indexvalue = 1/* not restart*/;
		char* keyplace = bufPad->serverKey;

		if (bufPad->getLength[4] != VALUE_NOT_FOUND)		// have key
		{
			indexnumber = ASCII_DEC[*(keyplace+2)] * 10 + ASCII_DEC[*(keyplace+3)];
			indexvalue = (long)referContent->HeadInfo->resultKey[indexnumber];
		}

		if (bufPad->getLength[4] == VALUE_NOT_FOUND || !indexvalue)			// without condition or condition is ok(0), restart;
		{
			ret_err = 0x31;
			ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverAction, bufPad->serverAction+bufPad->getLength[0]);
			//	should ignore Action="<--someting-->"
			bufferStart = memstr_no(REAL_BUFFER(fileContent), fileContent->NProcessSize, REAL_BUFFER(serBuffer), tpointer-REAL_BUFFER(serBuffer), '"');
			if (!bufferStart) break;
			searchEnd = memstr(bufferStart, fileContent->NProcessSize, NASZ(WAFA_BODY_START));

			if (searchEnd)
			{
				searchEnd += (sizeof(WAFA_BODY_START)-1);
				cliPad->checkStart = bufferStart;
				cliPad->checkEnd = searchEnd;								// any search will stop here, even NO search
				// 			PREPARE_DEFINE(FALSE)
				PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);

// 				if (!wafaStart || !wafaEnd) PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);	//	Move into PrepareDefine	// May 01 '14
			}
		}
		else											//	condition is not ok (1), continue next one
		{
			wafaStart = memstr(cliPad->checkStart, cliPad->checkEnd-cliPad->checkStart, NASZ(WAFA_DEFINE_START));
			cliPad->checkStart = wafaStart + sizeof(WAFA_DEFINE_START);		//	step wafadefine
			// 		PREPARE_DEFINE(FALSE)
			PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);

// 			if (!wafaStart || !wafaEnd) PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);	//	Move into PrepareDefine	// May 01 '14
		}

		mContext->PApplication->FreeApplicationBuffer(serBuffer);
		ret_err = 0;
		break;
	}

	ret_err = 0x70;
	if (!memcmp(bufPad->serverMethod, NASZ("SELECT")))
	{
		ret_err = 0x71;
		ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverAction, bufPad->serverAction+bufPad->getLength[0]);
		serBuffer->NProcessSize = tpointer-REAL_BUFFER(serBuffer);
		ret = GetSelect(serBuffer);
		if (ret) break;

		AddtoContentList(mContext, serBuffer, bufPad->serverName);

		wafaStart = memstr(cliPad->checkStart, cliPad->checkEnd-cliPad->checkStart, NASZ(WAFA_DEFINE_START));
		cliPad->checkStart = wafaStart + sizeof(WAFA_DEFINE_START);		//	step wafadefine
// 		PREPARE_DEFINE(FALSE)
		PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);

// 		if (!wafaStart || !wafaEnd) PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);	//	Move into PrepareDefine	// May 01 '14
//	have not test, but I think it should add
		mContext->PApplication->FreeApplicationBuffer(serBuffer);						// should add this? it lost buffer before add this, Apr. 25 '14
		ret_err = 0;
		break;

	}

	ret_err = 0x80;
	createFunction = GetCreateFunction(bufPad->serverMethod);
	if (createFunction)
	{
		if (oldSerContext)			//	MUST BE THIS, or CSystemApplication::usedContext.PPeer will be set
		{
			mContext->PPeer = mContext;
			oldSerContext->PPeer = oldSerContext;
		}

		//	if there are request send and should goon pause it; else do normal nop		//	Apr. 29 '14
		if (fileContent->HeadInfo->inConcurrency == CONCURRENCY_GOON && cliPad->inConcurrency > 0 )	
		{
#ifdef	DEBUG_CONCURRENCY
			printf("Find Define, pause concurrency by NOP/CREATE/PEER. \r\n");
#endif	DEBUG_CONCURRENCY 
			fileContent->HeadInfo->inConcurrency = CONCURRENCY_PAUSE;					//	NOP pause concurrency	//	Apr. 29 '14
		}
		else	// (fileContent->HeadInfo->inConcurrency == CONCURRENCY_PAUSE)
		{
			createFunction(mContext, usedBuffer, 0);		//	now for NOP, CREATE, PEER
			wafaStart = memstr(cliPad->checkStart, cliPad->checkEnd-cliPad->checkStart, NASZ(WAFA_DEFINE_START));
			cliPad->checkStart = wafaStart + sizeof(WAFA_DEFINE_START);		//	step wafadefine
			PrepareDefine(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);
// 			if (!wafaStart || !wafaEnd) PrepareReply(mContext, usedBuffer, cliPad, 0, wafaStart, wafaEnd);	//	Move into PrepareDefine	// May 01 '14
		}

		mContext->PApplication->FreeApplicationBuffer(serBuffer);						// should add this? it lost buffer before add this, Apr. 25 '14
		ret_err = 0;
		break;
	}

	ret_err = 0x90;
	if (!memcmp(bufPad->serverMethod, NASZ("POST")))
	{
		methodNum = 1;
		tpointer = AddString(tpointer, NASZ("POST "));
	}
	else if (!memcmp(bufPad->serverMethod, NASZ("GET"))) tpointer = AddString(tpointer, NASZ("GET "));
	else break;
	ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverAction, bufPad->serverAction+bufPad->getLength[0]);
	tpointer = AddString(tpointer, NASZ(" HTTP/1.1\r\n"));			// used for 1.0 & 1.1		// should ATTENTION
//	Many times, http 1.0 return without Context-Length, it use connection: close for stop

//	move to OnClientRead for use at any time
// 	cliPad = referContent->HeadInfo;
// 	cliPad->resultKey[HEADINFO_BOUNDARY] = &(urlinfo->getBoundary[0]);
// //	here 7 is the order of AnalysisHttpURL
// 	cliPad->getLength[HEADINFO_BOUNDARY] = urlinfo->getLength[7];				// the only fast way to finish multipart boundary

	ret_err = 0xa0;
	tpointer = AddString(tpointer, NASZ("Accept: text/html, application/xhtml+xml, */*\r\n"));
// 		if (bufPad->getLength[PARA_DEF_REFERER] != VALUE_NOT_FOUND)		//	always add referer, even at the first page
	if (bufPad->getLength[PARA_DEF_REFERER] != VALUE_NOT_FOUND)			// only if referer is in refer
	{
//	change the Referer use, it is a large string
// 		ProcessOneVar(mContext, referContent, tpointer, bufPad->serverReferer, bufPad->serverReferer+bufPad->getLength[5]);
		ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverReferer, bufPad->serverReferer+bufPad->getLength[5]);
	}

	tpointer = AddString(tpointer, NASZ("Accept-Language: zh-cn\r\n"));
	tpointer = AddString(tpointer, NASZ("User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)\r\n"));
// 	tpointer = AddString(tpointer, NASZ("Accept-Encoding: gzip, deflate\r\n"));					//	Not accept gzip now		// lazy way	Apr. 27 '14

//	The following is old use for HOST="name", now for HOST="http://www.abc.com:88/",	Jan. 14 '14
//	Host support wafa string now		//	May 12 '14
	ret_err = 0xb0;
	if (bufPad->getLength[PARA_DEF_HOST] == VALUE_NOT_FOUND) break;
	tpointer = AddString(tpointer, NASZ("Host: "));

	char* oldtpointer = tpointer;
	char *isAddr = bufPad->serverHost+7;
	struct addrinfo *aiList = NULL;
	char tempaddr[MAX_URL];

	if ( ( *((long*)&bufPad->serverHost[0]) | 0x40404040 ) == ISHTTP )				// HOST start with "http"
	{
		if ( ( *((long*)&bufPad->serverHost[4]) & 0xffffff ) != ISDIV ) break;		// is NOT "://"
		ProcessOneVar(mContext, referContent->HeadInfo, tpointer, isAddr, isAddr+bufPad->getLength[PARA_DEF_HOST]-7);
		if (*(tpointer-1) == '/') tpointer--;
		if (tpointer-oldtpointer > MAX_URL) break;
		memcpy(tempaddr, oldtpointer, tpointer-oldtpointer);
		if (MyGetInfoAddr(tempaddr, tpointer-oldtpointer, aiList)) break;

		serContext = mContext->PProtocol->GetProtocolContext();
		serContext->PApplication = mContext->PApplication;
		if (!serContext) break;

		CTCPContext* tcpContext = (CTCPContext*)serContext;
		memcpy( &tcpContext->addrServer, aiList->ai_addr, sizeof(sockaddr) );

//	for http:// ahead host and Named host can use keep-alive both	//	May 24 '14
// //	Add this because keep-alive in May 23 '14
// 		serContext->PPeer = mContext;
// 		mContext->PPeer = serContext;
// 		serContext->OldPeer = 0;
// //	Add this because keep-alive in May 23 '14

	}
	else
	{
// 		//	SHOULD use MultiApplication function to replace the PrepareContentPeer, to get load share, back to PreparePeer
//	Change to PreparePeer				//	May 19 '14
// 		cf = PrepareContentPeer((char*)&bufPad->serverHost[0]);
// 		if (!cf) break;
// 		isAddr = cf->ContentForwardHost;
// 		serContext = cf->ContentContext->PProtocol->GetDuplicateContext(cf->ContentContext);
// 		if (!serContext) break;
// 		tpointer = AddString(tpointer, isAddr, strlen(isAddr));				// http:// host mode
		CContentPadServer* pContentPad;
		ProtocolId* peerProtocol;

		pContentPad = (CContentPadServer*)mContext->PApplication;
		serContext = pContentPad->PreparePeer(mContext, (char*)&bufPad->serverHost[0]);
		if (!serContext) break;
		peerProtocol = serContext->PeerProtocol;

// 		serContext = peerContent->PProtocol->GetDuplicateContext(peerContent);
		tpointer = AddString(tpointer, peerProtocol->peerHost, peerProtocol->peerHostLen);				// http:// host mode

//	for http:// ahead host and Named host can use keep-alive both	//	May 24 '14
// //	Add this for keep-alive in May 22 '14
// 		if (oldSerContext && oldSerContext->OldPeer == peerProtocol)
// 		{
// 			serContext->PProtocol->FreeProtocolContext(serContext);
// 			serContext = oldSerContext;
// printf("Same ProtocolID, delete new context\r\n");
// 		}
// 		else
// 		{
// 			if (oldSerContext) oldSerContext->PPeer = oldSerContext;
// 			mContext->PPeer = serContext;
// 			serContext->PPeer = mContext;
// 			serContext->OldPeer = peerProtocol;
// 		}
// //	Add this for keep-alive in May 22 '14
	}
	if (oldSerContext && !SameContextAddress(serContext, oldSerContext))
	{
		serContext->PProtocol->FreeProtocolContext(serContext);
		serContext = oldSerContext;
#ifdef	DEBUG_PEER
	printf("Same addr, use old Context\r\n");
#endif	DEBUG_PEER
	}
	else
	{
		if (oldSerContext) oldSerContext->PPeer = oldSerContext;
		mContext->PPeer = serContext;
		serContext->PPeer = mContext;
		serContext->OldPeer = oldSerContext;
#ifdef	DEBUG_PEER
	printf("Different addr, use new Context\r\n");
#endif	DEBUG_PEER
	}

//	for http:// ahead host and Named host can use keep-alive both	//	May 24 '14


	namefor = serContext->ServerName;		//	temp save server side pad name, for concurrency use instead of in client pad
	if (bufPad->getLength[PARA_DEF_NAME] == VALUE_NOT_FOUND) strcpy_s(namefor, SMALL_CHAR, CONTENT_NAME_NONE);		// serverName
	else strcpy_s(namefor, SMALL_CHAR, bufPad->serverName);
// 	tpointer = AddString(tpointer, cf->ContentForwardHost, strlen(cf->ContentForwardHost));	
	tpointer = AddString(tpointer, NASZ("\r\n"));

	ret_err = 0xc0;
//	For now, close is safe	//	May 25 '14
// 	tpointer = AddString(tpointer, NASZ("Connection: Close\r\n"));
	tpointer = AddString(tpointer, NASZ("Connection: Keep-Alive\r\n"));

	if (methodNum)
	{
		tpointer = AddString(tpointer, NASZ("Content-Length: "));
		lengthPlace = tpointer;
		tpointer = AddString(tpointer, "     \r\n", SMALL_CONTENT_LENGTH_SIZE + 2);		// ahead have SMALL_CONTENT_LENGTH_SIZE space
	}

// 	if ( (bufPad->getLength[PARA_DEF_COOKIE] != VALUE_NOT_FOUND) &&			//	have Cookie= in wafafile
// 			urlinfo->getLength[PARA_DEF_COOKIE] != VALUE_NOT_FOUND )		//	have Cookie= in client request
// 	{
// 		tpointer = AddString(tpointer, NASZ("Cookie: "));
// 		if ( !memcmp(bufPad->serverCookie, NASZ("%cookie%"))  )
// 			tpointer = AddString(tpointer, urlinfo->getCookie, urlinfo->getLength[PARA_DEF_COOKIE]);	//the order in AnalysisHttpURL
// 		else
// 			ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverCookie, bufPad->serverCookie+bufPad->getLength[6]);
// 		tpointer = AddString(tpointer, NASZ("\r\n"));
// 	}				// in ContentPad normal not Set-Cookie

	ret_err = 0xd0;
	if (bufPad->getLength[PARA_DEF_COOKIE] != VALUE_NOT_FOUND)
	{
		if ( !memcmp(bufPad->serverCookie, NASZ("%cookie%"))  )
		{
			if (urlinfo->getLength[PARA_DEF_COOKIE] != VALUE_NOT_FOUND)
			{
				tpointer = AddString(tpointer, NASZ("Cookie: "));
				tpointer = AddString(tpointer, urlinfo->getCookie, urlinfo->getLength[PARA_DEF_COOKIE]);	//the order in AnalysisHttpURL
				tpointer = AddString(tpointer, NASZ("\r\n"));
			}
		}
		else
		{
			tpointer = AddString(tpointer, NASZ("Cookie: "));
			ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverCookie, bufPad->serverCookie+bufPad->getLength[6]);
			tpointer = AddString(tpointer, NASZ("\r\n"));
		}
	}

	tpointer = AddString(tpointer, NASZ("\r\n"));
	headPlace = tpointer;

	ret_err = 0xe0;
	if (methodNum)			// for POST body
	{
		ProcessOneVar(mContext, referContent->HeadInfo, tpointer, bufPad->serverKey, bufPad->serverKey+bufPad->getLength[4]);
		Myitoa(long(tpointer - headPlace), lengthPlace, SMALL_CONTENT_LENGTH_SIZE);
	}

	ret_err = 0xf0;
//	remove for have changed May 22 '14
// 	mContext->PPeer = serContext;			// normally add this just after accept
// 	serContext->PPeer = mContext;			// add this before connect
	serBuffer->NProcessSize = tpointer - REAL_BUFFER(serBuffer);

	MYINT nowCon = InterInc(&cliPad->inConcurrency);		//	Add concurrency, in CLIENTpad	// Apr. 26 '14
#ifdef	DEBUG_CONCURRENCY
	printf("Send connect, now Concurrency: %d. \r\n", nowCon);
#endif	DEBUG_CONCURRENCY

#ifdef	DEBUG_NTLM
	if (serContext != oldSerContext) printf("NewContext:%x ", serContext);
	else  printf("SameContext:%x ", serContext);
	char* lineend = (char*)memchr(REAL_BUFFER(serBuffer), 0xd, 500);
	if ((*(long*)(REAL_BUFFER(serBuffer))) == ISPOST )
	{
		*(REAL_BUFFER(serBuffer)+serBuffer->NProcessSize) = 0;
		printf("%s\r\n", REAL_BUFFER(serBuffer));
	}
	else if (lineend) 
	{
		*lineend = 0;
		printf("%s\r\n", REAL_BUFFER(serBuffer));
		*lineend = 0xd;
	}
#endif	DEBUG_NTLM
	if (serContext != oldSerContext)
		ret = NoneProFunc(serContext->PProtocol, fPostConnect)(serContext, serBuffer, serBuffer->NProcessSize, OP_CONNECT);
	else
		ret = NoneProFunc(serContext->PProtocol, fPostSend)(serContext, serBuffer, serBuffer->NProcessSize, OP_CONNECT, OPSIDE_SERVER);
	if (ret) break;
	
	__CATCH_BEGIN(MODULE_APPLICATION, "ProcessWafaDefine")
		RET_BETWEEN(0x40, 0x90) mContext->PApplication->FreeApplicationBuffer(serBuffer);		// surely?
	__CATCH_END
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long ProcessNormalReplay(CContextItem* mContext, CListItem* &mBuffer)
{
	long ret_err = 0x01;
	long ret;
	CListItem* sendBuffer;
	char* tpointer;
	CFileContext* fileContext = (CFileContext*)mContext->PPeer;		//	mContext is client
	URLInfo* urlinfo = &(((CHTTPContext*)(mContext->PPeer))->urlInfo);

	__TRY

	ret_err = 0x10;
	sendBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!sendBuffer) break;

	ret_err = 0x20;
	tpointer = REAL_BUFFER(sendBuffer);
	tpointer = AddString(tpointer, NASZ("HTTP/1.1 200 OK\r\n"));
	tpointer = AddString(tpointer, NASZ("Accept-Ranges: bytes\r\n"));
	tpointer = AddString(tpointer, NASZ("Connection: Keep-Alive\r\n"));

	tpointer = AddString(tpointer, NASZ("Content-Type: text/html; charset="));
	tpointer = AddString(tpointer, WAFA_CHARSET, sizeofCharset);
	tpointer = AddString(tpointer, NASZ("\r\n"));
//	A lazy way for charset return in 10001
// 	if (urlinfo->getLength[4] != VALUE_NOT_FOUND)				//	for Charset		
// 	{
// 		tpointer = AddString(tpointer, NASZ("Content-Type: "));
// 		tpointer = AddString(tpointer,urlinfo->getCharset, urlinfo->getLength[4]);
// 		tpointer = AddString(tpointer, NASZ("\r\n"));
// 	}


// 		tpointer = AddString(tpointer, "Cache-Control: no-cache\r\n", 25);

	tpointer = AddString(tpointer, NASZ("Content-Length: "));
	Myitoa(fileContext->fileLength, tpointer);
// 	tpointer += MAX_CONTENT_LENGTH_SIZE;
// 		sprintf_s(tpointer, 20, "%d", fileContext->fileLength);
// 	tpointer = (char*)memchr(tpointer, 0, MAX_CONTENT_LENGTH_SIZE+1);
// 	if (!tpointer) break;

	tpointer = AddString(tpointer, NASZ("\r\n\r\n"));
	memcpy(tpointer, mBuffer+1, mBuffer->NProcessSize);
	sendBuffer->NProcessSize = tpointer-REAL_BUFFER(sendBuffer) + mBuffer->NProcessSize;

	ret_err = 0x30;
	mContext->PApplication->FreeApplicationBuffer(mContext->FirstContent);
	mContext->FirstContent = 0;

	ret_err = 0x40;
	ret = NoneProFunc(mContext->PProtocol, fPostSend)
		(mContext, sendBuffer, sendBuffer->NProcessSize, OP_CLIENT_WRITE, 0);
	if (ret) break;

	__CATCH_BEGIN(MODULE_APPLICATION, "ProcessNormalReplay")
		RET_BETWEEN(0x20, 0x40)mContext->PApplication->FreeApplicationBuffer(sendBuffer);
	__CATCH_END
}

long ProcessHtmlReplay(CContextItem* mContext)//, CListItem* &mBuffer)
{
	char* tpointer;
	char* lengthPlace;
	char* headPlace;
	CListItem* fileContent, *cliContent;
	char* fileEnd;
	ContentPad *cliPad;	
	URLInfo* urlinfo = &(((CHTTPContext*)(mContext->PPeer))->urlInfo);		//	Set server side Set-Cookie
	long ret_err = 0x01;
	long ret;
	CListItem* mBuffer;
	char* oldCheckEnd;

	__TRY

	ret_err = 0x10;
	mBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!mBuffer) break;

	ret_err = 0x20;
	tpointer = REAL_BUFFER(mBuffer);
	tpointer = AddString(tpointer, NASZ("HTTP/1.1 200 OK\r\n"));
	tpointer = AddString(tpointer, NASZ("Accept-Ranges: bytes\r\n"));
	tpointer = AddString(tpointer, NASZ("Connection: Keep-Alive\r\n"));

// In BITIC project, only HOMEPAGE, and a open source WebEdit use UTF-8, which do NOT return useful Unicode, others are GBK.

#ifndef	QRCODE_FUNCTION
	if (urlinfo->getLength[4] != VALUE_NOT_FOUND)				//	for Charset		
#endif	QRCODE_FUNCTION
// 	SHOULD DO LATER should use val to change page-code
	{
		tpointer = AddString(tpointer, NASZ("Content-Type: text/html; charset="));
		tpointer = AddString(tpointer, WAFA_CHARSET, sizeofCharset);
		tpointer = AddString(tpointer, NASZ("\r\n"));
//	A lazy way for charset return in 10001
	}

//	This is return the server's charset, but there are multi server return, so it may mistake
// 	if (urlinfo->getLength[4] != VALUE_NOT_FOUND)				//	for Charset		
// 	{
// 		tpointer = AddString(tpointer, NASZ("Content-Type: "));
// 		tpointer = AddString(tpointer,urlinfo->getCharset, urlinfo->getLength[4]);
// 		tpointer = AddString(tpointer, NASZ("\r\n"));
// 	}

	tpointer = AddString(tpointer, NASZ("Cache-Control: no-cache\r\n"));

	tpointer = AddString(tpointer, NASZ("Content-Length: "));
	lengthPlace = tpointer;
	tpointer = AddString(tpointer, "     \r\n", SMALL_CONTENT_LENGTH_SIZE+2);		// ahead have SMALL_CONTENT_LENGTH_SIZE space

// Attention, Set-Cookie MUST be in the last command		// the order defend on HttpResPara
	if (urlinfo->getLength[3] != VALUE_NOT_FOUND)				//	for Set-Cookie
	{
		tpointer = AddString(tpointer, NASZ("Set-Cookie: "));
		tpointer = AddString(tpointer, urlinfo->getCookie, urlinfo->getLength[3]);
		tpointer = AddString(tpointer, NASZ("\r\n"));
	}

	tpointer = AddString(tpointer, NASZ("\r\n"));
	headPlace = tpointer;

	ret_err = 0x30;
	fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
	if (!fileContent) break;

	ret_err = 0x40;
	cliContent = GetContentByName(mContext, CONTENT_NAME_CLIENT);
	if (!cliContent) break;
	cliPad = cliContent->HeadInfo;
	oldCheckEnd = cliPad->checkEnd;

	//	cliPad->checkEnd point to end of WAFA_BODY_START
	//	fileEnd point to begin of WAFA_BODY_END
	fileEnd = memstr(oldCheckEnd, fileContent->NProcessSize, NASZ(WAFA_BODY_END));
	if (!fileEnd || (fileEnd > (REAL_BUFFER(fileContent)) + fileContent->NProcessSize) )
		fileEnd = (REAL_BUFFER(fileContent)) + fileContent->NProcessSize;

	//	mContext is CLIENT, mBuffer is new empty
	//	tpointer point to really buffer
	//	last two point between WAFA_BODY_START to WAFA_BODY_END
	ret = ProcessWafaPage(mContext, mBuffer, tpointer, oldCheckEnd, fileEnd, ProcessWafaVars);

	ret_err = 0x50;
	if (cliPad->checkEnd == oldCheckEnd)				// inter process ok, not change checkEnd
	{
		mBuffer->NProcessSize = tpointer - ( (char*)(mBuffer+1) );						
		Myitoa((long)(tpointer - headPlace), lengthPlace, SMALL_CONTENT_LENGTH_SIZE);
		if (ret) break;
		ret_err = 0x60;
	//	free content list
		mContext->PApplication->FreeApplicationBuffer(mContext->FirstContent);
		mContext->FirstContent = 0;

		ret_err = 0x70;
		ret = NoneProFunc(mContext->PProtocol, fPostSend)
			(mContext, mBuffer, mBuffer->NProcessSize, OP_CLIENT_WRITE, 0);
		if (ret) break;
	}
	else												//	have be restart by ERROR control
	{
		ret_err = 0x80;
		mContext->PApplication->FreeApplicationBuffer(mContext->FirstContent);
		mContext->FirstContent = 0;
	}

	ret_err = 0x90;

	__CATCH_BEGIN(MODULE_APPLICATION, "ProcessHtmlReplay")
		RET_BETWEEN(0x20, 0x80)mContext->PApplication->FreeApplicationBuffer(mBuffer);		// surely ?
	__CATCH_END
}

//	process between WAFA_BODY_START to WAFA_BODY_END
long ProcessWafaPage(CContextItem* mContext, CListItem* mBuffer, char* &tpointer, char* fileStart, char* fileEnd, WafaVarFunction nowfunc)
{
	//	mContext is client context
	char* nowfile = fileStart;
	char* nextRefer;

	ProcessWafaPartFile(tpointer, nowfile, fileEnd);						//	nowfile will pointer to WAFA_REFER_START or fileEND

	while (nowfile < fileEnd)
	{
		nextRefer = memstr(nowfile+sizeof(WAFA_REFER_START), fileEnd-nowfile-sizeof(WAFA_REFER_START), 	NASZ(WAFA_REFER_START));
		if (!nextRefer) nextRefer = fileEnd;

//	process from <wafarefer> to <wafarefer> | WAFA_BODY_END
		ProcessWafaRefer(mContext, mBuffer, tpointer, nowfile, nextRefer, nowfunc);	// after done, nowfile will pointer nextRefer

		nowfile = nextRefer;
	}																																//
	return 0;																														//
}

long TranCookieInfo(CContextItem* tarContext, CContextItem* souContext, long tarPlace, long souPlace)
{
	URLInfo* tarinfo = &((CHTTPContext*)tarContext)->urlInfo;
	URLInfo* souinfo = &((CHTTPContext*)souContext)->urlInfo;
	long infosize = souinfo->getLength[souPlace];

	if (infosize != VALUE_NOT_FOUND) memcpy(tarinfo->getCookie, souinfo->getCookie, infosize);
	tarinfo->getLength[tarPlace] = infosize;
	
	return 0;
}

long TranCharsetInfo(CContextItem* tarContext, CContextItem* souContext, long tarPlace, long souPlace)
{
	URLInfo* tarinfo = &((CHTTPContext*)tarContext)->urlInfo;
	URLInfo* souinfo = &((CHTTPContext*)souContext)->urlInfo;
	long infosize = souinfo->getLength[souPlace];

	if (infosize != VALUE_NOT_FOUND) memcpy(tarinfo->getCharset, souinfo->getCharset, infosize);
	tarinfo->getLength[tarPlace] = infosize;

	return 0;
}

//	Remove May 19 '14
// ContentForward* PrepareContentPeer(char* keySer)
// {
// 	MYINT i;
// 	long len;
// 	ContentForward* cf;
// 
// 	for ( i=0; i<configInfo->ContentForwardNumber; i++ )
// 	{
// 		cf = &(configInfo->cfc[i]);
// 		len = strlen(cf->ContentForwardName);
// 		if (!strncmp(cf->ContentForwardName, keySer, len)) break;
// 	}
// 
// 	if (i==configInfo->ContentForwardNumber) return NULL; //break;
// 	return cf;
// }

long GetClientPara(CContextItem* mContext, CListItem* &mBuffer, char* paraStart, long paraLength, int nowPlace)
{
	ContentPad* clientPad = mBuffer->HeadInfo;
	char* paraEnd = paraStart + paraLength;
	char* paraNow = paraStart;
	char* paraNext;

	clientPad->resultKey[nowPlace] = paraStart;
	clientPad->getLength[nowPlace] = paraLength;
	nowPlace ++;

	while (paraNow && (paraNow < paraEnd) )
	{
		paraNow = (char*)memchr(paraNow, '=', paraEnd-paraNow);
		if (paraNow)
		{
			paraNow++;
			paraNext = (char*)memchr(paraNow, '&', paraEnd-paraNow);
			if (!paraNext) paraNext = paraEnd;
			clientPad->resultKey[nowPlace] = paraNow;
			clientPad->getLength[nowPlace] = paraNext-paraNow;
			paraNow = paraNext+1;
			nowPlace++;
		}
	}
// 	clientPad->keyNumber = nowPlace;		//	NOT use keyNumber at all	//	May 24 '14

	return nowPlace;
}

//	mBuffer is HeadInfo, start with nowPlace; paraStart and paraLength are buffer
long GetJsonCommond(CContextItem* mContext, CListItem* &mBuffer, char* paraStart, long paraLength, int nowPlace)
{
	return 0;
}

long ProcessWafaPartFile(char* &mStart, char* &fileStart, char* fileEnd)
{
	return ProcessPartFile(mStart, fileStart, fileEnd, 	WAFA_REFER_START, sizeof(WAFA_REFER_START)-1);
}

inline long ProcessPartFile(char* &mStart, char* &fileStart, long copysize)
{
	if (mStart)
	{
		memcpy(mStart, fileStart, copysize);
		mStart += copysize;
	}
	fileStart += copysize;
	return 0;
}

long ProcessPartFile(char* &mStart, char* &fileStart, char* fileEnd, char* istart, long slength)
{
	long copysize;
	char* spointer;
	long ret_err = 0x01;																											//

	__TRY

	ret_err = 0x10;
	spointer = memstr(fileStart, fileEnd-fileStart, istart, slength);
	if (!spointer) spointer = fileEnd;
	copysize = spointer - fileStart;
	ProcessPartFile(mStart, fileStart, copysize);

	__CATCH(MODULE_APPLICATION, "ProcessPartFile")
}

//	process <wafarefer> to <wafarefer> | WAFA_BODY_END, step by </wafarefer>
inline long ProcessWafaRefer(CContextItem* mContext, CListItem* mBuffer, char* &mStart, char* fileStart, char* fileEnd, WafaVarFunction nowfunc)
{
	return ProcessOneRefer(mContext, mBuffer, mStart, fileStart, fileEnd, NASZ(WAFA_REFER_END), nowfunc);
}

unsigned char WafaReferMap[ASCII_NUMBER] = {
	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	IS_END,	0x00,	0x00,	IS_END,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	IS_SPC,	0x00,	IS_END,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	IS_KEY,	IS_KEY,	0x00,	0x00,	0x00,	IS_KEY,	0x00,	IS_KEY,	IS_KEY,	IS_KEY,	0x00,	0x00,	
	0x00,	0x00,	IS_KEY,	IS_KEY,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
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
KeyParaTran WafaReferPara[] = { sNASZ("Method=\""), sNASZ("Key=\""), sNASZ("ReferName=\""), sNASZ("Start=\""), sNASZ("End=\""), 
	sNASZ("Loop=\""), dNASZ("Start=%d"), dNASZ("End=%d"), dNASZ("Start="), dNASZ("End="), dNASZ("Keyfrom="), //{NULL} };
	sNASZ("ID=\""), sNASZ("Error=\""), sNASZ("Default=\""), sNASZ("If=\""), {NULL} };			// ID and Error add in Sept. 15 '13,	//	Default add in May 24 '14


//	process <wafarefer> to <wafarefer> | WAFA_BODY_END, step by </wafarefer>
long ProcessOneRefer(CContextItem* mContext, CListItem* mBuffer, char* &mStart, char* fileStart, char* fileEnd, char* rend, long rlen, WafaVarFunction nowfunc)
{
//	mContext is client context
	long ret_err = 0x01;																											//
	char* referstart, *referend;
	ContentPad* resultPad = mBuffer->HeadInfo, *namPad;
	WafaProcessFunction nowFunctin;
	char* nowstart, *nowend;
	char* nowkey, *nowloop = NULL;
	long keylen, loopstrlen, nowkeyfrom;
	CListItem* referContent;
	char* contentstart, *contentend;
	long ret = 1;
	long isloop = 0;

	CListItem* fileContent;//, *clientContent;
	ContentPad* clientPad;
	char* errorStart;

	__TRY

	ret_err = 0x10;
	referstart = fileStart;			//	point to begin of "<wafarefer>"
	referend = memstr(referstart, fileEnd-referstart, rend, rlen);
	if (!referend) 
		break;
	referend += rlen;				//	point to end of "</wafarefer>"

#ifdef DEBUG_WAFA
	memcpy (wafadebug, referstart, min(referend-referstart, MAX_DEBUG_STRING));
	wafadebug[min(referend-referstart, MAX_DEBUG_STRING)] = 0;
	printf("  For refer: %s\r\n", wafadebug);
#endif DEBUG_WAFA

	ret_err = 0x20;
	SET_PARA_STR(resultPad, PARA_REF_METHOD, serverMethod)
	SET_PARA_STR(resultPad, PARA_REF_KEY, serverKey)
	SET_PARA_STR(resultPad, PARA_REF_REFERNAME, serverReferName)
	SET_PARA_STR(resultPad, PARA_REF_STARTNAME, serverStartName)
	SET_PARA_STR(resultPad, PARA_REF_ENDNAME, serverEndName)
	SET_PARA_STR(resultPad, PARA_REF_LOOP, serverLoop)

	SET_PARA_TYPE(resultPad, PARA_REF_STARTREF, serverStartPlace, long)
	SET_PARA_TYPE(resultPad, PARA_REF_ENDREF, serverEndPlace, long)

	SET_PARA_TYPE(resultPad, PARA_REF_START, serverStartPlace, long)
	SET_PARA_TYPE(resultPad, PARA_REF_END, serverEndPlace, long)
	SET_PARA_TYPE(resultPad, PARA_REF_KEYFROM, serverKeyFrom, long)
	SET_PARA_STR(resultPad, PARA_REF_ID, serverID)
	SET_PARA_STR(resultPad, PARA_REF_ERROR, serverError)
	SET_PARA_STR(resultPad, PARA_REF_DEFAULT, serverDefault)
	SET_PARA_STR(resultPad, PARA_REF_IF, serverIf)				//	Add If Jun. 30 '14

	if ( CommandLineParaTran( referstart, referend-referstart, NULL, NULL, WafaReferPara, resultPad->valPlace, WafaReferMap) ) break;	//
	fileStart = referend;

	if ( resultPad->getLength[PARA_REF_ENDREF] != VALUE_NOT_FOUND )
	{
		int aaa = 1;
	}

	ret_err = 0x30;											//	Method
	if ( resultPad->getLength[PARA_REF_METHOD] == VALUE_NOT_FOUND ) break;
	nowFunctin = GetWafaFunction(resultPad->serverMethod);
	if (nowFunctin == Wafa_NULL)				// for LoopEnd Sept. 08 '13
	{
		ret_err = 0;
		break;
	}

	ret_err = 0x40;											//	Key
	if ( resultPad->getLength[PARA_REF_KEY] == VALUE_NOT_FOUND) 
	{
		nowkey = NULL;
		keylen = 0;
	}
	else
	{
		nowkey = resultPad->serverKey;
		keylen = resultPad->getLength[PARA_REF_KEY];
	}

//	contentstart, contentend point referContent
	ret_err = 0x50;											//	ReferName
	contentstart = contentend = NULL;
	referContent = NULL;
	if ( resultPad->getLength[PARA_REF_REFERNAME] != VALUE_NOT_FOUND)
	{
		referContent = GetContentByName(mContext, resultPad->serverReferName);
		if (referContent)
		{
			contentstart = (char*)(referContent+1);
			contentend = contentstart + referContent->NProcessSize;

			ret_err = 0x60;											//	Start
			if ( resultPad->getLength[PARA_REF_STARTREF] != VALUE_NOT_FOUND && referContent)
				nowstart = (char*)referContent->HeadInfo->resultKey[resultPad->serverStartPlace];
			else if ( resultPad->getLength[PARA_REF_START] != VALUE_NOT_FOUND )
				nowstart = contentstart + resultPad->serverStartPlace;
			else if ( resultPad->getLength[PARA_REF_STARTNAME] != VALUE_NOT_FOUND) 
			{
				nowstart = memstr(contentstart, contentend-contentstart, resultPad->serverStartName, resultPad->getLength[PARA_REF_STARTNAME]);
// 				if (!nowstart) nowstart = contentstart;
				if (!nowstart) break;

			}
			else nowstart = contentstart;

			if (!nowstart) nowstart = contentstart;					//	Add Jul 17 '13 for not find Start;

			ret_err = 0x70;											//	End
			if ( resultPad->getLength[PARA_REF_ENDREF] != VALUE_NOT_FOUND && referContent)
				nowend = (char*)referContent->HeadInfo->resultKey[resultPad->serverEndPlace];
			else if ( resultPad->getLength[PARA_REF_END] != VALUE_NOT_FOUND )
				nowend = contentstart + resultPad->serverEndPlace;
			else if ( resultPad->getLength[PARA_REF_ENDNAME] != VALUE_NOT_FOUND) 
			{
				nowend = memstr(nowstart, contentend-nowstart, resultPad->serverEndName, resultPad->getLength[PARA_REF_ENDNAME]);
// 				if (!nowend) nowend = contentend;
				if (!nowend) break;

			}
			else nowend = contentend;

//	this two line for start and end for value use	//	Jun. 27 '14
			if (nowstart < contentstart) nowstart = contentstart + ((long)nowstart);
			if (nowend < contentstart) nowend = contentstart + ((long)nowend);


			ret_err = 0x80;
			if (nowstart >= nowend)

			{
				nowstart = contentstart;
				nowend = contentend;
			}

			ret_err = 0x90;											//	Loop
			if ( resultPad->getLength[PARA_REF_LOOP] == VALUE_NOT_FOUND) 
			{
				nowloop = NULL;
				loopstrlen = 0;
			}
			else
			{
				nowloop = resultPad->serverLoop;
				loopstrlen = resultPad->getLength[PARA_REF_LOOP];
// This two line can delete, for resultPad be a para for all Wafa_ function, it can use resultPad directly.
// //	for SetIPMac with Loop, I must copy mBuffer->HeadInfo to referContext->HeadInfo
// //	maybe have better way, should do later
// 				memcpy (referContent->HeadInfo->serverLoop, resultPad->serverLoop, loopstrlen);
// 				referContent->HeadInfo->getLength[PARA_REF_LOOP] = loopstrlen;
			}

			ret_err = 0xa0;											//	Keyfrom
			if ( resultPad->getLength[PARA_REF_KEYFROM] == VALUE_NOT_FOUND) nowkeyfrom = 0;
			else nowkeyfrom = resultPad->serverKeyFrom;
		}
	}
	do 
	{
		if (nowloop)			//	move this out of if()
		{
// printf("%d\r\n", nowstart);
			ret_err = 0;
			nowstart = memstr(nowstart, nowend-nowstart, nowloop, loopstrlen);
			if (nowstart) isloop = 1;
			else break;
		}

		if (referContent)
		{
// 			if (nowloop)
// 			{
// 				ret_err = 0;
// 				nowstart = memstr(nowstart, nowend-nowstart, nowloop, loopstrlen);
// 				if (nowstart) isloop = 1;
// 				else break;
// 			}
			ret_err = 0xb0;
// ProcessWafaVar should use both referContent and resultPad.	//	May 24 '14
//	resultPad is the info source, record all info about wafarefer line. referContent is target, for record result.
// //	for get client ip, must translate client Context
// // 			ret = (*nowFunctin)(mBuffer, referContent, nowstart, nowend, nowkey, keylen, nowkeyfrom);
// 			ret = (*nowFunctin)(mContext, referContent, nowstart, nowend, nowkey, keylen, nowkeyfrom);
			ret = (*nowFunctin)(mContext, referContent, nowstart, nowend, nowkey, keylen, nowkeyfrom, resultPad);		//	this is Wafa_ function

//	add error control in Sept. 15 '13
			if (ret && resultPad->getLength[PARA_REF_ERROR] != VALUE_NOT_FOUND)
			{
				ret_err = 0xc0;
				fileContent = GetContentByName(mContext, NASZ(CONTENT_NAME_FILE));
				if (!fileContent) break;
				clientPad = GetPadByName(mContext, NASZ(CONTENT_NAME_CLIENT));
				if (!clientPad) break;

				//	should ignore Error="<!--error-->"
				ret_err = 0xd0;
				errorStart = memstr_no (REAL_BUFFER(fileContent), fileContent->NProcessSize, resultPad->serverError, resultPad->getLength[PARA_REF_ERROR], '"');
				if (!errorStart) break;
				errorStart = memstr (errorStart, fileContent->NProcessSize, NASZ(WAFA_BODY_START));
				if (!errorStart) break;

				clientPad->checkEnd = errorStart + sizeof(WAFA_BODY_START) - 1;
				
				clientPad->resultKey[81] = resultPad->serverID;
				clientPad->getLength[81] = resultPad->getLength[PARA_REF_ID];

				clientPad->resultKey[82] = resultPad->serverReferName;
				clientPad->getLength[82] = resultPad->getLength[PARA_REF_REFERNAME];

				clientPad->resultKey[83] = referstart + sizeof(WAFA_REFER_START);
				clientPad->getLength[83] = referend - referstart - sizeof(WAFA_REFER_START) - sizeof(WAFA_REFER_END);

				ret = ProcessHtmlReplay(mContext);
				break;		//		have replayed, break this loop
			}

			ret_err = 0xe0;
			if (!ret)					// should add this	// Sept. 28 '13
				(*nowfunc)(mContext, referContent->HeadInfo, mStart, fileStart, fileEnd, ret);		//	this usually is ProcessWafaVars, or ProcessConfigVars
			else
				ProcessNoneVars(mContext, referContent->HeadInfo, mStart, fileStart, fileEnd, ret);	//	add plain text //Oct. 14 '13
			// a lazy solution, I am crazy by error and error control

			//	for process Loop in Scanf	//	Have not test YET	//	May 25 '14
			if (resultPad->serverNowEnd) nowstart = resultPad->serverNowEnd;
			else nowstart += loopstrlen;
// 			nowstart += loopstrlen;
		}
		else
		{
			ret_err = 0xf0;
			namPad = GetPadByName(mContext, resultPad->serverReferName);
			if (namPad) (*nowfunc)(mContext, namPad, mStart, fileStart, fileEnd, 0);
			else ProcessNoneVars(mContext, NULL, mStart, fileStart, fileEnd, ret);	//	add plain text //Oct. 16 '13 for <--ReadOnly-->, should write plain text without refer
		}

		ret_err = 0;
	} while (isloop);

	if (ret_err) break;

	__CATCH(MODULE_APPLICATION, "ProcessOneRefer")	
}

long ProcessNoneVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror)
{
	char *nowvarstart, *nowvarend, *nextvarstart;
// 	long ret;

	if (!mStart) return 0;
	nowvarstart = memstr(fileStart, fileEnd-fileStart, WAFA_VAR_START, sizeof(WAFA_VAR_START)-1);
	if (!nowvarstart) nowvarstart = fileEnd;
	ProcessPartFile(mStart, fileStart, nowvarstart-fileStart);

	while (nowvarstart < fileEnd)
	{
		nextvarstart = memstr(nowvarstart+sizeof(WAFA_VAR_START), fileEnd-nowvarstart-sizeof(WAFA_VAR_START), 
			WAFA_VAR_START, sizeof(WAFA_VAR_START)-1);
		if (!nextvarstart) nextvarstart = fileEnd;

		nowvarend = memstr(nowvarstart, nextvarstart-nowvarstart, WAFA_VAR_END, sizeof(WAFA_VAR_END)-1);
		if (nowvarend)
		{
			nowvarend += sizeof(WAFA_VAR_END)-1;
			ProcessPartFile(mStart, nowvarend, nextvarstart-nowvarend);
		}

		nowvarstart = nextvarstart;
	}
	return 0;
}

long ProcessWafaVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror)
{
	char *nowvarstart, *nowvarend, *nextvarstart;
	long ret;

	if (!mStart) return 0;
	nowvarstart = memstr(fileStart, fileEnd-fileStart, WAFA_VAR_START, sizeof(WAFA_VAR_START)-1);
	if (!nowvarstart) nowvarstart = fileEnd;
	ProcessPartFile(mStart, fileStart, nowvarstart-fileStart);

	while (nowvarstart < fileEnd)
	{
		nextvarstart = memstr(nowvarstart+sizeof(WAFA_VAR_START), fileEnd-nowvarstart-sizeof(WAFA_VAR_START), 
			WAFA_VAR_START, sizeof(WAFA_VAR_START)-1);
		if (!nextvarstart) nextvarstart = fileEnd;

		nowvarend = memstr(nowvarstart, nextvarstart-nowvarstart, WAFA_VAR_END, sizeof(WAFA_VAR_END)-1);
		if (nowvarend)
		{
			ret = ProcessOneVar(mContext, hInfo, mStart, nowvarstart+sizeof(WAFA_VAR_START)-1, nowvarend);
			if (varerror && ret)					// have error control and have error
				return 1;							//	testuse
			nowvarend += sizeof(WAFA_VAR_END)-1;
			ProcessPartFile(mStart, nowvarend, nextvarstart-nowvarend);
		}

		nowvarstart = nextvarstart;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	copy from fileStart to fileEnd to mStart, and translateSpace wafavar depended on mBuffer->HeadInfo									//
//	can set a sub range by last four parameter, but I have not use yet																//
//	used for create GET/POST head and body																							//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long ProcessOneVar(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, 
				   char* rstart, long slen, char* rend, long elen)
{
	char* varstart, *varend;
	long ret_err = 0x01;
	char* varnow, *varnext;
	long copysize;
	long indexnumber;
	// 	ContentPad* resultPad = mBuffer->HeadInfo;
	ContentPad* resultPad = hInfo, *otherPad = 0;
	unsigned char idn, firstn, secondn;
	long translateSpace, translateHtml, translatePost;
	long trantoGB, trantoUTF;
	long tranQRCode;
	char *tranpoint;
	int i;

#ifdef DEBUG_WAFA
	char* oldstart = mStart;
#endif DEBUG_WAFA

	__TRY

	ret_err = 0x10;
	if (rstart)													//	set another start
	{
		varstart = memstr(fileStart, fileEnd-fileStart, rstart, slen);
		if (!varstart) break;
		varstart += slen;
	}
	else varstart = fileStart;

	ret_err = 0x20;
	if (rend)													//	set another end
	{
		varend = memstr(varstart, fileEnd-varstart, rend, elen);
		if (!varend) break;
	}
	else varend = fileEnd;

#ifdef DEBUG_WAFA
	if (varend>varstart)
	{
		memcpy (wafadebug, varstart, min(varend-varstart, MAX_DEBUG_STRING));
		wafadebug[min(varend-varstart, MAX_DEBUG_STRING)] = 0;
		printf("  For var: %s\r\n", wafadebug);
	}
#endif DEBUG_WAFA

	ret_err = 0x30;
	varnow = varstart;
// 	while (varnow < varend)										//	main loop
	while (varstart < varend && varnow < varend)
	{
		translateSpace = FALSE;
		translateHtml = FALSE;
		translatePost = FALSE;
		trantoGB = trantoUTF = FALSE;
		tranQRCode = FALSE;

		varnow = (char*)memchr(varstart, '%', varend-varstart);
		if (!varnow) varnow = varend;
		copysize = varnow - varstart;
		if (copysize)
		{
			memcpy(mStart, varstart, copysize);
			mStart += copysize;										//	normal char, only copy
		}

		if (varnow != varend)
		{
			indexnumber = VALUE_NOT_FOUND;
			idn = (unsigned char)*(varnow+1);
			firstn = (unsigned char)*(varnow+2);
			secondn = (unsigned char)*(varnow+3);

			if ( ( (idn & ~0x20) =='R') && (firstn == ':') )		// 'r' or 'R'
			{
				varnext = (char*)memchr(varnow+3, '%', varend-varnow-3);
				if (varnext)
				{
					otherPad = GetPadByName(mContext, varnow+3, varnext-varnow-3);
					varstart = varnext;
					continue;
				}
				else break;
			}
			else if (idn=='x') indexnumber = ASCII_HEX[firstn] * 16 + ASCII_HEX[secondn];
			else if (idn=='d') indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
			else if (idn=='X') indexnumber = ASCII_HEX[firstn] * 16 + ASCII_HEX[secondn] + POST_PARAMETER_START;
			else if (idn=='D') indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn] + POST_PARAMETER_START;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	add for translate GB and UTF, Apr. 23 '14
			else if (idn=='U')
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				trantoUTF = TRUE;
			}
			else if (idn=='G')
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				trantoGB = TRUE;
			}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			else if (idn=='s')				//	remove all blank for begin and end
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				translateSpace = TRUE;
			}
			else if (idn=='t')				// remove html format <>
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				translateSpace = TRUE;
				translateHtml = TRUE;
			}

			else if (idn=='v')
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				Myitoa((long)(resultPad->resultKey[indexnumber]), mStart);		//	force translateSpace
				varstart = varnow + 4;
				continue;
// 				indexnumber = VALUE_NOT_FOUND;									//	continue do following
			}

//	add in Apr. 14 '14 for url tran, change char to %XX
			else if (idn=='u')
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				translatePost = TRUE;
			}
//	add in Apr. 14 '14 for url tran

#ifdef	QRCODE_FUNCTION
			else if (idn='Q')
			{
				indexnumber = ASCII_DEC[firstn] * 10 + ASCII_DEC[secondn];
				tranQRCode = TRUE;
			}
#endif	QRCODE_FUNCTION
			else
			{
				*mStart = idn;
				mStart++;
				varstart = varnow + 2;
				continue;
			}

			if (indexnumber != VALUE_NOT_FOUND)	
			{
				if (otherPad)
				{
					resultPad = otherPad;
					otherPad = 0;
				}
				else resultPad = hInfo;

long aheadblank = 0, tailblank = 0;
char* isblank = resultPad->resultKey[indexnumber];

				if (resultPad->resultKey[indexnumber])
				{
//	add in Apr. 14 '14 for url tran, change char to %XX
					if (translatePost)
					{
						tranpoint = resultPad->resultKey[indexnumber];
						for (i=0; i<resultPad->getLength[indexnumber]; i++)
						{
							if (TRAN_POST[(unsigned char)(*isblank)])
							{
								*mStart++ = '%';
								Myitoh((unsigned char)(*isblank), mStart);			//	network order HEX format
								isblank++;
							}
							else *mStart++ = *isblank++;
						}
					}
//	add in Apr. 14 '14 for url tran
					else if (translateSpace)
					{
						for (i=0; i<resultPad->getLength[indexnumber]; i++)
						{
							if ((unsigned char)(*isblank) <= 0x20) isblank++;			//	not for display
							else break;
						}
						aheadblank = isblank - resultPad->resultKey[indexnumber];
						
						isblank = resultPad->resultKey[indexnumber] + resultPad->getLength[indexnumber];
						for (i=0; i<resultPad->getLength[indexnumber]; i++)
						{
							if ((unsigned char)(*(isblank-1)) <= 0x20) isblank--;			//	not for display
							else break;
						}
						tailblank = resultPad->resultKey[indexnumber] + resultPad->getLength[indexnumber] - isblank;

						tranpoint = resultPad->resultKey[indexnumber] + aheadblank;
long inlable = 0;
char thischar;
long lthischar;
						for (i=0; i<resultPad->getLength[indexnumber] - aheadblank - tailblank; i++)
						{
							thischar = *tranpoint;
							tranpoint++;

							if (translateHtml)
							{
								if (inlable)
								{
									if (thischar == '>') inlable --;
									continue;
								}
								lthischar = ( ( *((long*)tranpoint) ) | 0x20202020 );

								if (thischar == '<')
								{
									inlable ++;
									if ( (lthischar & 0xffff) == ISBR )		// process <br>, <br />
									{
										(*((long*)mStart)) = (long)DISPLAYCR;
										mStart += 4;
										tranpoint += 2;
									}
									if ( lthischar == ISSCRIPTBEGIN ) inlable++;
									if ( lthischar == ISSCRIPTEND ) inlable--;
									continue;
								}
								if (thischar == '&')
								{
									if ( lthischar == ISSPACE )
									{
										*mStart++ = ' ';
										tranpoint += 5;
									}
									continue;
								}
							}
// Add translate '$' format like {aa$bb$ccc}
							if ((thischar=='"') || (thischar=='\\') || (thischar=='/') || (thischar=='$')) *mStart++ = '\\';	//	for JSON translateSpace
							if (unsigned char(thischar) >= 0x20) *mStart++ = thischar;
						}
					}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Add for translate GB and UTF8, reuse copysize
					else if (trantoGB)
					{
						copysize = 0;
						UTF8toGB2312((PUCHAR)resultPad->resultKey[indexnumber], resultPad->getLength[indexnumber], (PUCHAR)mStart, copysize);
						mStart += copysize;
					}
					else if (trantoUTF)
					{
						copysize = 0;
						GB2312toUTF8((PUCHAR)resultPad->resultKey[indexnumber], resultPad->getLength[indexnumber], (PUCHAR)mStart, copysize);
						mStart += copysize;
					}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef	QRCODE_FUNCTION
					else if (tranQRCode)
					{
						copysize = 0;
						QRCodeTranslate((PUCHAR)resultPad->resultKey[indexnumber], resultPad->getLength[indexnumber], (PUCHAR)mStart, copysize);
						mStart += copysize;
					}
#endif	QRCODE_FUNCTION
					else 
					{
						memcpy(mStart, resultPad->resultKey[indexnumber], resultPad->getLength[indexnumber]);		// normally, mem copy is enough
						mStart += resultPad->getLength[indexnumber];
					}
				}
			}
// 			else												//	in var process, there are no error, is 0, or is VALUE_NOT_FOUND, will omit the result
// 				break;											//	for error control Sept, 17 '13
			varstart = varnow + 4;
		}
	}

#ifdef DEBUG_WAFA
	memcpy (wafadebug, oldstart, min(mStart-oldstart, MAX_DEBUG_STRING));
	wafadebug[min(mStart-oldstart, MAX_DEBUG_STRING)] = 0;
	printf("  For var result: %s\r\n", wafadebug);
#endif DEBUG_WAFA

	__CATCH(MODULE_APPLICATION, "ProcessOneVars")
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	find the first </wafavar--> in given range, return in fileStart. Have NOT use yet												//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long GetEndofWafaVars(char* &fileStart, char* fileEnd)																				//
{																																	//
	char* place;																													//
	place = memstr(fileStart, fileEnd-fileStart, WAFA_VAR_END, sizeof(WAFA_VAR_END)-1);												//
	if (place) fileStart = place + sizeof(WAFA_VAR_END)-1;																			//
	else fileStart = fileEnd;																										//
																																	//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long GetSelect(CListItem* mBuffer)
{
	long ret_err = 0x01;
	ConfigAdditionArray* mArray;
	long* mLength;
	int i = 0, j, len;
	char* tpointer = REAL_BUFFER(mBuffer);
	ContentPad* pad = mBuffer->HeadInfo;

	__TRY

	ret_err = 0x10;
	mArray = &(configInfo->ContendArray[0]);
	mLength = &(mArray->AdditionPad.getLength[0]);
	while (*mLength)
	{
		if (!memcmp(mArray->AdditionArray[i], tpointer, *mLength)) break;
		i++;
		mLength++;
	}
	if (!*mLength) break;				// do not find field name

	ret_err = 0x20;
	tpointer = (char*)memchr(tpointer+*mLength, '=', mBuffer->NProcessSize-*mLength);
	if (!tpointer) break;
	tpointer ++;
	while (*tpointer == ' ') tpointer++;
	len = mBuffer->NProcessSize - (tpointer - REAL_BUFFER(mBuffer));

	ret_err = 0x30;
	mArray ++;
	for (j=1; j<configInfo->ContentAdditionArrayNumber; j++)
	{
		if (mArray->AdditionPad.getLength[i] == len)
		{
			if (!memcmp(mArray->AdditionArray[i], tpointer, len))
			{
				i = 0;
				do 
				{
					COPY_PAD_VAL(pad, i, (&(mArray->AdditionPad)), i);
					i++;
				} while (mArray->AdditionPad.getLength[i]);

// 				ret_err = 0;
				break;
			}
		}
		mArray ++;
	}

	__CATCH(MODULE_APPLICATION, "GetSelect")
}

//	This function is single thread, no concurrency; IMPORTANT		// surely ??
long PrepareDefine(CContextItem* mContext, CListItem* mBuffer, ContentPad* pad, long /*reEnd*/, char* &wafaStart, char* &wafaEnd)
{
	static volatile MYINT	InWafaProcess = MARK_NOT_IN_PROCESS;																	//

	long ret = 1;
	char* oldCheckStart = pad->checkStart;
	long ret_err = 0x01;
	CListItem* fileContent, *cliContent;
	MYINT* pConcurrency;
// 	ContentPad* filePad;

	__TRY

	ret_err = 0x10;
	fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
	if (!fileContent) break;
	pConcurrency = &(fileContent->HeadInfo->inConcurrency);

	cliContent = GetContentByName(mContext, CONTENT_NAME_CLIENT);
	if (!cliContent) break;

//	Is it necessary ??	//	Add and comment May 01 '14
// 	while ( InterCmpExg(&InWafaProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	do 
	{
		ret_err = 0x20;
		wafaStart = memstr(pad->checkStart, pad->checkEnd-pad->checkStart, NASZ(WAFA_DEFINE_START));

		ret_err = 0x30;
		if (!wafaStart)	wafaStart = wafaEnd = 0;
		else wafaEnd = memstr(wafaStart, pad->checkEnd-wafaStart, NASZ(WAFA_DEFINE_END));

		if (!wafaEnd) wafaStart = wafaEnd = 0;

#ifdef	DEBUG_CONCURRENCY
		printf("Find Define, goon. \r\n");
#endif	DEBUG_CONCURRENCY

		ret_err = 0x40;
		*pConcurrency = CONCURRENCY_PAUSE;	//	?	//	May 19 '14

// 		*pConcurrency = CONCURRENCY_GOON;	//	still process define, not all request send.		//	Apr. 26 '14
		if (wafaEnd) wafaEnd += sizeof(WAFA_DEFINE_END) - 1;		// add Sept 12 '13
//		ret = ProcessWafaDefineRefer(mContext, mBuffer, wafaStart, wafaEnd, pad->checkStart);
		ret = ProcessWafaDefineRefer(mContext, mBuffer, pad, wafaStart, wafaEnd);

		ret_err = 0x50;
//	Change into ProcessDefineRefer
// 				if (oldCheckStart == pad->checkStart)		// for nested call by RESTART, if the nest have changed, here do NOT change Sept 12 '13
// 				{
// 					pad->checkStart = wafaEnd;
// 					oldCheckStart = pad->checkStart;
// 				}
		if (ret) break;
// 		else			// if no define found, means all request have been send, should mark concurrency in FILEpad		//	Apr. 26 '14
// 		{
// #ifdef	DEBUG_CONCURRENCY
// 			printf("Define end, pause. \r\n");
// #endif	DEBUG_CONCURRENCY
// 			*pConcurrency = CONCURRENCY_PAUSE;
// 		}
	} while (wafaStart && wafaEnd && *pConcurrency == CONCURRENCY_GOON);	// here usedBuffer is fileContent

	if ( (!wafaStart || !wafaEnd)  && !cliContent->HeadInfo->inConcurrency )
		PrepareReply(mContext, isNULL/*mBuffer*/, cliContent->HeadInfo, 0, wafaStart, wafaEnd);

	InWafaProcess = MARK_NOT_IN_PROCESS;

	__CATCH(MODULE_APPLICATION, "PrepareDefine")
}

long PrepareReply(CContextItem* mContext, CListItem* mBuffer, ContentPad* pad, long /*reEnd*/, char* &wafaStart, char* &wafaEnd)
{
	long ret;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x00;
	if (!mContext->PPeer) break;				//	for create functions, 

	ret_err = 0x10;
	wafaStart = memstr(pad->checkStart, pad->checkEnd-pad->checkStart, NASZ(WAFA_BODY_START));
	if (wafaStart) ret = ProcessHtmlReplay(mContext);
	else ret = ProcessNormalReplay(mContext, mBuffer);

	__CATCH(MODULE_APPLICATION, "PrepareReply")
}

// #endif ( defined(CONTENTPAD_APPLICATION) )
