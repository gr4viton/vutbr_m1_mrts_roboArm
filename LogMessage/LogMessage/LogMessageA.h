/***************
@filename		LogMessage.h
@author 		xdavid10, xslizj00 @ FEEC-VUTBR 
@contacts		Bc. Ji�� Sli�		<xslizj00@stud.feec.vutbr.cz>
				Bc. Daniel Dav�dek	<danieldavidek@gmail.com>
@date			2013_12_07
@brief			Log message class
@description	Class C_CircBuffer implements circular buffer and provides read/write operations.
				Class C_LogMessageA uses C_CircBuffer and provides methods for asynchronous logging using mutex.
***************/
#ifndef __LOGMESSAGEA__
#define __LOGMESSAGEA__

#include "main.h"
#define LENGTH_OF_BUFF	512
#define HMUTEX_SHARED_NAME TEXT("C_LogMessageA_hMutex.Name")

#define SEVERITY_MIN 0
#define SEVERITY_MAX 16
#define SEVERITY_LEVEL 7

//#define LOG_FILE "C:\\mrts\\xslizj00\\cv6\\LogMessage.txt"
#define LOG_FILE "D:\\LogMessage.txt"
#define LOG_SCREEN


/****************************************************************************
@class	C_CircBuffer
@brief	circular buffer class
***************/
class C_CircBuffer
{
private:
	char *buf;
	unsigned int Start, End;
	unsigned int freeSpace;

	char ReadOne();
	void WriteOne(char in);
public:
	// Constructor
	C_CircBuffer();
	// Destructor
	~C_CircBuffer();

	unsigned int Write(char *in);
	unsigned int Read(char* out);
	bool IsEmpthy();
};

/****************************************************************************
@class	C_LogMessageA
@brief	class provides methods fo asynchronous logging using mutex
***************/
class C_LogMessageA
{
private:
	// Mutex handle
	HANDLE hMutex;
	// File handle
	HANDLE Hfile; 
	int actSeverity;
	// Circular buffer
	C_CircBuffer *buf;
	// Flag indicating start/stop (true/false) of logging
	bool bLogging;
public:

	// Constructor
	C_LogMessageA();
	// Destructor
	~C_LogMessageA();

	unsigned int PushMessage(char* in, int iSeverity);

	unsigned int WriteBuffToFile();

	void LoggingStart(){bLogging = true;}

	void LoggingStop(){bLogging = false;}

	bool GetState(){return bLogging;}
};

#endif