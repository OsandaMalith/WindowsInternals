#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
using namespace std;

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

	RtlAdjustPrivilege(20, TRUE, FALSE, &t);
	CreateMinidump(PID, (LPCWSTR)(FileName + L"_dump.dmp").c_str(), 0); 

	return 0;
}
