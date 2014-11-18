
class CJson
{
public:
	CJson* ParentJson;
	char JsonName[SMALL_CHAR];
	__int64 NameId;
	unsigned char* JsonValue;
	char  JsonType;
};

class CJsonGroup
{
private:
	char *TotalEnd;
	CJson *JsonSign, *JsonNow;
	CJson *JsonNest[MAX_JSON_NEST];
	int	NowNumber, NowNest;

public:
	long ReadOneFormat(char* &nowplace, char* endplace);
	long ReadJsonFormat(CJson* json, int number, char* filemodel, int size);
};

__int64 ReadNameId(char* &nowplace);

CListItem* DoJsonPage(CContextItem* mContext, CListItem* mBuffer, long size);

typedef struct Json_ERROR_struct
{
	char* errorname;
}Json_ERROR_struct;


#define	JSON_FILE_TOO_LONG			1
