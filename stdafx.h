
#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include	<afxwin.h>																												//
#include	<afxext.h>																												//
#include	<afxdisp.h>																												//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include	<zlib/zlib.h>

#include	<stdio.h>																												//
#include	<tchar.h>																												//
#include	<WinSock2.h>																											//
#include	<Ws2tcpip.h>
#include	<mswsock.h>																												//
#include	<windows.h>																												//
#include	<iostream>																												//
#include	<math.h>																												//
#include	<sys/timeb.h>																											//
#include	<process.h>																												//
#include	<iphlpapi.h>


// #include	<sqlext.h>																												//
																																	//
#ifndef WINVER													// 指定最低平台需求為 Windows Vista。								//
	#define WINVER 0x0600										// 將它變更為針對 Windows 其他版本的適當值。						//
#endif																																//
																																	//
#ifndef _WIN32_WINNT											// 指定最低平台需求為 Windows Vista。								//
	#define _WIN32_WINNT 0x0600									// 將它變更為針對 Windows 其他版本的適當值。						//
#endif																																//
																																	//
#ifndef _WIN32_WINDOWS											// 指定最低平台需求為 Windows 98。									//
	#define _WIN32_WINDOWS 0x0410								// 將它變更為針對 Windows Me (含) 以後版本的適當值。				//
#endif																																//
																																	//
#ifndef _WIN32_IE												// 指定最低平台需求為 Internet Explorer 7.0。						//
	#define _WIN32_IE 0x0700									// 將它變更為針對 IE 其他版本的適當值。								//
#endif																																//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For use OCI/OCCI																												//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//using namespace		oracle::occi;																								//
using namespace		std;																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	For class declare ahead																											//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
class	_BASE;																														//
class	CMemoryListCriticalSection;																									//
class	CListItem;																													//
class	CContextItem;																												//
class	CResources;																													//
class	CService;																													//
class	CIOCP;																														//
class	CProtocol;																													//
class	CNoneProtocol;																												//
class	CInterProtocol;
class	CApplication;																												//
class	CNoneApplication;																											//
class	CInterApplication;
class	CMultiApplication;
class	CForwardApplication;
class	CDNSApplication;
class	CSystemApplication;																											//
class	CPassbyApplication;
class	CSSLServerApplication;
class	CSSLClientApplication;
class	CProxyApplication;
class	CWebApplication;
class	COracleApplication;
class	CTelnetApplication;																											//
class	CSimServer;
class	CContentPadServer;
class	CWafaContentPadServer;
class	CTunnelProtocol;
class	CTunnelApplication;
class	CContendNetworkServer;
class	CForwardLoopApplication;

struct	ContentPad;
struct	ProtocolId;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	I am lazy, only need more time to compile																						//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
#include	"common.h"																												//
#include	"MemoryList.h"																											//
#include	"Protocol.h"																											//
#include	"Application.h"
#include	"Service.h"																												//
// #include	"MultiApplication.h"																									//
// #include	"Proxy.h"

#ifdef	OCI_FUNCTION
#include	<oci.h>																													//
#include	"OciProtocol.h"																											//
#endif	OCI_FUNCTION

#ifdef	NTLM_FUNCTION
#include	"ntlm.h"
#endif	NTLM_FUNCTION

// #ifdef	CONTENTPAD_APPLICATION
#include	"OcciIOCP.h"																											//
// #endif	CONTENTPAD_APPLICATION

#include	"Json.h"
#include	"ContentPad.h"
#include	"ContentNetwork.h"
#include	"ForwardLoop.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #ifdef _DEBUG																													//
// #pragma comment(lib, "oraocci11d.lib")																							//
// #else  _DEBUG																													//
// #pragma comment(lib, "oraocci11.lib")																							//
// #endif																															//
#pragma comment (lib, "oci.lib")																									//
#pragma comment (lib, "ws2_32.lib")																									//
#pragma comment (lib, "iphlpapi.lib")		//	for SendARP
#pragma comment (lib, "libeay32.lib")																								//
#pragma comment (lib, "ssleay32.lib")
#pragma comment (lib, "wpcap.lib")
#pragma comment (lib, "Packet.lib")
// #pragma comment (lib, "zlib.lib")

#ifdef	QRCODE_FUNCTION
#pragma comment (lib, "libzbar-0.lib")
#pragma comment (lib, "Urlmon.lib")
#ifdef	_DEBUG
#pragma comment(lib, "CORE_DB_bzlib_.lib")
#pragma comment(lib, "CORE_DB_cairo_.lib")
#pragma comment(lib, "CORE_DB_croco_.lib")
#pragma comment(lib, "CORE_DB_ffi_.lib")
#pragma comment(lib, "CORE_DB_glib_.lib")
#pragma comment(lib, "CORE_DB_jbig_.lib")
#pragma comment(lib, "CORE_DB_jp2_.lib")
#pragma comment(lib, "CORE_DB_jpeg_.lib")
#pragma comment(lib, "CORE_DB_lcms_.lib")
#pragma comment(lib, "CORE_DB_librsvg_.lib")
#pragma comment(lib, "CORE_DB_libxml_.lib")
#pragma comment(lib, "CORE_DB_lqr_.lib")
#pragma comment(lib, "CORE_DB_Magick++_.lib")
#pragma comment(lib, "CORE_DB_magick_.lib")
#pragma comment(lib, "CORE_DB_openjpeg_.lib")
#pragma comment(lib, "CORE_DB_pango_.lib")
#pragma comment(lib, "CORE_DB_pixman_.lib")
#pragma comment(lib, "CORE_DB_png_.lib")
#pragma comment(lib, "CORE_DB_tiff_.lib")
#pragma comment(lib, "CORE_DB_ttf_.lib")
#pragma comment(lib, "CORE_DB_wand_.lib")
#pragma comment(lib, "CORE_DB_webp_.lib")
#pragma comment(lib, "CORE_DB_zlib_.lib")
#else	_DEBUG
#pragma comment(lib, "CORE_RL_bzlib_.lib")
#pragma comment(lib, "CORE_RL_cairo_.lib")
#pragma comment(lib, "CORE_RL_coders_.lib")
#pragma comment(lib, "CORE_RL_croco_.lib")
#pragma comment(lib, "CORE_RL_ffi_.lib")
#pragma comment(lib, "CORE_RL_filters_.lib")
#pragma comment(lib, "CORE_RL_glib_.lib")
#pragma comment(lib, "CORE_RL_jbig_.lib")
#pragma comment(lib, "CORE_RL_jp2_.lib")
#pragma comment(lib, "CORE_RL_jpeg_.lib")
#pragma comment(lib, "CORE_RL_lcms_.lib")
#pragma comment(lib, "CORE_RL_librsvg_.lib")
#pragma comment(lib, "CORE_RL_libxml_.lib")
#pragma comment(lib, "CORE_RL_lqr_.lib")
#pragma comment(lib, "CORE_RL_Magick++_.lib")
#pragma comment(lib, "CORE_RL_magick_.lib")
#pragma comment(lib, "CORE_RL_openjpeg_.lib")
#pragma comment(lib, "CORE_RL_pango_.lib")
#pragma comment(lib, "CORE_RL_pixman_.lib")
#pragma comment(lib, "CORE_RL_png_.lib")
#pragma comment(lib, "CORE_RL_tiff_.lib")
#pragma comment(lib, "CORE_RL_ttf_.lib")
#pragma comment(lib, "CORE_RL_wand_.lib")
#pragma comment(lib, "CORE_RL_webp_.lib")
#pragma comment(lib, "CORE_RL_zlib_.lib")
#endif	_DEBUG

#endif	QRCODE_FUNCTION