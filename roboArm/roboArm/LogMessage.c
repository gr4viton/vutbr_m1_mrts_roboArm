#include "LogMessage.h"


/**
 * @name 	LogMessage
 * @brief	function logs a message into the application log stream in the format: DD.MM.YYYY HH:MM:SS:MSS Message
 * @param [in]	iSeverity - an importance of the message in the range <SEVERITY_MIN, SEVERITY_MAX>. Messages lower than SEVERITY_LEVEL are not being logged
 * @param [in]	cMessage - a message to be logged
 * @param [in]	bBlocking - if 0 the function is blocking (synchronous write operation), otherwise function is non-blocking (asynchronous write operation with buffering)
 * @retval	0 - function succeeded
 * @retval	otherwise - an error occurred
*/
int LogMessage(int iSeverity, char *cMessage, int bBlocking){
	// Create
	HANDLE Hfile;      
	
	// Write
	DWORD BytesWritten;
	CHAR  DataBuffer[512];

	// TIME
	FILETIME FileTime; 
	SYSTEMTIME SystemTime;
	LARGE_INTEGER pTime;
	
	if ( iSeverity > SEVERITY_MAX || SEVERITY_MIN > iSeverity ){
		RtPrintf("\nLogMessage() Error: Invalid severity %d: %s\n", iSeverity , cMessage);
		return 1;
	}

	if ( iSeverity < SEVERITY_LEVEL){
		return 0;
	}

	if ( RtGetClockTime(CLOCK_1,&pTime) == FALSE ){
		RtPrintf("\nLogMessage() Error: RtGetClockTime\n");
		return 1;
	}
	FileTime.dwLowDateTime = pTime.LowPart;
	FileTime.dwHighDateTime = pTime.HighPart;
	if ( FileTimeToSystemTime(&FileTime,&SystemTime) == 0 ){
		RtPrintf("\nLogMessage() Error: FileTimeToSystemTime\n");
		return 1;
	}
	sprintf_s(DataBuffer,512,"%02d.%02d.%04d %02d:%02d:%02d.%03d %02d: %s \r\n",
		(int)SystemTime.wDay,(int)SystemTime.wMonth,(int)SystemTime.wYear,
		(int)SystemTime.wHour,(int)SystemTime.wMinute,(int)SystemTime.wSecond, (int)SystemTime.wMilliseconds ,//(int)SystemTime.wMilliseconds,
		iSeverity,cMessage);

	// CreateFile
	Hfile = CreateFile(
				TEXT(LOG_FILE),
				GENERIC_WRITE ,
				0,
				0,
				OPEN_ALWAYS,
				0,
				0
			);
	
	if ( Hfile == INVALID_HANDLE_VALUE ) {	
		RtPrintf("\nLogMessage() Error: Could not create file\r\n");
		return 1;
	}

		if ( SetFilePointer(	
			Hfile,
			0,
			NULL,
			FILE_END
		) == 0xFFFFFFFF ){
		RtPrintf("\nLogMessage() Error: SetFilePointer failed\r\n");
		return 1;
	}
	
	// WriteFile
	if ( WriteFile(
			Hfile,
			(LPCVOID) DataBuffer,
			strlen(DataBuffer),
			(LPDWORD) &BytesWritten,
			0
		) == FALSE ) {
		RtPrintf("\nLogMessage() Error: Could not wrtie to file\r\n"); 
		return 1;
	}

#if defined LOG_SCREEN
	RtPrintf("%s",DataBuffer);
#endif
	
	// [DD:MM:YYYY HH:MM:SS:MSS] 
	 if (CloseHandle(Hfile) == FALSE)
     {
		RtPrintf("\nLogMessage() Error: Could not close file\r\n"); 
        return 1;
     }

	return 0;
}