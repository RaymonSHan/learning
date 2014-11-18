
#include "inlineOCI.h"

#define		MAX_DEFINES			64

typedef struct OciLinkStruct
{
	ub4				OciMode;
	ub2				OciCharset;
	ub2				OciNCharset;
	char			OciLinkString[NORMAL_CHAR];
	char			OciLinkUser[NORMAL_CHAR];
	char			OciLinkPass[NORMAL_CHAR];
}OciLinkStruct;

class COciContext : public CContextItem																								//
{																																	//
public:																																//
	OCIEnv			*EnvHandle;
	OCIError		*ErrHandle;
	OCISession		*OciSession;
	OCIServer		*OciServer;
	OCISvcCtx		*OciServerContext;
	OCIStmt			*StmtHandle;
	MyOCIDefine		*DefineHandle[MAX_DEFINES];
};																																	//

class COciProtocol : public CProtocol
{
private:
	OciLinkStruct	OciLink;
public:
	COciProtocol() : CProtocol() {};
	long	CreateNew(CContextItem* mContext, void* para, long size);
	long	CreateRemote(CContextItem* mContext, void* para, long size);
	long	PostAccept(CContextItem* mContext, CListItem* mBuffer, long size, long op);
	long	PostConnect(CContextItem* mContext, CListItem* mBuffer, long size, long op);
	long	PostSend(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);
	long	PostReceive(CContextItem* mContext, CListItem* &mBuffer, long size, long op, long opSide);

	long	InitOciDefine(COciContext* ociContext, CListItem* mBuffer);
	long	InitOci(COciContext* mContext);
	long	ExitOci(COciContext* mContext, long state = 0xffff);
};


class COracleApplication : public CWebApplication																					//
{																																	//
public:																																//
	COracleApplication() : CWebApplication () {};																					//
	~COracleApplication() { };																										//
};																																	//
