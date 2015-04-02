
#include "stdafx.h"
// #include "ContentPad.h"

// #ifdef CONTENTPAD_APPLICATION

extern long BYTE_MASK[5];

// #if ( defined(CONTENTPAD_APPLICATION) )

extern	ContentPadConfig*	configInfo;
extern unsigned char ASCII_DEC[ASCII_NUMBER];
extern unsigned char ASCII_HEX[ASCII_NUMBER];
extern CService theService;

#ifdef	FORWARDLOOP_FUNCTION
extern	ForwardLoopConfig	*pForwardLoopConfig;
#endif	FORWARDLOOP_FUNCTION

WafaProcessFunction GetWafaFunction (char* functionName)
{
	if (!strncmp(functionName, NASZ("Add"))) return Wafa_Add;
	else if (!strncmp(functionName, NASZ("Exist"))) return Wafa_Exist;
	else if (!strncmp(functionName, NASZ("NotExist"))) return Wafa_NotExist;
	else if (!strncmp(functionName, NASZ("Scanf"))) return Wafa_Scanf;
	else if (!strncmp(functionName, NASZ("MultiScanf"))) return Wafa_MultiScanf;
	else if (!strncmp(functionName, NASZ("IfScanf"))) return Wafa_IfScanf;
	else if (!strncmp(functionName, NASZ("Var"))) return Wafa_Var;
	else if (!strncmp(functionName, NASZ("Count"))) return Wafa_Count;
	else if (!strncmp(functionName, NASZ("LoopEnd"))) return Wafa_Add;			// same as add
	else if (!strncmp(functionName, NASZ("Value"))) return Wafa_Value;
	else if (!strncmp(functionName, NASZ("Compare"))) return Wafa_Compare;
	else if (!strncmp(functionName, NASZ("Assign"))) return Wafa_Assign;
	else if (!strncmp(functionName, NASZ("Match"))) return Wafa_Match;
	else if (!strncmp(functionName, NASZ("Copy"))) return Wafa_Copy;			// Copy ReferName:Keyfrom to FILE:Start
	else if (!strncmp(functionName, NASZ("LoopPrint"))) return Wafa_LoopPrint;
	else if (!strncmp(functionName, NASZ("LoopAssign"))) return Wafa_LoopAssign;
	else if (!strncmp(functionName, NASZ("Replace"))) return Wafa_Replace;

#ifdef	NTLM_FUNCTION
	else if (!strncmp(functionName, NASZ("NTLM"))) return Wafa_NTLM;
#endif	NTLM_FUNCTION

#ifdef CONFIRMDNS_FUNCTION
	else if (!strncmp(functionName, NASZ("GetIPMac"))) return Red_GetIPMac;
	else if (!strncmp(functionName, NASZ("SetIPMac"))) return Red_SetIPMac;
#endif CONFIRMDNS_FUNCTION

	else return Wafa_NULL;
}

WafaCreateFunciont GetCreateFunction (char* functionName)
{
	if (!strncmp(functionName, NASZ("CREATE"))) return Create_Listen;
	else if (!strncmp(functionName, NASZ("PEER"))) return Create_Peer;
	else if (!strncmp(functionName, NASZ("NOP"))) return Create_NULL;
	else return NULL;
}

extern sockaddr_in AddrForward;

long Create_Listen(CContextItem* mContext, CListItem* &mBuffer, char* mStart)
{
	long	ret_err = 0x01;
	ContentPad* pad = mBuffer->HeadInfo;
	CContextItem* retContext, *retContentNest;
	CProtocol* pTCP = theService.pStartApplication->pTCP;
	struct addrinfo *aiList = NULL;
	__TRY

	if (!strncmp(pad->serverAction, DASZ("ContentPad")))
	{
		ret_err = 0x10;
		CContentPadServer* pContentPad = theService.pStartApplication->pContentPad;
		CInterProtocol* pInter = theService.pStartApplication->pInter;
		CSSLServerApplication* pSSLSerMode = theService.pStartApplication->pSSLSerMode;

		if (!memcmp(pad->serverHost, NASZ("http://")))
		{
			if (MyGetInfoAddr(pad->serverHost+7, pad->getLength[PARA_DEF_HOST]-7, aiList)) break;	//	add Dec. 22 '14 
			retContext = CreateApplication(pContentPad, pTCP, aiList->ai_addr,	sizeof(sockaddr_in), NULL, NULL, 0, atoi(pad->serverKey));
			if (!retContext) break;
		}
		if (!memcmp(pad->serverHost, NASZ("https://")))
		{
			if (MyGetInfoAddr(pad->serverHost+8, pad->getLength[PARA_DEF_HOST]-8, aiList)) break;	//	add Dec. 22 '14
			retContext = CreateApplication((CApplication*)pSSLSerMode, pTCP, aiList->ai_addr, sizeof(sockaddr_in), NULL, NULL, 0, atoi(pad->serverKey));
			if (!retContext) break;
			retContentNest = CreateApplication(pContentPad, (CProtocol*)pInter, retContext, sizeof(CContextItem*), NULL, NULL, 0);
			if (!retContentNest) break;
		}
	}

#ifdef	CONFIRMDNS_FUNCTION
	struct addrinfo *aiForward = NULL;

	if (!strncmp(pad->serverAction, DASZ("ConfirmDNS")))
	{
		CDNSApplication* pDNS = theService.pStartApplication->pDNS;
		CSingleUDPProtocol* pUDPSingle = theService.pStartApplication->pUDPSingle;
		CUDPProtocol* pUDP = theService.pStartApplication->pUDP;

		if (MyGetInfoAddr(pad->serverHost+6, pad->getLength[PARA_DEF_HOST]-6, aiList)) break;	//	add Dec. 22 '14
		if (MyGetInfoAddr(pad->serverReferer+6, pad->getLength[PARA_DEF_REFERER]-6, aiForward)) break;	//	add Dec. 22 '14


		if (!memcmp(pad->serverHost, NASZ("dns://")))
		{
			retContext = CreateApplication(pDNS, pUDPSingle, aiList->ai_addr, sizeof(sockaddr_in), pUDPSingle, aiForward->ai_addr, sizeof(sockaddr_in), atoi(pad->serverKey));
			if (!retContext) break;
		}
		if (!memcmp(pad->serverHost, NASZ("udp://")))
		{
			retContext = CreateApplication(pDNS, pUDP, aiList->ai_addr, sizeof(sockaddr_in), pUDP, aiForward->ai_addr, sizeof(sockaddr_in), atoi(pad->serverKey));
			if (!retContext) break;
		}
	}
#endif	CONFIRMDNS_FUNCTION

#ifdef	FORWARDLOOP_FUNCTION
 	if (!strncmp(pad->serverAction, DASZ("ForwardListen")))
	{
		ret_err = 0x10;
		long &nowNumber = pForwardLoopConfig->ContentListenNumber;
		CForwardLoopApplication* &nowForward = pForwardLoopConfig->pForwardLoop[nowNumber];
		nowForward = new CForwardLoopApplication();
		if (!nowForward) break;

		ret_err = 0x20;
		nowForward->SetAction(CApplication::Resoucce, "BUFFER");
		RegisterApplication(nowForward, APPLICATION_FORWARD_LOOP, FLAG_DUPLEX | FLAG_LATER_CONNECT | FLAG_MULTI_PROTOCOL );

		ret_err = 0x30;
		if (MyGetInfoAddr(pad->serverHost, pad->getLength[PARA_DEF_HOST], aiList)) break;	//	add Dec. 22 '14
		retContext = CreateApplication(nowForward, pTCP, aiList->ai_addr, sizeof(sockaddr_in), NULL, NULL, 0, atoi(pad->serverKey));
		if (!retContext) break;
		strncpy_s(pForwardLoopConfig->mForwardLoopName[nowNumber], pad->serverName, MAX_PATH);
		nowNumber++;
	}
#endif	FORWARDLOOP_FUNCTION

#ifdef	PROXY_FUNCTION
	if (!strncmp(pad->serverAction, DASZ("Proxy")))
	{
		CProxyApplication* pProxy = theService.pStartApplication->pProxy;
		CPassbyApplication* pPassby = theService.pStartApplication->pPassby;
		CDirectoryProtocol* pDir = theService.pStartApplication->pDir;
		CInterProtocol* pInter = theService.pStartApplication->pInter;
		AddrForward.sin_family = AF_INET;  
		AddrForward.sin_port = htons(0);    
		AddrForward.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");

		if (MyGetInfoAddr(pad->serverHost, pad->getLength[PARA_DEF_HOST], aiList)) break;	//	add Dec. 22 '14

//	Change to Referer, NOT test yet
		retContext = CreateApplication((CApplication*)pPassby, pTCP, aiList->ai_addr, sizeof(sockaddr_in), pDir, pad->serverReferer, strlen(pad->serverReferer)+1);
//		retContext = CreateApplication((CApplication*)pPassby, pTCP, aiList->ai_addr, sizeof(sockaddr_in), pDir, pad->serverName, strlen(pad->serverName)+1);
		if (!retContext) break;
		retContentNest = CreateApplication(pProxy, (CProtocol*)pInter, retContext, sizeof(CContextItem), pTCP, (void*)&AddrForward, sizeof(sockaddr_in), 10);
		if (!retContentNest) break;
	}
#endif	PROXY_FUNCTION

	__CATCH(MODULE_RESOURCE, "Create_Listen")
};

long Create_Peer(CContextItem* mContext, CListItem* &mBuffer, char* mStart)
{
	long	ret_err = 0x01;
	ContentPad* pad = mBuffer->HeadInfo;
	CContextItem* retContext = 0;
	CTCPProtocol* pTCP = theService.pStartApplication->pTCP;
	CTCPReadProtocol* pTCPRead = theService.pStartApplication->pTCPRead;

	CFileProtocol* pFile = theService.pStartApplication->pFile;
	CFileReadProtocol* pFileRead = theService.pStartApplication->pFileRead;

	struct addrinfo *aiList = NULL;
	long result;

	__TRY

	if (!strncmp(pad->serverAction, DASZ("ContentPad")))
	{
		CContentPadServer* pContentPad = theService.pStartApplication->pContentPad;

		if (!memcmp(pad->serverHost, NASZ("http://")))
		{
			if (pad->getLength[PARA_DEF_REFERER] != VALUE_NOT_FOUND) 
				result = MyGetInfoAddr(pad->serverReferer, pad->getLength[PARA_DEF_REFERER], aiList);	//	Add Dec. 22 '14
			else
				result = MyGetInfoAddr(pad->serverHost+7, pad->getLength[PARA_DEF_HOST]-7, aiList);		//	Add Dec. 22 '14
			if (result) break;
			retContext = pContentPad->AddProtocolPeer(pTCP, aiList->ai_addr, sizeof(sockaddr_in), pad->serverName, pad->serverHost+7, atoi(pad->serverKey));
		}
		else if (!memcmp(pad->serverHost, NASZ("httpread://")))
		{
			if (pad->getLength[PARA_DEF_REFERER] != VALUE_NOT_FOUND) 
				result = MyGetInfoAddr(pad->serverReferer, pad->getLength[PARA_DEF_REFERER], aiList);	//	Add Dec. 22 '14
			else
				result = MyGetInfoAddr(pad->serverHost+11, pad->getLength[PARA_DEF_HOST]-11, aiList);		//	Add Dec. 22 '14
			if (result) break;
			retContext = pContentPad->AddProtocolPeer(pTCPRead, aiList->ai_addr, sizeof(sockaddr_in), pad->serverName, pad->serverHost+11, atoi(pad->serverKey));
		}
		else if (!memcmp(pad->serverHost, NASZ("file://")))				//	Add Dec. 22 '14 for WriteCache
		{
			retContext = pContentPad->AddProtocolPeer(pFile, pad->serverHost+7, pad->getLength[PARA_DEF_HOST]-7, pad->serverName);
		}
		else if (!memcmp(pad->serverHost, NASZ("fileread://")))		// here 11 is sizeof("fileread://")-1
		{
			retContext = pContentPad->AddProtocolPeer(pFileRead, pad->serverHost+11, pad->getLength[PARA_DEF_HOST]-11, pad->serverName);
		}

// 		else
	}


#ifdef	FORWARDLOOP_FUNCTION
	CForwardLoopApplication* pfApp = NULL;
	struct addrinfo *aiList = NULL;

	if (!strncmp(pad->serverAction, DASZ("ForwardListen")))
	{
		ret_err = 0x10;
		GetApplicationByName(pad->serverName, pfApp);
		if (!pfApp) break;

		ret_err = 0x20;
		result = MyGetInfoAddr(pad->serverHost, pad->getLength[PARA_DEF_HOST], aiList);
		if (result) break;
		retContext = pfApp->AddProtocolPeer(pTCP, aiList->ai_addr, sizeof(sockaddr_in), pad->serverName, pad->serverHost, atoi(pad->serverKey));
	}
#endif	FORWARDLOOP_FUNCTION

	__CATCH(MODULE_RESOURCE, "Create_Peer")
};

long Create_NULL(CContextItem* mContext, CListItem* &mBuffer, char* mStart)
{
	return 0;
};

#ifdef CONFIRMDNS_FUNCTION
long Red_GetIPMac (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
//	mContext is necessary. for client ip is mContext->clientAddr
	DWORD ret;
	IPAddr DestIp = ((CTCPContext*)mContext)->clientAddr.sin_addr.S_un.S_addr;
	__int64 MacAddr = 0;					// for 6-byte hardware addresses
	ULONG PhysAddrLen = 6;					// default to length of six bytes
	ContentPad* resultPad = referContent->HeadInfo;
	char* place;

	ret = SendARP(DestIp, INADDR_ANY , &MacAddr, &PhysAddrLen);
	if (ret != NO_ERROR) MacAddr = 0;

	place = resultPad->clientIP;
// 	Myntoa((char*)&DestIp, place);			//	printable ip address format
	Myitoh(DestIp, place);			//	network order HEX format
	resultPad->resultKey[keyFrom] = resultPad->clientIP;
	resultPad->getLength[keyFrom] = place - resultPad->clientIP;

	place = resultPad->clientMac;
	Mymtoh((char*)&MacAddr, place);
	resultPad->resultKey[keyFrom+1] = resultPad->clientMac;
	resultPad->getLength[keyFrom+1] = place - resultPad->clientMac;
	return 0;
}

long Red_SetIPMac (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
// 	IPAddr ip;
// 	unsigned __int64 mac;
	ContentPad* resultPad = referContent->HeadInfo;

	char* nowloop = NULL;
	long loopstrlen;
	char* nowplace = start;
	long ret;

	if ( resultPad->getLength[PARA_REF_LOOP] != VALUE_NOT_FOUND && key && keyLen) 
	{
//	I think so, but have not tested		//	May 24 '14
// 		nowloop = resultPad->serverLoop;		//	Attention !!! maybe infoPad->serverLoop
		nowloop = infoPad->serverLoop;
		loopstrlen = resultPad->getLength[PARA_REF_LOOP];
		do 
		{
			nowplace = memstr(nowplace+loopstrlen, end-nowplace, nowloop, loopstrlen);
			if (!nowplace) break;
			ret = Wafa_Scanf(NULL, referContent, nowplace, end, key, keyLen, keyFrom);
			if (ret) break;
			SetOneIPMac(resultPad, keyFrom);

		} while (TRUE);
	}
	else SetOneIPMac(resultPad, keyFrom);
	return 0;
}
#endif CONFIRMDNS_FUNCTION

typedef struct NTLMStruct
{
	char* freePlace;									//	save temp vars for wafarefer use. 
	MYINT	inProcess;
	char domain_name[SMALL_CHAR];
	char host_name[SMALL_CHAR];
	char user_name[SMALL_CHAR];
	char pass_name[SMALL_CHAR];
	char nosence_name[SMALL_CHAR];
	char type1mess[MAX_URL];
	char type1base[MAX_URL];
	char type2mess[MAX_URL*3];
	char type2base[MAX_URL*4];
	char type3mess[MAX_URL*3];
	char type3base[MAX_URL*4];
	int type1messlen, type1baselen, type2messlen, type2baselen, type3messlen, type3baselen;
}NTLMStruct;

//	This two is used for save temp wafarefer vars	//	May 24 '14
CListItem* GetReferStruct(CContextItem* mContext)
{
	CListItem* freeContent;
	NTLMStruct* ntlm;

	freeContent = GetContentByName(mContext, NASZ(CONTENT_NAME_NTLM));
	if (!freeContent)
	{
		freeContent = mContext->PApplication->GetApplicationBuffer();
		if (!freeContent) return NULL;
		AddtoContentList(mContext, freeContent, CONTENT_NAME_NTLM);
		ntlm = (NTLMStruct*)REAL_BUFFER(freeContent);
		ntlm->freePlace = (char*)(ntlm+1)+0x10;
		ntlm->inProcess = MARK_NOT_IN_PROCESS;
	}
	return freeContent;
}
//	This two is used for save temp wafarefer vars	//	May 24 '14		//	If overload will crash
char* AddReferPlace(CContextItem* mContext, ContentPad* pad, char* str, long &len)
//	Wafa_LoopPrint do NOT use this function, it use GetReferStruct directly.
{
	CListItem* freeContent = GetReferStruct(mContext);
	if (!freeContent)
	{
		len = 0;
		return str;			//	NOT return 0, len = 0 is enough
	}
	NTLMStruct* ntlm = (NTLMStruct*)REAL_BUFFER(freeContent);

	while ( InterCmpExg(&(ntlm->inProcess), MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	char* oldplace = ntlm->freePlace;

	ProcessOneVar(mContext, pad, ntlm->freePlace, str, str+len );
	len = ntlm->freePlace - oldplace;
	ntlm->freePlace += 0x10;			// should control size for freePlace
	ntlm->inProcess = MARK_NOT_IN_PROCESS;
	return oldplace;
}

#ifdef	NTLM_FUNCTION
long Wafa_NTLM (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	CListItem* ntBuffer;
	NTLMStruct* ntstruct;
	ContentPad* resultPad = referContent->HeadInfo;
	char* mStart;

//	Change to GetReferStruct	//	May 24 '14
	ntBuffer = GetReferStruct(mContext);
	if (!ntBuffer) return 1;
// 	ntBuffer = GetContentByName(mContext, NASZ(CONTENT_NAME_NTLM));
// 	if (!ntBuffer)
// 	{
// 		ntBuffer = mContext->PApplication->GetApplicationBuffer();
// 		if (!ntBuffer) return 1;
// 		AddtoContentList(mContext, ntBuffer, CONTENT_NAME_NTLM);
// 	}
	ntstruct = (NTLMStruct*)REAL_BUFFER(ntBuffer);

	char* typeend, *domainend, *hostend, *userend, *passwend, *nosence;
	if (*key == '1')
	{
//	"1?csfans?SNPE-MAST-APP1"
		typeend = (char*)memchr(key, '?', keyLen);
		domainend = (char*)memchr(typeend+1, '?', keyLen-(typeend-key)-1);
		hostend = key+keyLen;
		mStart = ntstruct->domain_name;
		ProcessOneVar(mContext, resultPad, mStart, typeend+1, domainend);
		mStart = ntstruct->host_name;
		ProcessOneVar(mContext, resultPad, mStart, domainend+1, hostend);

		ntstruct->type1messlen = GenType1(ntstruct->domain_name, ntstruct->host_name, ntstruct->type1mess);
		ntstruct->type1baselen = Base64encode(ntstruct->type1base,  ntstruct->type1mess, ntstruct->type1messlen)-1;
		if (ntstruct->type1baselen)
		{
			resultPad->resultKey[keyFrom] = ntstruct->type1base;
			resultPad->getLength[keyFrom] = ntstruct->type1baselen;
		}
	}
	if (*key == '3')
	{
//	"3?csfans?SNPE-MAST-APP1?%d64?%d65?%r:SHEAD%d65"	
		typeend = (char*)memchr(key, '?', keyLen);
		domainend = (char*)memchr(typeend+1, '?', keyLen-(typeend-key)-1);
		hostend = (char*)memchr(domainend+1, '?', keyLen-(domainend-key)-1);
		userend = (char*)memchr(hostend+1, '?', keyLen-(hostend-key)-1);
		passwend = (char*)memchr(userend+1, '?', keyLen-(userend-key)-1);
		nosence = key+keyLen;
		mStart = ntstruct->domain_name;
		ProcessOneVar(mContext, resultPad, mStart, typeend+1, domainend);
		mStart = ntstruct->host_name;
		ProcessOneVar(mContext, resultPad, mStart, domainend+1, hostend);
		mStart = ntstruct->user_name;
		ProcessOneVar(mContext, resultPad, mStart, hostend+1, userend);
		mStart = ntstruct->pass_name;
		ProcessOneVar(mContext, resultPad, mStart, userend+1, passwend);
		mStart = ntstruct->type2base;
		ProcessOneVar(mContext, resultPad, mStart, passwend+1, nosence);

		ntstruct->type2messlen = Base64decode(ntstruct->type2mess, ntstruct->type2base);
		type_2_message* t2mess = (type_2_message*) ntstruct->type2mess;
		ntstruct->type3messlen = GenType3(ntstruct->domain_name, ntstruct->user_name, ntstruct->host_name, ntstruct->pass_name, (char*)&t2mess->nonce[0], (char*)&ntstruct->type3mess[0]);
		ntstruct->type3baselen = Base64encode(ntstruct->type3base,  ntstruct->type3mess, ntstruct->type3messlen)-1;
		if (ntstruct->type3baselen)
		{
			resultPad->resultKey[keyFrom] = ntstruct->type3base;
			resultPad->getLength[keyFrom] = ntstruct->type3baselen;
		}
	}

	return 0;
}
#endif	NTLM_FUNCTION


// key include '<', may have '>' , from one '<some mark>' to its match '</some>'
long Wafa_Match (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long byteMask = keyLen;
	unsigned char* nowplace = (unsigned char*)start;
	long* nowend;
	long* nowkey = (long*)key;
	unsigned char* valstart;
	long loopnest;
	ContentPad* resultPad = referContent->HeadInfo;

	if (*key == '<') 
	{
		nowkey = (long*)(key+1);
		byteMask--;
	}
	if (*(key+keyLen-1) == '>') byteMask--;
	byteMask = BYTE_MASK[byteMask%4];

	valstart = (unsigned char*)memstr(start, end-start, key, keyLen);
	if (!valstart) return 1;

	loopnest = 1;
	nowplace = valstart+keyLen;
	while (nowplace < (unsigned char*)end)
	{
		if (*nowplace++ == '<')
		{
			while (*nowplace <= 0x20 && nowplace<=(unsigned char*)end) nowplace++;
			if (*nowplace == '/')
			{
				nowplace++;
				nowend = (long*)(nowplace);
				if ( !((*nowkey ^ *nowend) & byteMask) )
				{
					loopnest--;
					if (!loopnest) 
					{
						while (*nowplace != '>' && nowplace<=(unsigned char*)end) nowplace++;
						if (nowplace > (unsigned char*)end) return 1;
						nowplace++;

						break;
					}

				}
			}
			else
			{
				nowend = (long*)(nowplace);
				if ( !((*nowkey ^ *nowend) & byteMask) )
				{
					loopnest++;

				}
			}
		}
	}
	if (!loopnest)
	{
		resultPad->resultKey[keyFrom] = (char*)valstart;
		resultPad->getLength[keyFrom] = nowplace - valstart;
	}

	return 0;
}


//	Attention, Assign is an unfinished function, it should use Mark for really remember the key place, 
//	instead of use serverKey in ContentPad, for serverKey will be rewriteed by another wafadefine or wafarefer
long Wafa_Assign (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long isequ;
	
	char* equval, *unequval;
	char* keyend = key + keyLen;
	ContentPad* resultPad = referContent->HeadInfo;
	char* nowkey;
	long nowlen;
	long notset = 0;										// if not found ::, set to 1, DO NOT change the val	// Nov. 14 '14

// 	equval = memstr(key, keyLen, NASZ("??"));
// 	if (!equval) return VALUE_NOT_FOUND;
// 	equval += 2;
// 
// 	unequval = memstr(equval, keyend-equval, NASZ("::"));
// //	if (!unequval) return VALUE_NOT_FOUND;					// for without :: // Nov. 14 '14
// //	unequval += 2;
// 	if (!unequval)
// 	{
// 		unequval = keyend;
// 		notset = 1;											// MUST set this, "??xx::" means set NULL while false, "??xx" means not set while false
// 	}
// 	else unequval += 2;
// 
// 	isequ = Wafa_Compare(mContext, referContent, start, end, key, equval-key-2, keyFrom, infoPad);
// 	if (isequ==VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
// 
// 	if (isequ)		// not OK, use var after ::
// 	{
// 		if (notset) return 0;								// not found ::, not set val	// Nov. 14 '14
// 		nowkey = unequval;
// 		nowlen = keyend-unequval;
// 	}
// 	else			// OK, use var before ::
// 	{
// 		nowkey = equval;
// //		nowlen = unequval-equval-2;					// a little bug for not sub 2 without ::	// Nov 16 '14
// 		nowlen = unequval - equval;
// 		if (!notset) nowlen -= 2;
// 	}

	equval = memstr(key, keyLen, NASZ("??"));
	if (equval)
	{
		equval += 2;
		unequval = memstr(equval, keyend-equval, NASZ("::"));
		//	if (!unequval) return VALUE_NOT_FOUND;					// for without :: // Nov. 14 '14
		//	unequval += 2;
		if (!unequval)
		{
			unequval = keyend;
			notset = 1;											// MUST set this, "??xx::" means set NULL while false, "??xx" means not set while false
		}
		else unequval += 2;

		isequ = Wafa_Compare(mContext, referContent, start, end, key, equval-key-2, keyFrom, infoPad);
		if (isequ==VALUE_NOT_FOUND) return VALUE_NOT_FOUND;

		if (isequ)		// not OK, use var after ::
		{
			if (notset) return 0;								// not found ::, not set val	// Nov. 14 '14
			nowkey = unequval;
			nowlen = keyend-unequval;
		}
		else			// OK, use var before ::
		{
			nowkey = equval;
			//		nowlen = unequval-equval-2;					// a little bug for not sub 2 without ::	// Nov 16 '14
			nowlen = unequval - equval;
			if (!notset) nowlen -= 2;
		}
	}
	else			// NO ?? ::, all is assigned key
	{
		nowkey = key;
		nowlen = keyLen;
	}


//	Change to AddReferPlace		//	May 24 '14
// //	this is a bug, fixed in Jan.17 '14, for must find the MARK in FILE content, not the referContent 
// // 	nowkey = memstr(REAL_BUFFER(referContent), referContent->NProcessSize, nowkey, nowlen);
// 	CListItem* fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
// 	if (!fileContent) return VALUE_NOT_FOUND;
// 	nowkey = memstr(REAL_BUFFER(fileContent), fileContent->NProcessSize, nowkey, nowlen);
// 	if (!nowkey) return VALUE_NOT_FOUND;
// 
// 	resultPad->resultKey[keyFrom] = nowkey;
// 	resultPad->getLength[keyFrom] = nowlen;
	CListItem* fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
	if (!fileContent) return VALUE_NOT_FOUND;
	ContentPad* filePad = fileContent->HeadInfo;

	resultPad->resultKey[keyFrom] = AddReferPlace(mContext, resultPad, nowkey, nowlen );
	resultPad->getLength[keyFrom] = nowlen;
	return 0;
}

//	Rewrite Compare by ProcessOneVar	// Jun. 30 '14
long Wafa_Compare (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	ContentPad* resultPad = referContent->HeadInfo;
	long ret_err = 0x01;
	char* firstkey = key, *firstend;
	char* secondkey, *secondend = key+keyLen;

	int cmpmode = 0;		// 1 for ==, /*2 for <<, 3 for <=, 4 for >>, 5 for >=,*/ 6 for !=
	int returnequ = 0;			// 0 for is OK, 1 for not OK

	char firstval[MAX_COOKIE];
	char secondval[MAX_COOKIE];
	char* firstplace = firstval, *secondplace = secondval;

	secondkey = memstr(key, keyLen, NASZ("=="));
	if (secondkey) cmpmode = 1;
	else
	{
		secondkey = memstr(key, keyLen, NASZ("!="));
		if (secondkey) cmpmode = 6;
	}

	if (!cmpmode) return VALUE_NOT_FOUND;
	firstend = secondkey;
	secondkey += 2;

	ProcessOneVar(mContext, resultPad, firstplace, firstkey, firstend );
	ProcessOneVar(mContext, resultPad, secondplace, secondkey, secondend );

	if ((firstplace-firstval) != (secondplace-secondval)) returnequ = 1;
	else
	{
		if (memcmp(firstval, secondval, firstplace-firstval)) returnequ = 1;
	}

	if (cmpmode==1) return returnequ;
	if (cmpmode==6) return 1-returnequ;

	return VALUE_NOT_FOUND;
}


#define SIZEOF_NAME		6

long Wafa_Value (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long ret_err = 0x01;
	char* instart = start;
	char* thisend;
	char* inend = end;
	char* nowend;
	char* keystart = key;
	char* keyend = key+keyLen;
	char* keynext;
	long nowkeylen;
	char keyvalue [NORMAL_CHAR];

	int nowkey = keyFrom;
	ContentPad* resultPad = referContent->HeadInfo;

	memcpy(keyvalue, NASZ("name=\""));			// SIZEOF_NAME
	while (instart<inend && keystart<keyend)
	{
		ret_err = 0x10;
		keynext = memstr(keystart, keyend - keystart, "&", 1);
		if (!keynext) keynext = keyend;
		nowkeylen = keynext - keystart;
		memcpy(keyvalue+SIZEOF_NAME, keystart, nowkeylen);
		*(keyvalue+SIZEOF_NAME+nowkeylen) = '"';

		instart = memstr(instart, inend-instart, keyvalue, nowkeylen + SIZEOF_NAME + 1/*the last "*/);
		if (!instart) break;

		ret_err = 0x20;
		thisend = memstr(instart, inend-instart, ">", 1);
		if (!thisend) break;

		ret_err = 0x30;
		instart = memstr(instart, thisend-instart, NASZ("value="));
		if (!instart) break;
		instart += sizeof("value=")-1;

		ret_err = 0x40;
		if (*instart == '"')
		{
			instart++;
			nowend = memstr(instart, thisend-instart, NASZ("\""));
			if (!nowend) break;
		}
		else
		{
			nowend = memstr(instart, thisend-instart, NASZ(" "));
			if (!nowend) nowend = thisend;
		}

		ret_err = 0x50;
		resultPad->resultKey[nowkey] = instart;
		resultPad->getLength[nowkey] = nowend-instart;
		nowkey++;
		if (nowkey>=MAX_CONTENT_KEY) break;
		instart = thisend + 1;
		keystart = keynext + 1;

		ret_err = 0;
	}

	return ret_err;
}


long Wafa_NULL (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	return 1;
}

long Wafa_Add (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	infoPad->serverNowEnd = 0;			//	used for Scanf Loop, this is for LoopEnd		//	May 25 '14
	return 0;			// zero is OK
}

long Wafa_Exist (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	char* ret;
	ContentPad* resultPad = referContent->HeadInfo;

	ret = memstr(start, end-start, key, keyLen);
	if (keyFrom)
	{
		if (ret) resultPad->resultKey[keyFrom] = 0;
		else resultPad->resultKey[keyFrom] = (char*)1;
		resultPad->getLength[keyFrom] = 0;
	}
	if (ret) return 0;
	else return 1;
}

long Wafa_NotExist (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	char* ret;
	ContentPad* resultPad = referContent->HeadInfo;

	ret = memstr(start, end-start, key, keyLen);
	if (keyFrom)
	{
		if (ret) resultPad->resultKey[keyFrom] = (char*)1;
		else resultPad->resultKey[keyFrom] = 0;
		resultPad->getLength[keyFrom] = 0;
	}
	if (ret) return 1;
	else return 0;
}

long Wafa_IfScanf (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	char *dStart = start, *kStart = key, *thiskEnd, *kend = key+keyLen;

	while (dStart < end && kStart < kend)
	{
		thiskEnd = (char*)memchr(kStart, '%', kend - kStart);
		if (!thiskEnd) thiskEnd = kend;						//	do not find mark

		dStart = memstr(dStart, end-dStart, kStart, thiskEnd-kStart);
		if (!dStart) return 1;						//	do not find the first key for Scanf
		
		kStart = thiskEnd + 2;
	}
	return Wafa_Scanf(mContext, referContent, start, end, key, keyLen, keyFrom, infoPad);
}

//	This function have bug, for % is NOT allow in key. should use %% to do this.		//	find this in Apr. 25 '14
long Wafa_Scanf (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long ret_err = 0x01;
	char* instart = start;
	char* inend = end;
	char* nowend = 0;
	char* keystart = key;
	char* keyend = key+keyLen;
	char* keynext = key;

	char nowmode = '*';
	int  nowkey = keyFrom;
	ContentPad* resultPad = referContent->HeadInfo;
// 	CListItem* fileContent;
// 	ContentPad* filePad;

	if (!key) return ret_err;
	infoPad->serverNowStart = 0;

	while (instart<inend && keystart<keyend)
	{
		ret_err = 0x10;
		if (*keystart == '%')
		{
			keystart++;
			nowmode = *(keystart++);
		}
		keynext = (char*)memchr(keystart, '%', keyend-keystart);
		if (!keynext) keynext = keyend;

		ret_err = 0x20;
		nowend = (keystart == keyend) ? inend : memstr(instart, inend-instart, keystart, keynext-keystart);	// Aug 29 '13 for key end by %s
		if (!infoPad->serverNowStart) infoPad->serverNowStart = nowend;											//	add for replace Jan. 07 '14

//		nowend = memstr(instart, inend-instart, keystart, keynext-keystart);

// 		if (!nowend) break;		//	If not found, use Default value,	add in May 24 '14, Now only support only one default key
		if (!nowend)
		{
			long len;
// 			fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
// 			if (!fileContent) break;
// 			filePad = fileContent->HeadInfo;

			if ( infoPad->getLength[PARA_REF_DEFAULT] != VALUE_NOT_FOUND )
			{
//	I first use MARK to save var, but now I use freeContent. 
// 				defaultkey = memstr(REAL_BUFFER(fileContent), fileContent->NProcessSize, filePad->serverDefault, filePad->getLength[PARA_REF_DEFAULT]);
// 				if (!defaultkey) break;
// 				resultPad->resultKey[keyFrom] = defaultkey;
// 				resultPad->getLength[keyFrom] = filePad->getLength[PARA_REF_DEFAULT];
				len = infoPad->getLength[PARA_REF_DEFAULT];
				resultPad->resultKey[keyFrom] = AddReferPlace(mContext, resultPad, infoPad->serverDefault, len );
				resultPad->getLength[keyFrom] = len;
				ret_err = 0;
			}
			break;
		}

		ret_err = 0x30;
		if (nowmode != '*')
		{
			resultPad->resultKey[nowkey] = instart;
			resultPad->getLength[nowkey] = nowend-instart;
			nowkey++;
			if (nowkey>=MAX_CONTENT_KEY) break;
		}
		instart = nowend + (keynext-keystart);
		keystart = keynext;
	}																																//

	infoPad->serverNowEnd = nowend;			//	Used in Loop in Scanf, Have not TEST YET			//	May 25 '14

	if (instart >= inend) ret_err = 0;		// is it ok ?? str:name="abc" /> while key:name="%s";%*>%s</Domain, it report OK ??? // Nov 18 '14
	else if (keynext >= keyend) ret_err = 0;

	long isequ;

	if (infoPad->getLength[PARA_REF_IF] != VALUE_NOT_FOUND)
	{
		isequ = Wafa_Compare(mContext, referContent, start, end, (char*)(infoPad->valPlace[PARA_REF_IF].pValue), infoPad->getLength[PARA_REF_IF], keyFrom, infoPad);
		if (isequ) return isequ;
	}

	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in Wafa_Scanf: 0x%x. In Key:%d\r\n", ret_err, keystart-key);		//
	}																																//
	return ret_err;																													//
}

long Wafa_MultiScanf (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long ret_err = 0x01;
	char *keystart = key;
	char *keyend = key + keyLen;
	char *keynext;
	long nowkeylen;
	int fail;

	int  nowkey = keyFrom;
	ContentPad* resultPad = referContent->HeadInfo;

	while (keystart < keyend)
	{
		ret_err = 0x10;
		fail = 0;
		keynext = memstr(keystart, keyend - keystart, "&", 1);
		if (!keynext) keynext = keyend;
		nowkeylen = keynext - keystart;

		fail = Wafa_Scanf(mContext, referContent, start, end, keystart, nowkeylen, nowkey, infoPad);
		if (fail)
		{
			resultPad->resultKey[nowkey] = 0;
			resultPad->getLength[nowkey] = 0;
		}
		nowkey++;
		if (nowkey>=MAX_CONTENT_KEY) break;
		keystart = keynext + 1;
	}
	return 0;
}

//	have not use yet, used for xml format key"value" now
long Wafa_Var(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long ret_err = 0x01;
	char *keystart = key;
	char *keyend = key + keyLen;
	char *keynext;
	long nowkeylen;
	int fail;

	char* instart, *inend;
	int  nowkey = keyFrom;
	ContentPad* resultPad = referContent->HeadInfo;

	while (keystart < keyend)
	{
		ret_err = 0x10;
		fail = 0;
		keynext = memstr(keystart, keyend - keystart, "&", 1);
		if (!keynext) keynext = keyend;
		nowkeylen = keynext - keystart;

		//	Add this following lines Jan. 02 '15
		instart = start;
		do 
		{
			instart = memstr(instart, end - instart, keystart, nowkeylen);
			if (instart) instart += nowkeylen;
		} while (instart && *instart!=' ' && *instart!='=' && *instart!=':' && *instart!='"');
		//	Add this above lines Jan. 02 '15
		//	instart = memstr(start, end - start, keystart, nowkeylen);

		if (instart)
		{
// 			instart += nowkeylen;
			do 
			{
				if (*instart == ' ') instart++;
				else if (*instart == '=') instart++;
				else if (*instart == ':') instart++;
				else if (*instart == '"')
				{
					inend = ++instart;
					while (inend < end && *inend != '"') inend++;
					if (inend >= end) fail = 1;
					break;
				}
				else
				{
					fail = 1;
					break;
				}
			} 
			while (instart < end);
		}
		else fail = 1;
		if (fail)
		{
			resultPad->resultKey[nowkey] = 0;
			resultPad->getLength[nowkey] = 0;
		}
		else
		{
			resultPad->resultKey[nowkey] = instart;
			resultPad->getLength[nowkey] = inend - instart;
		}
		nowkey++;
		if (nowkey>=MAX_CONTENT_KEY) break;
		keystart = keynext + 1;
	}
	return 0;
}

long Wafa_Count(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	char* nowstart = start;
	long count = 0;
	ContentPad* resultPad = referContent->HeadInfo;

	while (nowstart < end)
	{
		nowstart = memstr(nowstart, end-nowstart, key, keyLen);
		if (!nowstart) break;
		nowstart += keyLen;
		count ++;
	}
	resultPad->resultKey[keyFrom] = (char*)count;			// force translate
	resultPad->getLength[keyFrom] = 0;

	return 0;
}

long Wafa_Copy(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	CListItem* fileContent;
	char* coverstart;
	ContentPad* resultPad = referContent->HeadInfo;
	long copysize;

	fileContent = GetContentByName(mContext, CONTENT_NAME_FILE);
	if (!fileContent) return 1;

	coverstart = memstr_no(REAL_BUFFER(fileContent), fileContent->NProcessSize, key, keyLen, '"');
	if (!coverstart) return 1;
	coverstart += keyLen;
	copysize = resultPad->getLength[keyFrom];

	memmove(coverstart+copysize, coverstart, REAL_BUFFER(fileContent)+fileContent->NProcessSize-coverstart);
	memmove(coverstart, resultPad->resultKey[keyFrom], copysize);

	fileContent->NProcessSize += copysize;
	return 0;
}

long Wafa_LoopPrint(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	char* oldplace;//, *oneplace;
	int i, len;
	ContentPad* resultPad = referContent->HeadInfo;

	CListItem* freeContent = GetReferStruct(mContext);
	if (!freeContent) return 0;

	NTLMStruct* ntlm = (NTLMStruct*)REAL_BUFFER(freeContent);

	while ( InterCmpExg(&(ntlm->inProcess), MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	
	/*oneplace = */oldplace = ntlm->freePlace;
//	here start and end is the value for loop control
	int svalue = start - REAL_BUFFER(referContent);
	int evalue = end - REAL_BUFFER(referContent);
	int* pvalue = (int*)&(resultPad->resultKey[keyFrom]);		// the result value;

	for (i=svalue; i<=evalue; i++)
	{
		*pvalue = i;
		ProcessOneVar(mContext, resultPad, ntlm->freePlace, key, key+keyLen );
	}

	len = ntlm->freePlace - oldplace;
	ntlm->freePlace += 0x10;			// should control size for freePlace
	ntlm->inProcess = MARK_NOT_IN_PROCESS;

	resultPad->resultKey[keyFrom] = oldplace;
	resultPad->getLength[keyFrom] = len;

	return 0;
}


// char CITIC_USER_Scanf[] = "\"ID\"%*\"%s\"%*\"Name\"%*\"%s\"::<User guid=\"%d89\" name=\"%U90\"/>::\"ID\"";
// key format scanfkey::assignkey::loopkey  or
// key format scanfkey::assignkey::loopkey::loopadd
long Wafa_LoopAssign (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
#define LOOPAHEAD		3
	long ret_err = 0x01;
	long result = 0;
	long len = 0;
	char *nowstart, *nextstart;
	char *ifkey, *scanfkey, *assignkey, *loopkey, *loopadd;
	long iflen, scanflen, assignlen, looplen, loopaddlen;
	char *keyend = key + keyLen;
	long isequ;

	ContentPad* resultPad = referContent->HeadInfo;
	CListItem* freeContent = GetReferStruct(mContext);
	if (!freeContent) return ret_err;
	NTLMStruct* ntlm = (NTLMStruct*)REAL_BUFFER(freeContent);

	ifkey = key + LOOPAHEAD;
	scanfkey = memstr(ifkey, keyLen - LOOPAHEAD, NASZ("??"));
	if (scanfkey)
	{
		iflen = scanfkey - ifkey;
		isequ = Wafa_Compare(mContext, referContent, start, end, ifkey, iflen, keyFrom, infoPad);
		if (isequ) return 0;		// not true
		scanfkey += 2;
	}
	else scanfkey = key + LOOPAHEAD;
	assignkey = memstr(scanfkey, keyend - scanfkey, NASZ("::"));
	if (!assignkey) return VALUE_NOT_FOUND;
	scanflen = assignkey - scanfkey;

	assignkey += 2;
	loopkey = memstr(assignkey, keyend - assignkey, NASZ("::"));
	if (!loopkey) return VALUE_NOT_FOUND;
	assignlen = loopkey - assignkey;

	loopkey += 2;
	loopadd = memstr(loopkey, keyend - loopkey, NASZ("::"));
	if (!loopadd)
	{
		looplen = keyend - loopkey;
		loopaddlen = 0;
	}
	else
	{
		looplen = loopadd - loopkey;
		loopadd += 2;
		loopaddlen = keyend - loopadd;
	}

	while ( InterCmpExg(&(ntlm->inProcess), MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	char* oldplace = ntlm->freePlace;

	nowstart = memstr(start, end-start, loopkey, looplen);	
	while ((nowstart != end) && nowstart)
	{
		nextstart = memstr(nowstart+looplen, end-nowstart-looplen, loopkey, looplen);
		if (!nextstart) nextstart = end;
		if (*key =='V') result = Wafa_Var(mContext, referContent, nowstart, nextstart, scanfkey, scanflen, keyFrom, infoPad);
		else if (*key == 'S') 
		{
			result = Wafa_Scanf(mContext, referContent, nowstart, nextstart, scanfkey, scanflen, keyFrom, infoPad);
			if (result) break;			// scanf break it 
		}
		else if (*key == 'M') result = Wafa_MultiScanf(mContext, referContent, nowstart, nextstart, scanfkey, scanflen, keyFrom, infoPad);
		else break;
// 		if (result) break;			// may not break		// Add Dec. 10 '14
		ProcessOneVar(mContext, resultPad, ntlm->freePlace, assignkey, assignkey + assignlen );

		nowstart = nextstart;
		if ((nowstart != end) && loopaddlen)
		{
			ProcessOneVar(mContext, resultPad, ntlm->freePlace, loopadd, loopadd + loopaddlen );
		}
	} 

	resultPad->resultKey[keyFrom] = oldplace;
	resultPad->getLength[keyFrom] = ntlm->freePlace - oldplace;

	ntlm->freePlace += 0x10;			// should control size for freePlace
	ntlm->inProcess = MARK_NOT_IN_PROCESS;
	return 0;
}


long Wafa_Replace (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad)
{
	long ret_err = 0x01;
	long result = 0;
	long len = 0;
	char *ifkey, *scanfkey, *assignkey;
	long iflen, scanflen, assignlen;
	char *keyend = key + keyLen;
	long isequ, scaned;

	ContentPad* resultPad = referContent->HeadInfo;
	CListItem* freeContent = GetReferStruct(mContext);
	if (!freeContent) return ret_err;
	NTLMStruct* ntlm = (NTLMStruct*)REAL_BUFFER(freeContent);

	ifkey = key;
	scanfkey = memstr(ifkey, keyLen - LOOPAHEAD, NASZ("??"));
	if (scanfkey)
	{
		iflen = scanfkey - ifkey;
		isequ = Wafa_Compare(mContext, referContent, start, end, ifkey, iflen, keyFrom, infoPad);
		if (isequ) return 0;		// not true
		scanfkey += 2;
	}
	else scanfkey = key;
	assignkey = memstr(scanfkey, keyend - scanfkey, NASZ("::"));
	if (!assignkey) return VALUE_NOT_FOUND;
	scanflen = assignkey - scanfkey;

	assignkey += 2;
	assignlen = keyend - assignkey;

	while ( InterCmpExg(&(ntlm->inProcess), MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	char* oldplace = ntlm->freePlace;


	char *scanfstart = start;
	while (scanfstart)
	{
		result = Wafa_Scanf(mContext, referContent, scanfstart, end, scanfkey, scanflen, keyFrom, infoPad);
		if (infoPad->serverNowStart)
		{
			scaned = infoPad->serverNowStart - scanfstart;
			memcpy(ntlm->freePlace, scanfstart, scaned);
			ntlm->freePlace += scaned;
			ProcessOneVar(mContext, resultPad, ntlm->freePlace, assignkey, assignkey + assignlen );
		}
		else
		{
			scaned = end - scanfstart;
			memcpy(ntlm->freePlace, scanfstart, scaned);
			ntlm->freePlace += scaned;
		}
		scanfstart = infoPad->serverNowEnd;
	}
	resultPad->resultKey[keyFrom] = oldplace;
	resultPad->getLength[keyFrom] = ntlm->freePlace - oldplace;

	ntlm->freePlace += 0x10;			// should control size for freePlace
	ntlm->inProcess = MARK_NOT_IN_PROCESS;
	return 0;

}

long ParserCITIC(unsigned char* condition, int conditionLen, char* valStart, char* valEnd)
{
	unsigned char *nowplace;
	unsigned char *endplace = condition + conditionLen;
	long nowlevel = 0, maxlevel = 0, totallevel = 0;
	long inname = 0, inval = 0;
	unsigned char *nextlevelstart = 0, *nextlevelend = 0;
	long result, otherresult;

	if (conditionLen == 0) return 1;

	for (nowplace = condition; nowplace <= endplace; nowplace++)
	{
		if ( ((*((WORD*)nowplace) == 0xd2c7 /*且*/) || (*((WORD*)nowplace) == 0xf2bb /*或*/) || nowplace == endplace) && (nowlevel == 0))
		{
			if (maxlevel == 0) result = ParserCITICNoBracket(condition, nowplace - condition, valStart, valEnd);
			else if (maxlevel == 1) result = ParserCITICNoBracket(nextlevelstart, nextlevelend - nextlevelstart, valStart, valEnd);
			else result = ParserCITIC(nextlevelstart, nextlevelend - nextlevelstart, valStart, valEnd);
			if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;

			if (nowplace == endplace) return result;

			otherresult = ParserCITIC(nowplace + 2, endplace - nowplace - 2, valStart, valEnd);
			if (otherresult == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;

			if (*((WORD*)nowplace) == 0xd2c7 /*且*/) return result && otherresult;
			if (*((WORD*)nowplace) == 0xf2bb /*或*/) return result || otherresult;
		}
		if (*nowplace == '\\')
		{
			nowplace ++;
			continue;
		}
		if (*nowplace == ' ') continue;
		if (*nowplace == '[')
		{
			inname++;
			continue;
		}
		if (*nowplace == ']')
		{
			inname--;
			continue;
		}
		if (inname) continue;
		if (*nowplace == '\'')
		{
			inval = 1 - inval;
			continue;
		}
		if (inval) continue;

		if (*nowplace == '(' && !inname && !inval)
		{
			if (nowlevel == 0)	nextlevelstart = nowplace + 1;
			nowlevel ++;
			maxlevel ++;
			totallevel ++;
			continue;
		}
		if (*nowplace == ')' && !inname && !inval)
		{
			nowlevel --;
			if (nowlevel == 0) nextlevelend = nowplace;
			continue;
		}
	}
	if (totallevel == 0) return ParserCITICNoBracket(condition, conditionLen, valStart, valEnd);
	else return VALUE_NOT_FOUND;
}
#define OP_MASK				0xff
#define ISINCLUDE			10
#define ISNOTINCLUDE		11
#define ISEQUAL				12
#define ISNOTEQUAL			13
#define ISBIGEQUAL			14
#define ISSMALLEQUAL		15
#define ISBIG				16
#define ISSMALL				17

#define RE_MASK				0xff00
#define ISAND				(30 << 8)
#define ISOR				(31 << 8)

#define GETOP(val)	\
	result = memcmp(nowplace, NASZ(val)); if (!result) { nowplace += (sizeof(val) - 1); nowop |= stepop; break; } stepop++;

long ParserCITICNoBracket(unsigned char* condition, int conditionLen, char* valStart, char* valEnd)
{
	unsigned char *nowplace;
	unsigned char *conditionEnd = condition + conditionLen;
	long nowop = 0, stepop = 0;
	long result = VALUE_NOT_FOUND, resultother = VALUE_NOT_FOUND;
	long inname = 0, inval = 0;
	long isnumber = 0;

	if (conditionLen == 0) return 1;

	unsigned char *onevalstart = 0, *onevalend = 0, *twovalstart = 0, *twovalend = 0;
	char *resultstart = 0;
	long resultlen = 0;

	for (nowplace = condition; nowplace <= conditionEnd; nowplace++)
	{
		if ((nowop & RE_MASK) || (nowplace == conditionEnd))
		{
			long doop = nowop & OP_MASK;
			long reop = nowop & RE_MASK;

			result = 1;
			if (nowplace == conditionEnd && !twovalend) twovalend = nowplace;

			if (doop && resultstart && twovalend)
			{
				if (isnumber)
					result = StringIntCmp((unsigned char*)resultstart, (unsigned char*)resultstart + resultlen, twovalstart, twovalend, doop);
				else
				{
					if (doop == ISEQUAL) result = (0 == memcmp(resultstart, twovalstart, max(resultlen, twovalend-twovalstart)));
					if (doop == ISNOTEQUAL) result = (0 != memcmp(resultstart, twovalstart, max(resultlen, twovalend-twovalstart)));

					if (doop == ISINCLUDE) result = (0 != memstr(resultstart, resultlen, (char*)twovalstart, twovalend-twovalstart));
					if (doop == ISNOTINCLUDE) result = (0 == memstr(resultstart, resultlen, (char*)twovalstart, twovalend-twovalstart));
				}
			}
			if (reop == 0) return result;
			resultother = ParserCITICNoBracket(nowplace, conditionEnd-nowplace, valStart, valEnd);
			if (resultother == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
			if (reop == ISAND) return (result && resultother);
			if (reop == ISOR) return (result || resultother);
			return VALUE_NOT_FOUND;
		}

		if (*nowplace == '\\')
		{
			nowplace ++;
			continue;
		}
		if (*nowplace == '[')
		{
			onevalstart = nowplace + 1;
			inname++;
			continue;
		}
		if (*nowplace == ']' && inname == 1)
		{
			onevalend = nowplace;
			inname--;
			result = GetCITICValue(onevalstart, onevalend, valStart, valEnd, resultstart, resultlen);
			if (result == VALUE_NOT_FOUND) return VALUE_NOT_FOUND;
			continue;
		}
		if (inname) continue;

		if (ASCII_DEC[*nowplace] != 0xff && (!inval || isnumber))
		{
			if (!twovalstart)
			{
				twovalstart = nowplace;
				inval = 1;
				isnumber = 1;
			}
			continue;
		}
		if ((*nowplace == ',' || *nowplace == '.') && isnumber) continue;
		if (isnumber && inval) 
		{
			twovalend = nowplace;
			inval = 0;
		}
		if (*nowplace == ' ') continue;
		if (*nowplace == '\'')
		{
			if (!inval) twovalstart = nowplace + 1;
			else twovalend = nowplace;
			inval = 1 - inval;
			continue;
		}
		if (inval) continue;
		while (TRUE)
		{
			stepop = ISINCLUDE;
			GETOP("包含");
			GETOP("不包含");
			GETOP("等于");
			GETOP("不等于");
			GETOP("大于等于");
			GETOP("小于等于");
			GETOP("大于");
			GETOP("小于");

			stepop = ISAND;
			GETOP("且");
			stepop = ISOR;
			GETOP("或");
			break;
		}
		if (!nowop) return VALUE_NOT_FOUND;
		else continue;
	}
	return VALUE_NOT_FOUND;
}

#define CITIC_KEY_START		"Domain name=&quot;"
#define CITIC_KEY_END		"&quot;"
#define CITIC_VALUE_START	"&quot;&gt;"			//	these two value following 20002 Items get
#define CITIC_VALUE_END		"&lt;/Domain"

#define CDATA_HEAD			0x5b41544144435b21	//	('!'+('['<<8)+('C'<<16)+('D'<<24)+('A'<<32)+('T'<<40)+('A'<<48)+('['<<56))
#define CDATA_TAIL			(']'+(']'<<8))
long GetCITICValue(unsigned char *keystart, unsigned char* keyend, char *valStart, char *valEnd, char *&resultstart, long &resultlen)
{
	char keybuffer[NORMAL_CHAR];
	long keylen = keyend - keystart;
	char *resultend = 0;
	__int64 *cdatahead;
	PWORD cdatatail;

	memcpy(keybuffer, NASZ(CITIC_KEY_START));
	memcpy(keybuffer+sizeof(CITIC_KEY_START)-1, keystart, keylen);
	memcpy(keybuffer+sizeof(CITIC_KEY_START)+keylen-1, NASZ(CITIC_KEY_END));

	resultstart = memstr(valStart, valEnd-valStart, keybuffer, keylen+sizeof(CITIC_KEY_START)+sizeof(CITIC_KEY_END)-2);
	if (!resultstart) return VALUE_NOT_FOUND;
	resultstart = memstr(resultstart, valEnd-resultstart, NASZ(CITIC_VALUE_START));
	if (!resultstart) return VALUE_NOT_FOUND;
	resultstart += (sizeof(CITIC_VALUE_START) - 1);
	resultend = memstr(resultstart, valEnd-resultstart, NASZ(CITIC_VALUE_END));
	if (!resultend) return VALUE_NOT_FOUND;
	resultlen = resultend - resultstart;

	cdatahead = (__int64 *)(resultstart+4);			//	lazy way format like "&lt;![CDATA[103797.28]]&gt;"
	cdatatail = (PWORD)(resultend-6);
	if (*cdatahead == CDATA_HEAD && *cdatatail == CDATA_TAIL)
	{
		resultstart += 12;
		resultlen -= 18;
	}
	return 0;
}


#define CITIC_GROUP_START	"Group name=&quot;"
#define CITIC_GROUP_END		"&quot;"
#define CITIC_GROUP_FINISH	"&lt;/Group&gt;"
#define CITIC_ITEM_START	"&lt;Item "			//	these two value following 20002 Items get
#define CITIC_ITEM_END		"&lt;/Item&gt;"
//	if nowstart is 0, find the first item, or find the next item
long GetCITICGroup(unsigned char *keystart, unsigned char* keyend, char *valStart, char *valEnd, char *&nowstart, char *&nowend, char* &groupend)
{
	char keybuffer[NORMAL_CHAR];
	long keylen = keyend - keystart;
	char *resultend = 0;

	if (!nowstart)
	{
		memcpy(keybuffer, NASZ(CITIC_GROUP_START));
		memcpy(keybuffer+sizeof(CITIC_GROUP_START)-1, keystart, keylen);
		memcpy(keybuffer+sizeof(CITIC_GROUP_START)+keylen-1, NASZ(CITIC_GROUP_END));
		nowstart = memstr(valStart, valEnd-valStart, keybuffer, keylen+sizeof(CITIC_GROUP_START)+sizeof(CITIC_GROUP_END)-2);
		if (!nowstart) return VALUE_NOT_FOUND;
		groupend = memstr(nowstart, valEnd-nowstart, NASZ(CITIC_GROUP_FINISH));
		if (!groupend) return VALUE_NOT_FOUND;

		nowend = nowstart;
	}
	nowstart = memstr(nowend, groupend - nowend, NASZ(CITIC_ITEM_START));
	if (nowstart)
	{
		nowend = memstr(nowstart, groupend - nowstart, NASZ(CITIC_ITEM_END));
		if (!nowend) return VALUE_NOT_FOUND;
		nowend += (sizeof(CITIC_ITEM_END) - 1);
		return 0;
	}
	else return VALUE_NOT_FOUND;
}

long StringIntCmp(unsigned char *onestart, unsigned char *oneend, unsigned char *twostart, unsigned char *twoend, long op)
{
	unsigned char oneresult[NORMAL_CHAR];
	unsigned char tworesult[NORMAL_CHAR];
	long result;

	if ((oneend - onestart) > (NORMAL_CHAR >> 1)) return VALUE_NOT_FOUND;
	if ((twoend - twostart) > (NORMAL_CHAR >> 1)) return VALUE_NOT_FOUND;

	StringFormatInt(oneresult, onestart, oneend);
	StringFormatInt(tworesult, twostart, twoend);
	result = memcmp(oneresult, tworesult, NORMAL_CHAR);

	if (op == ISEQUAL) return (result == 0);
	if (op == ISNOTEQUAL) return (result != 0);
	if (op == ISBIGEQUAL) return (result >= 0);
	if (op == ISSMALLEQUAL) return (result <= 0);
	if (op == ISBIG) return (result > 0);
	if (op == ISSMALL) return (result < 0);
	return VALUE_NOT_FOUND;
}

// tar must with size NORMAL_CHAR
long StringFormatInt(unsigned char *tar, unsigned char *sourstart, unsigned char *sourend)
{
	unsigned char buffer[NORMAL_CHAR];
	unsigned char *pointplace = buffer, *nowplace, *nowcopy;
	unsigned long *full = (unsigned long *)tar;
	long beforepoint = 1, copysize;
	int i;

	for (i=0; i<(NORMAL_CHAR >> 2); i++) (*full++) = '0' * 0x1010101;

	for (nowplace = sourstart; nowplace <= sourend; nowplace++)
	{
		if (((*nowplace == '.') && beforepoint) || (nowplace == sourend))
		{
			copysize = pointplace - buffer;
			pointplace = buffer;
			nowcopy = tar + ((NORMAL_CHAR >> 1) - copysize);
			for (i=0; i<copysize; i++) *nowcopy++ = *pointplace++;
			beforepoint = 0;
			continue;
		}
		if ((ASCII_DEC[*nowplace] != 0xff) && beforepoint)
		{
			*pointplace++ = *nowplace;
			continue;
		}
		if ((ASCII_DEC[*nowplace] != 0xff) && !beforepoint)
		{
			*nowcopy++ = *nowplace;
			continue;
		}
		if (*nowplace == ',') continue;

		return VALUE_NOT_FOUND;
	}
	return 0;
}

// long Tunnel_Register(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom)
// {
// 
// 	return 0;
// }
// 
// long Tunnel_Add(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom)
// {
// 	return 0;
// }
// 
// long Tunnel_Remove(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom)
// {
// 	return 0;
// }
// 
// long Tunnel_Close(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom)
// {
// 	return 0;
// }


// #endif ( defined(CONTENTPAD_APPLICATION) )

long AddtoContentList(CContextItem* mContext, CListItem* &mBuffer, char* name)
{
//	printf("add content list: %s, %p\r\n", name, mBuffer);
//	It should be single thread for same CLIENT context, for ServerRead may return at the same time	
	static volatile MYINT	InContentProcess = MARK_NOT_IN_PROCESS;		//	Add concurrency control May 01 '14						//

// 	int i;
// 	long *pLength = &(mBuffer->HeadInfo->getLength[0]);
	// printf("AddtoContentList %x:%s\r\n", mBuffer, name);
	if (name) strcpy_s(mBuffer->HeadInfo->nameFor, SMALL_CHAR, name);
// 	for (i=0; i<MAX_CONTENT_KEY+SYSTEM_CONTENT_KEY; i++) *(pLength++) = VALUE_NOT_FOUND;		// surely? ad Sept. 17 '13

	while ( InterCmpExg(&InContentProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	mBuffer->HeadInfo->nextContent = mContext->FirstContent;
	mContext->FirstContent = mBuffer;
	InContentProcess = MARK_NOT_IN_PROCESS;

	return 0;
}

// #if ( defined(CONTENTPAD_APPLICATION) )

CListItem* GetContentByName(CContextItem* mContext, char* name)
{
	if (!mContext)
		return NULL;			// add in Apr. 28 '14
	CListItem* nowBuffer = mContext->FirstContent;

	while (nowBuffer)
	{
		if ( !strcmp(nowBuffer->HeadInfo->nameFor, name) )
			return nowBuffer;
		nowBuffer = nowBuffer->HeadInfo->nextContent;
	}
	return nowBuffer;
}

ContentPad* GetPadByName(CContextItem* mContext, char* name)
{
	if (!strcmp("VAR", name)) 
		return &(configInfo->ContentVar.AdditionPad);

	CListItem* mBuffer = GetContentByName(mContext, name);
	if (mBuffer) return mBuffer->HeadInfo;
	else return NULL;
}

CListItem* GetContentByName(CContextItem* mContext, char* name, long len)
{
	CListItem* nowBuffer = mContext->FirstContent;

	while (nowBuffer)
	{
		if ( !memcmp(nowBuffer->HeadInfo->nameFor, name, len) && nowBuffer->HeadInfo->nameFor[len] == 0 )
			return nowBuffer;
		nowBuffer = nowBuffer->HeadInfo->nextContent;
	}
	return nowBuffer;
}

ContentPad* GetPadByName(CContextItem* mContext, char* name, long len)
{
	if (!memcmp("VAR", name, len)) 
		return &(configInfo->ContentVar.AdditionPad);

	CListItem* mBuffer = GetContentByName(mContext, name, len);
	if (mBuffer) return mBuffer->HeadInfo;
	else return NULL;
}

// #endif ( defined(CONTENTPAD_APPLICATION) )


void DisplayContentByName(CContextItem* mContext)
{
	CListItem* nowBuffer = mContext->FirstContent;

	printf("Context:%x ", mContext);
	while (nowBuffer)
	{
		printf("%s(%x)-->", nowBuffer->HeadInfo->nameFor, nowBuffer);
		nowBuffer = nowBuffer->HeadInfo->nextContent;
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        