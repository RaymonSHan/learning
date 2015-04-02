
#define MAX_ADDITION				5			// for bitic, 2 is enough
#define MAX_ADDITION_ARRAY			256			// for bitic, is the max size of user
#define MAX_CONTENT_FORWARD			32
#define MAX_FORWARDLOOP_LISTEN		8
#define MAX_FORWARDLOOP_FORWARD		(MAX_FORWARDLOOP_LISTEN*MAX_SERVER_PEER)

long AddtoContentList(CContextItem* mContext, CListItem* &mBuffer, char* name);
CListItem* GetContentByName(CContextItem* mContext, char* name);
ContentPad* GetPadByName(CContextItem* mContext, char* name);

CListItem* GetContentByName(CContextItem* mContext, char* name, long len);
ContentPad* GetPadByName(CContextItem* mContext, char* name, long len);

void DisplayContentByName(CContextItem* mContext);

// #if ( defined(CONTENTPAD_APPLICATION) )

WafaProcessFunction GetWafaFunction (char* functionName);

//	This two is used for save temp wafarefer vars	//	May 24 '14
CListItem* GetReferStruct(CContextItem* mContext);
char* AddReferPlace(CContextItem* mContext, ContentPad* pad, char* str, long &len);

long ParserCITIC(unsigned char* condition, int conditionLen, char* valStart, char* valEnd);
long ParserCITICNoBracket(unsigned char* condition, int conditionLen, char* valStart, char* valEnd);
long GetCITICValue(unsigned char *keystart, unsigned char* keyend, char *valStart, char *valEnd, char *&resultstart, long &resultlen);
long GetCITICGroup(unsigned char *keystart, unsigned char* keyend, char *valStart, char *valEnd, char *&nowstart, char *&nowend, char* &groupend);

long StringIntCmp(unsigned char *onestart, unsigned char *oneend, unsigned char *twostart, unsigned char *twoend, long op);
long StringFormatInt(unsigned char *tar, unsigned char *sourstart, unsigned char *sourend);

long Wafa_NULL(CContextItem* mBuffer, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Add(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Exist(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_NotExist(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_IfScanf (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Scanf(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_MultiScanf(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Var(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Count(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Value(CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Compare (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Assign (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Match (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Copy (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_LoopPrint (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_LoopAssign (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Wafa_Replace (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);

#ifdef	NTLM_FUNCTION
long Wafa_NTLM (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
#endif	NTLM_FUNCTION

long Red_GetIPMac (CContextItem* mContext, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);
long Red_SetIPMac (CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom, ContentPad* infoPad);

// long Tunnel_Register(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom);
// long Tunnel_Add(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom);
// long Tunnel_Remove(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom);
// long Tunnel_Close(CContextItem*, CListItem* &referContent, char* start, char* end, char* key, long keyLen, long keyFrom);

WafaCreateFunciont GetCreateFunction (char* functionName);
long Create_Listen(CContextItem* mContext, CListItem* &mBuffer, char* mStart);
long Create_Peer(CContextItem* mContext, CListItem* &mBuffer, char* mStart);
long Create_NULL(CContextItem* mContext, CListItem* &mBuffer, char* mStart);


typedef char CHARS[MAX_PATH];

#if MAX_ADDITION > MAX_CONTENT_KEY																									//
	#error ERROR IN DEFINE MAX_ADDITION																								//
#endif

typedef struct ConfigAdditionArray
{
	ContentPad		AdditionPad;
	CHARS			AdditionArray[MAX_ADDITION];
}ConfigAdditionArray;

// typedef struct ContentForward
// {
// 	long			ContentForwardProtocol;
// 	char			ContentForwardHost[MAX_PATH];
// 	char			ContentForwardName[MAX_PATH];
// 	char			ContentForwardAddress[MAX_PATH];
// 	long			ContentForwardPort;
// 	long			ContentForwardAcceptNumber;
// 	CContextItem*	ContentContext;
// }ContentForward;

typedef struct ContentPadConfig
{
	long			ContentMemoryHttp;
	long			ContentMemoryFile;
	long			ContentMemoryBuffer;
	long			ContentThread;
	long			ContentListenProtocol;
	sockaddr_in		ContentListenAddress;
	long			ContentListenPostAccept;
	long			ContentForwardNumber;
// 	ContentForward	cfc[MAX_CONTENT_FORWARD];
	long			ContentAdditionArrayNumber;
	ConfigAdditionArray		ContendArray[MAX_ADDITION_ARRAY];
	ConfigAdditionArray		ContentVar;
}ContentPadConfig;

class CContentPadServer : public CMultiApplication
{
public:																																//
	CContentPadServer() :	CMultiApplication("\r\n\r\n", 4, "\r\n\r\n", 4) {};
	~CContentPadServer() {};																										//

	long	OnConnect(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClientRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnClientWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
	long	OnServerRead(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);										//
	long	OnServerWrite(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);
	long	OnClose(CContextItem* mContext, CListItem* &mBuffer, long size, long opSide);			//	should handle OnClose for concurrency, if not respond, should return other	//	Apr. 28 '14

	long	AnalysisHead(CContextItem* mContext, CListItem* &mBuffer, long size, int opSide);											//

	long	OnHttpFileRead(CContextItem* mContext, CListItem* &mBuffer, long size);													//
	long	OnRemoteServerRead(CContextItem* mContext, CListItem* &mBuffer, long size);												//
};


long TranCookieInfo(CContextItem* tarContext, CContextItem* souContext, long tarPlace, long souPlace);
long TranCharsetInfo(CContextItem* tarContext, CContextItem* souContext, long tarPlace, long souPlace);
// ContentForward* PrepareContentPeer(char* keySer);

long GetClientPara(CContextItem* mContext, CListItem* &mBuffer, char* paraStart, long paraLength, int nowPlace=0);
//	Changed May 01 '14 for change checkstart in concurrency
long ProcessWafaDefineRefer(CContextItem* mContext, CListItem* &mBuffer, ContentPad* pad, char* wafaStart, char* wafaEnd);
// long ProcessWafaDefineRefer(CContextItem* mContext, CListItem* &mBuffer, char* wafaStart, char* wafaEnd, char* lastStart);
long ProcessWafaDefine(CContextItem* mContext, CListItem* &mBuffer, char* wafaStart, char* wafaEnd);

//long ProcessWafaPage(CContextItem* mContext, CListItem* mBuffer, char* &tpointer, WafaVarFunction nowfunc);, char* checkend = 0);	// the last para is used for restart checkEnd of pad
long ProcessWafaPage(CContextItem* mContext, CListItem* mBuffer, char* &tpointer, char* fileStart, char* fileEnd, WafaVarFunction nowfunc);
inline long ProcessWafaRefer(CContextItem* mContext, CListItem* mBuffer, char* &mStart, char* fileStart, char* fileEnd, WafaVarFunction nowfunc);
long ProcessOneRefer(CContextItem* mContext, CListItem* mBuffer, char* &mStart, char* fileStart, char* fileEnd, 
					 char* rend, long rlen, WafaVarFunction nowfunc);

long ProcessHtmlReplay(CContextItem* mContext);//, CListItem* &mBuffer);
long ProcessNormalReplay(CContextItem* mContext, CListItem* &mBuffer);

//	here varerror change means, Sept. 17 '13, now it is for whether have error control, which is it have Error="<!--error-->", 0 for none
long ProcessNoneVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror);
long ProcessConfigVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror);
//	//	For ForwardLoop project config
// long ProcessForwardLoopVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror);
long ProcessWafaDefineInRefer(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror);
long ProcessWafaVars(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, long varerror);
long ProcessOneVar(CContextItem* mContext, ContentPad* hInfo, char* &mStart, char* fileStart, char* fileEnd, 
				   char* rstart=NULL, long slen=0, char* rend=NULL, long elen=0);

inline long ProcessWafaPartFile(char* &mStart, char* &fileStart, char* fileEnd);
inline long ProcessPartFile(char* &mStart, char* &fileStart, long copysize);
long ProcessPartFile(char* &mStart, char* &fileStart, char* fileEnd, char* istart, long slength);

long GetEndofWafaVars(char* &fileStart, char* fileEnd);

long GetJsonCommond(CContextItem* mContext, CListItem* &mBuffer, char* paraStart, long paraLength, int nowPlace=0);

long PrepareDefine(CContextItem* mContext, CListItem* mBuffer, ContentPad* pad, long /*reEnd*/, char* &wafaStart, char* &wafaEnd);
long PrepareReply(CContextItem* mContext, CListItem* mBuffer, ContentPad* pad, long /*reEnd*/, char* &wafaStart, char* &wafaEnd);

long GetSelect(CListItem* mBuffer);

#define		PARA_DEF_ACTION					0
#define		PARA_DEF_METHOD					1
#define		PARA_DEF_HOST					2
#define		PARA_DEF_NAME					3
#define		PARA_DEF_KEY					4
#define		PARA_DEF_REFERER				5
#define		PARA_DEF_COOKIE					6
#define		PARA_DEF_REFERNAME				7

// #define		SMALL_CONTENT_LENGTH_SIZE		5
#define		SMALL_CONTENT_LENGTH_SIZE		7

#define		PARA_REF_METHOD			0
#define		PARA_REF_KEY			1
#define		PARA_REF_REFERNAME		2
#define		PARA_REF_STARTNAME		3
#define		PARA_REF_ENDNAME		4
#define		PARA_REF_LOOP			5

#define		PARA_REF_STARTREF		6
#define		PARA_REF_ENDREF			7
#define		PARA_REF_START			8
#define		PARA_REF_END			9
#define		PARA_REF_KEYFROM		10
#define		PARA_REF_ID				11
#define		PARA_REF_ERROR			12
#define		PARA_REF_DEFAULT		13
#define		PARA_REF_IF				14

// #endif ( defined(CONTENTPAD_APPLICATION) )

/*
OnServerRead
[
	OnHttpFileRead
	OnRemoteServerRead
	{
		AddtoContentList
		ProcessWafaDefine
		ProcessHtmlReplay
		{
			ProcessWafaPage
			{
				ProcessWafaPartFile
				ProcessWafaRefer
				{
					ProcessOneRefer
					{
						Wafa_
						[
							ProcessConfigVars
							ProcessWafaVars
							{
								ProcessOneVar
							}
						]
					}
				}
			}
		}
	}
]
*/