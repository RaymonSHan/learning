
#include "stdafx.h"

CMemoryListUsed*	TempUseMemory;						//	for program memory use, avoid use malloc				//
// extern	ContentPadConfig*	configInfo;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Display Error Message																											//
//	va_list is a MACRO I have not know before 2012/2/14																				//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
void DEBUG_MESSAGE ( MYINT module, MYINT level, _In_z_ _Printf_format_string_ const char * _Format, ...)							//
{																																	//
	va_list ap;																														//
																																	//
	va_start(ap, _Format);																											//
	if ( level == MESSAGE_ERROR_L4 )																								//
	{																																//
		printf ("--- AnyOne see this message, call Raymon, the GREAT mistaken in my program  ---\r\n");								//
	}																																//
	if ( ((module & MODULE_NOW) && (level & MESSAGE_NOW)) || level == MESSAGE_ERROR_L4 )											//
	{																																//
		printf("Module:%04x - ", module);																							//
		vprintf(_Format, ap);																										//
	}																																//
	if ( level == MESSAGE_ERROR_L4 )																								//
	{																																//
		printf ("-------------------- tell me the OS version, and hardware  -------13706137217--\r\n\r\n");							//
	}																																//
																																	//
	va_end(ap);																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DEBUG_MESSAGE_CON ( MYINT module, MYINT level, _In_z_ _Printf_format_string_ const char * _Format, ...)						//
{																																	//
	va_list ap;																														//
																																	//
	va_start(ap, _Format);																											//
	if ( (module & MODULE_NOW) && (level & MESSAGE_NOW) )																			//
	{																																//
		vprintf(_Format, ap);																										//
	}																																//
	va_end(ap);																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DEBUG_MESSAGE_IPADDRESS_PORT (MYINT module, MYINT level, char* string, MYINT protocol, sockaddr_in ipaddr)					//
{																																	//
	DEBUG_MESSAGE(module, level, string, protocol, inet_ntoa(ipaddr.sin_addr), ntohs(ipaddr.sin_port) );							//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DEBUG_MESSAGE_IPADDRESS_PORT (MYINT module, MYINT level, char* string, sockaddr_in ipaddr)									//
{																																	//
	DEBUG_MESSAGE_CON(module, level, string, inet_ntoa(ipaddr.sin_addr), ntohs(ipaddr.sin_port) );									//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// typedef struct ParaTran
// {
// 	char*	pKeyword;
// 	char*	pFormat;
// 	void*	pValue;
// 	long*	pValueLength;
// }ParaTran;
//	Such as 
//	ParaTran para[2] = {"username", "s", pUsername, pLength, "times", "d", pTimes, pTimesLength};
//	CommandLineParaTran( "username=abc;password=123;times=5", "; ",  para );
//	pUsername will be abc, and pTimes will be 5, it is the vice of printf
//	Attention the four byte of different keyword must be DIFFERENT.

long BYTE_MASK[5] = { 0, 0xff, 0xffff, 0xffffff, 0xffffffff };
unsigned char ASCII_DEC[ASCII_NUMBER] = {
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
};
unsigned char ASCII_HEX[ASCII_NUMBER] = {
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0x0a,	0x0b,	0x0c,	0x0d,	0x0e,	0x0f,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0x0a,	0x0b,	0x0c,	0x0d,	0x0e,	0x0f,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,
};

unsigned char TRAN_ASCII[ASCII_NUMBER] = {
	0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0a,	0x0b,	0x0c,	0x0d,	0x0e,	0x0f,
	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,	0x18,	0x19,	0x1a,	0x1b,	0x1c,	0x1d,	0x1e,	0x1f,
	0x20,	0x21,	0x22,	0x23,	0x24,	0x25,	0x26,	0x27,	0x28,	0x29,	0x2a,	0x2b,	0x2c,	0x2d,	0x2e,	0x2f,
	0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,	0x38,	0x39,	0x3a,	0x3b,	0x3c,	0x3d,	0x3e,	0x3f,
	0x40,	0x41,	0x42,	0x43,	0x44,	0x45,	0x46,	0x47,	0x48,	0x49,	0x4a,	0x4b,	0x4c,	0x4d,	0x4e,	0x4f,
	0x50,	0x51,	0x52,	0x53,	0x54,	0x55,	0x56,	0x57,	0x58,	0x59,	0x5a,	0x5b,	0x5c,	0x5d,	0x5e,	0x5f,
	0x60,	0x61,	0x62,	0x63,	0x64,	0x65,	0x66,	0x67,	0x68,	0x69,	0x6a,	0x6b,	0x6c,	0x6d,	0x0a,	0x6f,
	0x70,	0x71,	0x0d,	0x73,	0x74,	0x75,	0x76,	0x77,	0x78,	0x79,	0x7a,	0x7b,	0x7c,	0x7d,	0x7e,	0x7f,
	0x80,	0x81,	0x82,	0x83,	0x84,	0x85,	0x86,	0x87,	0x88,	0x89,	0x8a,	0x8b,	0x8c,	0x8d,	0x8e,	0x8f,
	0x90,	0x91,	0x92,	0x93,	0x94,	0x95,	0x96,	0x97,	0x98,	0x99,	0x9a,	0x9b,	0x9c,	0x9d,	0x9e,	0x9f,
	0xa0,	0xa1,	0xa2,	0xa3,	0xa4,	0xa5,	0xa6,	0xa7,	0xa8,	0xa9,	0xaa,	0xab,	0xac,	0xad,	0xae,	0xaf,
	0xb0,	0xb1,	0xb2,	0xb3,	0xb4,	0xb5,	0xb6,	0xb7,	0xb8,	0xb9,	0xba,	0xbb,	0xbc,	0xbd,	0xbe,	0xbf,
	0xc0,	0xc1,	0xc2,	0xc3,	0xc4,	0xc5,	0xc6,	0xc7,	0xc8,	0xc9,	0xca,	0xcb,	0xcc,	0xcd,	0xce,	0xcf,
	0xd0,	0xd1,	0xd2,	0xd3,	0xd4,	0xd5,	0xd6,	0xd7,	0xd8,	0xd9,	0xda,	0xdb,	0xdc,	0xdd,	0xde,	0xdf,
	0xe0,	0xe1,	0xe2,	0xe3,	0xe4,	0xe5,	0xe6,	0xe7,	0xe8,	0xe9,	0xea,	0xeb,	0xec,	0xed,	0xee,	0xef,
	0xf0,	0xf1,	0xf2,	0xf3,	0xf4,	0xf5,	0xf6,	0xf7,	0xf8,	0xf9,	0xfa,	0xfb,	0xfc,	0xfd,	0xfe,	0xff,
};

unsigned char TRAN_POST[ASCII_NUMBER] = {
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
};

unsigned char TRAN_POST_NEW[ASCII_NUMBER] = {
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x00,	0x00,	0x11,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x11,	0x11,	0x11,	0x11,	0x00,
	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,	0x11,
};
//    0      1      2      3      4      5      6      7      8      9      a      b      c      d      e      f



//	Attention last byte in inBuffer, will be changed to 0 !!!
long CommandLineParaTran (char* inBuffer, long length, char* seprChar, char* preChar, 
						  KeyParaTran* keyFormat, ValParaTran* valFormat, unsigned char* charMap)
{
	long ret_err = 0x01;
	CListItem*	tempContext = 0;
	unsigned char* isChar;

	char* spointer;
	char* lastpointer;
	char  oriValue;
	char* tpointer;
	char* tendpointer;
	char* vpointer;
	long* spint;
	long* tpint;
	KeyParaTran* oneKey;
	ValParaTran* oneVal;
	char haveRead[MAX_PARAMETER+4];

	long keynumber, keylength, keydiv;
	__int64 valSol;
	long	valSingle;

	memset(haveRead, 0, MAX_PARAMETER);
	while (TRUE)
	{
		if (charMap) isChar = charMap;
		else
		{
			ret_err = 0x10;
			if (!TempUseMemory) break;
			if (TempUseMemory->BufferSize < ASCII_NUMBER+8) break;

			ret_err = 0x20;
			tempContext = TempUseMemory->GetContext();
			if (!tempContext) break;
			isChar =  (unsigned char*)(tempContext+1);

			memset(isChar, 0, ASCII_NUMBER);
			spointer = seprChar;
			while (*spointer) isChar[(unsigned char)(*spointer++)] |= IS_END;

			spointer = preChar;
			while (*spointer) isChar[(unsigned char)(*spointer++)] |= IS_PRE;

			oneKey = keyFormat;
			while (oneKey->pKeyword) isChar[(unsigned char)(*(oneKey++->pKeyword))] |= IS_KEY;
			isChar[0] |= IS_KEY;
		}

		ret_err = 0x30;
		spointer = inBuffer;
		lastpointer = spointer + length;
		oriValue = *lastpointer;
		*lastpointer = 0;

		while (spointer<lastpointer)							//	Main loop, char in inBuffer
		{
			while ( !(isChar[(unsigned char)(*spointer)] & IS_KEY) ) spointer++;//	Find the key start
			vpointer = NULL;
			oneKey = keyFormat;									//	oneKey for loop the key
			keynumber = 0;										//	keynumber for the order of key
			while (oneKey->pKeyword)							//	To found which key
			{
				spint = (long*)spointer;						//	begin of source
				tpint = (long*)oneKey->pKeyword;				//	begin of key
				keylength = 0;									//	record the length, how long the key have compared
				keydiv = oneKey->pKeywordLength%4;				//	get the length mod 4
				while ( keylength < (oneKey->pKeywordLength)>>2 )
				{
					if (*spint++ != *tpint++)					//	compare 4 byte
					{
						keylength = -1;							//	not this key
						break;
					}
					keylength++;								//	continue compare
				}

				if (keylength != -1)							//	not this key, try next key
				{
					if ( ( (*spint)&BYTE_MASK[keydiv] ) == ( (*tpint)&BYTE_MASK[keydiv] ) )
					{											//	compare the rest of 4 byte
						vpointer = ((char*)spint)+keydiv;		//	vpointer for the value begin

					while (isChar[(unsigned char)(*vpointer)] & IS_PRE) vpointer++;

						tpointer = (char*)((valFormat+keynumber)->pValue);	//	tpointer for the desc begin
						tendpointer = tpointer+*((valFormat+keynumber)->pValueLength);	// tendpointer for desc end
						haveRead[keynumber] = 1;

						switch ( *(oneKey->pFormat) )
						{
						case 'p' :								//	only record the place and size
							*((char**)tpointer) = vpointer;		//	record the place
							*((valFormat+keynumber)->pValueLength) = 0;
							while ( !(isChar[(unsigned char)(*vpointer)] & IS_END) )
							{
// 								if (*vpointer=='\\') vpointer++;	// add for translate char	// add in May 30 '13
								*((valFormat+keynumber)->pValueLength)++;
								vpointer++;
							}									//	record the length of value
// 							*vpointer++ = 0;
							break;
						case 's' :								//	string, copy from source
							while (tpointer < tendpointer)		//	not memory error
							{
								if (*vpointer=='\\') 
								{
									vpointer++;	// add for translate char	// add in May 29 '13
// 									*tpointer++ = *vpointer++;				// add in Jun 02 '13
									*tpointer++ = TRAN_ASCII[*(unsigned char*)vpointer];	// changed in Jul 27 '13
									vpointer++;
								}
								else if ( !(isChar[(unsigned char)(*vpointer)] & IS_END) ) *tpointer++ = *vpointer++;
								else break;
							}
							*tpointer = 0;						//	end of the desc string
							*((valFormat+keynumber)->pValueLength) = tpointer - (char*)((valFormat+keynumber)->pValue);
							break;								//	record the size of desc
						case 'S' :								//	string, do % translate
							while (tpointer < tendpointer)
							{
								if (*vpointer=='\\') 
								{
									vpointer++;	// add for translate char	// add in May 29 '13
// 									*tpointer++ = *vpointer++;				// add in Jun 02 '13
									*tpointer++ = TRAN_ASCII[*(unsigned char*)vpointer];	// changed in Jul 27 '13
// here to translate \r \n, I found this for 3 days, // Sept. 30 '13
									vpointer++;
								}
								if (*vpointer == '%') 
								{
									if ( (ASCII_HEX[*(vpointer+1)] | ASCII_HEX[*(vpointer+2)]) == 0xff) *tpointer++ = *vpointer++;
									else
									{
										*tpointer++ = (char)((ASCII_HEX[*(vpointer+1)]<<4) + ASCII_HEX[*(vpointer+2)]);
										vpointer += 3;
									}
								}
								else if ( !(isChar[(unsigned char)(*vpointer)] & IS_END) ) *tpointer++ = *vpointer++;
								else break;
							}
							*tpointer = 0;
							*((valFormat+keynumber)->pValueLength) = tpointer - (char*)((valFormat+keynumber)->pValue);
							break;								//	record the size of desc
						case 'd' :								//	number
							valSol = 0;
							while ( !(isChar[(unsigned char)(*vpointer)] & IS_END) )
							{
								valSingle = ASCII_DEC[*vpointer++];
								if (valSingle == 0xff) break;
								valSol = valSol*10+valSingle;
							}
							memcpy(tpointer, &valSol, min(sizeof(__int64), *((valFormat+keynumber)->pValueLength)));
// 							*((valFormat+keynumber)->pValueLength) = 0;
							break;
						case 'x' :								//	hex
							valSol = 0;
							while ( !(isChar[(unsigned char)(*vpointer)] & IS_END) )
							{
								valSingle = ASCII_HEX[*vpointer++];
								if (valSingle == 0xff) break;
								valSol = (valSol<<4)+valSingle;
							}
							memcpy(tpointer, &valSol, min(sizeof(__int64), *((valFormat+keynumber)->pValueLength)));
// 							*((valFormat+keynumber)->pValueLength) = 0;
							break;
						case 'm' :								//	money type
							break;
						case 'i' :								//	ip address
							break;
						}
						oneKey = (KeyParaTran*)lastpointer;			// this means oneKey->pKeyword == 0
						break;
					}
				}
				oneKey++;
				keynumber++;
			}
			spointer = vpointer ? vpointer+1 : spointer+1;
		}

		oneKey = keyFormat;									//	oneKey for loop the key
		oneVal = valFormat;
		char* isRead = haveRead;
		while (oneKey->pKeyword)							//	To found which key
		{
			if (!(*isRead++)) *(oneVal->pValueLength) = VALUE_NOT_FOUND;
			oneKey++;
			oneVal++;
		}

		if (!charMap && tempContext) TempUseMemory->FreeContext(tempContext);
		*lastpointer = oriValue;
		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_PROTOCOL, MESSAGE_ERROR_L2, "Error in CommandLineParaTran, 0x%x\r\n", ret_err);						//
	}																																//
	return ret_err;																													//
}																																	//

char* memstr(char* sourstr, long size, char* substr, long subsize)																	//
{																																	//
	char *nowstart, *endplace;																										//
	long first, mask;
	
//	if (!size) return 0;										//	add Nov. 26 '14
	if (!size || !sourstr || !substr || !subsize) return 0;	//	change to this Dec. 11 '14
	endplace = sourstr + size - subsize;																							//
	first = *((long*)substr);																										//
	if (subsize <= 0) return NULL;
	if (subsize <= 4)
	{
		mask = BYTE_MASK[subsize];
		for (nowstart=sourstr; nowstart<=endplace; nowstart++)																		//
			if ( ( (*((long*)nowstart) ^ first ) & mask ) == 0 ) return nowstart;
	}
	else
	{
		for (nowstart=sourstr; nowstart<=endplace; nowstart++)																		//
			if (*((long*)nowstart) == first && !memcmp(nowstart, substr, subsize)) return nowstart;								//
	}
																																	//
	return NULL;																													//
}																																	//

char* memstr_no(char* sourstr, long size, char* substr, long subsize, char nochar)													//
{
	char* place = sourstr;

	do 
	{
		place = memstr(place, size-(place-sourstr), substr, subsize);
		if (!place) return 0;
		if (*place != nochar && *(place+subsize) != nochar) return place;
		place += subsize;
	} while (place < sourstr+size);

	return 0;
}

char* memstrs(char* sourstr, long size, char* substr, long subsize)
{
// 	char* *endsub, *nowsour, *nowsub, *nextsub;
// 
// 	endsub = substr + subsize;
// 	nowsub = substr;
// 	while (nowsub && nowsub < endsub)
// 	{
// 		nextsub = mem
// 	}
// 	// should do later, for substr like "abc%sdef%s123"
	return NULL;
}

long Myitoa(char ivalue, char* &buffer)
{
	char temp[SMALL_CHAR];
	char *bufferend;
	long length = 0;

	if (ivalue < 0)
	{
		ivalue = -1 * ivalue;
		*buffer++ = '-';
	}
	if (ivalue == 0) *buffer++ = '0';
	for (bufferend = &temp[SMALL_CHAR]; ivalue;)
	{
		bufferend--;
		length++;
		*bufferend = ivalue % 10 + 0x30;
		ivalue = ivalue / 10;
	}
	memcpy(buffer, bufferend, length);
	buffer += length;
	return 0;
}

long Myitoa(long ivalue, char* &buffer)
{
	char temp[SMALL_CHAR];
	char *bufferend;
	long length = 0;

	if (ivalue < 0)
	{
		ivalue = -1 * ivalue;
		*buffer++ = '-';
	}
	if (ivalue == 0) *buffer++ = '0';
	for (bufferend = &temp[SMALL_CHAR]; ivalue;)
	{
		bufferend--;
		length++;
		*bufferend = ivalue % 10 + 0x30;
		ivalue = ivalue / 10;
	}
	memcpy(buffer, bufferend, length);
	buffer += length;
	return 0;
}

long Myitoa(__int64 ivalue, char* &buffer)
{
	char temp[SMALL_CHAR];
	char *bufferend;
	long length = 0;

	if (ivalue < 0)
	{
		ivalue = -1 * ivalue;
		*buffer++ = '-';
	}
	if (ivalue == 0) *buffer++ = '0';
	for (bufferend = &temp[SMALL_CHAR]; ivalue;)
	{
		bufferend--;
		length++;
		*bufferend = ivalue % 10 + 0x30;
		ivalue = ivalue / 10;
	}
	memcpy(buffer, bufferend, length);
	buffer += length;
	return 0;
}

long Myitoa(long ivalue, char* buffer, long length)
{
	char* bufferend;
// 	unsigned char onebit;
	for (bufferend = buffer + length; bufferend--, length--;)// length)
	{
		*bufferend = ivalue % 10 + 0x30;
		ivalue = ivalue / 10;
// 		*bufferend = ASCII_DEC[onebit];
	}
	return 0;
}

long Myitoa(__int64 ivalue, char* buffer, long length)
{
	char* bufferend;

	for (bufferend = buffer + length; bufferend--, length--;)
	{
		*bufferend = ivalue % 10 + 0x30;
		ivalue = ivalue / 10;
	}
	return 0;
}

long MyString(char* source, int len, char* &buffer)
{
	memcpy(buffer, source, len);
	buffer += len;
	return 0;
}

char ToHex[] = "0123456789ABCDEF    ";

long Myitoh(unsigned char ivalue, char* &buffer)
{
	*buffer++ = ToHex[ivalue >> 4];
	*buffer++ = ToHex[ivalue & 0xf];
	return 0;
}

long Myitoh(WORD ivalue, char* &buffer)
{
	*buffer++ = ToHex[ivalue >> 12];
	*buffer++ = ToHex[(ivalue >> 8) & 0xf];
	*buffer++ = ToHex[(ivalue >> 4) & 0xf];
	*buffer++ = ToHex[ivalue & 0xf];
	return 0;
}

long Myitoh(unsigned long ivalue, char* &buffer)
{
	char temp[SMALL_CHAR];
	char *bufferend;
	long length = 0;

	for (bufferend = &temp[SMALL_CHAR]; ivalue;)
	{
		bufferend--;
		length++;
		*bufferend = ToHex[ivalue & 0xf];
		ivalue = ivalue >> 4;
	}
	memcpy(buffer, bufferend, length);
	buffer += length;
	return 0;
}

long Myitoh(unsigned __int64 ivalue, char* &buffer)
{
	char temp[SMALL_CHAR];
	char *bufferend;
	long length = 0;

	for (bufferend = &temp[SMALL_CHAR]; ivalue;)
	{
		bufferend--;
		length++;
		*bufferend = ToHex[ivalue & 0xf];
		ivalue = ivalue >> 4;
	}
	memcpy(buffer, bufferend, length);
	buffer += length;
	return 0;
}

//	output should be network order
long Mymtoh(char* mac, char* &buffer)				//	for MAC to HEX
{
//	This is a Intel order
// 	for (int i=0; i<6; i++)
// 	{
// 		*buffer++ = ToHex[(*mac >> 4) & 0xf];
// 		*buffer++ = ToHex[*mac & 0xf];
// 		mac++;
// 	}

//	This is network order
	char* macend = mac+5;
	for (int i=0; i<6; i++)
	{
		*buffer++ = ToHex[(*macend >> 4) & 0xf];
		*buffer++ = ToHex[*macend & 0xf];
		macend--;
	}
	return 0;
}

long Myntoa(char* ipaddr, char* &buffer)
{
	long i, tran;
	for (i=0; i<4; i++)
	{
		tran = (long)(*ipaddr) & 0xff;
		Myitoa((long)tran, buffer);
		buffer = AddString(buffer, NASZ("."));
		ipaddr++;
	}
	buffer--;
	return 0;
}

void memcpy_up(char* desc, char* sour, long size)
{
	for (int i=0; i<size; i++)
	{
		if (*sour>='a' && *sour<='z') *desc++ = (*sour++) & 0xdf;
		else *desc++ = *sour++;
	}
}

void memcpy_u(char* desc, char* sour, long size)
{
	for (int i=0; i<size; i++)
	{
		*desc++ = *sour++;
		*desc++ = 0;
	}
}

void memcpy_u_up(char* desc, char* sour, long size)
{
	for (int i=0; i<size; i++)
	{
		if (*sour>='a' && *sour<='z') *desc++ = (*sour++) & 0xdf;
		else *desc++ = *sour++;
		*desc++ = 0;
	}
}

char* FindString(char* inBuffer, long inLength, char* keyHead, long keyLength, char* outBuffer, long outLength, char sprChar, char aheadChar)
{
	char* inStart;
	char* outStart = outBuffer;
	char* inEnd = (char*)-1;
	char* outEnd = (char*)-1;
	char svalue;

	if (inLength) inEnd = inBuffer + inLength;
	if (outLength) outEnd = outBuffer + outLength;

	inStart = memstr(inBuffer, inLength, keyHead, keyLength );
	if (!inStart) return (char*)VALUE_NOT_FOUND;

	svalue = *inStart++;
	while (inStart<inEnd && outStart<outEnd && svalue!=sprChar)
	{
		*outStart++ = svalue;
		svalue = *inStart++;
	}
	return inStart;
}

char* FindInt(char* inBuffer, long inLength, char* keyHead, long keyLength, long& lvalue)
{
	unsigned char* inStart;
	unsigned char* inEnd = (unsigned char*)-1;
	char svalue;
	lvalue = 0;

	if (inLength) inEnd = (	unsigned char*)inBuffer + inLength;

	inStart = (unsigned char*)memstr(inBuffer, inLength, keyHead, keyLength );
	if (!inStart) return (char*)VALUE_NOT_FOUND;

	inStart += keyLength;
	svalue = ASCII_DEC[*inStart++];
	while (inStart<=inEnd && svalue!=-1)		// <= or < , testuse, shouldtest
	{
		lvalue = lvalue*10+svalue;
		svalue = ASCII_DEC[*inStart++];
	}
	return (char*)inStart;
}

long GetInt(char* inBuffer, long inLength)
{
	unsigned char* inStart =(unsigned char*)inBuffer;
	long lvalue = 0;
	char svalue = 0;
	unsigned char* inEnd = inStart + inLength;

	svalue = ASCII_DEC[*inStart++];
	if (svalue==-1) return VALUE_NOT_FOUND;			//	add Dec. 19 '14

	while (inStart<=inEnd && svalue!=-1)
	{
		lvalue = lvalue*10+svalue;
		svalue = ASCII_DEC[*inStart++];
	}
	return lvalue;
}

unsigned __int64 GetHex(char* &buffer)
{
	unsigned __int64 value = 0;
	if (*buffer == '\r') return -1;
	while (*buffer != '\r') value = (value<<4)+(ASCII_HEX[*buffer++]);
	buffer += 2;

	return value;
}

unsigned __int64 GetHex(char* buffer, long len)
{
	unsigned __int64 value = 0;
	for (int i=0; i<len; i++) value = (value<<4)+(ASCII_HEX[*buffer++]);
	return value;
}

long MyGetInfoAddr(char* ipstring, long strsize, addrinfo* &ipaddr)
{
	static char hostport[] = "80";
// 	char* processstring = ipstring;
// 	char	processstring[NORMAL_CHAR];
	char *isPort = hostport, *isSem, *isDiv;
	struct addrinfo aiHints;
	long ret_err = 0x01;

	__TRY

	ret_err = 0x10;
	if (strsize > NORMAL_CHAR-1) break;
// 	memcpy(processstring, ipstring, strsize);
	isSem = memstr(ipstring, strsize, ":", 1);
	isDiv = ipstring + strsize;
	if (*(isDiv-1) == '/' ) isDiv--;
	if (isSem) isPort = isSem + 1;
	else isSem = isDiv;
	*isDiv = *isSem = 0;

	ret_err = 0x20;
	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	if ( getaddrinfo(ipstring, isPort, &aiHints, &ipaddr) ) break;

	__CATCH(MODULE_APPLICATION, "MyGetInfoAddr")
}

long ReplaceHttpRequest(CListItem* mBuffer, char* key, char* replace, long repsize)
{
#define		HOSTLEN			8			//	sizeof "\r\nHost: "
#define		CRCRLEN			4			//	sizeof "\r\n\r\n"

	char* tpointer = REAL_BUFFER(mBuffer), *sEnd, *sHost, *sCRend;
	long* lpointer = (long*)tpointer, copysize, copyoffset;

	if ((*lpointer != ISGET) && (*lpointer != ISPOST)) return 0;				//	not http request;
	sEnd = memstr(tpointer, mBuffer->NProcessSize, NASZ("\r\n\r\n"));
	if (!sEnd) return 0;

	sHost = memstr(tpointer, sEnd-tpointer, NASZ("\r\nHost: "));
	if (!sHost) return 0;

	sCRend = memstr(sHost+HOSTLEN, sEnd-sHost/*should -6*/, NASZ("\r\n"));		//	must found, at last is \r\n\r\n
	copysize = sEnd - sCRend + CRCRLEN;
	copyoffset = sCRend - sHost - HOSTLEN -repsize;
	memmove(sCRend-copyoffset, sCRend, copysize);
	memcpy(sHost+HOSTLEN, replace, repsize);
	mBuffer->NProcessSize -= copyoffset;

	return 0;
}


// extern WORD Unicode_GB[];
// extern WORD GB_Unicode[];

extern WORD Unicode_GBK[];
extern WORD GBK_Unicode[];

unsigned char* UTF8toGB2312(unsigned char* utf8, long usize, unsigned char* gb2312, long &gsize)
{
	unsigned char *ustart, *uend;
	unsigned char *gstart, *gend;
	unsigned char uchar;
	WORD	 oneword;

	if (!utf8 || !usize || !gb2312) return 0;

	ustart = utf8;
	gstart = gb2312;
	uend = gend = (unsigned char*)-1;

	if (usize) uend = utf8+usize;//		-3; should
	if (gsize) gend = gb2312+gsize;//	-2; should

	uchar = *ustart;
	while (ustart<uend && gstart<gend && uchar)
	{
		if ( (uchar & 0xE0) == 0xE0 )
		{
			oneword = (((WORD)(uchar&0xf))<<12) + (((WORD)(*(ustart+1))&0x3f)<<6) + (((WORD)*(ustart+2))&0x3f);
			(*(WORD*)gstart) = Unicode_GBK[oneword];
			ustart += 3;
			gstart += 2;
		}
		else if ( (uchar & 0xC0) == 0xC0 )
		{
			oneword = (((WORD)(uchar&0x1f))<<6) + (((WORD)*(ustart+1))&0x3f);
			(*(WORD*)gstart) = Unicode_GBK[oneword];
			ustart += 2;
			gstart += 2;
		}
		else
		{
			*gstart++ = uchar;
			ustart++;
		}
		uchar = *ustart;
	}

	if (!uchar) *gstart = 0;
	usize = ustart-utf8;
	gsize = gstart-gb2312;
	return gstart;
}

unsigned char* GB2312toUTF8(unsigned char* gb2312, long gsize, unsigned char* utf8, long &usize)
{
	unsigned char *ustart, *uend;
	unsigned char *gstart, *gend;
	unsigned char gchar;
	WORD	oneword;

	if (!gb2312 || !gsize || !utf8) return 0;

	ustart = utf8;
	gstart = gb2312;
	uend = gend = (unsigned char*)-1;

	if (usize) uend = utf8+usize;//		-3; should
	if (gsize) gend = gb2312+gsize;//	-2; should

	gchar = *gstart;
	while (ustart<uend && gstart<gend && gchar)
	{
		if ( gchar > 0x80 )
		{
			oneword = *((WORD*)gstart);
			oneword = GBK_Unicode[oneword];
			if (oneword != 0x2020)
			{
				*ustart++ = 0xE0 | ((char)(oneword >> 12));
				oneword &= 0x0FFF;
//	Here should be 0x80, I used 0xC0. it is wrong, but it display OK in IE
				*ustart++ = 0x80 | ((char)(oneword >> 6));
				oneword &= 0x003F;
				*ustart++ = 0x80 | ((char)oneword);

				gstart += 2;
			}
			else
			{
				*ustart++ = gchar;
				gstart++;
			}
		}
		else
		{
			*ustart++ = gchar;
			gstart++;
		}
		gchar = *gstart;
	}

	if (!gchar) *ustart = 0;
	usize = ustart-utf8;
	gsize = gstart-gb2312;
	return ustart;
}

unsigned char* TranEscape(unsigned char* gb2312, long gsize, unsigned char* unicode, long &usize, long stupid)
{
	unsigned char *gstart = gb2312, *gend = gstart + gsize;
	unsigned char *ustart = unicode;
	unsigned char gchar;
	WORD	oneword;

	if (!gb2312 || !gsize || !unicode) return 0;

	gchar = *gstart;
	while (gstart<gend && gchar)
	{
		if ( gchar > 0x80 )
		{
			oneword = *((WORD*)gstart);
			if (stupid) oneword = GBK_Unicode[oneword];			// for citic upload file control, post is unicode
			*ustart++ = '%';
			*ustart++ = 'u';
			Myitoh(oneword, (char*&)ustart);
			gstart += 2;
		}
		else
		{
			if (TRAN_POST_NEW[gchar])		//	may change to TRAN_POST_NEW
			{
				*ustart++ = '%';
				Myitoh(gchar, (char*&)ustart);			//	network order HEX format
				gstart++;
			}
			else *ustart++ = *gstart++;
		}
		gchar = *gstart;
	}
	if (!gchar) *ustart = 0;
	usize = ustart - unicode;
	return ustart;
}

#define			IS_INPUT			('I'+('N'<<8)+('P'<<16)+('U'<<24))
#define			IS_TABLE			('T'+('A'<<8)+('B'<<16)+('L'<<24))
#define			IS_HERF				('A')
#define			KEY_ORDER			16
#define			VAL_ORDER			18

#define			INPUT_BEGIN			" value=\""
#define			INPUT_END			"\">"
PUCHAR FormatFormCITIC(PUCHAR formStart, long formLen, char* valStart, char* valEnd, PUCHAR mStart, long &nsize, CListItem *tempbuffer, long innest)
{
	PUCHAR keystart, keyend, keynow, resultstart;
	PUCHAR inputend, tableend, hrefend, hrefEND, loopstart, loopend;
	PULONG keycomp;
	ContentPad *temppad = tempbuffer->HeadInfo;
	char *nowvalstart, *nowvalend;
	long loopsize;
	long ret;
	long shouldadd;

	if (!formStart || !formLen || !valStart || !valEnd || !mStart || !tempbuffer) return 0;

	keystart = keynow = formStart;
	keyend = formStart + formLen;
	resultstart = mStart;
	
	nowvalstart = valStart;
	nowvalend = valEnd;

	for (keynow = keystart; keynow < keyend; keynow++)
	{
		if (*keynow == '<')
		{
			keycomp = (PULONG)(keynow + 1);
			if (((*keycomp) & ~0x20202020) == IS_INPUT )
			{
				inputend = FormatInputCITIC(keynow, keyend, resultstart, shouldadd);
				if (shouldadd)
				{
					if (!inputend) break;
					Wafa_Var(NULL, tempbuffer, (char*)keynow, (char*)inputend, NASZ("dm_name"), KEY_ORDER, NULL);
					if (temppad->resultKey[KEY_ORDER] == NULL)
					{
						*resultstart++ = *keynow;
						continue;
					}

					ret = GetCITICValue((PUCHAR)temppad->resultKey[KEY_ORDER], (PUCHAR)temppad->resultKey[KEY_ORDER] + temppad->getLength[KEY_ORDER], 
						nowvalstart, nowvalend, temppad->resultKey[VAL_ORDER], temppad->getLength[VAL_ORDER]);
					resultstart = AddString(resultstart, NASZ(INPUT_BEGIN));
					if (ret != VALUE_NOT_FOUND)
						resultstart = AddString(resultstart, temppad->resultKey[VAL_ORDER], temppad->getLength[VAL_ORDER]);
					else
						resultstart = AddString(resultstart, NASZ("NOT FOUND VALUE"));
					resultstart = AddString(resultstart, NASZ(INPUT_END));

				}

				keynow = inputend - 1;
			}
			else if (((*keycomp) & ~0x20202020) == IS_TABLE )
			{
				tableend = (PUCHAR)memchr(keynow, '>', keyend - keynow);
				if (!tableend) break;
				Wafa_Var(NULL, tempbuffer, (char*)keynow, (char*)tableend, NASZ("group_name="), KEY_ORDER, NULL);
				if ((temppad->resultKey[KEY_ORDER] == NULL) || innest)
				{
					*resultstart++ = *keynow;
					continue;
				}
				else
				{
					tableend = (PUCHAR)memstr((char*)keynow, keyend - keynow, NASZ("</TABLE>"));
					if (!tableend) break;
					tableend += (sizeof("</TABLE>") - 1);

					loopstart = (PUCHAR)memstr((char*)keynow, tableend - keynow, NASZ("<TR"));
					if (!loopstart) break;
					loopstart += 4;
					loopstart = (PUCHAR)memstr((char*)loopstart, tableend - loopstart, NASZ("<TR"));
					if (!loopstart) break;
					loopend = (PUCHAR)memstr((char*)loopstart, tableend - loopstart, NASZ("</TR>"));
					if (!loopend) break;
					loopend += (sizeof("</TD>") - 1);

					char *subkeystart, *subkeyend, *nowstart, *nowend, *groupend;
					subkeystart = temppad->resultKey[KEY_ORDER];
					subkeyend = temppad->resultKey[KEY_ORDER] + temppad->getLength[KEY_ORDER];

					loopsize = 0;
					FormatFormCITIC(keynow, loopstart - keynow, valStart, valEnd, resultstart, loopsize, tempbuffer, 1);	// in nest
					resultstart += loopsize;

					nowstart = 0;
					while (!GetCITICGroup((PUCHAR)subkeystart, (PUCHAR)subkeyend, valStart, valEnd, nowstart, nowend, groupend))
					{
						loopsize = 0;
						FormatFormCITIC(loopstart, loopend - loopstart, nowstart, nowend, resultstart, loopsize, tempbuffer, 1);// in nest
						resultstart += loopsize;
					}
					loopsize = 0;
					FormatFormCITIC(loopend, tableend - loopend, valStart, valEnd, resultstart, loopsize, tempbuffer, 1);	// in nest
					resultstart += loopsize;
					keynow = tableend - 1;
					continue;
				}
			}
			else if (((*keycomp) & ~0xffff2020) == IS_HERF)
			{
				hrefend = (PUCHAR)memstr((char*)keynow, keyend - keynow, NASZ("</a>"));
				hrefEND = (PUCHAR)memstr((char*)keynow, keyend - keynow, NASZ("</A>"));
				if (!hrefend) hrefend = keyend;
				if (!hrefEND) hrefEND = keyend;
				keynow = min(hrefend, hrefEND);
				if (keynow == keyend) break;
				keynow += (sizeof("</a>") -2);
				continue;
			}
			else *resultstart++ = *keynow;
		}
		else *resultstart++ = *keynow;

	}

	nsize = resultstart - mStart;


// 	memcpy(mStart, formStart, formLen);
// 	nsize = formLen;
	return mStart;
}

#define FULL_BLANK(key)														\
	search = (PUCHAR)memstr((char*)resultStart, keylen, NASZ(key));			\
	if (search)																\
	{																		\
		searchend = search + sizeof(key) - 1;								\
		while (*searchend != ' ' && searchend < resultend) searchend++;	\
		memset(search, ' ', searchend-search);								\
	}

PUCHAR FormatInputCITIC(PUCHAR inputStart, PUCHAR end, PUCHAR &resultStart, long &shouldadd)	//	return endof input place of '>', but do NOT copy / and >
{
	if (!inputStart || !end || !resultStart) return 0;
// 	PUCHAR &result = (PUCHAR)&resultStart;
	PUCHAR keyend;//, anotherend;;
	long keylen;
	PUCHAR search, searchend, resultend;

	// remove value=, disable
	shouldadd = 0;

	keyend = (PUCHAR)memchr(inputStart, '>', end - inputStart);
	if (!keyend) return 0;
	keylen = keyend - inputStart;
// 	anotherend = (PUCHAR)memchr(inputStart, '/', keyend - inputStart);
// 	if (anotherend) keyend = anotherend;
	
	if (((PUCHAR)memstr((char*)inputStart, keylen, NASZ("dm_displaytype=\"hidden\""))) ||
		(PUCHAR)memstr((char*)inputStart, keylen, NASZ("type=\"hidden\"")))
	{
		shouldadd = 0;
		return keyend + 1;
	}

	resultend = resultStart + keylen;

	memcpy(resultStart, inputStart, keylen);
	search = (PUCHAR)memstr((char*)resultStart, keylen, NASZ(" disabled "));
	if (search) memcpy(search, NASZ(" readonly "));

	FULL_BLANK(" value=");
	FULL_BLANK(" onclick=");

// 	search = (PUCHAR)memstr((char*)resultStart, keylen, NASZ(" value="));
// 	if (search)
// 	{
// 		searchend = search + sizeof(" value=") - 1;
// 		while (*searchend != ' ' && searchend < resultend) searchend++;
// 		memset(search, ' ', searchend-search);
// 	}


	if ((PUCHAR)memstr((char*)resultStart, keylen, NASZ(" dm_displaytype=\"number\" ")) ||
		(PUCHAR)memstr((char*)resultStart, keylen, NASZ(" class=\"num\" ")))
		resultend = AddString(resultend, NASZ(" style=\"TEXT-ALIGN: right; border:none\""));
	else
		resultend = AddString(resultend, NASZ(" style=\"border:none\""));
	shouldadd = 1;
	resultStart = resultend;
	return keyend + 1;
}

PUCHAR Bmemchr(PUCHAR charend, UCHAR search, PUCHAR charstart)
{
	while (charend >= charstart) 
	{
		charend --;
		if (*charend == search) return charend;
	}
	return 0;
}
#define		PATHMARK			'/'

PUCHAR GetFilePath(PUCHAR fileStart, long fileLen, PUCHAR mStart, long &nsize)
{
	if (!fileStart || !fileLen || !mStart) return 0;
	PUCHAR pathend = Bmemchr(fileStart + fileLen, PATHMARK, fileStart);
	if (pathend)
	{
		pathend ++;
		nsize = pathend - fileStart;
		memcpy (mStart, fileStart, nsize);
	}
	else nsize = 0;
	return 0;
}


PUCHAR TranslatePath(PUCHAR pathStart, long pathLen, char* fileStart, char* fileEnd, PUCHAR mStart, long &nsize)
{
	PUCHAR pathend, pathnow, resultstart;

	if (!pathStart || !pathLen || !fileStart || !fileEnd || !mStart) return 0;
	if (*fileStart == PATHMARK)
	{
		memcpy(mStart, fileStart, fileEnd - fileStart);
		nsize = fileEnd - fileStart;
		return 0;
	}
	resultstart = mStart;
	pathend = pathStart + pathLen;
	if (*(pathend - 1) == PATHMARK) pathend --;
	pathnow = pathend;
	while (TRUE)
	{
		if ((*fileStart=='.') && (*(fileStart+1)=='.'))
		{
			if  ( (*(fileStart+2) != PATHMARK) || fileStart+3 >= fileEnd) return 0;
			fileStart += 3;
			pathend = Bmemchr(pathend, PATHMARK, pathStart);
			if (!pathend) return 0;
			continue;
		}
		if (*fileStart=='.')
		{
			if  ( (*(fileStart+1) != PATHMARK) || fileStart+2 >= fileEnd) return 0;
			fileStart += 2;
			continue;
		}
		pathLen = pathend-pathStart;
		memcpy(resultstart, pathStart, pathLen);
		resultstart += pathLen;
		*resultstart = PATHMARK;
		resultstart ++;
		pathLen = fileEnd - fileStart;
		memcpy(resultstart, fileStart, pathLen);
		resultstart += pathLen;
		break;
	}

	nsize = resultstart - mStart;
	return mStart;
}

static const unsigned char pr2six[256] =
{
	/* ASCII table */
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

unsigned char* Base64Decode(unsigned char* base64str, long bsize, unsigned char* normalstr, long &nsize)
{
	unsigned char *bufin, *bufout;
	long remainbyte = bsize;

	if ( *(base64str + remainbyte - 1) == '=' ) remainbyte--;
	if ( *(base64str + remainbyte - 1) == '=' ) remainbyte--;
	if ( *(base64str + remainbyte - 1) == '=' ) remainbyte--;

	nsize = ((bsize + 3) / 4) * 3;
	bufin = base64str;
	bufout = normalstr;

	while (remainbyte > 4)
	{
		*(bufout++) =
			(unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
		*(bufout++) =
			(unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
		*(bufout++) =
			(unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
		bufin += 4;
		remainbyte -= 4;
	}
	if (remainbyte > 1) {
		*(bufout++) =
			(unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
	}
	if (remainbyte > 2) {
		*(bufout++) =
			(unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
	}
	if (remainbyte > 3) {
		*(bufout++) =
			(unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
	}
	nsize -= (4 - remainbyte) & 3;
	return 0;
}

static const char basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char* Base64Encode(unsigned char* normalstr, long nsize, unsigned char* base64str, long &bsize)
{
	long i;
	unsigned char *p;

	p = base64str;
	for (i = 0; i < nsize - 2; i += 3) {
		*p++ = basis_64[(normalstr[i] >> 2) & 0x3F];
		*p++ = basis_64[((normalstr[i] & 0x3) << 4) |
			((int) (normalstr[i + 1] & 0xF0) >> 4)];
		*p++ = basis_64[((normalstr[i + 1] & 0xF) << 2) |
			((int) (normalstr[i + 2] & 0xC0) >> 6)];
		*p++ = basis_64[normalstr[i + 2] & 0x3F];
	}
	if (i < nsize) {
		*p++ = basis_64[(normalstr[i] >> 2) & 0x3F];
		if (i == (nsize - 1)) {
			*p++ = basis_64[((normalstr[i] & 0x3) << 4)];
			*p++ = '=';
		}
		else {
			*p++ = basis_64[((normalstr[i] & 0x3) << 4) |
				((int) (normalstr[i + 1] & 0xF0) >> 4)];
			*p++ = basis_64[((normalstr[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}
	bsize = p - base64str;
	return 0;
}

#include <openssl/md5.h>

unsigned char* MD5Encode(unsigned char* normalstr, long nsize, unsigned char* md5str, long &msize)
{
	unsigned char md5buf[20];
	unsigned char* pmd5 = md5buf;

	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, normalstr, nsize);
	MD5_Final(md5buf, &ctx);

	for (int i=0; i<16; i++)
	{
		*md5str++ = ToHex[*pmd5 >> 4];
		*md5str++ = ToHex[*pmd5++ & 0xf];
	}

	msize = 16*2;
	return 0;
}

char* CommandLineParaTran (char* &inStart, long inLength, char* &outStart, long outLength,
						  char* keyPara, long keyLength, KeyParaAddition* keyAddition[] )
{
// 	char* inStart = inBuffer;
	char* inEnd = inStart + inLength;
// 	char* outStart = outBuffer;
	char* outEnd = outStart + outLength;
	char* keyStart = keyPara;
	char* keyEnd = keyStart + keyLength;
	char* keyNext;

	char nowMode = '*';
	int  nowKey = 0;
	KeyParaAddition* nowAddition = keyAddition[0];
	char* nowend;

	while (inStart<inEnd && outStart<outEnd && keyStart<keyEnd)
	{
		if (*keyStart == '%')
		{
			keyStart++;
			nowMode = *(keyStart++);
		}
		keyNext = (char*)memchr(keyStart, '%', keyEnd-keyStart);
		if (!keyNext) keyNext = keyEnd;

		nowend = memstr(inStart, inEnd-inStart, keyStart, keyNext-keyStart);
		if (!nowend) 
			break;
		if (nowMode != '*')
		{
			if ( outStart + nowAddition->headLength + (nowend - inStart) + nowAddition->tailLength > outEnd ) 
				break;
			memcpy(outStart, nowAddition->headChar, nowAddition->headLength);
			outStart += nowAddition->headLength;
			memcpy(outStart, inStart, nowend-inStart);
			outStart += nowend-inStart;
			memcpy(outStart, nowAddition->tailChar, nowAddition->tailLength);
			outStart += nowAddition->tailLength;

			nowAddition = keyAddition[++nowKey];
		}
		inStart = nowend + (keyNext-keyStart);
		keyStart = keyNext;
	}

	if (inStart >= inEnd) return inEnd;
	else if (keyNext >= keyEnd) return inStart;
	else return NULL;
}

#define CHUNKED_HEADER_MAX		8
// 	Input: mContext->MoreBuffer, size is MoreBuffer->NProcessSize and  mBuffer size is size
// 	mContext->BodyRemain means in CONTEXT_MODE_LENGTH, or, mContext->OverlapOffset means the end of body
CListItem* ProcessChunkedCommand( CContextItem* mContext, CListItem* mBuffer, long size )
{
	CListItem* moreBuffer = mContext->MoreBuffer;
	char *sPointer, *vPointer, *lPointer;
	char *bPointer, *sEnd;
	long newLength, bodyLength, newBody;
	long copySize, remainSize = size;

	long ret_err = 0x01;

	sPointer = (char*)(mBuffer+1);
	vPointer = ((char*)(moreBuffer+1))+moreBuffer->NProcessSize;
	bPointer = ((char*)(moreBuffer+1))+mContext->OverlapOffset;
	newLength = (long)mContext->OverlapOffset - moreBuffer->NProcessSize + size;
	sEnd = sPointer + size;

	while (sPointer < sEnd)
	{
		// ATTENTION, if the HEAD divide into three part, It will error happen
		if (mContext->ContentMode == CONTENT_MODE_HEAD)
		{
			*(long*)bPointer = *(long*)sPointer;
			*(long*)(bPointer+4) = *(long*)(sPointer+4);		//	the LENGTH head is less than 8 byte
			*(long*)(bPointer+CHUNKED_HEADER_MAX) = 0x0a0d0a0d;					//	make a end for search

			lPointer = vPointer;
			newBody = (long)GetHex(lPointer) + 2;
			bodyLength = lPointer - vPointer;

			if (bodyLength <= min(CHUNKED_HEADER_MAX, newLength))
			{
				if (newBody > 2)
				{
					// ok find body length
					mContext->BodyRemain = newBody;
					sPointer += bodyLength - (bPointer -vPointer);
					newLength -= bodyLength;
					//vPointer no change
					mContext->ContentMode = CONTENT_MODE_LENGTH;
				}
				else
				{
					mContext->BodyRemain = 0;
					mContext->MoreBuffer = mBuffer;
					mContext->OverlapOffset = 0;
					mBuffer->NProcessSize = 0;
					moreBuffer->NProcessSize = vPointer - (char*)(moreBuffer+1);
					mContext->ContentMode = CONTENT_MODE_HEAD;
					return moreBuffer;
					// finish
				}
			}
			else if (newLength < CHUNKED_HEADER_MAX)
			{
				long smallLength = min(newLength, bodyLength);
				// income too small, wait again, still CONTENT_MODE_HEAD
				sPointer += smallLength;
				mContext->OverlapOffset += smallLength;
				mContext->ContentMode = CONTENT_MODE_HEAD;
			}
			else
			{
				//	format error, not find header begin
				moreBuffer->NProcessSize = 0;
				mContext->BodyRemain = 0;
				mContext->OverlapOffset = 0;
				mBuffer->NProcessSize = BUFFER_TOO_LARGE;
				mContext->ContentMode = CONTENT_MODE_HEAD;
				return mBuffer;
			}
		}

		if (mContext->ContentMode == CONTENT_MODE_LENGTH)
		{
			copySize = min(sEnd-sPointer, (long)mContext->BodyRemain);
			memcpy(vPointer, sPointer, copySize );

			sPointer += copySize;
			vPointer += copySize;
			moreBuffer->NProcessSize += copySize;
			newLength -= copySize;
			if (copySize == mContext->BodyRemain)
			{
				vPointer -= 2;
				moreBuffer->NProcessSize -= 2;
				mContext->ContentMode = CONTENT_MODE_HEAD;
			}
			mContext->BodyRemain -= copySize;
			bPointer = vPointer;
			mContext->OverlapOffset = moreBuffer->NProcessSize;
		}
	}
	mBuffer->NProcessSize = 0;
	return mBuffer;
}

CListItem* ProcessMoreCommand( CContextItem* mContext, CListItem* mBuffer, long size )
{
	CListItem* moreBuffer = mContext->MoreBuffer;
	CContextItem* messpeer;

	if (mContext->MessageContext)
	{
		messpeer = mContext->MessageContext->PPeer;			//	Add	Dec. 30 '14 for CONTEXT_MEMORY_FILE
	}
	else
	{
		moreBuffer->NProcessSize = 0;
		mContext->BodyRemain = 0;
		mContext->OverlapOffset = 0;
		mBuffer->NProcessSize = BUFFER_TOO_LARGE;
		mContext->ContentMode = CONTENT_MODE_HEAD;
		return mBuffer;
	}

	if (messpeer->DyControl & FLAG_MEMORY_FILE)			//	add Dec. 30 '14			//	only for mContext is SERVER side
	{
		if (!messpeer->DyMemoryFile)
		{
			messpeer->DyMemoryFile = new COleStreamFile;
			messpeer->DyMemoryFile->CreateMemoryStream(NULL);
		}

		messpeer->DyMemoryFile->Write(REAL_BUFFER(mBuffer), size);
		moreBuffer->NProcessSize += size;
		mBuffer->NProcessSize = 0;
	}
	else
	{
		if ((long)(moreBuffer->NProcessSize) + size > moreBuffer->BufferType->BufferSize)
		{
	// 		__asm int 3
			moreBuffer->NProcessSize = 0;
			mContext->BodyRemain = 0;
			mContext->OverlapOffset = 0;
			mBuffer->NProcessSize = BUFFER_TOO_LARGE;
			mContext->ContentMode = CONTENT_MODE_HEAD;
			return mBuffer;
		}
		else
		{
			memcpy (REAL_BUFFER(moreBuffer)+moreBuffer->NProcessSize, REAL_BUFFER(mBuffer), size );
			moreBuffer->NProcessSize += size;
			mBuffer->NProcessSize = 0;
		}
	}

	if (mContext->BodyRemain) 
		return mBuffer;
	else
	{
		mContext->MoreBuffer = mBuffer;
		return moreBuffer;
	}
}

void DisplayContextList()
{
	char* mchar = (char*)0x063800a8;
	CContextItem* mContext;
	long appnum, pronum;

	for (int i=0; i<8; i++)
	{
		mContext = (CContextItem*)mchar;
		if (mContext->PApplication) appnum = mContext->PApplication->ApplicationNumber;
		else appnum = 0;
		if (mContext->PProtocol) pronum = mContext->PProtocol->ProtocolNumber;
		else pronum = 0;
		printf("Self:%8x, Peer:%8x, En:%8x, De:%8x, App:%d, Pro:%d\r\n", 
			mContext, mContext->PPeer, mContext->EncapContext, mContext->DecapContext, appnum, pronum);
		mchar += 0x2740;
	}
	printf("\r\n");
}

void ClearCache(char* pathname, long timebefore)		// for FILETIME, hi-DWORD 200 about 1 day 
{
	char filepathname [MAX_PATH];
	long pathlen;
	char* namestart;
	WIN32_FIND_DATA fdata;
	HANDLE handle;
	SYSTEMTIME	nowsystemtime;
	FILETIME	nowfiletime;
	DWORD &hifiletime = nowfiletime.dwHighDateTime;
	
	GetSystemTime(&nowsystemtime);
	SystemTimeToFileTime(&nowsystemtime, &nowfiletime);
	hifiletime -= timebefore;
	strcpy_s(filepathname, MAX_PATH, pathname);
	pathlen = strlen(filepathname);
	namestart = &filepathname[pathlen];
	pathlen = MAX_PATH - pathlen - 2;

	strcpy_s(namestart, pathlen , "*.*");
	handle = FindFirstFile(filepathname, &fdata);
	if (handle == INVALID_HANDLE_VALUE) return;
	do 
	{
		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		if (fdata.ftLastAccessTime.dwHighDateTime < hifiletime)
		{
			strcpy_s(namestart, pathlen, fdata.cFileName);
			DeleteFile(filepathname);
		}
	} 
	while(FindNextFile(handle, &fdata));
}

// Mar. 10 '15\\

#include "atlimage.h"
using namespace Gdiplus;

int ChangeImg(CContextItem* mContext, CListItem* serBuffer, int width)
{
	CImage *sourImage, *descImage;
	COleStreamFile *sourFile, *descFile;
	int newx, newy;
	double changeradio;
	HDC ddc;

	char *tpointer = REAL_BUFFER(serBuffer);
	if (mContext->DyMemoryFile) sourFile = mContext->DyMemoryFile;
	else return 1;

	descFile = new COleStreamFile;
	sourImage = new CImage;
	descImage = new CImage;
	if (!descFile || !sourImage || !descImage)
	{
		mContext->DyMemoryFile = 0;
		sourFile->Close();
		if (sourFile) delete sourFile;
		if (descFile) delete descFile;
		if (sourImage) delete sourImage;
		if (descImage) delete descImage;
		return 1;
	}
	descFile->CreateMemoryStream(NULL);
	sourFile->SeekToBegin();
	sourImage->Load(sourFile->GetStream());
	serBuffer->NProcessSize = (UINT)0;					// add this in Mar. 26 '15

	if (!sourImage->IsNull())
	{
		newx = sourImage->GetWidth();
		newy = sourImage->GetHeight();
		if (newx > 0 && newy > 0)
		{
			changeradio = (double)newx / width;
			newx = width;
			newy = (int)(newy/changeradio);

			descImage->Create(newx, newy, 16);
			ddc = descImage->GetDC();
			::SetStretchBltMode(ddc, HALFTONE);
			sourImage->StretchBlt(ddc,0,0,newx,newy,SRCCOPY);
			descImage->ReleaseDC();

			descImage->Save(descFile->GetStream(), ImageFormatJPEG);
			ULONGLONG fsize = descFile->GetLength();
			descFile->SeekToBegin();

			descFile->Read(tpointer, (UINT)fsize);
			serBuffer->NProcessSize = (UINT)fsize;
		}
	}
	sourImage->Destroy();
	descImage->Destroy();
	delete sourImage;
	delete descImage;
	mContext->DyMemoryFile = 0;
	sourFile->Close();
	descFile->Close();
	delete sourFile;
	delete descFile;
	return 0;
}


#ifdef	QRCODE_FUNCTION

#include <iostream>
#include <Magick++.h>
#include <zbar.h>
// #define STR(s) #s

using namespace zbar;

unsigned char* QRCodeTranslate(unsigned char* pic, long gsize, unsigned char* mStart, long &usize)
{
	unsigned char* oldStart = mStart;
	std::string getcode;

	Magick::Blob somen(pic, gsize);
	Magick::Image sopic(somen);
	Magick::Blob rawmen;              // extract the raw data

	int width = sopic.columns();   // extract dimensions
	int height = sopic.rows();

	if (!width || !height) return 0;

	sopic.modifyImage();
	sopic.write(&rawmen, "GRAY", 8);
	const void *raw = rawmen.data();

 	ImageScanner scanner;
	Image image(width, height, "Y800", raw, width * height);
	// configure the reader
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	// scan the image for barcodes
	int n = scanner.scan(image);

	for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end();	++symbol)
	{
// 		mStart = AddString(mStart, NASZ(" TYPE:"));
// 		getcode = symbol->get_type_name();
// 		mStart = AddString(mStart, (char*)getcode.c_str(), getcode.size());
// 		mStart = AddString(mStart, NASZ(" Code:"));
		getcode = symbol->get_data();
		mStart = AddString(mStart, (char*)getcode.c_str(), getcode.size());
	}

	// clean up
	image.set_data(NULL, 0);
	usize = mStart-oldStart;

	return 0;
}

#endif	QRCODE_FUNCTION
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                