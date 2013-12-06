//////////////////////////////////////////////////////////////////
//
// LogMessage.c - C file
//
// This file was generated using the RTX Application Wizard 
// for Visual Studio.  
//////////////////////////////////////////////////////////////////
#include "main.h"
C_LogMessageA CLogMsg;
HANDLE hThread;
C_LogMessageA logMsg;	

ULONG RTFCNDCL ThreadHandler(void *nContext);

bool bLogThreadRun = FALSE;

void  _cdecl main(int  argc, char **argv, char **envp)
{
	//// Test C_CircBuffer
	//C_CircBuffer buff;
	//char pStr[512];
	//
	//for(int i = 0 ; i < 50 ; i++)
	//{
	//	// Write to circular buffer
	//	unsigned int err = buff.Write("Test string");

	//	// Read
	//	err = buff.Read(pStr);

	//	printf("%s", pStr);
	//}	
	bLogThreadRun = TRUE;
	hThread = CreateThread(0, 0, ThreadHandler, NULL, CREATE_SUSPENDED, 0);

	// Handle thread creation fail
	if(hThread == NULL)
	{
		printf_s( "Error:\tCould not create 'hThread'.\n");
		ExitProcess(0);
	}
	
	printf( "OK\t\t'hThread' created. Handlet = 0x%X\n", hThread);

	// Set thread prority
	if(RtSetThreadPriority(hThread, RT_PRIORITY_MAX - 5))
	{
		printf("OK\t\t'hThread' priority was set to: RT_PRIORITY_MAX - 5.\n");
	}
	else
	{
		CloseHandle(hThread);
		printf("Error:\tPriority of 'hThread' wasn't set\n");
		ExitProcess(0);
	}

	// Start hThread
	if(RtResumeThread(hThread) != 0XFFFFFFFF)
	{
		printf("OK\t\t'hThread' resumed.\n");
	}
	else
	{
		CloseHandle(hThread);
		printf("Error:\t'hThread' cannot be resumed.\n");
		ExitProcess(0);
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//		Test asyn. log message
	logMsg.PushMessage("Test message 1", SEVERITY_MAX - 1);
	Sleep(100);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
bLogThreadRun = FALSE;	
	// Waiting for thread end
	bool allThreadsEnded;
	DWORD ExitCode;
	while(1)
	{
		allThreadsEnded = TRUE;
			if(hThread != NULL)
			{
				if ( GetExitCodeThread(hThread,&ExitCode) == TRUE )
				{
					if(ExitCode != STILL_ACTIVE)
					{
						printf("OK\t\t'hThread' has ended.\n");
					}
					else
					{
						allThreadsEnded = FALSE;
						RtSleep(10);
					}
				}
				else
				{
					DWORD err = GetLastError();
					printf("Error:\tGetLastError()=%04x\n", err);
				}
			}
		
		if(allThreadsEnded)break;
	}

    ExitProcess(0);
}

ULONG RTFCNDCL ThreadHandler(void *nContext) 
{
	while(bLogThreadRun)
	{
		logMsg.WriteBuffToFile();
		Sleep(10);
	}

	ExitThread(0);
}