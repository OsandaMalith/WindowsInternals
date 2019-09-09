#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
using namespace std;

/*
Title: Faultrep!CreateMinidump Hot Patch to get a full dump passing MiniDumpWithFullMemory as the MINIDUMP_TYPE.
Author: Osanda Malith Jayathissa (@OsandaMalith)

The function CreateMinidump is only available in Windows XP and Windows Server 2003.
If you want to get this working in other Windows systems, copy the DLL from XP or Server 2003 and place it in the same folder.
This DLL has no ASLR enabled and therfore the address are hardcoded.

32-bit faultrep.dll = 6945AEBF push 21
64-bit faultrep.dll = 7FF6E010945 mov r9d,21 
*/

typedef int(WINAPI *CreateMinidumpProc)(DWORD, LPCWSTR, struct tagSMDumpOptions *);

typedef NTSTATUS(WINAPI *_RtlAdjustPrivilege)(
	ULONG Privilege, BOOL Enable,
	BOOL CurrentThread, PULONG Enabled);

int _tmain(int argc, _TCHAR* argv[]) {

	if (argc < 2) {
		wcerr << "[~] Usage: " << argv[0] << " Process Name" << endl;
		return -1;
	}

	DWORD PID = 0;
	LPCWSTR Name = argv[1];
	wstring FileName(Name);
	LPCWSTR processName = L"";
	ULONG t;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry = {};
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	wcout << "[~] Faultrep!CreateMinidump Undocumented API from Windows XP and Windows Server 2003" << endl;
	wcout << "[+] Author: Osanda Malith Jayathissa (@OsandaMalith)" << endl;
	wcout << "[+] Website: https://osandamalith.com" << endl;


	if (Process32First(snapshot, (PROCESSENTRY32*)&processEntry)) {
		while (_wcsicmp(processName, Name) != 0) {
			Process32Next(snapshot, &processEntry);
			processName = processEntry.szExeFile;
			PID = processEntry.th32ProcessID;
		}
		wcout << "[+] Got " << Name << " PID: " << PID << endl;
	}
	else wcout << "[-] Process Name Not Found!" << endl;

	_RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(GetModuleHandle(L"ntdll"), "RtlAdjustPrivilege");
	CreateMinidumpProc CreateMinidump = (CreateMinidumpProc)GetProcAddress(LoadLibrary(L"faultrep.dll"), "CreateMinidumpW");

	{
#ifndef _WIN64
		UCHAR *Patch = (UCHAR*)CreateMinidump + 0x72CC;
#else
		UCHAR *Patch = (UCHAR*)CreateMinidump + 0xBF47;
#endif
		DWORD old;
		VirtualProtect((LPVOID)Patch, sizeof(UCHAR), PAGE_EXECUTE_READWRITE, &old);
		*Patch = 0x2; // MiniDumpWithFullMemory
	}

	wcout << "[+] Dumping" << endl;
	RtlAdjustPrivilege(20, TRUE, FALSE, &t);
	CreateMinidump(PID, (LPCWSTR)(FileName + L"_dump.dmp").c_str(), 0); 

	return 0;
}