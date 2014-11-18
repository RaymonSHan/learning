
#include "stdafx.h"
#include "SSLApplication.h"

int SSLInited = 0;

extern const unsigned char *cert_data;
extern int cert_len;
extern const unsigned char *pkey_data;
extern int pkey_len;

void reportError(SSL* ssl, int result)
{
	if (result <= 0)
	{
		int error = SSL_get_error(ssl, result);

		switch (error)
		{
		case SSL_ERROR_ZERO_RETURN:
			cout << "SSL_ERROR_ZERO_RETURN" << endl;
			break;
		case SSL_ERROR_NONE:
			cout << "SSL_ERROR_NONE" << endl;
			break;
		case SSL_ERROR_WANT_READ:
			cout << "SSL_ERROR_WANT_READ" << endl;
			break;
		default:
			char buffer[256];
			while (error != 0)
			{
				ERR_error_string_n(error, buffer, sizeof(buffer));
				cout << "Error: " << error << " - " << buffer << endl;
				error = ERR_get_error();
			}
			break;
		}
	}
}

long CreateEncapContext(CContextItem* mContext, CContextItem* modelContext, CListItem* &mBuffer, long size)
{
	CContextItem* enContext;
	long ret;
	long ret_err = 0x01;																											//

	__TRY

	ret_err = 0x10;
	enContext = modelContext->PProtocol->GetDuplicateContext(modelContext);
	if (!enContext) break;

	enContext->DecapContext = mContext;
	mContext->EncapContext = enContext;

	ret_err = 0;
	enContext->BHandle = 0;
	ret = NoneAppFunc(enContext->PApplication, fOnAccept)(enContext, mBuffer, size, OP_PASSBY);			//	Free mBuffer

	enContext->PProtocol->FreeProtocolContext(enContext);		// should free ? when ret != 0 ?

	__CATCH(MODULE_APPLICATION, "CreateEncapContext")																	//
}

long CInterProtocol::CreateNew(CContextItem* mContext, void* para, long size)														//
{
	CContextItem* deContext = (CContextItem*)para;
	deContext->EncapContext = mContext;
	mContext->DecapContext = deContext;
	mContext->countDown = TIMEOUT_INFINITE;																							//
	return 0;
}

long CInterProtocol::CreateRemote(CContextItem* mContext, void* para, long size)													//
{
	CContextItem* enContext = (CContextItem*)para;
	enContext->DecapContext = NULL;
	mContext->EncapContext = enContext;
	mContext->countDown = TIMEOUT_INFINITE;																							//
	return 0;
}

long CInterProtocol::PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op)
{
	CContextItem* firstcontext;

	if (mContext->EncapContext)
	{
		return CreateEncapContext(mContext, mContext->EncapContext, mBuffer, size);
	}
	else
	{
//	five hours for add three line to complete InterProtocol SERVER side nest, Jun 21 '13
		firstcontext = mContext->PApplication->FirstContext;
		if (firstcontext->EncapContext)
		{
			return CreateEncapContext(mContext, firstcontext->EncapContext, mBuffer, size);
		}
		else
		{
			firstcontext = mContext->PApplication->FirstServerContext;
			if (firstcontext->EncapContext)
			{
				return CreateEncapContext(mContext, firstcontext->EncapContext, mBuffer, size);
			}
		}
	}
	return 1;													//	for error, 
}

long CInterProtocol::PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)						//
{
	CContextItem* deContext;
	CContextItem* modelContext;
	long ret;
	long ret_err = 0x01;																											//
	long opside = opSide;

	__TRY

	ret_err = 0x10;
	if (!opside)
	{
		if (op == OP_CLIENT_WRITE) opside = OPSIDE_CLIENT;
		else if (op == OP_SERVER_WRITE) opside = OPSIDE_SERVER;
		else break;
	}

	if (mContext->DecapContext && (opside == OPSIDE_CLIENT) && mContext->DecapContext->EncapContext == mContext)
	{
		ret_err = 0;
		NoneAppFunc(mContext->DecapContext->PApplication, fOnServerRead)(mContext->DecapContext, mBuffer, size, opSide);
		break;
	}

	if (mContext->EncapContext && (opside == OPSIDE_SERVER))
	{
		if (!mContext->EncapContext->DecapContext)
		{
			ret_err = 0x20;
			modelContext = mContext->EncapContext;

			deContext = modelContext->PProtocol->GetDuplicateContext(modelContext);
			if (!deContext) break;

			ret_err = 0x30;
			deContext->EncapContext = mContext;
			mContext->DecapContext = deContext;

			ret_err = 0;
			ret = NoneAppFunc(deContext->PApplication, fOnAccept)(deContext, mBuffer, size, opSide);
			deContext->PProtocol->FreeProtocolContext(deContext);		// should free ? when ret != 0 ?
			if (ret) break;
		}
		else if (mContext->EncapContext->DecapContext == mContext)
		{
			ret_err = 0;
			NoneAppFunc(mContext->EncapContext->PApplication, fOnClientRead)(mContext->EncapContext, mBuffer, size, opSide);
			break;
		}
	}

	__CATCH(MODULE_APPLICATION, "CInterProtocol - PostSend")																		//
}

long CInterProtocol::PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)						//
{
	long opside = opSide;
	if (!opside)
	{
		if (op == OP_CLIENT_READ) opside = OPSIDE_CLIENT;
		else if (op == OP_SERVER_READ) opside = OPSIDE_SERVER;
		else return 1;
	}

	if (mContext->DecapContext && (opside == OPSIDE_CLIENT) && mContext->DecapContext->EncapContext == mContext)
		return NoneProFunc(mContext->DecapContext->PProtocol, fPostReceive)(mContext->DecapContext, mBuffer, size, op, opSide);

	if (mContext->EncapContext && (opside == OPSIDE_SERVER) && mContext->EncapContext->DecapContext == mContext)
		return NoneProFunc(mContext->EncapContext->PProtocol, fPostReceive)(mContext->EncapContext, mBuffer, size, op, opSide);

	if (mContext->PPeer && (opside == OPSIDE_PEER) && mContext->PPeer->PPeer == mContext)
		return NoneProFunc(mContext->PPeer->PProtocol, fPostReceive)(mContext->PPeer, mBuffer, size, op, opSide);

	return 1;
}

long CInterApplication::OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)								//
{
	CContextItem* decapContext = mContext->DecapContext;
	long ret;

	if (decapContext && decapContext->EncapContext == mContext)
		ret = NoneAppFunc(decapContext->PApplication, fOnConnect)(decapContext, mBuffer, size, 0);
	return 0;
}

long CInterApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{
	long ret;
	long ret_err = 0x01;																											//

	__TRY

	ret_err = 0x10;
	if (!mContext->EncapContext)
	{
		if (FirstContext && FirstContext->EncapContext)
		{
			ret_err = 0x20;
			ret = CreateEncapContext(mContext, FirstContext->EncapContext, mBuffer, size);
		}
		else
		{
			ret_err = 0x30;
			if (opSide)
				ret = NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, opSide-OP_BASE, opSide);
			else
				ret = NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, OP_SERVER_WRITE, 0);
		}
		if (ret) break;
	}
	else
	{
		ret_err = 0;
		ret = NoneAppFunc(mContext->EncapContext->PApplication, fOnClientRead)(mContext->EncapContext, mBuffer, size, opSide);
		if (ret) break;
	}

	__CATCH(MODULE_APPLICATION, "InterApplicaton - OnClientRead")																	//
}

long CInterApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{
	long ret;
	CContextItem* decapContext = mContext->DecapContext;

	if (decapContext && decapContext->EncapContext == mContext)
	{
		ret = NoneAppFunc(decapContext->PApplication, fOnServerRead)(decapContext, mBuffer, size, opSide);
		return 0;
	}
	else
	{
		if (opSide)
			return NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, opSide-OP_BASE, opSide);					//
		else
	 		return NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, size, OP_CLIENT_WRITE, 0);						//
	}
}

long CPassbyApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{
	CContextItem* peerContext = mContext->PPeer;
	CContextItem* logContext = mContext->LogFileContext;
	CListItem* passbyBuffer;
	long ret_err = 0x01;
	long ret;

	__TRY

	ret_err = 0x10;
	passbyBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!passbyBuffer) break;
	memcpy ((passbyBuffer+1), (mBuffer+1), size);

	ret_err = 0x20;
	if (!logContext)
	{
		peerContext->PPeer = peerContext;
		mContext->LogFileContext = peerContext;
		mContext->PPeer = mContext;
		ret = NoneProFunc(peerContext->PProtocol, fPostConnect)(peerContext, passbyBuffer, size, OP_PASSBY);
	}
	else
		ret = NoneProFunc(logContext->PProtocol, fPostSend)(logContext, passbyBuffer, size, OP_PASSBY, OPSIDE_CLIENT);
// 		ret = NoneProFunc(logContext->PProtocol, fPostSend)(logContext, passbyBuffer, size, OP_SERVER_WRITE, OPSIDE_PEER);

	if (ret) OnClose(NULL, passbyBuffer, 0, 0);					//	do NOT use NoneAppFunc(OnClose)
//	passby error do NOT effect main process

	ret_err = 0x30;
	if (CInterApplication::OnClientRead(mContext, mBuffer, size, opSide)) break;													//

	__CATCH(MODULE_APPLICATION, "CPassbyApplication - OnClientRead")
}

long CPassbyApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)							//
{
	CContextItem* logContext = mContext->LogFileContext;
	CListItem* passbyBuffer;
	long ret_err = 0x01;
	long ret;

	__TRY

	ret_err = 0x10;
	passbyBuffer = mContext->PApplication->GetApplicationBuffer();
	if (!passbyBuffer) break;
	memcpy ((passbyBuffer+1), (mBuffer+1), size);

	ret_err = 0x20;
	if (logContext)
		ret = NoneProFunc(logContext->PProtocol, fPostSend)(logContext, passbyBuffer, size, OP_PASSBY, 0);
// 		ret = NoneProFunc(logContext->PProtocol, fPostSend)(logContext, passbyBuffer, size, OP_SERVER_WRITE, opSide);
	if (ret) OnClose(NULL, passbyBuffer, 0, 0);				//	do NOT use NoneAppFunc(OnClose)

	ret_err = 0x30;
	if (CInterApplication::OnServerRead(mContext, mBuffer, size, opSide)) break;													//

	__CATCH(MODULE_APPLICATION, "CPassbyApplication - OnServerRead")
}


CSSLApplication::CSSLApplication()
{
	if (SSLInited) return;
	SSLInited = 1;
	NeedDataOp = 0;

#ifdef _DEBUG
	// OpenSSL internal memory-leak checkers
	CRYPTO_malloc_debug_init();
	CRYPTO_dbg_set_options(V_CRYPTO_MDEBUG_ALL);
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
#endif

	openssl_locks = new Synchronizer[CRYPTO_num_locks()];

	// callbacks for static lock
	CRYPTO_set_locking_callback(funcOpenSSLLockingCallback);
	CRYPTO_set_id_callback(funcOpenSSLIDCallback);

	// callbacks for dynamic lock
	CRYPTO_set_dynlock_create_callback(funcOpenSSLDynCreateCallback);
	CRYPTO_set_dynlock_destroy_callback(funcOpenSSLDynDestroyCallback);
	CRYPTO_set_dynlock_lock_callback(funcOpenSSLDynLockCallback);

	// Load algorithms and error strings.
	SSL_load_error_strings();
	SSL_library_init();
};

long CSSLApplication::OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)								//
{
	return OnServerRead(mContext, mBuffer, size, opSide);
}

long CSSLApplication::OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)									//
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	SSL* forCloseSSL;

	if (sslContext)
	{
		forCloseSSL = (SSL*)InterlockedExchangePointer((void**)&(sslContext->ssl), 0);
		if (forCloseSSL) SSL_free(forCloseSSL);

		if (sslContext->waitBuffer) FreeApplicationBuffer(sslContext->waitBuffer);
	}
	return CApplication::OnClose(mContext, mBuffer, size, opSide);
}

long CSSLApplication::WriteEncode(CContextItem* mContext, CListItem* mBuffer, long size)
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	char* bufferstart = (char*)(mBuffer+1);
	long bytetotal = 0;
	long byteused;

	do 
	{
		byteused = BIO_write(sslContext->bioIn, bufferstart+bytetotal, size-bytetotal);
		if (byteused <= 0) break;
		bytetotal += byteused;
	}
	while (size-bytetotal > 0);
	if (byteused < 0) return byteused;
	return bytetotal;
}

long CSSLApplication::ReadDecode(CContextItem* mContext, CListItem* mBuffer, long size)
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	char* bufferstart = (char*)(mBuffer+1);
	long bytetotal = 0;
	long byteused;

	do 
	{
		byteused = SSL_read(sslContext->ssl, bufferstart+bytetotal, size-bytetotal);
		if (byteused <= 0) break;
		bytetotal += byteused;
	}
	while (byteused > 0);
	return bytetotal;
}

long CSSLApplication::WriteDecode(CContextItem* mContext, CListItem* mBuffer, long size)
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	char* bufferstart = (char*)(mBuffer+1);
	long bytetotal = 0;
	long byteused;

	do 
	{
		byteused = SSL_write(sslContext->ssl, bufferstart+bytetotal, size-bytetotal);
		if (byteused <= 0) break;
		bytetotal += byteused;
	}
	while (size-bytetotal > 0);
	if (byteused < 0) return byteused;
	return bytetotal;
}

long CSSLApplication::ReadEncode(CContextItem* mContext, CListItem* mBuffer, long size)
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	char* bufferstart = (char*)(mBuffer+1);
	long bytetotal = 0;
	long byteused;

	do 
	{
		byteused = BIO_read(sslContext->bioOut, bufferstart+bytetotal, size-bytetotal);
		if (byteused <= 0) break;
		bytetotal += byteused;
	}
	while (byteused > 0);
	return bytetotal;
}

long CSSLApplication::TranSSLforWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide)
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	long ret_err = 0x01;
	int byteTotal;																							//

	__TRY

	ret_err = 0x10;
	size_t pending = BIO_ctrl_pending(sslContext->bioOut);
	if (pending > 0)
	{
		ret_err = 0x20;
		byteTotal = ReadEncode(mContext, mBuffer, pending);

		if (byteTotal > 0)
		{
			ret_err = 0x30;
			if (mContext->PPeer)
			{
				if ( NoneProFunc(mContext->PProtocol, fPostSend)(mContext, mBuffer, byteTotal, op, opSide) ) break;			
			}
			else
			{
				mContext->PPeer = mContext;
				if ( NoneProFunc(mContext->PProtocol, fPostConnect)(mContext, mBuffer, byteTotal, OP_CONNECT) ) break;			
			}

		}
		else if (BIO_should_retry(sslContext->bioOut)) ret_err = 0xff;
	}
	else
	{
//	In handshake
		ret_err = 0x40;
// 			if (SSLMode == MODE_SSL_SERVER) needdataop = OP_CLIENT_READ;
// 			if (SSLMode == MODE_SSL_CLIENT) needdataop = OP_SERVER_READ;
		if ( NeedDataOp && NoneProFunc(mContext->PProtocol, fPostReceive)														//
			(mContext, mBuffer, mBuffer->BufferType->BufferSize, NeedDataOp, 0) ) break;
	}

	__CATCH_LOOP(MODULE_APPLICATION, "SSLApplicaton - TranSSLforWrite", 0xff)
}

CSSLServerApplication::CSSLServerApplication() : CSSLApplication()
{
	const SSL_METHOD* method;

	SSLMode = MODE_SSL_SERVER;
	NeedDataOp = OP_CLIENT_READ;

	// Create new context from method.
	method = SSLv23_server_method();
	ctx = SSL_CTX_new(method);

	//	These are for load certificate data from memory
	X509 *cert = NULL;
	RSA *rsa = NULL;
	BIO *cbio, *kbio;

	cbio = BIO_new_mem_buf((void*)cert_data, -1);
	cert = PEM_read_bio_X509(cbio, NULL, 0, NULL);
	ASSERT(cert != NULL);
	if (SSL_CTX_use_certificate(ctx, cert) <= 0)
	{
		ERR_print_errors_fp(stdout);
		exit(1);
	}

	kbio = BIO_new_mem_buf((void*)pkey_data, -1);
	rsa = PEM_read_bio_RSAPrivateKey(kbio, NULL, 0, NULL);
	ASSERT(rsa != NULL);
	if (SSL_CTX_use_RSAPrivateKey(ctx, rsa) <= 0)
	{
		ERR_print_errors_fp(stdout);
		exit(1);
	}

// 		for read from file, use this
// 		if (SSL_CTX_use_certificate_file(ctx, "Z:\\Develop\\opensslbin\\server.pem", SSL_FILETYPE_PEM) <= 0)
// 		{
// 			ERR_print_errors_fp(stdout);
// 			exit(1);
// 		}
// 		if (SSL_CTX_use_PrivateKey_file(ctx, "Z:\\Develop\\opensslbin\\ca-nocap.key", SSL_FILETYPE_PEM) <= 0)
// 		{
// 			ERR_print_errors_fp(stdout);
// 			exit(1);
// 		}

	if (!SSL_CTX_check_private_key(ctx))
		cerr << "Private key is invalid." << endl;
	else
		cout << "Private key is OK" << endl;

	return;
}

long CSSLServerApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	long ret_err = 0x01;																											//

	int byteTotal;

	__TRY

	ret_err = 0x10;
	if (mBuffer->NOperation == OP_PROCESS_ACCEPT)
	{
		sslContext->ssl = SSL_new(ctx);
		sslContext->bioIn = BIO_new(BIO_s_mem());
		sslContext->bioOut = BIO_new(BIO_s_mem());

		SSL_set_bio(sslContext->ssl, sslContext->bioIn, sslContext->bioOut);
		SSL_set_accept_state(sslContext->ssl);
	}

	ret_err = 0x20;
	if (WriteEncode(mContext, mBuffer, size) <= 0) break;
		
	ret_err = 0x30;
	byteTotal = ReadDecode(mContext, mBuffer, mBuffer->BufferType->BufferSize);

	if (byteTotal > 0)
	{
		ret_err = 0x40;
		if (CInterApplication::OnClientRead(mContext, mBuffer, byteTotal, opSide)) break;
	}
	else
	{
		ret_err = 0x50;
		if (TranSSLforWrite(mContext, mBuffer, 0, OP_SERVER_WRITE, OPSIDE_CLIENT)) break;
		//	op means return with OnServerWrite, then post client receive. opSide means this is client side 
	}

	__CATCH(MODULE_APPLICATION, "CSSLServerApplication - OnClientRead")
}

long CSSLServerApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	long ret_err = 0x01;																											//

	__TRY

	ret_err = 0x10;
	if (WriteDecode(mContext, mBuffer, size) <=0 ) break;

	ret_err = 0x20;
	if (TranSSLforWrite(mContext, mBuffer, 0, OP_CLIENT_WRITE, 0)) break;

	__CATCH(MODULE_APPLICATION, "CSSLServerApplication - OnServerRead")
}


CSSLClientApplication::CSSLClientApplication() : CSSLApplication()
{	
	const SSL_METHOD* method;

	SSLMode = MODE_SSL_CLIENT;
	NeedDataOp = OP_SERVER_READ;
	method = SSLv23_client_method();
	ctx = SSL_CTX_new(method);

	return;
}

long CSSLClientApplication::OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	long ret_err = 0x01;																											//

	int byteTotal;

	__TRY

	ret_err = 0x10;
	if (mBuffer->NOperation == OP_PROCESS_ACCEPT)
	{
		sslContext->ssl = SSL_new(ctx);
		sslContext->bioIn = BIO_new(BIO_s_mem());
		sslContext->bioOut = BIO_new(BIO_s_mem());

		SSL_set_bio(sslContext->ssl, sslContext->bioIn, sslContext->bioOut);
		SSL_set_connect_state(sslContext->ssl);
	}

	ret_err = 0x20;
	byteTotal = WriteDecode(mContext, mBuffer, size);

	if (byteTotal < 0)
	{
		sslContext->waitBuffer = mBuffer;
		mBuffer = GetApplicationBuffer();
		if (!mBuffer) break;
	}

	ret_err = 0x30;
	if (TranSSLforWrite(mContext, mBuffer, 0, OP_SERVER_WRITE, 0)) break;

	__CATCH(MODULE_APPLICATION, "CSSLClientApplication - OnClientRead")
}

long CSSLClientApplication::OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide)						//
{
	CSSLContext* sslContext = (CSSLContext*)mContext;
	long ret_err = 0x01;																											//
	int result;

	int byteTotal;

	__TRY

	ret_err = 0x10;
	if (WriteEncode(mContext, mBuffer, size) <= 0) break;

	ret_err = 0x20;
	byteTotal = ReadDecode(mContext, mBuffer, mBuffer->BufferType->BufferSize);

	if (byteTotal > 0)
	{
		ret_err = 0x30;	
		if ( CInterApplication::OnServerRead(mContext, mBuffer, byteTotal, opSide) ) break;
	}
	else
	{
		ret_err = 0x40;
		if (TranSSLforWrite(mContext, mBuffer, 0, OP_CLIENT_WRITE, OPSIDE_SERVER)) break;

		ret_err = 0x50;
		CListItem* wBuffer = sslContext->waitBuffer;
		if (wBuffer && SSL_is_init_finished(sslContext->ssl))
		{
			sslContext->waitBuffer = 0;
			wBuffer->NOperation = OP_CLIENT_READ;
			result = OnClientRead(sslContext, wBuffer, wBuffer->NProcessSize, 0);
// 				FreeApplicationBuffer(mBuffer);
			mBuffer = 0;
		}
	}

	__CATCH(MODULE_APPLICATION, "CSSLClientApplication - OnServerRead")
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       