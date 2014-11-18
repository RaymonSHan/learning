

typedef struct ForwardLoopConfig
{
	long			ContentListenNumber;
	CHARS			mForwardLoopName[MAX_FORWARDLOOP_LISTEN];
	CForwardLoopApplication*	pForwardLoop[MAX_FORWARDLOOP_LISTEN];
}ForwardLoopConfig;

class CForwardLoopApplication : public CMultiApplication
{
public:																																//
	CForwardLoopApplication() :	CMultiApplication(NULL, 0, NULL, 0) {};
	~CForwardLoopApplication() {};																										//

// 	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
// 	long	OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
// 	long	OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
// 	long	OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);											//
};

long GetApplicationByName(char* name, CForwardLoopApplication* &pApp);