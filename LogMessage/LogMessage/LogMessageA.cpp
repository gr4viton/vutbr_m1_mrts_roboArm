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
bool C_CircBuffer::IsEmpthy()
{
	if(freeSpace == LENGTH_OF_BUFFER)return TRUE;
	else return FALSE;
}

/****************************************************************************
@class		C_CircBuffer
@function   Write
@brief      write message to buffer.
@param[in]	(char*)
@param[out] -
@return     (unsigned int)
************/
unsigned int  C_CircBuffer::Write(char *in)
{
	// secure strlen
	unsigned int inStrLen = 0;
	for(inStrLen=0; in[inStrLen] != 0; inStrLen++){
		if(inStrLen > LENGTH_OF_BUFFER) return(ERROR_STRING_TO_WRITE_IS_TOO_LONG);
	}
	// is there enaugh space?
	if(inStrLen > freeSpace) return(ERROR_NOT_ENAUGH_SPACE_IN_BUFFER);

	// put in message to buffer
	strcpy(buf[End], in);
	End++;
	if(End >= LENGTH_OF_BUFFER)End = 0;
	freeSpace--;
	
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@class		C_CircBuffer
@function   Read
@brief      read message from the beginning of buffer.
@param[in]	-
@param[out] (char*)
@return     (unsigned int)
************/
unsigned int C_CircBuffer::Read(char out[LENGTH_OF_MESSAGE_HEAD])
{
	// no message here
	if(freeSpace == LENGTH_OF_BUFFER) return(ERROR_BUFFER_IS_EMPTY);
	
	// pull message
	strcpy(out, buf[Start]);
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
@return     (unsigned int)
************/
unsigned int C_LogMessageA::PushMessage(char in[LENGTH_OF_MESSAGE], int iSeverity)
{	
	// Time
	FILETIME FileTime; 
	SYSTEMTIME SystemTime;
	LARGE_INTEGER pTime;	

	if ( iSeverity > SEVERITY_MAX )
	{
		RtPrintf("\nLogMessage() Error: Severity is too high [%d] > max[%d]: %s\n", 
			iSeverity, SEVERITY_MAX, in);
		return ERROR_SEVERITY_BIGGER_THAN_MAX;
	}
	else if( SEVERITY_MIN > iSeverity )
	{
		RtPrintf("\nLogMessage() Error: Severity is too low [%d] < min[%d]: %s\n", 
			iSeverity, SEVERITY_MIN, in);
		return ERROR_SEVERITY_LOWER_THAN_MIN;
	}

	if ( iSeverity < SEVERITY_LEVEL)
	{
		return LOG_IS_NOT_SEVERE_ENAUGH;
	}

	if ( RtGetClockTime(CLOCK_1,&pTime) == FALSE )
	{
		RtPrintf("\nLogMessage() Error: RtGetClockTime\n");
		return ERROR_COULD_NOT_GET_CLOCKTIME;
	}
	FileTime.dwLowDateTime = pTime.LowPart;
	FileTime.dwHighDateTime = pTime.HighPart;
	if ( FileTimeToSystemTime(&FileTime,&SystemTime) == 0 )
	{
		RtPrintf("\nLogMessage() Error: FileTimeToSystemTime\n");
		return ERROR_FILETIMETOSYSTEMTIME_FAIL;
	}

	CHAR  DataBuffer[LENGTH_OF_MESSAGE_HEAD];

	if(sprintf_s(DataBuffer,LENGTH_OF_MESSAGE_HEAD,"%02d.%02d.%04d %02d:%02d:%02d.%03d %02d: %s \r\n",
		(int)SystemTime.wDay,(int)SystemTime.wMonth,(int)SystemTime.wYear,
		(int)SystemTime.wHour,(int)SystemTime.wMinute,(int)SystemTime.wSecond, (int)SystemTime.wMilliseconds ,
		iSeverity,in) == -1)
	{
		RtPrintf("\nLogMessage() Error: sprintf_s fail.\n");
		return ERROR_SPRINTF_S_FAIL;
	}

	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section
	buf.Write(DataBuffer);
	RtReleaseMutex(hMutex);

	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@class		C_LogMessageA
@function   WriteBuffToFile
@brief      writes whole buffer to the file
@param[in]	(char*)
			(int)
@param[out] -
@return     (unsigned int)
************/
unsigned int C_LogMessageA::WriteBuffToFile()
{
	if(!bLogging)return 10;

	CHAR cMessage[LENGTH_OF_MESSAGE_HEAD];
	unsigned int err = 0;
	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section
	err = buf.Read(cMessage);
	RtReleaseMutex(hMutex);

	if(err != FLAWLESS_EXECUTION) return(err);

	// Write to file
	DWORD BytesWritten;	

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
	
	if ( Hfile == INVALID_HANDLE_VALUE ) 
	{	
		RtPrintf("\nLogMessage() Error: Could not create file\r\n");
		return 2;
	}

	if ( SetFilePointer(	
			Hfile,
			0,
			NULL,
			FILE_END
		) == 0xFFFFFFFF )
	{
		RtPrintf("\nLogMessage() Error: SetFilePointer failed\r\n");
		return 3;
	}
	
	// WriteFile
	if ( WriteFile(
			Hfile,
			(LPCVOID) cMessage,
			strlen(cMessage),
			(LPDWORD) &BytesWritten,
			0
		) == FALSE ) 
	{
		RtPrintf("\nLogMessage() Error: Could not wrtie to file\r\n"); 
		return 4;
	}

#if defined LOG_SCREEN
	RtPrintf("%s",cMessage);
#endif
	
	// [DD:MM:YYYY HH:MM:SS:MSS] 
	 if (CloseHandle(Hfile) == FALSE)
     {
		RtPrintf("\nLogMessage() Error: Could not close file\r\n"); 
        return 5;
     }

	return 0;
}