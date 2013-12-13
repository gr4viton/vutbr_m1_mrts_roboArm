/***************
@project  roboArm_starter
@filename roboArm
@author   
@date     
@brief    
***************/

#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>

#include <rtapi.h>

int main (int argc, _TCHAR*argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	int result = 0;
	DWORD ExitCode;

	result = RtCreateProcess(TEXT("C:\\mrts\\robo_hand.rtss"), TEXT("C:\\mrts\\robo_hand.rtss C:\\mrts\\data\\data.txt"), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (result == 0)
	{
		printf("Can't create process. Last error is: %d\n", GetLastError());
	}
	else
	{
		printf("Process created and running!\n");
		printf("Waiting. Process id: %d\n", pi.dwProcessId);
	}
	Sleep(1000);
	while(1)
	{
		
		//printf("Waiting. Process id: %d\n", pi.dwProcessId);
		result = RtGetExitCodeProcess(pi.hProcess, &ExitCode);
		if (ExitCode == STILL_ACTIVE)
			printf("Still running!\n");
		else
		{
			printf("Process is ko! Exit code is %d\n", ExitCode);
			break;
		}
		Sleep(1000);

	}
	//WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	printf("Process ended!\n");
	return (0);
}