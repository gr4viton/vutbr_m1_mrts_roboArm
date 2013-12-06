#include "LogMessageA.hpp"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								C_CircBuffer
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Constructor C_CircBuffer
C_CircBuffer::C_CircBuffer()
{
	Start = 0;
	End = 0;
	freeSpace = LENGTH_OF_BUFF;

	buf = new char[LENGTH_OF_BUFF];
}

// Destructor C_CircBuffer
C_CircBuffer::~C_CircBuffer()
{
	delete buf;
}

char C_CircBuffer::ReadOne()
{
	char ret = buf[Start];
	Start++;
	freeSpace++;
	if(Start >= LENGTH_OF_BUFF)Start = 0;
	if(LENGTH_OF_BUFF-freeSpace < 0)return 0;
	return ret;
}

void C_CircBuffer::WriteOne(char in)
{
	if(freeSpace > 0)
	{
		buf[End] = in;
		End++;
		if(End >= LENGTH_OF_BUFF)End = 0;
		freeSpace--;
	}
}

unsigned int  C_CircBuffer::Write(char *in)
{
	unsigned int inStrLen = strlen(in);

	if(inStrLen > freeSpace)
	{
		return 1;
	}

	for(unsigned int i = 0 ; i < inStrLen ; i++)
	{
		WriteOne(in[i]);
	}
	return 0;
}

unsigned int C_CircBuffer::Read(char* out)
{
	if(freeSpace == LENGTH_OF_BUFF)return 1;
	
	char r = 1;
	unsigned int len = LENGTH_OF_BUFF-freeSpace;
	unsigned int i;
	for(i = 0 ; i < len; i++)
	{
		r = ReadOne();
		if(r != 0)
		{
			out[i] = r;
		}
		else out[i] = ' ';
	}
	out[i] = '\0';
	return 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								C_LogMessageA
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Constructor C_LogMessageA
C_LogMessageA::C_LogMessageA()
{
	// init buffer
	buf = new C_CircBuffer();
	// init mutex
	hMutex = RtCreateMutex(NULL, FALSE, HMUTEX_SHARED_NAME);
}

// Destructor C_LogMessageA
C_LogMessageA::~C_LogMessageA()
{
	delete buf;
	RtReleaseMutex(hMutex);
}

unsigned int C_LogMessageA::PushMessage(char* in, int iSeverity)
{
	actSeverity = iSeverity;
	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section
	buf->Write(in);
	RtReleaseMutex(hMutex);

	return 0;
}

unsigned int C_LogMessageA::WriteBuffToFile()
{
	CHAR cMessage[LENGTH_OF_BUFF];
	unsigned int err = 0;
	RtWaitForSingleObject(hMutex,INFINITE); // wait to own hMutex
	// Critical section
	err = buf->Read(cMessage);
	RtReleaseMutex(hMutex);

	if(err == 1)return 9;

	// Write to file
	DWORD BytesWritten;
	CHAR  DataBuffer[512];

	// Time
	FILETIME FileTime; 
	SYSTEMTIME SystemTime;
	LARGE_INTEGER pTime;

	if ( actSeverity > SEVERITY_MAX || SEVERITY_MIN > actSeverity )
	{
		RtPrintf("\nLogMessage() Error: Invalid severity %d: %s\n", actSeverity , cMessage);
		return 1;
	}

	if ( actSeverity < SEVERITY_LEVEL)
	{
		return 2;
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
	sprintf_s(DataBuffer,512,"%02d.%02d.%04d %02d:%02d:%02d.%03d %02d: %s \r\n",
		(int)SystemTime.wDay,(int)SystemTime.wMonth,(int)SystemTime.wYear,
		(int)SystemTime.wHour,(int)SystemTime.wMinute,(int)SystemTime.wSecond, (int)SystemTime.wMilliseconds ,//(int)SystemTime.wMilliseconds,
		actSeverity,cMessage);

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
		return 5;
	}

	if ( SetFilePointer(	
			Hfile,
			0,
			NULL,
			FILE_END
		) == 0xFFFFFFFF )
	{
		RtPrintf("\nLogMessage() Error: SetFilePointer failed\r\n");
		return 6;
	}
	
	// WriteFile
	if ( WriteFile(
			Hfile,
			(LPCVOID) DataBuffer,
			strlen(DataBuffer),
			(LPDWORD) &BytesWritten,
			0
		) == FALSE ) 
	{
		RtPrintf("\nLogMessage() Error: Could not wrtie to file\r\n"); 
		return 7;
	}

#if defined LOG_SCREEN
	RtPrintf("%s",DataBuffer);
#endif
	
	// [DD:MM:YYYY HH:MM:SS:MSS] 
	 if (CloseHandle(Hfile) == FALSE)
     {
		RtPrintf("\nLogMessage() Error: Could not close file\r\n"); 
        return 8;
     }

	return 0;
}