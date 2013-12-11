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

/****************************************************************************
@class	C_CircBuffer
@brief	circular buffer class
***************/
// Constructor C_CircBuffer
C_CircBuffer::C_CircBuffer()
{
	Start = 0;
	End = 0;
	freeSpace = LENGTH_OF_BUFF;
}

// Destructor C_CircBuffer
C_CircBuffer::~C_CircBuffer()
{
}

bool C_CircBuffer::IsEmpthy()
{
	if(freeSpace == LENGTH_OF_BUFF)return TRUE;
	else return FALSE;
}


unsigned int  C_CircBuffer::Write(char *in)
{
	unsigned int inStrLen = strlen(in);

	// if in message is larger than message max length
	if(inStrLen > LENGTH_OF_MESSAGE)
	{
		return 1;
	}

	// put in message to buffer
	strcpy(buf[End], in);
	End++;
	if(End >= LENGTH_OF_BUFF)End = 0;
	freeSpace--;
	
	return 0;
}

unsigned int C_CircBuffer::Read(char out[LENGTH_OF_MESSAGE_HEAD])
{
	// no message here
	if(freeSpace == LENGTH_OF_BUFF)return 1;
	
	// pull message
	strcpy(out, buf[Start]);
	Start++;
	freeSpace++;
	if(Start >= LENGTH_OF_BUFF)Start = 0;
	if(LENGTH_OF_BUFF-freeSpace < 0)return 0;

	return 0;
}

/****************************************************************************
@class	C_LogMessageA
@brief	class for asynchronous logging
***************/
// Constructor C_LogMessageA
C_LogMessageA::C_LogMessageA()
{
	// init mutex
	hMutex = RtCreateMutex(NULL, FALSE, HMUTEX_SHARED_NAME);

	bLogging = false;
}

// Destructor C_LogMessageA
C_LogMessageA::~C_LogMessageA()
{
	RtReleaseMutex(hMutex);
}

unsigned int C_LogMessageA::PushMessage(char in[LENGTH_OF_MESSAGE], int iSeverity)
{	
	// Time
	FILETIME FileTime; 
	SYSTEMTIME SystemTime;
	LARGE_INTEGER pTime;	

	if ( iSeverity > SEVERITY_MAX || SEVERITY_MIN > iSeverity )
	{
		RtPrintf("\nLogMessage() Error: Invalid severity %d: %s\n", iSeverity , in);
		return 1;
	}

	if ( iSeverity < SEVERITY_LEVEL)
	{
		return 2; // lower than set severity - throw away
	}

	if ( RtGetClockTime(CLOCK_1,&pTime) == FALSE )
	{
		RtPrintf("\nLogMessage() Error: RtGetClockTime\n");
		return 3;
	}
	FileTime.dwLowDateTime = pTime.LowPart;
	FileTime.dwHighDateTime = pTime.HighPart;
	if ( FileTimeToSystemTime(&FileTime,&SystemTime) == 0 )
	{
		RtPrintf("\nLogMessage() Error: FileTimeToSystemTime\n");
		return 4;
	}

	CHAR  DataBuffer[LENGTH_OF_MESSAGE_HEAD];

	if(sprintf_s(DataBuffer,LENGTH_OF_MESSAGE_HEAD,"%02d.%02d.%04d %02d:%02d:%02d.%03d %02d: %s \r\n",
		(int)SystemTime.wDay,(int)SystemTime.wMonth,(int)SystemTime.wYear,
		(int)SystemTime.wHour,(int)SystemTime.wMinute,(int)SystemTime.wSecond, (int)SystemTime.wMilliseconds ,
		iSeverity,in) == -1)
	{
		RtPrintf("\nLogMessage() Error: sprintf_s fail.\n");
		return 5;
	}

	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section
	buf.Write(DataBuffer);
	RtReleaseMutex(hMutex);

	return 0;
}

unsigned int C_LogMessageA::WriteBuffToFile()
{
	if(!bLogging)return 10;

	CHAR cMessage[LENGTH_OF_MESSAGE_HEAD];
	unsigned int err = 0;
	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section
	err = buf.Read(cMessage);
	RtReleaseMutex(hMutex);

	if(err == 1)
	{	
		// buffer is empthy, nothing to log
		return 1;
	}

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