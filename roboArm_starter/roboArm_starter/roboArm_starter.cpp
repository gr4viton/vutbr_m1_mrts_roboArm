/***************
@project  roboArm_starter
@filename roboArm
@author   xdavid10, xslizj00 @ FEEC-VUTBR 
@date     2013_12_02
@brief    win32api console project for starting an RTX .rtss executable program file
***************/

// defines
#define FLAWLESS_EXECUTION	0

// includes
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>

#include <rtapi.h>


/****************************************************************************
@function   main
@brief      it starts the RTX roboArm process and waits for the end of it
			then closes
@param[in]  int argc | number of input parameters
			_TCHAR*argv[] | array of input parameter strings
@return     
************/
int main (int argc, _TCHAR*argv[])
{
	STARTUPINFO startUpInfo;
	PROCESS_INFORMATION processInfo;
	DWORD ExitCode;
	int ret_val = 0;
	bool waiting = false;
	LARGE_INTEGER waitInterval; waitInterval.QuadPart = 1000;

	ret_val = RtCreateProcess(
		TEXT("C:\\mrts\\roboArm.rtss"), 
		TEXT("C:\\mrts\\roboArm.rtss C:\\mrts\\control.txt"), 
		NULL, NULL, FALSE, 0, NULL, NULL, &startUpInfo, &processInfo);
	if (ret_val == 0)
	{
		RtPrintf("Create process failed with : %04x\n", GetLastError());
	}
	else
	{
		RtPrintf("Successfully created process!\n");
		RtPrintf("Waiting. Process id: %d\n", processInfo.dwProcessId);
	}
	RtSleepFt(&waitInterval);
	while(!waiting)
	{
		ret_val = RtGetExitCodeProcess(processInfo.hProcess, &ExitCode);
		if (ExitCode == STILL_ACTIVE)
		{
			RtPrintf("RTX Process roboArm is still active!\n");
		}
		else
		{
			RtPrintf("RTX Process roboArm ended with exit code %d\n", ExitCode);
			break;
		}
		RtSleepFt(&waitInterval);
	}
	//WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	RtPrintf("Exiting roboArm_starter.\n");
	return (FLAWLESS_EXECUTION);
}