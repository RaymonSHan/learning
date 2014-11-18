

#undef boolean													// for oratypes.h define boolean as int
#include	<openssl/ssl.h>
#include	<openssl/err.h>

class Synchronizer
{
private:
	CRITICAL_SECTION lock_;

public:
	__forceinline Synchronizer()
	{
		InitializeCriticalSectionAndSpinCount(&lock_, 4000);
	}
	__forceinline ~Synchronizer()
	{
		DeleteCriticalSection(&lock_);
	}

	__forceinline void acquire()
	{
		__try
		{
			EnterCriticalSection(&lock_);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	__forceinline void release()
	{
		LeaveCriticalSection(&lock_);
	}
};

static Synchronizer* openssl_locks = 0;

struct CRYPTO_dynlock_value
{
	Synchronizer s;
};

static void funcOpenSSLLockingCallback( int mode, int type, const char* file, int line )
{
	if (mode & CRYPTO_LOCK) openssl_locks[type].acquire();
	else openssl_locks[type].release();
}

static unsigned long funcOpenSSLIDCallback(void)
{
	return GetCurrentThreadId();
}

static CRYPTO_dynlock_value* funcOpenSSLDynCreateCallback( const char* file, int line )
{
	return new CRYPTO_dynlock_value;
}

static void funcOpenSSLDynDestroyCallback( CRYPTO_dynlock_value* l, const char* file, int line )
{
	delete l;
}

static void funcOpenSSLDynLockCallback( int mode, CRYPTO_dynlock_value* l, const char* file, int line )
{
	if (mode & CRYPTO_LOCK) l->s.acquire();
	else l->s.release();
}

void reportError(SSL* ssl, int result);

class CSSLContext : public CTCPContext
{
public:
	SSL*				ssl;
	BIO*				bioIn;
	BIO*				bioOut;
// 	long				haveHandshake;
	CListItem*			waitBuffer;								//	the data wait for SSL handshake
};

long CreateEncapContext(CContextItem* mContext, CContextItem* modelContext, CListItem* &mBuffer, long size);

class CInterProtocol : public CProtocol
{
public:																																//
	CInterProtocol() : CProtocol() 
	{
		MyCloseHandle = CloseNullHandle;																							//
	};																																//
	long	CreateNew(CContextItem* mContext, void* para, long size);																//
	long	CreateRemote(CContextItem* mContext, void* para, long size);															//
	long	PostConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long op);
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);								//
};

class CInterApplication : public CApplication
{
public:																																//
	CInterApplication (char* clicomend = NULL, long cliendsize = 0, char* sercomend =  NULL, long serendsize = 0)
		: CApplication(clicomend, cliendsize, sercomend, serendsize) {};
	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};

class CPassbyApplication : public CInterApplication
{
protected:
	CContextItem* passbyContext;								//	peer, by CreateRemote
public:																																//
	CPassbyApplication (char* clicomend = NULL, long cliendsize = 0, char* sercomend =  NULL, long serendsize = 0)
		: CInterApplication(clicomend, cliendsize, sercomend, serendsize) {};
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};

class CSSLApplication : public CInterApplication
{
protected:
	int		SSLMode;
	SSL_CTX* ctx;
	long	NeedDataOp;
public:
 	CSSLApplication ();
	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);

	long	TranSSLforWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);
	inline long	WriteEncode(CContextItem* mContext, CListItem* mBuffer, long size);
	inline long	ReadDecode(CContextItem* mContext, CListItem* mBuffer, long size);
	inline long	WriteDecode(CContextItem* mContext, CListItem* mBuffer, long size);
	inline long	ReadEncode(CContextItem* mContext, CListItem* mBuffer, long size);

};

class CSSLServerApplication : public CSSLApplication																				//
{																																	//
public:
	CSSLServerApplication ();
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//

};																																	//

class CSSLClientApplication : public CSSLApplication																				//
{																																	//
public:
	CSSLClientApplication ();
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
};	