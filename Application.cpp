
#include "stdafx.h"

extern CListItem*				isNULL;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Of course, My IOCP begin from SOCKET for TCP SERVER, but now it can do any PROTOCOL for CLIENT and SERVER. It is abstract.		//
//	The main struct divide to two part, PROTOCOL and APPLICATION. PROTOCOL for translate DATA, while APPLICATION process it.		//
//	But before process, it should alloc memory, and determine which HANDLE should do. It is the NoneApplication class do.			//
//	In another words, it create or find its CONTEXT and PEER CONTEXT.																//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	NONE																											//
//	ret_err 0x28 :	NONE																											//
//	ret_err 0x30 :	break :	Jan 23 '13																								//
//	ret_err 0x40 :	break :	Jan 23 '13																								//
//	ret_err 0x50 :	break :	Jan 23 '13																								//
//	ret_err 0x58 :	NONE																											//
//	ret_err 0x60 :	break :	Jan 23 '13																								//
//	ret_err 0x70 :	MUST DO AGAIN
//	ret_err 0x80 :	break :	Jan 27 '13																								//
//	ret_err 0x90 :	break :	Jan 30 '13 SURE																							//
//	0x10 - 0x8f should do again, not sure. and I add CNullProtocol
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	Apr. 01 '13 redo
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	NONE																											//
//	ret_err 0x28 :	NONE																											//
//	ret_err 0x30 :	
//	ret_err 0x40 :	
//	ret_err 0x50 :	
//	ret_err 0x58 :	
//	ret_err 0x60 :	
//	ret_err 0x70 :	
//	ret_err 0x80 :	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneApplication::OnAccept(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)								//
{																																	//
	CContextItem *newContext = 0, *serContext = 0;																					//
	CListItem* newBuffer;																											//
	CProtocol* pProtocol;																											//
	long ret_err = 0x01;																											//
	long ret = 0;																													//
	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		pProtocol = mContext->PProtocol;																							//

		if (pProtocol->ProtocolFlag & FLAG_MULTI_INPUT)																				//
		{																															//
			if (pProtocol->CompareParaLength) newContext = (CContextItem*)pProtocol->ContextInfo->Comparekeyword					//
				(pProtocol->CompareParaStart, pProtocol->CompareParaLength, ((char*)&(mBuffer->AdditionInfo)));						//
			if (newContext)																											//
			{																														//
printf("OnAccept Same context\r\n");
				ret_err = 0;									//	Complete ACCPET, begin READ										//
				mBuffer->NOperation = OP_CLIENT_READ;			//	Change the control, by Jan 30 '13								//
				ret = OnClientRead(newContext, mBuffer, size, opSide);	//////////////////////////////////////////////////////////////////////
				break;																												//
			}																														//
		}																															//
		else																														//
		{																															//
			ret_err = 0x20;																											//
			newBuffer = mContext->PApplication->GetApplicationBuffer();																//
			if (!newBuffer)										//	NOT enough BUFFER, use the old buffer to ACCEPT again.			//
			{													//////////////////////////////////////////////////////////////////////
				if (NoneProFunc(mContext->PProtocol, fPostAccept)(mContext, mBuffer, mContext->PApplication->BufferInfo->BufferSize, OP_ACCEPT))						//
				{																													//
					DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L4, "ReAccept Error in OnAccept 0x%x\r\n", ret_err);			//
					ret_err = 0x28;								// 	This error could NOT test, and I think it will few happened		//
				}												//////////////////////////////////////////////////////////////////////
				break;																												//
			}																														//
			//////////////////////////////////////////////////////////////////////
			ret_err = 0x30;										//	After ACCEPT, must PostAccept again, or no accept again.		//
			if (NoneProFunc(mContext->PProtocol, fPostAccept)(mContext, newBuffer, mBuffer->BufferType->BufferSize, OP_ACCEPT))
			{
				break;																												//
			}
		}														//	If PostAccept FAIL, no LISTEN again. no good solution			//
		//////////////////////////////////////////////////////////////////////
		//	ATTENTION for ReturnApplication
		if ( !(mContext->PApplication->ApplicationFlag & FLAG_PROCESS_ACCEPT) )		// only accept in Feb 18 '13
			mBuffer->NOperation = OP_CLIENT_READ;				//	Change the control, in Jan 30 '13								//
		else mBuffer->NOperation = OP_PROCESS_ACCEPT;			// add in Feb 21 '13 for OnClose
		// 		mBuffer->NOperation = OP_CLIENT_READ;					// for use CWebLogin protocol, in Feb 21 '13

		ret_err = 0x40;																												//
		newContext = mContext->PProtocol->GetProtocolContext();																		//
		if (!newContext) break;																										//
		mContext->PProtocol->DuplicateContextAddition(newContext, mContext);

//	Add the following line for get IP address of client, used by ConfirmDNS, I read the address from Accepted memory,
//	It is the same way as GetAcceptExSockaddrs.			//	Oct. 22 '13		// change to GetAcceptExSockaddrs	//	Oct. 24. 13
		if (pProtocol->ProtocolNumber == PROTOCOL_TCP || pProtocol->ProtocolNumber == PROTOCOL_TCP_POOL)
		{
			LPSOCKADDR loaddr, readdr;
			INT lolen, relen;
			CIOCP::m_lpfnGetAcceptExSockaddrs(REAL_BUFFER(mBuffer), mBuffer->BufferType->BufferSize-((sizeof(sockaddr_in)+16)*2), 
				sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16, &loaddr, &lolen, &readdr, &relen);
			((CTCPContext*)newContext)->clientAddr = *((sockaddr_in*)(readdr));

// 			((CTCPContext*)newContext)->clientAddr = *((sockaddr_in*)(REAL_BUFFER(mBuffer)+mBuffer->BufferType->BufferSize-26));
		}

//	Same reason for record UDP client address			//	Oct. 23 '13
		if (pProtocol->ProtocolNumber == PROTOCOL_UDP || pProtocol->ProtocolNumber == PROTOCOL_SINGLEUDP)
			((CTCPContext*)newContext)->clientAddr = (mBuffer->FromAddr),
						
																											//
		ret_err = 0x50;																												//
// 		newContext->PApplication = mContext->PApplication;																			//
		newContext->PApplication = (mContext->PApplication->ReturnApplication)(mContext, mBuffer, size);

		if (mContext->DecapContext && mContext->DecapContext->EncapContext)//&& (mContext->DecapContext->EncapContext!=mContext))
		{
			mContext->DecapContext->EncapContext = newContext;
			newContext->DecapContext = mContext->DecapContext;
		}
//		else newContext->EncapContext = mContext->EncapContext;	//	Add this line, or use FirstClientContext ? Jun 18 '13			//

		if (mContext->CliBuffer) mBuffer->CliWaitData = 0;		//	MoreBuffer should be 0, but CliBuffer point to BUFFER			//

		if ( newContext->PApplication->CliCommandEndSize )																			//
		{																															//
			ret_err = 0x58;																											//
			newContext->MoreBuffer = newContext->PApplication->GetApplicationBuffer();												//
			if (!newContext->MoreBuffer) break;																						//
			newContext->MoreBuffer->NProcessSize = 0;
		}																															//
																																	//
		ret_err = 0x60;																												//
		if (mContext->PPeer)
		{
			if (!(mContext->PPeer->PPeer))						//	LISTENING also has its peer										//
			{													//////////////////////////////////////////////////////////////////////
				serContext = mContext->PPeer->PProtocol->GetDuplicateContext(mContext->PPeer);										//
				if (!serContext) break;																								//
				serContext->EncapContext = 0;					//	Encap link is depend on FirstContext/FirstServerContext
				newContext->PPeer = serContext;																						//
// where is the best place for serContext->PPeer = newContext;	//	Maybe just before PostConnect is better Jun 23 '13
			}																														//
			else																													//
				newContext->PPeer = newContext;					//	self PROCESS SERVER, such as ECHO, PEER is self					//
		}
		else
		{
//	five hours for add this line to complete InterProtocol SERVER side nest, Jun 21 '13
//	and it is a better way than add lines in CInterProtocol::PostConnect();
// 			if (mContext->EncapContext)
// 				newContext->EncapContext = mContext->EncapContext;	//	which is better, it is a question! Jun 23 '13
//	without peer, for inter protocol server side
		}
		//////////////////////////////////////////////////////////////////////
		ret_err = 0x70;											//	For UDP, record the SOCKET for next use							//
		if ( (pProtocol->ProtocolFlag & FLAG_MULTI_INPUT) && (pProtocol->CompareParaLength) )										//
		{																															//
			newContext->BHandle = mContext->BHandle;																				//
			printf("OnAccept Different context,start:%d\r\n", +pProtocol->CompareParaStart);
			memcpy( ((char*)(newContext))+pProtocol->CompareParaStart, ((char*)&(mBuffer->AdditionInfo)),							//
				pProtocol->CompareParaLength );																						//
			newContext->ListFlag |= FLAG_LATER_CLOSE;																				//
		}																															//
		else if (pProtocol->ProtocolFlag & FLAG_SINGLE)
		{
			CUDPContext* udpContext = (CUDPContext*)newContext;

			newContext->BHandle = mContext->BHandle;
			udpContext->addrServer = mBuffer->FromAddr;
			newContext->listFlag |= FLAG_LATER_CLOSE;
		}
		else																														//
		{																															//
			if (mContext->BHandle)								//	Add in Jun 18 '13 for InterProtocol Server side
			{
				ret_err = 0x80;									//////////////////////////////////////////////////////////////////////
				newContext->BHandle = mBuffer->CliHandle;		//	Normal ACCEPT to join IOCP										//
				if (mContext->PProtocol->ProtocolIOCP)
					if (!::CreateIoCompletionPort((HANDLE)newContext->BHandle, mContext->PProtocol->ProtocolIOCP->mIOCP,			//
						(ULONG_PTR)newContext, 0)) DEBUG_MESSAGE_WSA_ERROR_BREAK(MODULE_IOCP, MESSAGE_ERROR_L3);					//
			}
		}																															//
		//
		ret_err = 0;											//	Complete ACCPET, begin READ										//
		ret = OnClientRead(newContext, mBuffer, size, opSide);	//////////////////////////////////////////////////////////////////////
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in NoneApplicaton - OnAccept, 0x%x\r\n  ", ret_err);				//
		RET_BETWEEN(0x10, 0x8f) if (mBuffer->CliHandle) mContext->PProtocol->MyCloseHandle(mBuffer->CliHandle);						//
		RET_BETWEEN(0x30, 0x30) mContext->PApplication->FreeApplicationBuffer(newBuffer);											//
		RET_BETWEEN(0x50, 0x8f) mContext->PProtocol->FreeProtocolContext(newContext);												//
		RET_BETWEEN(0x50, 0x8f) mContext->PApplication->FreeApplicationBuffer(mBuffer);												//
		RET_BETWEEN(0x70, 0x8f) if (serContext) serContext->PProtocol->FreeProtocolContext(serContext);			// NOT SURE	
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	After CONNECT, post RECEIVE for both CLIENT and SERVER, begin data translate													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE				Jan 30 '13																					//
//	ret_err 0x20 :	break :	Jan 23 '13	Jan 30 '13																					//
//	ret_err 0x30 :	break :	Jan 23 '13	Jan 30 '13																					//
//	ret_err 0x40 :	break :	Jan 24 '13	Jan 30 '13																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//

long CNoneApplication::OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)								//
{																																	//
	CListItem* newBuffer;																											//
	long ret_err = 0x01;																											//
	CProtocol* pProtocol;																											//

																																	//
	while (TRUE)																													//
	{																																//
		pProtocol = mContext->PProtocol;						//	Here mContext is UDP listen SOCKET, find the LISTENED SOCKET	//
		ret_err = 0x10;																												//

		if ( mContext->PApplication->SerCommandEndSize && !mContext->MoreBuffer)													//
		{														//	SERVER side MoreBuffer											//
			mContext->MoreBuffer = mContext->PApplication->GetApplicationBuffer();													//
			if (!mContext->MoreBuffer) break;																						//
			mContext->MoreBuffer->NProcessSize = 0;
		}																															//
		//
		if (mContext->PApplication->ApplicationFlag & FLAG_DUPLEX)																	//
		{																															//
			ret_err = 0x20;																											//
			newBuffer = mContext->PApplication->GetApplicationBuffer();																//
			if (!newBuffer) break;																									//
			//
			ret_err = 0x30;																											//
			if (NoneProFunc(mContext->PProtocol, fPostReceive)																		//
				(mContext, newBuffer, newBuffer->BufferType->BufferSize, OP_SERVER_READ, 0)) break;									//
		}														//	SERVER side PostReceive											//
																//////////////////////////////////////////////////////////////////////
																//	should post later for LATE_CONNECT flag
		if (pProtocol->ProtocolFlag & FLAG_SINGLE)
		{
			FreeApplicationBuffer(mBuffer);
			ret_err = 0;
			break;
		}
		ret_err = 0x40;																												//
		if (AppFunc(mContext->PApplication, fOnConnect)(mContext, mBuffer, size, opSide)) break;

		ret_err = 0;											//	CLIENT side PostReceive											//
		break;													//////////////////////////////////////////////////////////////////////
	}																																//
	if (ret_err)																													//
	{																																//
		//	UDP have not test
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in NoneApplicaton - OnConnect, 0x%x\r\n", ret_err);				//
		RET_BETWEEN(0x10, 0x40) OnClose(mContext, mBuffer, 0, opSide);																//
		RET_BETWEEN(0x30, 0x30) mContext->PApplication->FreeApplicationBuffer(newBuffer);											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	YES, in this project, I remove all the unnecessary code, all the unused code, but I will remain the following lines				//
//	which is replaced by one line - OnClose(); the following cost me too many time,													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
// 		RET_BETWEEN(0x10, 0x30) if (mContext->PPeer) mContext->PPeer->PProtocol->FreeProtocolContext(mContext->PPeer);				//
// 		RET_BETWEEN(0x10, 0x30) mContext->PProtocol->FreeProtocolContext(mContext);													//
// 		RET_BETWEEN(0x10, 0x40) mContext->PApplication->FreeApplicationBuffer(mBuffer);												//
// 		RET_BETWEEN(0x40, 0x40) mContext->PProtocol->MyCloseHandle(mContext->BHandle);												//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	While reading, complete COMMAND and PROCESS																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	break ;	Jan. 28 '13																								//
//	ret_err 0x20 :	break ;	Jan. 28 '13																								//
//	ret_err 0x30 :	break ;	Jan. 28 '13																								//
//	ret_err 0x40 :	break ;	Jan. 28 '13																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	// 	CListItem *newBuffer;																										//
	CContextItem *peerContext;																										//
	long ret_err = 0x01;																											//
	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		if (mContext->PProtocol->ProtocolFlag & FLAG_ADD_OVERLAP) 
			mContext->OverlapOffset += size;					//	Add here or CFileProtocol::PostReceive							//

		peerContext = mContext->PPeer;																								//
		if ( !peerContext ) 
		{
			if (mContext->DecapContext) peerContext = mContext;	//	For InterProtocol SERVER side, without peer						//
			else break;				// should test again
		}
		// 		if ( !peerContext ) break;								//	The PEER CONTEXT is closed, close the HANDLE			//
//////////////////////////////////////////////////////////////////////
		mBuffer = ProcessClientCommand(mContext, mBuffer, size);																	//

		ret_err = 0x20;
// 		if (mBuffer->NProcessSize == BUFFER_TOO_LARGE) return -1;			//	COMMAND too long									//
		if (mBuffer->NProcessSize > BUFFER_ERROR) return -1;


		//
		if (mBuffer->NProcessSize)																									//
		{																															//
//	Attention, after GRACE_CLOSE, the client side mContext->PPeer == mContext, should close		Jul 02 '13
			if (!(mContext->PApplication->ApplicationFlag & FLAG_LATER_CONNECT) && peerContext && !peerContext->PPeer)				//
				// for http server, later connect			 // now I think, http is protocol and web is application 13.01.25
			{																														//
				ret_err = 0x30;																										//
				peerContext->PPeer = mContext;																						//
				if ( NoneProFunc(peerContext->PProtocol, fPostConnect)(peerContext, mBuffer, mBuffer->NProcessSize, OP_CONNECT) ) break;			//
			}																														//
			else																													//
			{																														//
#ifdef DEBUG_TCP
				*(REAL_BUFFER(mBuffer)+size) = 0;
				printf ("In NoneOnClientRead\r\n%sEndof NoneOnClientRead\r\n", REAL_BUFFER(mBuffer));
#endif DEBUG_TCP
				ret_err = 0x40;																										//
				mBuffer->NOperation = mBuffer->NOperation;	// for single_accept use		// in old time, it is = newBuffer->
				if ( AppFunc(mContext->PApplication, fOnClientRead)(mContext, mBuffer, mBuffer->NProcessSize, opSide) ) break;		//
			}																														//
		}																															//
		else																														//
		{																															//
			ret_err = 0x50;																											//
			if ( NoneProFunc(mContext->PProtocol, fPostReceive)																		//
				(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide) ) break;								//
		}																															//
		//
		ret_err = 0;																												//
		if (mContext->ContentMode == CONTENT_MODE_AFTERHEAD) 
		{
			mContext->ContentMode = CONTENT_MODE_LENGTH;
		}
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		//	UDP have not test
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in NoneApplicaton - OnClientRead, Context:%x, 0x%x\r\n", mContext, ret_err);			//
		RET_BETWEEN(0x10, 0x50) OnClose(mContext, mBuffer, 0, opSide);																//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	While Write, PostReceive for PEER																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	break ;	Jan. 28 '13																								//
//	ret_err 0x20 :	break ;	Jan. 28 '13																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneApplication::OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	CContextItem *newContext = mContext;																							//
	CContextItem* serContext = 0;
	CProtocol* pProtocol;																											//
	long ret_err = 0x01;																											//

	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
// 		if (mContext->PProtocol->ProtocolFlag & FLAG_ADD_OVERLAP)
// 			mContext->OverlapOffset += size;					//	Add here or CFileProtocol::PostSend								//
																//////////////////////////////////////////////////////////////////////
		pProtocol = mContext->PProtocol;						//	Here mContext is UDP listen SOCKET, find the LISTENED SOCKET	//
		if ( (pProtocol->ProtocolFlag & FLAG_MULTI_INPUT) && (pProtocol->CompareParaLength) )										//
			newContext = (CContextItem*)pProtocol->ContextInfo->Comparekeyword														//
			(pProtocol->CompareParaStart, pProtocol->CompareParaLength, ((char*)&(mBuffer->AdditionInfo)));							//
																																	//
		if (pProtocol->ProtocolFlag & FLAG_SINGLE)
		{
			OnClose(mBuffer->UsedContext, mBuffer, 0, 0);
// 			FreeApplicationBuffer(mBuffer);		//	it is free in OnClose
			ret_err = 0;
			break;
		}

		ret_err = 0x20;																												//
		if (newContext && newContext->PPeer)
		{
			ret_err = 0x30;
			if (newContext->EncapContext && (newContext == newContext->PPeer))	// add in Mar. 25 '13			//
				return CNoneApplication::OnClientWrite(newContext->EncapContext, mBuffer, size, opSide);

			ret_err = 0x40;
			if (AppFunc(newContext->PApplication, fOnClientWrite)(newContext, mBuffer, size, opSide)) break;
		}
		else 
			break;												//	The PEER CONTEXT is closed, close the HANDLE					//
																//////////////////////////////////////////////////////////////////////
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		//	UDP have not test
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in NoneApplicaton - OnClientWrite, Context:%x, 0x%x\r\n", mContext, ret_err);			//
		RET_BETWEEN(0x10, 0x30) OnClose(newContext, mBuffer, 0, opSide);		// new change in May 31 '13	

	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	While reading, complete COMMAND and PROCESS																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	break ;	Jan. 28 '13																								//
//	ret_err 0x20 :	break ;	Jan. 28 '13																								//
//	ret_err 0x30 :	break ;	Jan. 28 '13																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	// 	CListItem *newBuffer;																										//
	CContextItem *peerContext;																										//
	long ret_err = 0x01;																											//

	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		if (mContext->PProtocol->ProtocolFlag & FLAG_ADD_OVERLAP)
			mContext->OverlapOffset += size;					//	Add here or CFileProtocol::PostReceive							//

		peerContext = mContext->PPeer;																								//
		if ( !peerContext ) break;								//	The PEER CONTEXT is closed, close the HANDLE					//
		//////////////////////////////////////////////////////////////////////
		mBuffer = ProcessServerCommand(mContext, mBuffer, size);																	//

		ret_err = 0x20;
// 		if (mBuffer->NProcessSize == BUFFER_TOO_LARGE) return -1;			//	COMMAND too long									//
		if (mBuffer->NProcessSize > BUFFER_ERROR) return -1;

		if (mBuffer->NProcessSize)																									//
		{																															//
			ret_err = 0x30;																											//
#ifdef DEBUG_TCP
			if (mContext->PProtocol->ProtocolNumber == PROTOCOL_TCP)
			{
				*(REAL_BUFFER(mBuffer)+size) = 0;
				printf ("In OnServerRead\r\n%sEndof OnServerRead\r\n", REAL_BUFFER(mBuffer));
			}
#endif DEBUG_TCP
			if ( AppFunc(mContext->PApplication, fOnServerRead)(mContext, mBuffer, mBuffer->NProcessSize, opSide) ) break;			//
		}																															//
		else																														//
		{																															//
			ret_err = 0x40;																											//
			if ( NoneProFunc(mContext->PProtocol, fPostReceive)																		//
				(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_SERVER_READ, opSide) ) break;								//
		}																															//
		//
		ret_err = 0;																												//
		if (mContext->ContentMode == CONTENT_MODE_AFTERHEAD)
		{
			mContext->ContentMode = CONTENT_MODE_LENGTH;
		}
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		//	UDP have not test
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in NoneApplicaton - OnServerRead, Context:%x, 0x%x\r\n", mContext, ret_err);			//
		RET_BETWEEN(0x10, 0x40) OnClose(mContext, mBuffer, 0, opSide);																//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	While reading, complete COMMAND and PROCESS																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	break ;	Jan. 28 '13																								//
//	ret_err 0x20 :	break ;	Jan. 28 '13																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneApplication::OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	long ret_err = 0x01;																											//
	CProtocol* pProtocol;																											//

	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
// 		if (mContext->PProtocol->ProtocolFlag & FLAG_ADD_OVERLAP)
// 			mContext->OverlapOffset += size;					//	Add here or CFileProtocol::PostSend								//

		pProtocol = mContext->PProtocol;						//	Here mContext is UDP listen SOCKET, find the LISTENED SOCKET	//
		if (pProtocol->ProtocolFlag & FLAG_SINGLE)
		{
			ret_err = 0;
			break;
		}

		ret_err = 0x20;																												//
		if (mContext && mContext->PPeer)																							//
		{																															//
			if (mContext->DecapContext && (mContext == mContext->PPeer))	// add in Jun. 19 '13 for PPeer control					//
				return CNoneApplication::OnServerWrite(mContext->DecapContext, mBuffer, size, opSide);

			ret_err = 0x30;	// PASSBY will return before entry here
			if (AppFunc(mContext->PApplication, fOnServerWrite)(mContext, mBuffer, size, opSide)) break;
		}																															//
		else break;												//	The PEER CONTEXT is closed, close the HANDLE					//
		//////////////////////////////////////////////////////////////////////
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		//	UDP have not test
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in NoneApplicaton - OnServerWrite, Context:%x, 0x%x\r\n", mContext, ret_err);			//
		RET_BETWEEN(0x10, 0x30) OnClose(mContext, mBuffer, 0, opSide);																//
	}																																//
	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	There are two way into this function: one is close the LISTEN SOCKET, another is close the ACCEPT only SOCKET without DATA		//
//	the different is BHandle, for close LISTEN, it is 0.																			//
//	Windows BUG	: the active close will wait TIME_WAIT, for received but not send.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CNoneApplication::OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)								//
{																																	//
#ifdef DEBUG_CLOSE
	/*if (umContext->PProtocol->ProtocolNumber <10 ) */printf("\r\nIn close:Context:%8x,Buffer:%8x,size:%6d, Peer:%8x\r\n", mContext, mBuffer, size, mContext->PPeer);
#endif DEBUG_CLOSE

	static volatile MYINT	InCloseProcess = MARK_NOT_IN_PROCESS;																	//
	long	ret_err = 0x00;
	CContextItem* peerContext;
	//	CContextItem* peerContext = mContext->PPeer;
	//
	if (!mContext) return 0;									//	mContext will return 0, find in Jan 30 '13						//
																//////////////////////////////////////////////////////////////////////
	if ( mBuffer && (mBuffer->NOperation == OP_ACCEPT) && mBuffer->CliHandle )														//
	{																																//
		mContext->PProtocol->MyCloseHandle(mBuffer->CliHandle);																		//
		mBuffer->CliHandle = 0;																										//
																																	//
		if (mContext->BHandle)									//	close the ACCEPT SOCKET, should ACCEPT again					//
		{														//////////////////////////////////////////////////////////////////////
			NoneProFunc(mContext->PProtocol, fPostAccept)(mContext, mBuffer, mBuffer->BufferType->BufferSize, OP_ACCEPT);			//
			return 0;																												//
		}																															//
		else mContext->CliBuffer = 0;							//	To clear CliBuffer												//
	}															//////////////////////////////////////////////////////////////////////

	if (mBuffer && (size!=FLAG_KEEPALIVE) ) AppFunc(mContext->PApplication, fOnClose)(mContext, mBuffer, 0, opSide);				//

	while ( InterCmpExg(&InCloseProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
																//	Make sure, the peers do NOT close at same time					//
	peerContext = mContext->PPeer;

	//	if (peerContext)										//	Add in Jun 23 '13, for another 3 hours, after encap decap check
	if (peerContext && peerContext->PPeer == mContext)			//
	{															//	I am addicted to this segment, so beautiful the code is			//
		peerContext->PPeer = peerContext;																							//
		mContext->PPeer = NULL;									// surely, add in May 20, '13
		if (size & FLAG_KEEPALIVE)								// for http keepalive
		{
			InCloseProcess = MARK_NOT_IN_PROCESS;																					//
			if (NoneProFunc(peerContext->PProtocol, fPostReceive)																	//
				(peerContext, mBuffer, mBuffer->BufferType->BufferSize, OP_CLIENT_READ, opSide) ) 
				ret_err = size;
		}
		else if (size & FLAG_GRACE_CLOSE)
		{
// 			mContext->PPeer = NULL;								// surely, add in May 20, '13
			InCloseProcess = MARK_NOT_IN_PROCESS;																					//
		}
		else
		{
// 			mContext->PPeer = NULL;
			InCloseProcess = MARK_NOT_IN_PROCESS;																					//
			if (peerContext->PPeer)
			{
#ifdef DEBUG_CLOSE
				printf("\r\nClose called by Peer:");
#endif DEBUG_CLOSE
				OnClose(peerContext, isNULL, 0, opSide);
			}
		}
		ProFunc(mContext->PProtocol, fPostClose)(mContext, isNULL, 0, 0);								//
	}																																//
	else
		InCloseProcess = MARK_NOT_IN_PROCESS;																						//

// if (peerContext && peerContext->PPeer != mContext)
// {
// 	int aa = 1;
// }

	while ( InterCmpExg(&InCloseProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	peerContext = mContext->EncapContext;
	//	if (peerContext && !size)									//	Change in Jun 22 '13, after four hours work
	if (peerContext && !size && peerContext->DecapContext == mContext)																//
	{
		peerContext->DecapContext = NULL;																							//
		mContext->EncapContext = NULL;
		InCloseProcess = MARK_NOT_IN_PROCESS;																						//
#ifdef DEBUG_CLOSE
		printf("\r\nClose called by Encap:");
#endif DEBUG_CLOSE
		OnClose(peerContext, isNULL, 0, opSide);
	}
	else InCloseProcess = MARK_NOT_IN_PROCESS;																						//

	while ( InterCmpExg(&InCloseProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	peerContext = mContext->DecapContext;
	//	if (peerContext && !size)									//	Change in Jun 22 '13, after four hours work
	if (peerContext && !size && peerContext->EncapContext == mContext)																//
	{
		peerContext->EncapContext = NULL;																							//
		mContext->DecapContext = NULL;
		InCloseProcess = MARK_NOT_IN_PROCESS;																						//
#ifdef DEBUG_CLOSE
		printf("\r\nClose called by Decap:");
#endif DEBUG_CLOSE
		OnClose(peerContext, isNULL, 0, opSide);
	}
	else InCloseProcess = MARK_NOT_IN_PROCESS;																						//

	while ( InterCmpExg(&InCloseProcess, MARK_IN_PROCESS, MARK_NOT_IN_PROCESS) == MARK_IN_PROCESS );								//
	peerContext = mContext->LogFileContext;
	if (peerContext)
	{
		mContext->LogFileContext = NULL;
		if (peerContext->LogFileContext) peerContext->LogFileContext = 0;	// add Jun 13 '13
		InCloseProcess = MARK_NOT_IN_PROCESS;																						//
#ifdef DEBUG_CLOSE
		printf("\r\nClose called by Log:");
#endif DEBUG_CLOSE
		// 		if (mContext->PProtocol->ProtocolNumber == PROTOCOL_TCP)				// test
		OnClose(peerContext, isNULL, 0, opSide);
	}
	else InCloseProcess = MARK_NOT_IN_PROCESS;																						//

	return ret_err;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	So easy for these APPLICATION																									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CEchoApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	return NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, OP_SERVER_WRITE, OPSIDE_CLIENT);					//
	//	two op is different, attention
};																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CPrintApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{																																	//
	char* place = (char*)(mBuffer+1) + mBuffer->NProcessSize;
	*place = 0;
	printf("%s", (char*)(mBuffer+1));
	return NoneAppFunc(mContext->PApplication, fOnServerWrite)(mContext, mBuffer, size, 0);											//
};																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CForwardApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{																																	//
	return SendToPeer(mContext, mBuffer, size, OP_SERVER_WRITE);																	//
}																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CForwardApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{																																	//
	return SendToPeer(mContext, mBuffer, size, OP_CLIENT_WRITE);																	//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned char DNSHeadPad[] = {0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
unsigned char DNSAnswerPad[] = {0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x04 };

extern ContentPadConfig* configInfo;
extern CMemoryListUsed*	TempUseMemory;							//	for program memory use, avoid use malloc				//
extern ULONG volatile GlobalTime;
CDNSApplication* pDNSApplication;

long SetDNS (CListItem* mBuffer, DWORD addr)
{
	char *bufferstart, *questionEnd;
	DWORD* IPplace;

	bufferstart = REAL_BUFFER(mBuffer);

	memcpy (bufferstart+4, DASZ(DNSHeadPad));
	questionEnd = (char*)memchr(bufferstart+0x0c, '\0', mBuffer->NProcessSize);
	if (!questionEnd) return VALUE_NOT_FOUND;

	memcpy (questionEnd+5, DASZ(DNSAnswerPad));
	IPplace = (DWORD*)(questionEnd+5+sizeof(DNSAnswerPad));
	*IPplace = addr;
	mBuffer->NProcessSize = (char*)IPplace-bufferstart+4;
	return 0; 
}


long SetOneIPMac (ContentPad* resultPad, long keyFrom)
{
	IPAddr ip;
	unsigned __int64 mac;

	ip = (ULONG)GetHex(resultPad->resultKey[keyFrom], resultPad->getLength[keyFrom]);
	mac = GetHex(resultPad->resultKey[keyFrom+1], resultPad->getLength[keyFrom+1]);
	if (pDNSApplication->InMACList(mac)) pDNSApplication->AddIPMACList(ip, mac);
	return 0;
}

long CDNSApplication::BuildList()
{
	isForward = 0;					//	after IP & MAC list ok, it begin work
	DNSMACList = DNSIPList = 0;
	pDNSApplication = this;

	DNSIPList = TempUseMemory->GetContext();
	DNSMACList = TempUseMemory->GetContext();

	if (TempUseMemory->BufferSize < MAX_IP_NUMBER * sizeof(__int64))
	{
// 		_asm int 3
	}

	if (DNSIPList && DNSMACList)
	{
		pIPTable = (IPStateTable*)REAL_BUFFER(DNSIPList);
		pMACTable = (MACStateTable*)REAL_BUFFER(DNSMACList);

// 		pIPTable->ipTableNumber = 0;
		pMACTable->macTableNumber = 0;
		isForward = 1;
	}
	return 0;
}																//

CDNSApplication::~CDNSApplication()
{
	isForward = 0;
	if (DNSIPList) TempUseMemory->FreeContext(DNSIPList);
	if (DNSMACList) TempUseMemory->FreeContext(DNSMACList);
}

//	in list return 0, not in return 1
long CDNSApplication::InMACList(__int64 mac)
{
	int i, macNumber;
	MACStateList* nowMac;

	macNumber = pMACTable->macTableNumber;
	nowMac = &pMACTable->MACState[0];
	for (i=0; i<macNumber; i++)
	{
		if (mac == nowMac->macAddress) return 0;
		nowMac++;
	}
	return 1;
}

//	in list return 0, not in return 1
long CDNSApplication::InIPList(IPAddr ipaddr)
{
	long iphost;
	__int64 MacAddr = 0;					// for 6-byte hardware addresses
	ULONG PhysAddrLen = 6;					// default to length of six bytes
	long ret;

	iphost = (ipaddr >> 16)&0xffff;
	IPStateList* nowip = &(pIPTable->IPState[iphost]);

	if ( nowip->accessTime && (ULONG)(GlobalTime-nowip->accessTime) < DNS_MAC_FLUSH_TIME ) return 0;
	ret = SendARP(ipaddr, INADDR_ANY , &MacAddr, &PhysAddrLen);
	if (ret != NO_ERROR) MacAddr = 0;
	else
	{
		if (!InMACList(MacAddr))
		{
			nowip->accessTime = GlobalTime;
			return 0;
		}
	}
	return 1;
}

long CDNSApplication::AddIPMACList(IPAddr ipaddr, __int64 mac)
{
	long nownumber;
	long iphost;

	nownumber = InterInc(&(pMACTable->macTableNumber))-1;
	pMACTable->MACState[nownumber].macAddress = mac;

	if (ipaddr)
	{
		iphost = (ipaddr >> 16)&0xffff;
		pIPTable->IPState[iphost].accessTime = GlobalTime;
	}
	return 0;
}

long CDNSApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{
																																	//
	if (!isForward) return 1;									//	for system data update, stop forward
	return SendToPeer(mContext, mBuffer, size, OP_SERVER_WRITE);																	//
}																																	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
long CDNSApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{																																	//
	long ret;

	if (!isForward) return 1;

	IPAddr clientIP = ((CTCPContext*)(mContext->PPeer))->clientAddr.sin_addr.S_un.S_addr;
	if (InIPList(clientIP))
	{
		ret = SetDNS(mBuffer, configInfo->ContentListenAddress.sin_addr.S_un.S_addr);
		if (ret) return 1;
	}

	return SendToPeer(mContext, mBuffer, size, OP_CLIENT_WRITE);																	//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
