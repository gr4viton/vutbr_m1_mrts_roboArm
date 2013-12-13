/***********
@project  	roboArm
@filename	LogMessageA.cpp
@author 		xdavid10, xslizj00 @ FEEC-VUTBR 
@contacts	Bc. Jiøí Sliž		<xslizj00@stud.feec.vutbr.cz>
			Bc. Daniel Davídek	<danieldavidek@gmail.com>
@date		2013_12_07
@brief		Log message class
@descrptn	Class C_CircBuffer implements circular buffer and provides read/write operations.
			Class C_LogMessageA uses C_CircBuffer and provides methods for asynchronous logging using mutex.
***********/

#include "LogMessageA.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class C_CircBuffer - circular buffer class - member function definitions

/****************************************************************************
@class		C_CircBuffer
@function   C_CircBuffer
@brief      Constructor
@param[in]	-
@param[out] -
@return     -
************/
C_CircBuffer::C_CircBuffer()
{
	Start = 0;
	End = 0;
	freeSpace = LENGTH_OF_BUFFER;
}

/****************************************************************************
@class		C_CircBuffer
@function   ~C_CircBuffer
@brief      Destructor
@param[in]	-
@param[out] -
@return     -
************/
C_CircBuffer::~C_CircBuffer()
{

}

/****************************************************************************
@class		C_CircBuffer
@function   IsEmpty
@brief      is buffer empty?
@param[in]	-
@param[out] -
@return     (bool) return true if the bufer is empty
************/
bool C_CircBuffer::IsEmpty()
{
	if(freeSpace == LENGTH_OF_BUFFER) return true;
	else return false;
}

/****************************************************************************
@class		C_CircBuffer
@function   strcpySafe
@brief      safe version of strcpy
@param[out]	(char*) destination string
@param[in]	(char*) input string
@return     (DWORD)
************/
DWORD C_CircBuffer::strcpySafe(char *dest, char *in)
{
	// secure strlen
	unsigned int inStrLen = 0;
	for(inStrLen=0; in[inStrLen] != 0; inStrLen++){
		if(inStrLen > LENGTH_OF_BUFFER) return(ERROR_STRING_TO_WRITE_IS_TOO_LONG);
	}

	for(DWORD i = 0 ; i <= inStrLen ; i++)
	{
		dest[i] = in[i];
	}

	return FLAWLESS_EXECUTION;
}

/****************************************************************************
@class		C_CircBuffer
@function   WriteOne
@brief      Write one message to buffer
@param[in]	(char*) input string
@param[out] -
@return     (DWORD) viz. returnCodeDefines.h
************/
DWORD  C_CircBuffer::Write(char *in)
{
	// secure strlen
	unsigned int inStrLen = 0;
	for(inStrLen=0; in[inStrLen] != '\0'; inStrLen++){
		if(inStrLen > LENGTH_OF_BUFFER) return(ERROR_STRING_TO_WRITE_IS_TOO_LONG);
	}
	// is there enaugh space?
	if(inStrLen > freeSpace) return(ERROR_NOT_ENAUGH_SPACE_IN_BUFFER);

	// put in message to buffer
	strcpySafe(buf[End], in);
	End++;
	if(End >= LENGTH_OF_BUFFER) End = 0;
	freeSpace--;
	
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@class		C_CircBuffer
@function   Read
@brief      Reads one message from buffer
@param[in]	-
@param[out] (char*) output string
@return     (DWORD) viz. returnCodeDefines.h
************/
DWORD C_CircBuffer::Read(char out[MAX_FULL_MESSAGE_LENGTH])
{
	// no message here
	if(freeSpace == LENGTH_OF_BUFFER) return(ERROR_BUFFER_IS_EMPTY);
	
	// pull message
	strcpySafe(out, buf[Start]);
	Start++;
	freeSpace++;
	if(Start >= LENGTH_OF_BUFFER)Start = 0;
	if(LENGTH_OF_BUFFER-freeSpace < 0)return 0;

	return(FLAWLESS_EXECUTION);
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// class C_LogMessageA - methods for asynchronous logging - member function definitions

/****************************************************************************
@class		C_LogMessageA
@function   C_LogMessageA
@brief      Constructor
@param[in]	-
@param[out] -
@return     -
************/
C_LogMessageA::C_LogMessageA()
{
	// init mutex
	hMutex = RtCreateMutex(NULL, FALSE, HMUTEX_SHARED_NAME);
	bLogging = false;
}

/****************************************************************************
@class		C_LogMessageA
@function   ~C_LogMessageA
@brief      Destructor
@param[in]	-
@param[out] -
@return     -
************/
C_LogMessageA::~C_LogMessageA()
{
	RtReleaseMutex(hMutex);
}

/****************************************************************************
@class		C_LogMessageA
@function   PushMessage
@brief      pushes message into the buffer
@param[in]	(char*)
			(int)
@param[out] -
@return     (DWORD)error_sum | viz. returnCodeDefines.h
************/
DWORD C_LogMessageA::PushMessage(char* inMsg, int iSeverity, int bBlocking)
{	
	//____________________________________________________
	// Severities
	if ( iSeverity > SEVERITY_MAX )
	{
		RtPrintf("\nLogMessage() Error: Severity is too high [%d] > max[%d]: %s\n", 
			iSeverity, SEVERITY_MAX, inMsg);
		return ERROR_SEVERITY_BIGGER_THAN_MAX;
	}
	else if( SEVERITY_MIN > iSeverity )
	{
		RtPrintf("\nLogMessage() Error: Severity is too low [%d] < min[%d]: %s\n", 
			iSeverity, SEVERITY_MIN, inMsg);
		return ERROR_SEVERITY_LOWER_THAN_MIN;
	}

	if ( iSeverity < SEVERITY_LEVEL)
	{
		return LOG_IS_NOT_SEVERE_ENAUGH;
	}
	
	// Time
	FILETIME			FileTime; 
	SYSTEMTIME		SystemTime;
	LARGE_INTEGER	ClockTime;	
	//____________________________________________________
	// Clock time -> file time -> system time
	if ( RtGetClockTime(CLOCK_LOG_ACTUAL, &ClockTime) == FALSE )
	{
		RtPrintf("\nLogMessage() Error: RtGetClockTime\n");
		return ERROR_COULD_NOT_GET_CLOCKTIME;
	}
	FileTime.dwLowDateTime = ClockTime.LowPart;
	FileTime.dwHighDateTime = ClockTime.HighPart;
	if ( FileTimeToSystemTime(&FileTime, &SystemTime) == 0 )
	{
		RtPrintf("\nLogMessage() Error: FileTimeToSystemTime\n");
		if(bBlocking == 0)
			return ERROR_FILETIMETOSYSTEMTIME_FAIL;
		else
			return ERROR_FILETIMETOSYSTEMTIME_FAIL;
	}

	//____________________________________________________
	// get string length
	DWORD error_sum;
	DWORD strlen = GET_stringLength(inMsg, MAX_MESSAGE_LENGTH, &error_sum);
	if(error_sum != FLAWLESS_EXECUTION) {
		RtPrintf("\nLogMessage() Error: String to write is too long\n");
		return(ERROR_STRING_LENGHT_LARGER_THAN_TRESHOLD);
	}
	// filter all ending new-line characters (\n)
	while(inMsg[strlen-1] == '\n') 
	{
		inMsg[strlen-1]='\0';
		strlen--;
	}
	// possible --> FILTER_invalidMessageChars();
	
	CHAR  DataBuffer[MAX_FULL_MESSAGE_LENGTH];
	// write only needed count of chars - do not write chars past the zero byte
	if(sprintf_s(DataBuffer, strlen + MESSAGE_HEAD_LENGTH, 
		"%02d.%02d.%04d %02d:%02d:%02d.%03d %02d: %s%s",
		(int)SystemTime.wDay,(int)SystemTime.wMonth,(int)SystemTime.wYear,
		(int)SystemTime.wHour,(int)SystemTime.wMinute,(int)SystemTime.wSecond, (int)SystemTime.wMilliseconds ,
		iSeverity, 
		inMsg,
		LOGMSG_LINE_END
		) == -1)
	{
		RtPrintf("\nLogMessage() Error: sprintf_s fail.\n");
		return ERROR_SPRINTF_S_FAIL;
	}
	
#if defined SHOW_LOG_ON_SCREEN
	// write it before it is written into file -> 
	// when debugging you see it as you step through program 
	// and not after writing to file after a few steps
	RtPrintf("%s", DataBuffer);
#endif

	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section [START]
	buf.Write(DataBuffer);
	RtReleaseMutex(hMutex); // Critical section [END]
	
	return(FLAWLESS_EXECUTION);	
}

/****************************************************************************
@class		C_LogMessageA
@function   WriteBuffToFile
@brief      writes whole buffer to the file
@param[in]	-
@param[out] -
@return     (DWORD) viz. returnCodeDefines.h
************/
DWORD C_LogMessageA::WriteBuffToFile()
{
	if(buf.IsEmpty())return ERROR_BUFFER_IS_EMPTY;

	CHAR cMessage[MAX_FULL_MESSAGE_LENGTH];
	DWORD err = FLAWLESS_EXECUTION;
	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section [Start]
	err = buf.Read(cMessage);
	RtReleaseMutex(hMutex);	// Critical section [End]

	if(err != FLAWLESS_EXECUTION) return(err);
	
	//____________________________________________________
	// CreateFile
	Hfile = CreateFile(
				TEXT(LOG_FILE_PATH),
				GENERIC_WRITE ,
				0,
				0,
				OPEN_ALWAYS,
				0,
				0
			);
	
	if ( Hfile == INVALID_HANDLE_VALUE ) 
	{	
		RtPrintf("\nLogMessage() Error: Could not create file\r\n");
		return ERROR_CREATEFILE_FAIL;
	}

	//____________________________________________________
	// SetFilePointer
	if ( SetFilePointer(	Hfile, 0, NULL, FILE_END) == 0xFFFFFFFF )
	{
		RtPrintf("\nLogMessage() Error: SetFilePointer failed\r\n");
		return( CLOSE_handleAndReturn(Hfile,ERROR_SETFILEPOINTER_FAIL) );
	}
	
	//____________________________________________________
	// WriteFile
	DWORD BytesWritten;	
	// secure strlen
	unsigned int inStrLen = 0;
	for(inStrLen=0; cMessage[inStrLen] != 0; inStrLen++){
		if(inStrLen > LENGTH_OF_BUFFER) return(ERROR_STRING_TO_WRITE_IS_TOO_LONG);
	}
	if ( WriteFile(Hfile, (LPCVOID) cMessage, 
		(DWORD) inStrLen, (LPDWORD) &BytesWritten, 0
		) == FALSE ) 
	{
		RtPrintf("\nLogMessage() Error: Could not wrtie to file\r\n");
		return( CLOSE_handleAndReturn(Hfile, ERROR_WRITEFILE_FAIL) );
	}
	
	// [DD:MM:YYYY HH:MM:SS:MSS] 
	
	return( CLOSE_handleAndReturn(Hfile, FLAWLESS_EXECUTION, false) );
}


