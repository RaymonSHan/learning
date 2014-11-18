
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef	_DEBUG
extern	long	Started;
#endif	_DEBUG

#ifdef	USE_ARGCARGV
	int Argc;
	TCHAR** Argv;
#endif

#ifdef WAFA
CService theService("Wafa Virtualzation 1.0","Wafa Mobile Virtualzation", "敏行虚拟化服务");
#endif WAFA

#ifdef REDANT
CService theService("RedAnt Network Manager", "RedAnt Network Manager", "瑞安网络管理系统");
#endif REDANT

CService::CService (LPCTSTR sName, LPCTSTR dName, LPCTSTR description)
{
	srand((unsigned)time(NULL));

	serviceStatusHandle = 0;
	stopServiceEvent = 0;

	strcpy_s(serviceName, NORMAL_CHAR, sName);
	strcpy_s(displayName, NORMAL_CHAR, dName);
	strcpy_s(descriptionName, NORMAL_CHAR, description);
	Description.lpDescription = descriptionName;
}

CService::~CService()
{
//	stopServiceEvent = 0;
}

void CService::installService()
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE | SERVICE_START);
	SC_HANDLE service;

	if (serviceControlManager)
	{
		TCHAR path[ _MAX_PATH + 1 ];
		if (GetModuleFileName(0, path, sizeof(path) / sizeof(path[0])) > 0)
		{
			printf("Installing ...");
			service = CreateService(serviceControlManager, serviceName, displayName,
				SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path, 0, 0, 0, 0, 0);
			if (service)
			{
// 
// 				SERVICE_DESCRIPTION Description;
// 				TCHAR szDescription[1024];
// 				ZeroMemory(szDescription, 1024);
// 				ZeroMemory(&Description, sizeof(SERVICE_DESCRIPTION));
// 				lstrcpy(szDescription, _T("服務的描述."));
// 				Description.lpDescription = szDescription;
				::ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &Description);


				printf("Successfully installed.. !\n");
				printf("Start Service ... ");
				StartService(service, 0, NULL);
				printf("Started!\r\n");
				CloseServiceHandle(service);
			}
			else
			{
				showError(GetLastError());
			}
		}
		CloseServiceHandle(serviceControlManager);
	}
}

BOOL CService::stopService(SC_HANDLE service)
{
	if (service)
	{
		SERVICE_STATUS serviceStatus;
		QueryServiceStatus(service, &serviceStatus);
		if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
		{
			ControlService(service, SERVICE_CONTROL_STOP, &serviceStatus);
			printf("Stopping Service.");
			for (int i = 0; i < 20; i++)
			{
				QueryServiceStatus(service, &serviceStatus);
				if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
				{
					printf("Stopped\n");
					return true;
				}
				else
				{
					Sleep(500);
					printf(".");
				}
			}
			printf("Failed\n");
			return false;
		}
	}
	return true;
}

void CService::uninstallService()
{
	SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);

	if (serviceControlManager)
	{
		SC_HANDLE service = OpenService(serviceControlManager,
			serviceName, SERVICE_QUERY_STATUS | SERVICE_STOP | DELETE);
		if (service)
		{
			if (stopService(service))
			{
				if (DeleteService(service))
					printf("Successfully Removed !\n");
				else
					showError(GetLastError());
			}
			else
				printf("Failed to Stop Service..\n");

			CloseServiceHandle(service);
		}
		else
			printf("Service Not Found..\n");

		CloseServiceHandle(serviceControlManager);
	}
}

void CService::runService(void)
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{serviceName, ::ServiceMain},
		{0, 0}
	};

	StartServiceCtrlDispatcher(serviceTable);
}

BOOL CService::SetExit(void)
{
	SetEvent(stopServiceEvent);
	return TRUE;
}

void CService::showError(DWORD enumber)
{
	LPTSTR lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, enumber, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf, 0, NULL );
	printf("%s\n", lpMsgBuf);
}

// #if ( defined(USE_ARGC)|| defined(USE_PATH) )
sockaddr_in AddrListen;
sockaddr_in AddrForward;
// #endif

// #ifdef USE_PATH
char LogFile[MAX_PATH];
// #endif USE_PATH

#ifdef FIDDLER_ADDR
sockaddr_in FiddlerAddr;
#endif FIDDLER_ADDR


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
//int _tt(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	BOOL result;

#ifdef FIDDLER_ADDR

	FiddlerAddr.sin_family = AF_INET;    
	FiddlerAddr.sin_port = htons(8888);    
	FiddlerAddr.sin_addr.S_un.S_addr = inet_addr(argv[1]);
	if (FiddlerAddr.sin_addr.S_un.S_addr != INADDR_NONE)	theService.runProgram();
	else printf("TEST use only, input ContentPad fiddleraddress,\r\n such as \"ContentPad 192.168.1.5\"\r\n ");
	return 0;
#endif FIDDLER_ADDR

	result = GetVersionEx(&osvi);
	if (result && osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT)
	{
#ifdef USE_SERVICE
		if (argc == 2 && lstrcmpi(argv[1], "install") == 0)
		{
			theService.installService();
		}
		else if (argc == 2 && lstrcmpi(argv[1], "uninstall") == 0)
		{
			theService.uninstallService();
		}
		else if (argc == 2 && lstrcmpi(argv[1], "start") == 0)
		{
			SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
			BOOL serviceStopped = true;

			if (serviceControlManager)
			{
				SC_HANDLE service = OpenService(serviceControlManager, theService.serviceName, SERVICE_QUERY_STATUS | SERVICE_START);
				if (service)
				{
					printf("Start Service ... ");
					StartService(service, 0, NULL);
					printf("Started!\r\n");
					CloseServiceHandle(service);
				}
				else
				{
					theService.showError(GetLastError());
				}
				CloseServiceHandle(serviceControlManager);
			}
		}
		else if (argc == 2 && lstrcmpi(argv[1], "stop") == 0)
		{
			SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
			BOOL serviceStopped = true;

			if (serviceControlManager)
			{
				SC_HANDLE service = OpenService(serviceControlManager, theService.serviceName, SERVICE_QUERY_STATUS | SERVICE_STOP);
				if (service)
				{
					serviceStopped = theService.stopService(service);
					CloseServiceHandle(service);
				}
				CloseServiceHandle(serviceControlManager);
			}
		}
		else if (argc == 2 && lstrcmpi(argv[1], "run") == 0)
		{
#endif USE_SERVICE

#ifdef USE_ARGC
 		if (argc == 4)
		{
#endif USE_ARGC

#ifdef USE_PATH
		if (argc == 3)
		{
#endif USE_PATH

#ifdef	USE_ARGCARGV
		if (TRUE)
		{
			Argc = argc;
			Argv = &argv[0];
#endif USE_ARGCARGV
			SC_HANDLE serviceControlManager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
			BOOL serviceStopped = true;

			if (serviceControlManager)
			{
				SC_HANDLE service = OpenService(serviceControlManager, theService.serviceName, SERVICE_QUERY_STATUS | SERVICE_STOP);
				if (service)
				{
					serviceStopped = theService.stopService(service);
					CloseServiceHandle(service);
				}
				CloseServiceHandle(serviceControlManager);
			}

			if (serviceStopped)
			{
#if ( defined(USE_ARGC)|| defined(USE_PATH) )
				AddrListen.sin_family = AF_INET;    
				AddrListen.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
				AddrListen.sin_port = htons(atoi(argv[1]));    

				AddrForward.sin_family = AF_INET;    
				AddrForward.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
				AddrForward.sin_port = 0;

// 				AddrForward.sin_family = AF_INET;    
// 				AddrForward.sin_addr.S_un.S_addr = inet_addr(argv[2]);
// 				AddrForward.sin_port = htons(atoi(argv[3]));    
#endif ( defined(USE_ARGC)|| defined(USE_PATH) )

#ifdef USE_PATH
				strcpy_s(LogFile, MAX_PATH, argv[2]);
#endif USE_PATH

				theService.runProgram();
			}
			else
				printf("Failed to Stop Service\n");
		}
		else 
		{
			TCHAR namebuff[_MAX_PATH];
			GetModuleFileName(NULL, namebuff, _MAX_PATH);
			printf("Program Path:%s Version 1.2 for Concurrency PostConnect. Compiled at %s\r\n", namebuff, __TIMESTAMP__);
			
#ifdef USE_SERVICE
			printf("[Option]=[ install | uninstall | start | stop | run ]\r\n");
#endif USE_SERVICE

#ifdef	FORWARDLOOP_FUNCTION
			printf("    With ForwardLoop Function\r\n");
#endif	FORWARDLOOP_FUNCTION

#ifdef	QRCODE_FUNCTION
			printf("    With Qrcode decode Function\r\n");
#endif	QRCODE_FUNCTION

#ifdef USE_ARGC
			printf("Parameter: LocalPort ForwardAddress ForwardPor\r\nexample: %s 7001 192.168.1.1 8001", namebuff);
#endif USE_ARGC

#ifdef USE_PATH
			printf("Parameter: ListenPort LogDirName\r\nexample: %s 7001 c:\\log\\", namebuff);

// 			printf("Parameter: LocalPort ForwardAddress ForwardPort LogDirName\r\nexample: %s 7001 192.168.1.1 8001 c:\\log\\", namebuff);
#endif USE_PATH
			// I think, when the service run, it do "main" again, and without any para. the service started at the second time
			theService.runService();
			// should use this line ?
			// Why it is ok in this place?
		}
	}
// 	else if (argc == 1 || lstrcmpi(argv[1], TEXT("run")) == 0)
// 		theService.runProgram();
	else
		printf("This program is not available on Windows95/98/ME\n");

	return 0;

	return nRetCode;
}

void WINAPI ServiceMain(DWORD argc, TCHAR* argv[])
{
	theService.ServiceMain( argc,  argv);
}

void WINAPI ServiceControlHandler(DWORD controlCode)
{
	theService.ServiceControlHandle(controlCode);
}

void CService::ServiceControlHandle(DWORD controlCode)
{
	switch (controlCode)
	{
	case SERVICE_CONTROL_INTERROGATE:
		break;

	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		serviceStatus.dwWaitHint = 20000;
		serviceStatus.dwCheckPoint = 1;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		SetExit();
		//theService.SetExit();
		return;

	case SERVICE_CONTROL_PAUSE:
		break;

	case SERVICE_CONTROL_CONTINUE:
		break;

	default:
		if (controlCode >= 128 && controlCode <= UCHAR_MAX)
			break;
		else
			break;
	}

	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void CService::ServiceMain(DWORD /*argc*/, TCHAR* /*argv*/[])
{
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ::ServiceControlHandler);

	if (serviceStatusHandle)
	{
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		if (InitProcess() ) 
		{
			serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			return ;
		}

		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		while (WaitForSingleObject(stopServiceEvent, 1000) == WAIT_TIMEOUT)
		{
			MainProcess();
		}

		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		//serviceStatus.dwCheckPoint = 2;
		//serviceStatus.dwWaitHint = 1000;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);

		ExitProcess();

		serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
	}
};

void CService::runProgram(void)
{
#ifdef _DEBUG
//	::TestListDemo();
#endif _DEBUG

	if (InitProcess()) return;
	while (WaitForSingleObject(stopServiceEvent, 1000) == WAIT_TIMEOUT)
	{
		MainProcess();
	}
	ExitProcess();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The above is almost copy from Microsoft samples, the following is my code														//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
//	ret_err 0x10 :	NONE																											//
//	ret_err 0x20 :	break	Jan. 09 '13																								//
//	ret_err 0x30 :	break	Jan. 09 '13																								//
//	ret_err 0x40 :	NONE																											//
//	ret_err 0x50 :	break	Jan. 09 '13																								//
//	ret_err 0x60 :	break	Jan. 09 '13																								//
//	ret_err 0x70 :	break	Jan. 09 '13																								//
//	ret_err 0x80 :	break	Jan. 09 '13																								//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CService::InitProcess(void)																									//
{																																	//
	MYINT ret_err = 1;																												//
//	CContextItem* retContext;
	long ret;																														//
// 	sockaddr_in addrTelnet;																											//
																																	//
	while (TRUE)																													//
	{																																//
		ret_err = 0x10;																												//
		stopServiceEvent = CreateEvent(0, TRUE, FALSE, 0);		//	Any thread, include IOCP waiting for this stop event			//
		if (!stopServiceEvent) break;							//////////////////////////////////////////////////////////////////////
																//																	//
		ret_err = 0x20;											//																	//
		pNoneProtocol = new CNoneProtocol();					//	CNoneProtocol is the base of other PROTOCOL						//
		if (!pNoneProtocol) break;																									//
		pNoneProtocol->SetAction(&ServiceResources, NULL, NULL);																	//
		RegisterProtocol(pNoneProtocol, PROTOCOL_NONE, 0);																			//
																																	//
		ret_err = 0x30;																												//
		pNoneApplication = new CNoneApplication();				//	CNoneApplication is the base for APPLICATION					//
		if (!pNoneApplication) break;							//////////////////////////////////////////////////////////////////////	
		pNoneApplication->SetAction(&ServiceResources, NULL);																		//
		RegisterApplication(pNoneApplication, APPLICATION_NONE, 0);																	//

// 		ret_err = 0x40;																												//
// 		if (ServiceResources.InitProcess()) break;				//	Prepare MEMORYLIST, and IOCP									//
// 																//////////////////////////////////////////////////////////////////////
// 		ret_err = 0x50;											//																	//
// 		pStartProtocol = new CStartProtocol();					//	SystemApplication is a telnet console, base on TCP				//
// 		if (!pStartProtocol) break;																									//
// 		pStartProtocol->SetAction(&ServiceResources, "TEL_IOCP", "TEL_TCP");														//
// 		RegisterProtocol(pStartProtocol, PROTOCOL_TCP, 0);																			//
																																	//
		ret_err = 0x60;																												//
		pStartApplication = new CStartApplication();																				//
		if (!pStartApplication) break;																								//
// 		pStartApplication->SetAction(&ServiceResources, "TEL_BUFFER");																//
// 		RegisterApplication(pStartApplication, APPLICATION_SYSTEM, 0);																//
																																	//
// 		ret_err = 0x70;																												//
// 		addrTelnet.sin_family = AF_INET;																							//
// 		addrTelnet.sin_port = htons(23);						//////////////////////////////////////////////////////////////////////
// 		addrTelnet.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");	//	console listening TCP 23										//
// 		retContext = CreateApplication(pStartApplication, pStartProtocol, (void*)&addrTelnet, sizeof(sockaddr_in), NULL, NULL, 0);	//
// 		if (!retContext) break;																										//
																																	//
		ret_err = 0x80;																												//
		ret = pStartApplication->InitProcess();					//	setup all other PROTOCOL and APPLICAITON, for really use		//
		if (ret) break;											//////////////////////////////////////////////////////////////////////
																																	//
#ifdef	_DEBUG
		Started = 1;
#endif	_DEBUG

		ret_err = 0;																												//
		break;																														//
	}																																//
	if (ret_err)																													//
	{																																//
		DEBUG_MESSAGE(MODULE_SERVICE, MESSAGE_ERROR_L3, "Error in InitProcess, 0x%x\r\n", ret_err);									//
		RET_BETWEEN(0x80, 0x80) CloseApplication(pStartApplication);																//
		RET_BETWEEN(0x70, 0x80) delete pStartApplication;																			//
// 		RET_BETWEEN(0x60, 0x80) delete pStartProtocol;																				//
		RET_BETWEEN(0x40, 0x80) delete pNoneApplication;																			//
		RET_BETWEEN(0x30, 0x80) delete pNoneProtocol;																				//
		RET_BETWEEN(0x20, 0x80) CloseHandle(stopServiceEvent);																		//
		return TRUE;											//	This means error												//
	}															//////////////////////////////////////////////////////////////////////
	return FALSE;												//	This means OK													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	A timer call this																												//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CService::MainProcess(void)																									//
{																																	//
	ServiceResources.MainProcess();								//	Timeout MEMONRYLIST												//
																//////////////////////////////////////////////////////////////////////
	return FALSE;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	It will be called, when set the stopServiceEvent																				//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		//
BOOL CService::ExitProcess(void)																									//
{																																	//
	ServiceResources.ExitProcess();																									//
	pStartApplication->ExitProcess();																								//
																																	//
	delete pStartApplication;																										//
// 	delete pStartProtocol;																											//
	delete pNoneApplication;																										//
	delete pNoneProtocol;																											//
																																	//
	CloseHandle(stopServiceEvent);																									//
	stopServiceEvent = 0;																											//
																																	//
	return FALSE;																													//
}																																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///C:\debugssl\lib;c:\oci\lib\msvc


// #include <iostream>
// #include <Magick++.h>
// #include <zbar.h>
// #define STR(s) #s
// 
// // using namespace std;
// using namespace zbar;
// 
// int main (int argc, char **argv)
// {
// 	if(argc < 2) return(1);
// 
// #ifdef MAGICK_HOME
// 	// http://www.imagemagick.org/Magick++/
// 	//    under Windows it is necessary to initialize the ImageMagick
// 	//    library prior to using the Magick++ library
// 	Magick::InitializeMagick(MAGICK_HOME);
// #endif
// 
// 	// create a reader
// 	ImageScanner scanner;
// 
// 	// configure the reader
// 	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
// 
// 	// obtain image data
// 	Magick::Image magick(argv[1]);  // read an image file
// 	int width = magick.columns();   // extract dimensions
// 	int height = magick.rows();
// 	Magick::Blob blob;              // extract the raw data
// 	magick.modifyImage();
// 	magick.write(&blob, "GRAY", 8);
// 	const void *raw = blob.data();
// 
// 	// wrap image data
// 	Image image(width, height, "Y800", raw, width * height);
// 
// 	// scan the image for barcodes
// 	int n = scanner.scan(image);
// 
// 	// extract results
// 	for(Image::SymbolIterator symbol = image.symbol_begin();
// 		symbol != image.symbol_end();
// 		++symbol) {
// 			// do something useful with results
// 			cout << "decoded " << symbol->get_type_name()
// 				<< " symbol \"" << symbol->get_data() << '"' << endl;
// 	}
// 
// 	// clean up
// 	image.set_data(NULL, 0);
// 
// 	return(0);
// }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    