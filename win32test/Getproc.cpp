#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include "Getproc.h"

DWORD GetProc::GetProcId(const char* procName)
{
	DWORD pid = 0;
	HANDLE hProcSnap;
	PROCESSENTRY32 procEntry;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hProcSnap == INVALID_HANDLE_VALUE)
	{
		printf("hProc 모듈 얻기 실패");
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hProcSnap, &procEntry);
	do
	{
		if (!_stricmp(procEntry.szExeFile, procName))
		{
			pid = procEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcSnap, &procEntry));
	CloseHandle(hProcSnap);

	return pid;
}

