
#pragma once

#define WAFA
// #define REDANT

#define FIDDLER_ADDR					// for sangfor vpn use, must set fiddler proxy address

// #define USE_ARGC
// #define USE_PATH
#define USE_SERVICE
// #define USE_FILE
// #define USE_ARGCARGV

// #define FILETRANSLATE_APPLICATION
// #define TEST_APPLICATION
// #define SIMSERVER_APPLICATION
#define CONTENTPAD_APPLICATION
//	// #define PROXY_APPLICATION
// #define PROXYSERVER_APPLICATION
// #define CONFIRMDNS_APPLICATION
// #define FORWARDLOG_APPLICATION

// #define	NTLM_FUNCTION				//	Windows AD auth
// #define OCI_FUNCTION
// #define FORWARDLOOP_FUNCTION
// #define QRCODE_FUNCTION
// #define	PROXY_FUNCTION
// #define CONFIRMDNS_FUNCTION
// #define WINPCAP_FUNCTION

// #define	DEBUG_BUFFER
// #define	DEBUG_CONTEXT
// #define	DEBUG_POST
// #define	DEBUG_CLOSE
// #define	DEBUG_TCP
#define	DEBUG_WAFA
// #define	DEBUG_CONCURRENCY
// #define	DEBUG_PEER
// #define  DEBUG_NTLM


#if	 (defined CONFIRMDNS_FUNCTION || defined QRCODE_FUNCTION)
#undef	WAFA
#define	REDANT
#endif	 (defined CONFIRMDNS_FUNCTION || defined QRCODE_FUNCTION)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	System info, Yes, should get it from SystemInfo, but it never changed															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define	CACHE_SIZE					64																								//
#define PAGE_SIZE					4096																							//
#define SIZEOFCONTENT				4096						//	without cookie	// 2196:Jun 02 '13
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	All the identifier, such as LIST name, SERVICE name, is limited NAME length														//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define NORMAL_CHAR					128																								//
#define SMALL_CHAR					64
#define MAX_URL						260
#define MAX_COOKIE					8192
#define MAX_PARAMETER				96
#define	POST_PARAMETER_START		32
#define	ASCII_NUMBER				256

#define	MAX_CONTENT_KEY				MAX_PARAMETER
#define SYSTEM_CONTENT_KEY			4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MAX char number of cliCommandLineEnd and serCommandLineEnd																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define COMMAND_END_SIZE			32																								//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The array number, used in CResources, this value can be changed as you wish														//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MAX_RESOURCE				128																								//
#define MAX_IOCP					10																								//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MAX number of thread, a IOCP can be attached. For I use WaitMulti func, so the limit is MAXIMUM_WAIT_OBJECTS which equal 64		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MAX_THREAD					MAXIMUM_WAIT_OBJECTS																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Used for CMultiApplication, the MAX number of PEER PROTOCOL, can be changed														//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MAX_SERVER_PEER				32																								//
#define MAX_PAGE_PEER				128
#define MAX_JSON_NUMBER				1000
#define MAX_JSON_NEST				24
#define	MAX_JSON_NAMEID				8
#define MAX_JSONG_ERROR				100
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For compatible x86 and x64																										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#ifdef _WIN64																														//
#define MYINT						LONGLONG																						//
#define InterExgAdd					InterlockedExchangeAdd64																		//
#define InterCmpExg					InterlockedCompareExchange64																	//
#define InterExg					InterlockedExchange64																			//
#define InterDec					InterlockedDecrement64																			//
#define InterInc					InterlockedIncrement64																			//
#else _WIN64																														//
#define MYINT						LONG																							//
#define InterExgAdd					InterlockedExchangeAdd																			//
#define InterCmpExg					InterlockedCompareExchange																		//
#define InterExg					InterlockedExchange																				//
#define InterDec					InterlockedDecrement																			//
#define InterInc					InterlockedIncrement																			//
#endif _WIN64																														//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For Non-Directly Free MEMORYLIST, the timeout value																				//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define TIMEOUT_INFINITE			(1<<30)																							//
#define TIMEOUT_TCP					60							//	change for Catch, and now ContentPad only active one connect, it is enough	//	Apr. 28 '14
#define	TIMEOUT_TCPCONNECT			5							//	after connect, no respond from server will close soon			//
#define TIMEOUT_UDP					10																								//
#define TIMEOUT_FILE				4																								//
																																	//
#define	TIMEOUT_ACCEPT				8							//	Timeout for NO DATA after ACCEPT for TCP NON-directly free		//
																//////////////////////////////////////////////////////////////////////
#define TIMEOUT_QUIT				2							//	Wait one circle for TIMEOUT_ACCEPT to clear CliBufferNext		//
																																	//
#ifndef _DEBUG																														//
	#if TIMEOUT_FILE > TIMEOUT_TCP																									//
		#error TIMEOUT DEFINE ERROR																									//
	#endif																															//
#endif _DEBUG																														//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MAX number of PROTOCAL the IOCP supported, every PROTOCOL should register a group of functions to process message.				//
//	This is the array number of a Global variable, the size can changed, but it is enough, I think.									//
//	The ID of PROTOCOL is as my wish, and do NOT in any package trans to network. But I choice the number for easy to remember		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MAX_PROTOCOL				128																					  //		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define PROTOCOL_NONE				0																								//
#define PROTOCOL_TCP				6																								//
#define PROTOCOL_TCP_POOL			7
#define PROTOCOL_UDP				17																								//
#define PROTOCOL_SINGLEUDP			18
#define PROTOCOL_FILE				21																								//
#define PROTOCOL_FILE_READ			22																								//
#define	PROTOCOL_DIRECTORY			27
#define	PROTOCOL_DIRECTORY_READ		28
#define PROTOCOL_OCI				31
#define PROTOCOL_WINPCAP			33

#define PROTOCOL_TUNNEL				78
#define PROTOCOL_FILESG				98																								//

#define PROTOCOL_INTER				127

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MAX number of APPLICATION the IOCP supported, every APPLICATION should register a group of functions to process message			//
//	This is the array number of a Global variable, the size can changed, but it is enough, I think.									//
//	The ID of APPLICATION is as my wish, and do NOT in any package trans to network. But I choice the number for easy to remember	//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MAX_APPLICATION				128																					  //		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define APPLICATION_NONE			0																								//
#define APPLICATION_SYSTEM			1																								//
#define	APPLICATION_PASSBY			3
#define APPLICATION_ECHO			7
#define APPLICATION_PRINT			8			//
#define APPLICATION_FORWARD			9																								//
#define	APPLICATION_FORWARD_LOOP	10			//	Add May 05 '14
#define APPLICATION_TELNET			23																								//
#define APPLICATION_SSLSERVER		43
#define APPLICATION_SSLCLIENT		44
#define	APPLICATION_DNS				53
#define APPLICATION_CONTENTPAD		67

#define APPLICATION_TUNNEL			78
// #define APPLICATION_TUNNELSERVER	76
// #define APPLICATION_TUNNELCLIENT	77
// #define APPLICATION_TUNNELTRANSFER	78

#define APPLICATION_WEB				80
#define APPLICATION_SIM				87
#define	APPLICATION_PROXY			88
#define APPLICATION_PROXYSIM		89
#define APPLICATION_ORACLE			101
#define APPLICATION_TEST			110																								//

#define APPLICATION_INTER			127
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	MODE_SSL_SERVER				1
#define MODE_SSL_CLIENT				2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	These FLAG define the behavior the PROTOCOL and APPLICATION																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//						//	TCP		// UDP		//	FILE	// OCI		//															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	FLAG_DUPLEX			//				Application Flag				//															//
//	FLAG_BUFFER_WRITE	//			Expire, Now in CommandEnd			//															//
//	FLAG_ADD_OVERLAP	//			//			//	OK		//			//															//
//	FLAG_LATER_CONNECT	//				Application Flag				//															//
//	FLAG_MULTI_INPUT	//			//	OK		//			//			//															//
//	FLAG_LATER_CLOSE	//			//	OK		//			//			//															//
//	FLAG_KEEPALIVE		//	OK		//			//			//			//															//
//	FLAG_IS_CONTEXT		//	OK		//	OK		//	OK		//	OK		//															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//						//	Echo	// Forward	//	HTTP	//																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	FLAG_DUPLEX			//			//	OK/NOT	//	OK		//																		//
//	FLAG_BUFFER_WRITE	//			Protocol Flag			//																		//
//	FLAG_ADD_OVERLAP	//			Protocol Flag			//																		//
//	FLAG_LATER_CONNECT	//			//			//	OK		//																		//
//	FLAG_MULTI_INPUT	//			Protocol Flag			//																		//
//	FLAG_LATER_CLOSE	//			Protocol Flag			//																		//
//	FLAG_KEEPALIVE		//			//			//	use		//																		//
//	FLAG_IS_CONTEXT		//			Protocol Flag			//																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	These FLAG define the behavior the PROTOCOL and APPLICATION																		//
//	FLAG_DUPLEX:		Most for Forward, A send B, and B send back A, it is DUPLEX, such as TCP; for FILE forward is NOT			//
//	FLAG_BUFFER_WRITE:	Expire, now in CommandEnd																					//
//	FLAG_ADD_OVERLAP:	For some PROTOCOL, should record the PACKET offset of STREAM, so should add the bytes						//
//	FLAG_LATER_CONNECT:	CONNECT means get new SERVER object, for CLIENT request several SERVER object, such as HTTP 1.1				//
//	FLAG_MULTI_INPUT:	Such as UDP, several CLIENT send to SAME SOCKET.															//
//	FLAG_LATER_CLOSE:	When CLIENT finished, do NOT close the CONNECT, such as UDP, assorted with FLAG_MULTI_INPUT					//
//																																	//
//	FLAG_IS_CONTEXT:	Mark the CONTEXT, to distinguish with BUFFER																//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define	FLAG_DUPLEX					0x0001																							//
// #define	FLAG_BUFFER_WRITE			0x0002																							//
#define	FLAG_ADD_OVERLAP			0x0004																							//
#define	FLAG_LATER_CONNECT			0x0008																							//
#define	FLAG_MULTI_INPUT			0x0010																							//
#define	FLAG_LATER_CLOSE			0x0020																							//
#define FLAG_MULTI_PROTOCOL			0x0040																							//
#define FLAG_KEEPALIVE				0x0080																							//
#define FLAG_PROCESS_ACCEPT			0x0100
#define FLAG_PROCESS_URL			0x0200					// for these who use URLInfo struct
#define	FLAG_PROCESS_FILE			0x0400
// #define FLAG_NONE_IOCP				0x0400
#define FLAG_GRACE_CLOSE			0x0800					//	for forward use, half close
#define	FLAG_PASSBY					0x1000
#define	FLAG_REMOVE_HTTPHEAD		0x2000
#define FLAG_SINGLE					0x4000
																																	//
#define	FLAG_IS_CONTEXT				0x10000000																						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For CApplication Function Pointer Array																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define	fOnAccept					0																								//
#define	fOnConnect					1																								//
#define	fOnClientRead				2																								//
#define	fOnClientWrite				3																								//
#define	fOnServerRead				4																								//
#define	fOnServerWrite				5																								//
#define	fOnClose					6																								//

#define fOnPassby					7
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For IOCP nOperation number, order by function pointer array																		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define	OP_BASE						0x100																				  //		//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define	OP_ACCEPT					(OP_BASE+fOnAccept)																				//
#define	OP_CONNECT					(OP_BASE+fOnConnect)																			//
#define	OP_CLIENT_READ				(OP_BASE+fOnClientRead)																			//
#define	OP_CLIENT_WRITE				(OP_BASE+fOnClientWrite)																		//
#define	OP_SERVER_READ				(OP_BASE+fOnServerRead)																			//
#define	OP_SERVER_WRITE				(OP_BASE+fOnServerWrite)																		//
#define	OP_CLOSE					(OP_BASE+fOnClose)																				//
#define	OP_PASSBY					(OP_BASE+fOnPassby)

#define OP_PROCESS_ACCEPT			(OP_BASE+100)

#define OPSIDE_CLIENT				(OP_BASE+OP_SERVER_WRITE)	//	0x205
#define OPSIDE_SERVER				(OP_BASE+OP_CLIENT_WRITE)	//	0x203
#define OPSIDE_PEER					(OPSIDE_CLIENT+1)			//	0x206
#define OPSIDE_PEER_CLIENT			(OPSIDE_CLIENT+2)			//	0x207
#define OPSIDE_PEER_SERVER			(OPSIDE_CLIENT+3)			//	0x028
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Other Operation NOT define in function																							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define OP_SHUTDOWN					0x200																							//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The following segment is my error catch define																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	This is error level																												//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MESSAGE_DEBUG_DETAIL		0x0001																							//
#define MESSAGE_DEBUG				0x0002																							//
#define MESSAGE_STATUS				0x0010																							//
#define MESSAGE_SYSTEM				0x0100																							//
#define MESSAGE_ERROR_L1			0x1000																							//
#define MESSAGE_ERROR_L2			0x2000																							//
#define MESSAGE_ERROR_L3			0x4000																							//
#define MESSAGE_ERROR_L4			0xffff																							//
																																	//
#define MESSAGE_ALL					0xffffffff																						//
#define MESSAGE_NOW					(MESSAGE_ALL)																					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	This is module define																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define MODULE_SERVICE				0x0001																							//
#define MODULE_MEMORY				0x0002																							//
#define MODULE_RESOURCE				0x0004																							//
#define MODULE_IOCP					0x0008																							//
#define MODULE_PROTOCOL				0x0010																							//
#define MODULE_APPLICATION			0x0020																							//
#define MODULE_HTTP					0x0040																							//
#define MODULE_JSON					0x0080
																																	//
#define MODULE_ALL					0xffffffff																						//
#define MODULE_NOW					(MODULE_ALL)																					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Display error message																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
void DEBUG_MESSAGE (MYINT module, MYINT level, _In_z_ _Printf_format_string_ const char * _Format, ...);							//
void DEBUG_MESSAGE_CON ( MYINT module, MYINT level, _In_z_ _Printf_format_string_ const char * _Format, ...);						//
																																	//
#define DEBUG_MESSAGE_JSON_ERROR_BREAK(code)\
	{printf ("Json Error:%d, %s\r\n", code, JSON_ERROR[code].errorname); break;}
#define DEBUG_MESSAGE_WSA_ERROR_BREAK(module, level)\
	{DEBUG_MESSAGE (module, level, "WSA Error code:%d\r\n  ", WSAGetLastError()); break;}											//
#define DEBUG_MESSAGE_FILE_ERROR_BREAK(module, level)\
	{DEBUG_MESSAGE (module, level, "File Error code:%d\r\n  ", GetLastError()); break;}												//

#define	DEBUG_MESSAGE_OCI_ERROR(module, level, state)\
	{DEBUG_MESSAGE (module, level, "OCI Error code:%d\r\n  ", state);};
#define	DEBUG_MESSAGE_OCI_ERROR_BREAK(module, level, state)\
	{DEBUG_MESSAGE (module, level, "OCI Error code:%d\r\n  ", state); break;};
																																	//
void DEBUG_MESSAGE_IPADDRESS_PORT (MYINT module, MYINT level, char* string, sockaddr_in ipaddr);									//
void DEBUG_MESSAGE_IPADDRESS_PORT (MYINT module, MYINT level, char* string, MYINT protocol, sockaddr_in ipaddr);					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Range detect																													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define RET_BETWEEN(smallone, bigone)				if ( ret_err >= smallone && ret_err <= bigone )								//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	This is used for MyCloseHandle function pointer																					//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
inline BOOL CloseSocket(HANDLE handle) { return closesocket((SOCKET)handle); };											//
inline BOOL CloseOtherHandle(HANDLE handle) { return ::CloseHandle(handle); };
inline BOOL CloseNullHandle(HANDLE handle) { return 0; };
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Every PROTOCOL/APPLICATION must provide the function to handle the IOCP, the follow is the prototype.							//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
typedef	long (CProtocol::*ProtocolNew)(CContextItem*, void*, long);																	//
typedef	CContextItem* (CProtocol::*ProtocolAccept)(CProtocol*, void*, CListItem*, long);											//
//typedef long (CProtocol::*ProtocolHandle)(CContextItem*, CListItem* &, long);				// change in Jun 28 '13
typedef long (CProtocol::*ProtocolHandle)(CContextItem*, CListItem* &, long, long);														//
// typedef long (CProtocol::*ProtocolAction)(CContextItem*, CListItem* &, long, long);												//
typedef long (CProtocol::*ProtocolAction)(CContextItem*, CListItem* &, long, long, long);	// change in Jun 23 '13					//
// typedef long (CApplication::*ApplicationHandle)(CContextItem*, CListItem* &, long);												//
typedef long (CApplication::*ApplicationHandle)(CContextItem*, CListItem* &, long, long);	// change in Jun 23 '13					//


typedef long (*ProcessHandle)(CContextItem*, CContextItem*, CListItem* &, long);																//

typedef long (*WafaVarFunction)(CContextItem*, ContentPad*, char* &, char*, char*, long);				//	for do page vars
typedef long (*WafaProcessFunction)(CContextItem*, CListItem* &, char*, char*, char*, long, long, ContentPad*);		//	for wafa_ function

typedef long (*WafaCreateFunciont)(CContextItem*, CListItem* &, char*);									//	for create in define

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Every PROTOCOL must provide the function to handle the IOCP, the follow is the prototype.										//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
typedef struct ProtocolHandles																										//
{																																	//
	ProtocolNew			fCreateNew;																									//
	ProtocolNew			fCreateRemote;																								//
	ProtocolHandle		fPostAccept;																								//
	ProtocolHandle		fPostConnect;																								//
	ProtocolAction		fPostSend;																									//
	ProtocolAction		fPostReceive;																								//
	ProtocolHandle		fPostClose;																									//
}ProtocolHandles;																													//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Every APPLICATION must provide the function to handle the IOCP, the follow is the prototype.									//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
typedef struct ApplicationHandles																									//
{																																	//
	ApplicationHandle	fOnFunction[16];																								//
}ApplicationHandles;																												//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MACRO for function pointer array, This is the master function for my IOCP														//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#define NoneProFunc(pProtocol, pFunction) (pProtocol->* \
	(&CIOCP::fProtocolFunction[0])->pFunction)																						//
#define NoneAppFunc(pApplication, pFunction) (pApplication->* \
	(CIOCP::fApplicationFunction[0].fOnFunction[pFunction]))																		//
																																	//
#define ProFunc(pProtocol, pFunction) (pProtocol->* \
	(&CIOCP::fProtocolFunction[pProtocol->ProtocolNumber])->pFunction)																//
#define AppFunc(pApplication, pFunction) ((pApplication)->* \
	(CIOCP::fApplicationFunction[(pApplication)->ApplicationNumber].fOnFunction[pFunction]))										//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For given 'sourstr', length 'size', find the 'substr', length 'subsize' substring												//
//	The last char of 'sourstr' and 'substr' can be any char, NOT need \0 at the end.												//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
char* memstr( char* sourstr, long size, char* substr, long subsize );																//
char* memstr_no(char* sourstr, long size, char* substr, long subsize, char nochar);												//

char* memstrs( char* sourstr, long size, char* substr, long subsize );
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long Myitoa(char ivalue, char* &buffer);
long Myitoa(long ivalue, char* &buffer);
long Myitoa(__int64 ivalue, char* &buffer);
long Myitoa(long ivalue, char* buffer, long length);
long Myitoa(__int64 ivalue, char* buffer, long length);

long MyString(char* source, int len, char* &buffer);

long Myitoh(unsigned char ivalue, char* &buffer);
long Myitoh(unsigned long ivalue, char* &buffer);
long Myitoh(unsigned __int64 ivalue, char* &buffer);

long Mymtoh(char* mac, char* &buffer);				//	for MAC to HEX
long Myntoa(char* ipaddr, char* &buffer);

void memcpy_up(char* desc, char* sour, long size);
void memcpy_u(char* desc, char* sour, long size);
void memcpy_u_up(char* desc, char* sour, long size);

long MyGetInfoAddr(char* ipstring, long strsize, addrinfo* &ipaddr);
// inline long SameContextAddress(CTCPContext* one, CTCPContext* two) {return SameAddress(one->addrServer, two->addrServer);};
inline long SameAddress(sockaddr_in* one, sockaddr_in* two) {return memcmp(one, two, sizeof(sockaddr_in));};
#define SameContextAddress(one, two)	SameAddress(&(((CTCPContext*)one)->addrServer), &(((CTCPContext*)two)->addrServer))

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For given 'filename', whether it with extend name in the list followed															//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
inline long IN_EXT_NAME(char* filename, char* ext, ... )																			//
{																																	//
	MYINT namelength = strlen(filename);																							//
	MYINT extlength;																												//
	char* nowext = ext;																												//
	va_list ap;																														//
																																	//
	va_start( ap, ext );																											//
	while( nowext )																													//
	{																																//
		extlength = strlen(nowext);																									//
		if (!memcmp(filename+namelength-extlength, nowext, extlength)) return TRUE;													//
																																	//
		nowext = va_arg( ap, char* );																								//
	}																																//
	va_end( ap );																													//
																																	//
	return 0;																														//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct KeyParaTran
{
	char*		pKeyword;
	long		pKeywordLength;
	char*		pFormat;
}KeyParaTran;

typedef struct ValParaTran
{
	void*		pValue;
	long*		pValueLength;
}ValParaTran;

typedef struct KeyParaAddition
{
	char*		headChar;
	char*		tailChar;
	int			headLength;
	int			tailLength;
}KeyParaAddition;

#define	IS_END					1
#define	IS_KEY					2
#define	IS_PRE					4

#define IS_SPE					(IS_END | IS_KEY)
#define IS_SPC					(IS_END | IS_PRE)

#define	VALUE_NOT_FOUND			(-1)
#define	BUFFER_ERROR			0x7ffffff0
#define BUFFER_NOMOREBUFFER		0x7ffffff1
#define BUFFER_TOO_LARGE		0x7ffffff2

#define MAX_CONTENT_LENGTH_SIZE		12

long CommandLineParaTran (char* inBuffer, long length, char* seprChar, char* aheadChar, 
						  KeyParaTran* keyFormat, ValParaTran* valFormat, unsigned char* charMap = NULL);
char* CommandLineParaTran (char* &inStart, long inLength, char* &outStart, long outLength,
						   char* keyPara, long keyLength, KeyParaAddition* keyAddition[] );

char* FindString(char* inBuffer, long inLength, char* keyHead, long keyLength, char* outBuffer, long outLength, char sprChar='\"', char aheadChar=' ');
char* FindInt(char* inBuffer, long inLength, char* keyHead, long keyLength, long& lvalue);
long GetInt(char* inBuffer, long inLength);

unsigned __int64 GetHex(char* &buffer);
unsigned __int64 GetHex(char* buffer, long len);


inline char* AddString(char* inBuffer, char* str, long length)
{
	memcpy(inBuffer, str, length);
	return (inBuffer+length);
}
inline unsigned char* AddString(unsigned char* inBuffer, char* str, long length)
{
	memcpy(inBuffer, str, length);
	return (inBuffer+length);
}

inline unsigned char* AddInt(unsigned char* inBuffer, long val, long length)
{
	Myitoa(val, (char*)inBuffer, length);
	return (inBuffer+length);
}

CListItem* ProcessMoreCommand( CContextItem* mContext, CListItem* mBuffer, long size );
CListItem* ProcessChunkedCommand( CContextItem* mContext, CListItem* mBuffer, long size );
CListItem* ProcessFullCommand(CContextItem* mContext, CListItem* mBuffer, long size);

long ReplaceHttpRequest(CListItem* mBuffer, char* key, char* replace, long repsize);

unsigned char* UTF8toGB2312(unsigned char* utf8, long usize, unsigned char* gb2312, long &gsize);
unsigned char* GB2312toUTF8(unsigned char* gb2312, long gsize, unsigned char* utf8, long &usize);
unsigned char* QRCodeTranslate(unsigned char* pic, long gsize, unsigned char* mStart, long &usize);


#define	__TRY \
	ret_err = 0x01;\
	while (TRUE)\
	{

#define __CATCH(Module, Function) \
		ret_err = 0;\
		break;\
	}\
	if (ret_err)\
	{\
		DEBUG_MESSAGE(Module, MESSAGE_ERROR_L2, "Error in %s, 0x%x\r\n", Function, ret_err);\
	}\
	return ret_err;

#define __CATCH_BEGIN(Module, Function) \
	ret_err = 0;\
	break;\
	}\
	if (ret_err)\
	{\
	DEBUG_MESSAGE(Module, MESSAGE_ERROR_L2, "Error in %s, 0x%x\r\n", Function, ret_err);

#define __CATCH_END \
	}\
	return ret_err;

#define	__CATCH_END_ \
	}

#define	__CATCH_LOOP(Module, Function, loop) \
		if (ret_err != loop)\
		{\
			ret_err = 0;\
			break;\
		}\
	}\
	if (ret_err)\
	{\
	DEBUG_MESSAGE(Module, MESSAGE_ERROR_L2, "Error in %s, 0x%x\r\n", Function, ret_err);\
	}\
	return ret_err;

#define SET_PARA_STR(info, num, para)\
	info->valPlace[num].pValue = info->para;\
	info->valPlace[num].pValueLength = &info->getLength[num];\
	info->getLength[num] = sizeof(info->para)-8;

#define SET_PARA_TYPE(info, num, para, type)\
	info->valPlace[num].pValue = &(info->para);\
	info->valPlace[num].pValueLength = &info->getLength[num];\
	info->getLength[num] = sizeof(type);

#define SET_PARA_OTHER_STR(info, num, para)\
	info->valPlace[num].pValue = para;\
	info->valPlace[num].pValueLength = &info->getLength[num];\
	info->getLength[num] = sizeof(info->para)-8;

#define SET_PARA_OTHER_TYPE(info, num, para, type)\
	info->valPlace[num].pValue = &(para);\
	info->valPlace[num].pValueLength = &info->getLength[num];\
	info->getLength[num] = sizeof(type);


#define	REAL_BUFFER(mBuffer) ((char*)(mBuffer+1))
#define REAL_ULBUFFER(mBuffer) ((unsigned long*)(mBuffer+1))

struct CTunnelHead;
#define TUNNEL_BUFFER(mBuffer) ((CTunnelHead*)(((char*)(mBuffer+1))-8))						// 8 means sizeof(CTunnelHead)


#define	DASZ(name) name, sizeof(name)

#define	NASZ(name) name, sizeof(name)-1
#define SNASZ(name) {name, sizeof(name)-1, "S"}
#define sNASZ(name) {name, sizeof(name)-1, "s"}
#define dNASZ(name) {name, sizeof(name)-1, "d"}

#define TO_INT(a,b,c,d) (((char)(a))+((char)(b)<<8)+((char)(c)<<16)+((char)(d)<<24))

#define COPY_PAD_STRING(desc, pad, num) \
	memcpy(desc, pad->resultKey[num], pad->getLength[num]);\
	desc[pad->getLength[num]] = 0;

#define COPY_PAD_INT(desc, pad, num) \
	desc = GetInt(pad->resultKey[num], pad->getLength[num]);

#define COPY_PAD_CHARS(desc, dnum, pad, num) \
	memcpy(desc->AdditionArray[dnum], pad->resultKey[num], pad->getLength[num]);\
	desc->AdditionPad.resultKey[dnum] = desc->AdditionArray[dnum];\
	desc->AdditionPad.getLength[dnum] = pad->getLength[num];

#define COPY_PAD_VAL(desc, dnum, sour, num) \
	desc->resultKey[dnum] = sour->resultKey[num];\
	desc->getLength[dnum] = sour->getLength[num];

#define		WAFA_DEFINE_START	"<!--wafadefine"
#define		WAFA_DEFINE_END		"/wafadefine-->"

#define		WAFA_REFER_START	"<!--wafarefer"
#define		WAFA_REFER_END		"/wafarefer-->"

#define		WAFA_VAR_START		"<!--wafavar>"
#define		WAFA_VAR_END		"</wafavar-->"

#define		WAFA_POST_MARK		"<!--wafapost-->"

#define		WAFA_BODY_START		"<!--wafastart-->"
#define		WAFA_BODY_END		"<!--wafaend-->"

#define		WAFA_CREATE_START	"<!--wafacreate>"
#define		WAFA_CREATE_END		"</wafacreate-->"

#define		CONTENT_NAME_FILE		"FILE"
#define		CONTENT_NAME_CLIENTHEAD	"CHEAD"						//	for POST head
#define		CONTENT_NAME_CLIENT		"CLIENT"					//	for POST body and Get head
#define		CONTENT_NAME_SERVERHAED	"SHEAD"						//	now only one server head is valid
#define		CONTENT_NAME_NONE		"NONE"						//	for without server name
#define		CONTENT_NAME_NTLM		"NTLMS"			//	for record ntlm info

#ifdef _DEBUG
#define		TESTBREAK	{ static int testbreak=0;	testbreak++;	if (testbreak % 5 == 4 ) break; }
void testtran();
void DisplayContextList();

CListItem* MakeBufferStr(CContextItem* mContext, char* str);
void TestChunkedInput(CContextItem* mContext);
#endif _DEBUG

