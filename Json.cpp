
#include "stdafx.h"

// class CJson
// {
// public:
// 	CJson* ParentJson;
// 	char JsonName[NORMAL_CHAR];
// 	char Name[SMALL_CHAR];
// 	DWORD NameLengthMod;
// 	DWORD NameLengthTimes;
// 	int  JsonType;
// 	unsigned char* JsonValue;
// };

// class CJsonGroup
// {
// public:
// 	static long ReadJsonFormat(CJson* json, int number, char* jstart);
// };

//	PagePeer[nowPage].pageModel = GetApplicationBuffer();

#define	J_TYP			0x01
#define	J_INC			0x02
#define	J_DEC			0x04
#define	J_SPE			0x08

unsigned char Json_ASCII_Map[ASCII_NUMBER] = {
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	J_SPE,	0x00,	0x00,	0x00,	0x00,	0x00,	J_SPE,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	J_INC,	0x00,	J_DEC,	0x00,	0x00,	
	0x00,	J_TYP,	0x00,	0x00,	0x00,	0x00,	0x00,	J_TYP,	0x00,	J_TYP,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	J_TYP,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	J_INC,	0x00,	J_DEC,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
};

unsigned char Json_NameId_Map[ASCII_NUMBER] = {
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0a,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x0b,	0x0c,	0x0d,	0x0e,	0x0f,	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,	0x18,	0x19,	
	0x1a,	0x1b,	0x1c,	0x1d,	0x1e,	0x1f,	0x20,	0x21,	0x22,	0x23,	0x24,	0x00,	0x00,	0x00,	0x00,	0x3f,	
	0x00,	0x25,	0x26,	0x27,	0x28,	0x29,	0x2a,	0x2b,	0x2c,	0x2d,	0x2e,	0x2f,	0x30,	0x31,	0x32,	0x33,	
	0x34,	0x35,	0x36,	0x37,	0x38,	0x39,	0x3a,	0x3b,	0x3c,	0x3d,	0x3e,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
};

#define	JSON_SIGN_TOO_MUCH			2
#define	JSON_NO_HEADSTART			3
#define	JSON_NO_HEADEND				4
#define	JSON_NO_JSONSTART			5
#define	JSON_NO_JSONEND				6
#define	JSON_FORMAT_ERROR			7
#define	JSON_UNKNOW_TYPE			8
#define	JSON_NEST_TOO_MUCH			9
#define	JSON_NEED_SINGLE_QUOTE		10
#define	JSON_NAME_BRACKET_ERROR		11
#define	JSON_NEED_GREATER			12
// #define			13

Json_ERROR_struct JSON_ERROR[MAX_JSONG_ERROR] = {
/*   0 */	{"No ERROR"},
/*   1 */	{"HTML model file is too large, normal less than 200K"},
/*   2 */	{"SIGN too much in one HTML model, normal less the 1000"},
/*   3 */	{"Do NOT find <head> in HTML model file"},
/*   4 */	{"Do NOT find </head> in HTML model file"},
/*   5 */	{"Do NOT find <!--json> in HTML model file"},
/*   6 */	{"Do NOT find </json> in HTML model file"},
/*   7 */	{"<json> and </json> ERROR in HTML model file"},
/*   8 */	{"Unknow type in <json x>"},
/*   9 */	{"Nest too much, normal less than 24"},
/*  10 */	{"Need single quote"},
/*  11 */	{"Json FullName with unexcept bracket"},
/*  12 */	{"Need '>'"},
/*  13 */	{" "},

};

#define HEAD_START				"<head"
#define HEAD_END				"</head>"
#define JSON_START				"<!--json>"
#define JSON_END				"</json-->"

#define JSON_DEFINE_START		"<json"
#define JSON_DEFINE_END			"</json>"

#define JSON_START_INT			0x6e6f736a				//	"json"
#define JSON_END_INT			0x6f736a2f				//	"/jso"
#define JSON_END_WORD			0x3e6e					//	"n>"

long CJsonGroup::ReadJsonFormat(CJson* json, int number, char* filemodel, int size)
{
	long ret_err = 0x01;
	char *headstart, *headend, *jsonstart, *jsonend;
	char *nowplace;
	
	TotalEnd = filemodel + size;
	JsonSign = json, JsonNow = json;
	NowNumber = 0, NowNest = -1;

	while (TRUE)																													//
	{																																//
		ret_err = 0x10;
		headstart = memstr(filemodel, TotalEnd-filemodel, HEAD_START, sizeof(HEAD_START)-1);
		if (!headstart) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NO_HEADSTART);
		headstart += sizeof(HEAD_START)-1;

		jsonstart = memstr(headstart, TotalEnd-headstart, JSON_START, sizeof(JSON_START)-1);
		if (!jsonstart) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NO_JSONSTART);
		jsonstart += sizeof(JSON_START)-1;

		jsonend = memstr(jsonstart, TotalEnd-jsonstart, JSON_END, sizeof(JSON_END)-1);
		if (!jsonend) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NO_JSONEND);

		headend = memstr(jsonend, TotalEnd-jsonend, HEAD_END, sizeof(HEAD_END)-1);
		if (!headend) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NO_HEADEND);

		break;
	}

	ret_err = 0x20;
	nowplace = jsonstart;
	while (nowplace < jsonend)
	{
		while (*nowplace != '<') nowplace++;
		nowplace++;
		if ( *((long*)nowplace) == JSON_START_INT )
		{
			ret_err = 0x30;
			nowplace += 4;
			NowNest++;
			if (NowNest >= MAX_JSON_NEST) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NEST_TOO_MUCH)

			while (*nowplace == ' ') nowplace++;
			ret_err = ReadOneFormat(nowplace, jsonend);
			if ( ret_err ) break;
		}
		else if ( *((long*)nowplace) == JSON_END_INT && *((WORD*)(nowplace+4))==JSON_END_WORD )
		{
			NowNest--;
			nowplace += 6;
		}
		else if (nowplace > jsonend)
		{
			ret_err = 0;
			break;
		}
		else DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_FORMAT_ERROR);
	}

//	The following should test the body, do it later;

	jsonstart -= (sizeof(JSON_START)-1);
	jsonend += (sizeof(JSON_END)-1);
	memcpy (jsonstart, jsonend, size-(jsonend-jsonstart));
// should set		pPage->pageModel->NProcessSize = (DWORD)pPage->pageModSize;
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in ReadJsonFormat, 0x%x\r\n", ret_err);			//
		return 0;
	}																																//
	return size-(jsonend-jsonstart);																													//
}

long CJsonGroup::ReadOneFormat(char* &nowplace, char* endplace)
{
	long ret_err = 0x01;
	char* nownamestart;
	char nownamelength;
	char nowtype;
	__int64 nowid;
	
	char bracket = 0;
	unsigned char nowchar;

	while (nowplace < endplace)																													//
	{																																//
		ret_err = 0x40;
		if (Json_ASCII_Map[(unsigned char)(*nowplace)] & J_TYP) nowtype = (*nowplace++);
		else DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_UNKNOW_TYPE);

		ret_err = 0x50;
		while (*nowplace == ' ') nowplace++;
		if ( !(Json_ASCII_Map[(unsigned char)(*nowplace++)] & J_SPE) ) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NEED_SINGLE_QUOTE);	//	is '
		nownamestart = nowplace;

		ret_err = 0x60;
		nowchar = (unsigned char)(*nowplace++);
		while  ( !(Json_ASCII_Map[nowchar] & J_SPE) )			// '-' with J_SPE, for end the loop, because the string end by </json--> 
		{
			if (Json_ASCII_Map[nowchar] & J_INC) bracket++;
			if (Json_ASCII_Map[nowchar] & J_DEC) bracket--;
			nowchar = (unsigned char)(*nowplace++);
		}
		if (bracket || nowchar!=0x27) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NAME_BRACKET_ERROR);
		nownamelength = nowplace - nownamestart - 1;

		ret_err = 0x70;
		while (*nowplace == ' ') nowplace++;
		if (*nowplace++ != '>' ) DEBUG_MESSAGE_JSON_ERROR_BREAK(JSON_NEED_GREATER);
		while (*nowplace == ' ') nowplace++;

		ret_err = 0x80;
		nowid = ReadNameId(nowplace);

		ret_err = 0x90;
		while (*nowplace != '<') nowplace++;

		NowNumber++;
		memcpy (JsonNow->JsonName, nownamestart, nownamelength);
		JsonNow->JsonName[nownamelength] = 0;
		JsonNow->NameId = nowid;
		JsonNow->JsonValue = NULL;
		JsonNow->JsonType = nowtype;
		JsonNest[NowNest] = JsonNow;
		if (NowNest > 0) JsonNow->ParentJson = JsonNest[NowNest-1];
		JsonNow++;
		
		ret_err = 0;
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_APPLICATION, MESSAGE_ERROR_L2, "Error in ReadJsonFormat, 0x%x\r\n", ret_err);			//
	}																																//
	return ret_err;																													//
}

__int64 ReadNameId(char* &nowplace)
{
	__int64 ret = 0;
	unsigned char nowchar;

	for (int i=0; i<MAX_JSON_NAMEID; i++)
	{
		nowchar = *nowplace;
		if ( Json_NameId_Map[nowchar] )
		{
			ret = (ret << 5) + Json_NameId_Map[nowchar];
			nowplace++;
		}
		else break;
	}
	return ret;
}


CListItem* DoJsonPage(CContextItem* mContext, CListItem* mBuffer, long size)
{
	return NULL;
}


/*
DEFINE, in head
in json HTML model file, there are one and only one <!--json> & </json--> pair in head, no space in it, <!-- json> is invalid
Any json variable is defined by <json> & </json> pair, which NAME between them, although the length of variable name is unlimited, 
	only FIRST TEN letter is used. which can be combined by upper/lower case, number and underline.
Any '{' and '[' enter a new nest of json, the max nest is 24 now.

USE, in body
Every refer is begin with '<!--json XX>', end with '</json-->' 
The fastest way to refer the define is use the ORDER number of define, ZERO base from the begin.
In array, the order can be relative to the array name, with the number with underline ahead.
*/