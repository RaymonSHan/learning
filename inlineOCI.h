
//	For me, use some inline to make OCI more readable
#ifdef _WIN64																														//
{
	int unkonw [0x20];
}MyOCIDefine;
#else _WIN64
typedef struct MyOCIDefine
{
	int unkonw [0x10];
	int	DefineLength;
	int	DefineStart;
}MyOCIDefine;
#endif _WIN64

inline sword OCIInitialize( void )
{
	return OCIInitialize( (ub4)OCI_DEFAULT, (dvoid *)0, 
		(dvoid * (*)(dvoid *, size_t))0, (dvoid * (*)(dvoid *, dvoid *, size_t))0, (void (*)(dvoid *, dvoid *))0 );
};
inline sword OCIEnvInit(OCIEnv* &EnvHandle)
{
	return OCIEnvInit( &EnvHandle, OCI_DEFAULT, (size_t)0, (dvoid **)0 );
}
inline sword OCIEnvNlsCreate(OCIEnv* &EnvHandle, ub4 mode, ub2 charset, ub2 ncharset)
{
	return OCIEnvNlsCreate(&EnvHandle, mode, (dvoid *)0, 
		(dvoid * (*)(dvoid *, size_t))0, (dvoid * (*)(dvoid *, dvoid *, size_t))0, (void (*)(dvoid *, dvoid *))0,
		(size_t)0, (dvoid **)0, charset, ncharset );
}
inline sword OCIEnvHandleFree(OCIEnv* EnvHandle)
{
	if (EnvHandle) return OCIHandleFree((dvoid *)EnvHandle, OCI_HTYPE_ENV);
	else return OCI_INVALID_HANDLE;
}

inline sword OCIErrorHandleAlloc(OCIEnv* EnvHandle, OCIError* &ErrHandle)
{
	return OCIHandleAlloc( (dvoid *)EnvHandle, (dvoid **)&ErrHandle, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
}
inline sword OCIErrorHandleFree(OCIError* ErrHandle)
{
	if (ErrHandle) return OCIHandleFree((dvoid *)ErrHandle, OCI_HTYPE_ERROR);
	else return OCI_INVALID_HANDLE;
}

inline sword OCIServerHandleAlloc(OCIEnv* EnvHandle, OCIServer* &ServerHandle)
{
	return OCIHandleAlloc( (dvoid *)EnvHandle, (dvoid **)&ServerHandle, OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0);
}
inline sword OCIServerHandleFree(OCIServer* ServerHandle)
{
	if (ServerHandle) return OCIHandleFree((dvoid *)ServerHandle, OCI_HTYPE_SERVER);
	else return OCI_INVALID_HANDLE;
}

inline sword OCIContextHandleAlloc(OCIEnv* EnvHandle, OCISvcCtx* &ContextHandle)
{
	return OCIHandleAlloc( (dvoid *)EnvHandle, (dvoid **)&ContextHandle, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0);
}
inline sword OCIContextHandleFree(OCISvcCtx* ContextHandle)
{
	if (ContextHandle) return OCIHandleFree((dvoid *)ContextHandle, OCI_HTYPE_SVCCTX);
	else return OCI_INVALID_HANDLE;
}

inline sword OCIStmtHandleAlloc(OCIEnv* EnvHandle, OCIStmt* &StmtHandle)
{
	return OCIHandleAlloc( (dvoid *)EnvHandle, (dvoid **)&StmtHandle, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
}
inline sword OCIStmtHandleFree(OCIStmt* StmtHandle)
{
	if (StmtHandle) return OCIHandleFree((dvoid *)StmtHandle, OCI_HTYPE_STMT);
	else return OCI_INVALID_HANDLE;
}

inline sword OCISessionHandleAlloc(OCIEnv* EnvHandle, OCISession* &SessionHandle)
{
	return OCIHandleAlloc( (dvoid *)EnvHandle, (dvoid **)&SessionHandle, OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0);
}
inline sword OCISessionHandleFree(OCISession* SessionHandle)
{
	if (SessionHandle) return OCIHandleFree((dvoid *)SessionHandle, OCI_HTYPE_SESSION);
	else return OCI_INVALID_HANDLE;
}

inline sword OCISubscriptionHandleAlloc(OCIEnv* EnvHandle, OCISubscription* &SubHandle)
{
	return OCIHandleAlloc( (dvoid *)EnvHandle, (dvoid **)&SubHandle, OCI_HTYPE_SUBSCRIPTION, (size_t)0, (dvoid **)0);
}
inline sword OCISubscriptionHandleFree(OCISubscription* SubHandle)
{
	if (SubHandle) return OCIHandleFree((dvoid *)SubHandle, OCI_HTYPE_SUBSCRIPTION);
	else return OCI_INVALID_HANDLE;
}

inline sword OCIServerAttach(OCIServer* ServerHandle, OCIError* ErrHandle, text* LinkString)
{
	return OCIServerAttach( ServerHandle, ErrHandle, LinkString, (sb4)strlen((char*)LinkString), OCI_DEFAULT);
}

inline sword OCIStmtPrepare(OCIStmt* StmtHandle, OCIError* ErrHandle, text* SQL)
{
	return OCIStmtPrepare(StmtHandle, ErrHandle, SQL, (ub4)strlen((char*)SQL), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
}

inline sword OCIStmtExecute(OCISvcCtx* ContextHandle, OCIStmt* StmtHandle, OCIError* ErrHandle, ub4 iter, ub4 mode)
{
	return OCIStmtExecute(ContextHandle, StmtHandle, ErrHandle, iter, (ub4)0, (CONST OCISnapshot *)NULL, (OCISnapshot *)NULL, mode);
}

inline sword OCIBindStrByName(OCIStmt* StmtHandle, OCIBind* &BindHandle, OCIError* ErrHandle, text* PlaceName, text* StrName, sb4 StrLen)
{
	return OCIBindByName(StmtHandle, &BindHandle, ErrHandle, PlaceName, -1, (dvoid*)StrName, StrLen, SQLT_STR, 
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DEFAULT);
}
inline sword OCIBindIntByName(OCIStmt* StmtHandle, OCIBind* &BindHandle, OCIError* ErrHandle, text* PlaceName, sb4 &IntName)
{
	return OCIBindByName(StmtHandle, &BindHandle, ErrHandle, PlaceName, -1, (dvoid*)&IntName, (sb4)sizeof(sb4), SQLT_INT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DEFAULT);
}
inline sword OCIBindStrByPos(OCIStmt* StmtHandle, OCIBind* &BindHandle, OCIError* ErrHandle, ub4 Place, text* StrName, sb4 StrLen)
{
	return OCIBindByPos(StmtHandle, &BindHandle, ErrHandle, Place, (dvoid*)StrName, StrLen, SQLT_STR, 
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DEFAULT);
}
inline sword OCIBindIntByPos(OCIStmt* StmtHandle, OCIBind* &BindHandle, OCIError* ErrHandle, ub4 Place, sb4 &IntName)
{
	return OCIBindByPos(StmtHandle, &BindHandle, ErrHandle, Place, (dvoid*)&IntName, (sb4)sizeof(sb4), SQLT_INT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)0, (ub4 *)0, OCI_DEFAULT);
}

inline sword OCIDefineStrByPos(OCIStmt* StmtHandle, MyOCIDefine* &DefineHandle, OCIError* ErrHandle, ub4 Place, text* StrName, sb4 StrLen)
{
	return OCIDefineByPos(StmtHandle, &((OCIDefine* &)DefineHandle), ErrHandle, Place, (dvoid*)StrName, StrLen, SQLT_STR,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
}
inline sword OCIDefineIntByPos(OCIStmt* StmtHandle, MyOCIDefine* &DefineHandle, OCIError* ErrHandle, ub4 Place, sb4 &IntName)
{
	return OCIDefineByPos(StmtHandle, &((OCIDefine* &)DefineHandle), ErrHandle, Place, (dvoid*)&IntName, (sb4)sizeof(sb4), SQLT_INT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
}


inline sword OCIAttrSetUserPass(OCISession* SessionHandle, OCIError* ErrHandle, text* username, text* password)
{
	OCIAttrSet((dvoid *)SessionHandle, (ub4)OCI_HTYPE_SESSION, (dvoid *)username, (ub4) strlen((char *)username),
		(ub4) OCI_ATTR_USERNAME, ErrHandle);

	return OCIAttrSet((dvoid *)SessionHandle, (ub4)OCI_HTYPE_SESSION, (dvoid *)password, (ub4) strlen((char *)password),
		(ub4) OCI_ATTR_PASSWORD, ErrHandle);
}



inline sword OCIAttrGetFetchedRows(OCIStmt* StmtHandle, OCIError* ErrHandle, ub4 &Rows)
{
	ub4 sizep = sizeof(ub4);
	return OCIAttrGet((void*)StmtHandle, (ub4)OCI_HTYPE_STMT, (void*)&Rows, &sizep, (ub4)OCI_ATTR_ROWS_FETCHED, ErrHandle);
}