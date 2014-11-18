
#include "stdafx.h"

#ifdef	OCI_FUNCTION

long COciProtocol::CreateNew(CContextItem* mContext, void* para, long size)
{
	return 0;
};

long COciProtocol::CreateRemote(CContextItem* mContext, void* para, long size)
{
	COciContext* ociContext = (COciContext*)mContext;																				//
	long ret_err = 0x01;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		ociContext->countDown = TIMEOUT_INFINITE;																					//
		memcpy((void*)&OciLink, para, size);
																																	//
		ret_err = 0;																												//
		break;																														//
	}																																//
	return ret_err;																													//
};

long COciProtocol::PostAccept(CContextItem* mContext, CListItem* mBuffer, long size, long op)
{
	return 0;
};

void checkerr(OCIError *errhp, sword status)
{
	text errbuf[512];
	sb4 errcode = 0;

	switch (status)
	{
		case OCI_SUCCESS:
			break;
		case OCI_SUCCESS_WITH_INFO:
			(void) printf("Error - OCI_SUCCESS_WITH_INFO\n");
			break;
		case OCI_NEED_DATA:
			(void) printf("Error - OCI_NEED_DATA\n");
			break;
		case OCI_NO_DATA:
			(void) printf("Error - OCI_NODATA\n");
			break;
		case OCI_ERROR:
			(void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
							 errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			(void) printf("Error - %.*s\n", 512, errbuf);
			break;
		case OCI_INVALID_HANDLE:
			(void) printf("Error - OCI_INVALID_HANDLE\n");
			break;
		case OCI_STILL_EXECUTING:
			(void) printf("Error - OCI_STILL_EXECUTE\n");
			break;
		case OCI_CONTINUE:
			(void) printf("Error - OCI_CONTINUE\n");
			break;
		default:
			break;
	}
}

long COciProtocol::PostConnect(CContextItem* mContext, CListItem* mBuffer, long size, long op)
{
	COciContext* ociContext = (COciContext*)mContext;
	long ret_err = 0x01;
	sword	ret = 0;
	ub4		iters;

// 	text errbuf[512];
// 	sb4 errcode = 0;

	ub4 rowsGet = 0;

	while (TRUE)
	{
		ret_err = 0x10;
		if (!mContext->BHandle && InitOci(ociContext) ) break;

		MyOCIDefine		*definePos1 = (MyOCIDefine *) 0, *definePos2 = (MyOCIDefine *) 0, *definePos3 = (MyOCIDefine *) 0, *definePos4 = (MyOCIDefine *) 0;

		ret_err = 0x20;
		ret = OCIStmtHandleAlloc(ociContext->EnvHandle, ociContext->StmtHandle);
		if (ret) DEBUG_MESSAGE_OCI_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2, ret);	

		ret_err = 0x30;
		ret = OCIStmtPrepare(ociContext->StmtHandle, ociContext->ErrHandle, (text*)"select eid, name from zzb_unit");
		if (ret) DEBUG_MESSAGE_OCI_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2, ret);	

		ret_err = 0x40;
		ret_err = 0x40;
		ociContext->DefineHandle[0] = (MyOCIDefine *)20;
		ociContext->DefineHandle[1] = (MyOCIDefine *)70;
		ociContext->DefineHandle[2] = (MyOCIDefine *)0;
		iters = InitOciDefine(ociContext, mBuffer);
		if (!iters) break;

		ret_err = 0x50;
		ret = OCIStmtExecute(ociContext->OciServerContext, ociContext->StmtHandle, ociContext->ErrHandle, iters, OCI_STMT_SCROLLABLE_READONLY);


// 		(void) OCIErrorGet((dvoid *)ociContext->ErrHandle, (ub4) 1, (text *) NULL, &errcode, errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
// 		printf("Error - %.*s\n", 512, errbuf);

		if (ret) DEBUG_MESSAGE_OCI_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2, ret);	
		ret = OCIAttrGetFetchedRows(ociContext->StmtHandle, ociContext->ErrHandle, rowsGet);
		if (ret) DEBUG_MESSAGE_OCI_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2, ret);	

		ret_err = 0x60;
		ret = OCIStmtHandleFree(ociContext->StmtHandle);
		if (ret) DEBUG_MESSAGE_OCI_ERROR_BREAK(MODULE_PROTOCOL, MESSAGE_ERROR_L2, ret);	

// 		ub4 RowsGet = 0;
// 		do 
// 		{
// 			OCIAttrGetFetchedRows(StmtFirstHandle, ErrHandle, RowsGet);
// 			printdata(RowsGet);
// 			OCIStmtFetch2(StmtFirstHandle, ErrHandle, (ub4)10, OCI_FETCH_NEXT, (sb4)0, OCI_DEFAULT);
// 		} while (RowsGet);
// 		OCIStmtFetch2(StmtFirstHandle, ErrHandle, (ub4)0, OCI_FETCH_NEXT, (sb4)0, OCI_DEFAULT);		// free cursor

		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in COciProtocol - PostConnect, 0x%x\r\n  ", ret_err);				//
	}																																//
	return ret_err;																													//
}

long COciProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	return 0;
};

long COciProtocol::PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	return 0;
};

#define 	DEFINE_MASK		8

long COciProtocol::InitOciDefine(COciContext* ociContext, CListItem* mBuffer)
{
	int i, totalRows, oneBytes, totalBytes = 0, totalCols;
	text* bufferStart = (text*)(mBuffer+1);
	sword ret;

	for (i=0; i<MAX_DEFINES; i++)
	{
		oneBytes = (sb4)ociContext->DefineHandle[i];
		if (oneBytes)
		{
			oneBytes = ( oneBytes-1 & (int)(-1)*DEFINE_MASK ) + DEFINE_MASK;
			ociContext->DefineHandle[i] = (MyOCIDefine*)oneBytes;
			totalBytes += oneBytes;
		}
		else
		{
			i--;
			break;
		}
	}
	totalCols = i+1;
	totalRows = mBuffer->BufferType->BufferSize / totalBytes;

	MyOCIDefine		*definePos1 = (MyOCIDefine *) 0;

	for (i=0; i<totalCols; i++)
	{
		oneBytes = (sb4)ociContext->DefineHandle[i];
		//		ret = OCIDefineStrByPos(ociContext->StmtHandle, ociContext->DefineHandle[i], ociContext->ErrHandle, i, bufferStart, oneBytes);
		ret = OCIDefineStrByPos(ociContext->StmtHandle, definePos1, ociContext->ErrHandle, i+1, bufferStart, oneBytes);

		if (ret)
		{
			if (ret) DEBUG_MESSAGE_OCI_ERROR(MODULE_PROTOCOL, MESSAGE_ERROR_L2, ret);	
			return 0;
		}
		bufferStart += oneBytes * totalRows;
	}

	return totalRows;
}

long COciProtocol::InitOci(COciContext* mContext)
{
	long ret_err = 0x01;
	OciLinkStruct* link = &OciLink;

	while (TRUE)
	{
		ret_err = 0x10;
		if ( InterlockedExchangePointer(&(mContext->BHandle), link) ) break;

		ret_err = 0x20;
		if (OCIEnvNlsCreate(mContext->EnvHandle, link->OciMode, link->OciCharset, link->OciNCharset) ) break;

		ret_err = 0x30;
		if ( OCIErrorHandleAlloc(mContext->EnvHandle, mContext->ErrHandle) ) break;

		ret_err = 0x40;
		if ( OCIServerHandleAlloc(mContext->EnvHandle, mContext->OciServer) ) break;

		ret_err = 0x50;
		if ( OCIContextHandleAlloc(mContext->EnvHandle, mContext->OciServerContext) ) break;

		ret_err = 0x60;
		if ( OCIServerAttach(mContext->OciServer, mContext->ErrHandle, (text *)link->OciLinkString) ) break;

		ret_err = 0x70;
		if ( OCIAttrSet( (dvoid *)mContext->OciServerContext, OCI_HTYPE_SVCCTX, (dvoid *)mContext->OciServer,
			(ub4) 0, OCI_ATTR_SERVER, (OCIError *)mContext->ErrHandle) ) break;

		ret_err = 0x80;
		if ( OCISessionHandleAlloc(mContext->EnvHandle, mContext->OciSession) ) break;

		ret_err = 0x90;
		if ( OCIAttrSet((dvoid *)mContext->OciSession, (ub4) OCI_HTYPE_SESSION, (dvoid *)link->OciLinkUser, 
			(ub4) strlen((char *)link->OciLinkUser), (ub4) OCI_ATTR_USERNAME, mContext->ErrHandle) ) break;

		ret_err = 0xa0;
		if ( OCIAttrSet((dvoid *)mContext->OciSession, (ub4) OCI_HTYPE_SESSION, (dvoid *)link->OciLinkPass,
			(ub4) strlen((char *)link->OciLinkPass), (ub4) OCI_ATTR_PASSWORD, mContext->ErrHandle) ) break;

		ret_err = 0xb0;
		if ( OCISessionBegin(mContext->OciServerContext, mContext->ErrHandle, mContext->OciSession,
			OCI_CRED_RDBMS, (ub4) OCI_DEFAULT) ) break;

		ret_err = 0xc0;
		if ( OCIAttrSet((dvoid *)mContext->OciServerContext, (ub4) OCI_HTYPE_SVCCTX, (dvoid *)mContext->OciSession, 
			(ub4) 0, (ub4) OCI_ATTR_SESSION, mContext->ErrHandle) ) break;

// 		ret_err = 0xd0;
// 		if ( OCIStmtHandleAlloc(mContext->EnvHandle, mContext->StmtHandle) ) break;

		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L1, "Error in COciProtocol - InitOci, 0x%x\r\n  ", ret_err);							//
		ExitOci(mContext, ret_err);
	}																																//
	return ret_err;																													//
}
																																	//
long	COciProtocol::ExitOci(COciContext* mContext, long state)
{
// 	if (state > 0xd0) OCIStmtHandleFree(mContext->StmtHandle);
	if (state > 0xb0) OCISessionEnd(mContext->OciServerContext, mContext->ErrHandle, mContext->OciSession, OCI_DEFAULT);
	if (state > 0x80) OCISessionHandleFree(mContext->OciSession);
	if (state > 0x60) OCIServerDetach(mContext->OciServer, mContext->ErrHandle, (ub4)OCI_DEFAULT);
	if (state > 0x50) OCIContextHandleFree(mContext->OciServerContext);
	if (state > 0x40) OCIServerHandleFree(mContext->OciServer);
	if (state > 0x30) OCIErrorHandleFree(mContext->ErrHandle);
	if (state > 0x20) OCIEnvHandleFree(mContext->EnvHandle);
	mContext->BHandle = 0;

	return 0;

}
// static void cleanup(OCIEnv *envHandle, OCIError *errHandle, OCIServer *serverHandle, 
// 					OCISvcCtx *contextHandle, OCISession *sessionHandle, OCIStmt *stmtHandle)
// {
// 	/* detach from the server */
// }



#endif	OCI_FUNCTION

