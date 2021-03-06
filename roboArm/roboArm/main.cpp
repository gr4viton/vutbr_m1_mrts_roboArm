//////////////////////////////////////////////////////////////////
//
// LogMessage.c - C file
//
// This file was generated using the RTX Application Wizard 
// for Visual Studio.  
//////////////////////////////////////////////////////////////////
#include "main.h"
HANDLE hThread1;
HANDLE hThread2;
C_LogMessageA logMsg;	

//  timer
HANDLE	hTimerPWM;
unsigned int timerCounter;

ULONG RTFCNDCL ThreadHandler(void *nContext);
ULONG RTFCNDCL ThreadHandler2(void *nContext);

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
	logMsg.LoggingStart();
	hThread1 = CreateThread(0, 0, ThreadHandler, NULL, CREATE_SUSPENDED, 0);
	// Handle thread creation fail
	if(hThread1 == NULL)
	{
		printf_s( "Error:\tCould not create 'hThread1'.\n");
		ExitProcess(0);
	}
	hThread2 = CreateThread(0, 0, ThreadHandler2, NULL, CREATE_SUSPENDED, 0);
	// Handle thread creation fail
	if(hThread2 == NULL)
	{
		printf_s( "Error:\tCould not create 'hThread2'.\n");
		ExitProcess(0);
	}
	
	printf( "OK\t\t'hThread1' and 'hThread1' created. Handlet = 0x%X\n", hThread1);

	// Set thread prority
	if(RtSetThreadPriority(hThread1, RT_PRIORITY_MAX - 5))
	{
		printf("OK\t\t'hThread1' priority was set to: RT_PRIORITY_MAX - 5.\n");
	}
	else
	{
		CloseHandle(hThread1);
		printf("Error:\tPriority of 'hThread1' wasn't set\n");
		ExitProcess(0);
	}

	// Set thread prority
	if(RtSetThreadPriority(hThread2, RT_PRIORITY_MAX - 5))
	{
		printf("OK\t\t'hThread2' priority was set to: RT_PRIORITY_MAX - 5.\n");
	}
	else
	{
		CloseHandle(hThread2);
		printf("Error:\tPriority of 'hThread2' wasn't set\n");
		ExitProcess(0);
	}

	// Start hThread1
	if(RtResumeThread(hThread1) != 0XFFFFFFFF)
	{
		printf("OK\t\t'hThread1' resumed.\n");
	}
	else
	{
		CloseHandle(hThread1);
		printf("Error:\t'hThread1' cannot be resumed.\n");
		ExitProcess(0);
	}

	// Start hThread2
	if(RtResumeThread(hThread2) != 0XFFFFFFFF)
	{
		printf("OK\t\t'hThread2' resumed.\n");
	}
	else
	{
		CloseHandle(hThread2);
		printf("Error:\t'hThread2' cannot be resumed.\n");
		ExitProcess(0);
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//		Test asyn. log message
	logMsg.PushMessage("Test message 1", SEVERITY_MAX - 1);
	logMsg.PushMessage("Test message 2", SEVERITY_MAX - 1);
	logMsg.PushMessage("Test message 3", SEVERITY_MAX - 1);


	// Timer test
	timerCounter = 0;
	LARGE_INTEGER timerLastTime, minTmrPeriod, tmrPeriod;
	timerLastTime.QuadPart = 0;
	char strMessage[256];
	hTimerPWM = RtCreateTimer(NULL, 0, OnTimerPWMTick, NULL, RT_PRIORITY_MAX, CLOCK_3);	// Create timer
	if(hTimerPWM == NULL)
	{
		sprintf_s(strMessage, 256, "Error: Timer was not created.\r\n");
		logMsg.PushMessage(strMessage, SEVERITY_MAX - 1);
	}
	if(!RtGetClockTimerPeriod(CLOCK_3, &minTmrPeriod))									// Get minimum timer period
	{
		sprintf_s(strMessage, 256, "Error: Timer period was not set.\r\n");
		logMsg.PushMessage(strMessage, SEVERITY_MAX - 1);
	}
	tmrPeriod.QuadPart = minTmrPeriod.QuadPart * 10;				
	printf("\nPeriod = %I64d\n", tmrPeriod.QuadPart);
	RtSetTimerRelative(hTimerPWM, &tmrPeriod, &tmrPeriod);								// Set required (1ms) timer period

	Sleep(1000);		// 1s

	LARGE_INTEGER cancelCode;
	RtCancelTimer(hTimerPWM, &cancelCode);												// Close timer
	if(hTimerPWM != NULL)RtDeleteTimer(hTimerPWM);										// Delete timer


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	logMsg.LoggingStop();
	// Waiting for thread end
	bool allThreadsEnded;
	DWORD ExitCode;
	while(1)
	{
		allThreadsEnded = TRUE;
			if(hThread1 != NULL || hThread1 != NULL)
			{
				if ( GetExitCodeThread(hThread1,&ExitCode) == TRUE )
				{
					if(ExitCode != STILL_ACTIVE)
					{
						printf("OK\t\t'hThread1' has ended.\n");
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

				if ( GetExitCodeThread(hThread1,&ExitCode) == TRUE )
				{
					if(ExitCode != STILL_ACTIVE)
					{
						printf("OK\t\t'hThread2' has ended.\n");
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
	while(logMsg.GetState())
	{
		logMsg.WriteBuffToFile();
		Sleep(10);
	}

	ExitThread(0);
}

ULONG RTFCNDCL ThreadHandler2(void *nContext) 
{
	// test multithread logging OK :]
	logMsg.PushMessage("Test message 4", SEVERITY_MAX - 1);
	Sleep(5);
	logMsg.PushMessage("Test message 5", SEVERITY_MAX - 1);
	logMsg.PushMessage("Test message 6", SEVERITY_MAX - 1);
	ExitThread(0);
}

// TimerPWM handler
void RTFCNDCL OnTimerPWMTick(PVOID context)
{
	timerCounter++;
	if(timerCounter >= 100)
	{
		timerCounter = 0;
		logMsg.PushMessage("Timer tick", SEVERITY_MAX - 1);
	}
}