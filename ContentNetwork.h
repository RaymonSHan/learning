
class CContendNetworkServer : public CMultiApplication
{
public:																																//
	CContendNetworkServer() :	CMultiApplication("\r\n\r\n", 4, "\r\n\r\n", 4) {};
	~CContendNetworkServer() {};																										//

	long	InitProcess(void);
	long	ExitProcess(void);

// 	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
// 	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
// 	long	OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
// 	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
// 	long	OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
// 	long	AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int opSide);											//
// 
// 	long	OnHttpFileRead(CContextItem* mContext, CListItem* &mBuffer, long size);													//
// 	long	OnRemoteServerRead(CContextItem* mContext, CListItem* &mBuffer, long size);												//
};
