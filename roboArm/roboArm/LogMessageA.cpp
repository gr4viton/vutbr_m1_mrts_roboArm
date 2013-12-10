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

	buf = new char[LENGTH_OF_BUFFER];
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
	delete buf;
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
	if(freeSpace == LENGTH_OF_BUFFER)return TRUE;
	else return FALSE;
}

/****************************************************************************
@class		C_CircBuffer
@function   ReadOne
@brief      
@param[in]	-
@param[out] -
@return     (char)
************/
char C_CircBuffer::ReadOne()
{
	char ret = buf[Start];
	Start++;
	freeSpace++;
	if(Start >= LENGTH_OF_BUFFER)Start = 0;
	if(LENGTH_OF_BUFFER-freeSpace < 0)return 0;
	return ret;
}

/****************************************************************************
@class		C_CircBuffer
@function   WriteOne
@brief      
@param[in]	(char) 
@param[out] -
@return     -
************/
void C_CircBuffer::WriteOne(char in)
{
	if(freeSpace > 0)
	{
		buf[End] = in;
		End++;
		if(End >= LENGTH_OF_BUFFER)End = 0;
		freeSpace--;
	}
}

/****************************************************************************
@class		C_CircBuffer
@function   WriteOne
@brief      
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

	for(unsigned int i = 0 ; i < inStrLen ; i++)
	{
		WriteOne(in[i]);
	}
	return(FLAWLESS_EXECUTION);
}

/****************************************************************************
@class		C_CircBuffer
@function   Read
@brief      
@param[in]	-
@param[out] (char*)
@return     (unsigned int)
************/
unsigned int C_CircBuffer::Read(char* out)
{
	if(freeSpace == LENGTH_OF_BUFFER) return(ERROR_BUFFER_IS_EMPTY);
	
	char r = 1;
	unsigned int len = LENGTH_OF_BUFFER-freeSpace;
	unsigned int i;
	for(i = 0 ; i < len; i++)
	{
		r = ReadOne();
		if(r != 0)
			out[i] = r;
		else	
			out[i] = ' ';
	}
	out[i] = '\0';
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
	// init buffer
	buf = new C_CircBuffer();
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
	delete buf;
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
unsigned int C_LogMessageA::PushMessage(char* in, int iSeverity)
{
	printf(in);
	// FOR DEBUGGING - uncoment after
	/*
	if(!bLogging)
	{
		RtPrintf("(Logging blocked)\t%s\n",in);
		return 1;
	}
	actSeverity = iSeverity;
	while(!buf->IsEmpty())Sleep(50);
	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section [START]
	buf->Write(in);	
	RtReleaseMutex(hMutex); // Critical section [END]
	*/
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
	// try if loggign is not locked
	if(!bLogging) return(ERROR_LOGGING_IS_LOCKED);

	CHAR cMessage[LENGTH_OF_BUFFER];
	unsigned int err = FLAWLESS_EXECUTION;
	RtWaitForSingleObject(hMutex, INFINITE); // wait to own hMutex
	// Critical section [START]
	err = buf->Read(cMessage);
	RtReleaseMutex(hMutex); // Critical section [END]

	if(err != FLAWLESS_EXECUTION) return(err);

	// Write to file
	DWORD BytesWritten;
	CHAR  DataBuffer[512];

	// Time
	FILETIME FileTime; 
	SYSTEMTIME SystemTime;
	LARGE_INTEGER pTime;
	
	if ( actSeverity > SEVERITY_MAX )
	{
		RtPrintf("\nLogMessage() Error: Severity is too high [%d] > max[%d]: %s\n", 
			actSeverity, SEVERITY_MAX, cMessage);
		return ERROR_SEVERITY_BIGGER_THAN_MAX;
	}
	else if( SEVERITY_MIN > actSeverity )
	{
		RtPrintf("\nLogMessage() Error: Severity is too low [%d] < min[%d]: %s\n", 
			actSeverity, SEVERITY_MIN, cMessage);
		return ERROR_SEVERITY_LOWER_THAN_MIN;
	}

	if ( actSeverity < SEVERITY_LEVEL)
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
	//sprintf_s(DataBuffer,512,"%02d.%02d.%04d %02d:%02d:%02d.%03d %02d: %s \r\n",
	sprintf_s(DataBuffer,512,"%02d.%02d.%04d %02d:%02d:%02d.%03d: %s \r\n",
		(int)SystemTime.wDay,  (int)SystemTime.wMonth,  (int)SystemTime.wYear,
		(int)SystemTime.wHour, (int)SystemTime.wMinute, (int)SystemTime.wSecond, (int)SystemTime.wMilliseconds ,
		cMessage);
		//actSeverity, cMessage);

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
		return ERROR_CREATEFILE_FAIL;
	}

	if ( SetFilePointer(	
			Hfile,
			0,
			NULL,
			FILE_END
		) == 0xFFFFFFFF )
	{
		RtPrintf("\nLogMessage() Error: SetFilePointer failed\r\n");
		return( CLOSE_handleAndReturn(Hfile,ERROR_SETFILEPOINTER_FAIL) );
	}
	
	// WriteFile
	// bezpeèný strLEN !!! %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if ( WriteFile(
			Hfile,
			(LPCVOID) DataBuffer,
			(DWORD) strlen(DataBuffer),
			(LPDWORD) &BytesWritten,
			0
		) == FALSE ) 
	{
		RtPrintf("\nLogMessage() Error: Could not wrtie to file\r\n");
		return( CLOSE_handleAndReturn(Hfile, ERROR_WRITEFILE_FAIL) );
	}
	
	// [DD:MM:YYYY HH:MM:SS:MSS] 
#if defined SHOW_LOG_ON_SCREEN
	RtPrintf("%s",DataBuffer);
#endif
	
	return( CLOSE_handleAndReturn(Hfile, FLAWLESS_EXECUTION) );
}