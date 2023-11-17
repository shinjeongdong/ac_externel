#include <iostream>
#include <Windows.h>
#include "Getproc.h"
#include "main.h"
#include "class.h"
#include "Cheats.h"

HANDLE hProc = 0;

int main()
{
	DWORD pid = GetProc::GetProcId("ac_client.exe");
	
	hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

	while(1)
	{
		Cheats::Run();
		
	}
	
	return 0;
}