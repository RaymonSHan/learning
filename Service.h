#pragma once

#include <Winsvc.h>

#define CStartApplication			CSystemApplication
// #define CStartApplication			CContendNetworkServer				//	changed in Oct. 10 '13
#define CStartProtocol				CTCPProtocol

using namespace std;

class CService 
{
public:
	SERVICE_STATUS serviceStatus;
	SERVICE_STATUS_HANDLE serviceStatusHandle;
	HANDLE stopServiceEvent;

	TCHAR				serviceName[NORMAL_CHAR];
	TCHAR				displayName[NORMAL_CHAR];
	TCHAR				descriptionName[NORMAL_CHAR];
	SERVICE_DESCRIPTION Description;

public:
	CService (LPCTSTR sName, LPCTSTR dName, LPCTSTR description);
	~CService();
	void installService();
	void uninstallService();
	BOOL stopService(SC_HANDLE service);

	void ServiceMain(DWORD /*argc*/, TCHAR* /*argv*/[]);
	void ServiceControlHandle(DWORD controlCode);
	void runService(void);
	void runProgram(void);

	BOOL InitProcess(void);
	BOOL MainProcess(void);
	BOOL ExitProcess(void);

	virtual BOOL SetExit(void);

	void showError(DWORD enumber);
	
	CResources			ServiceResources;
	CStartApplication*	pStartApplication;
// 	CStartProtocol*		pStartProtocol;

	static CNoneProtocol*		pNoneProtocol;
	static CNoneApplication*	pNoneApplication;

};

void WINAPI ServiceMain(DWORD /*argc*/, TCHAR* /*argv*/[]);
void WINAPI ServiceControlHandler(DWORD controlCode);
