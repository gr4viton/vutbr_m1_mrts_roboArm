/***********
@project  	roboArm
@filename	LogMessageA.h
@author 		xdavid10, xslizj00 @ FEEC-VUTBR 
@contacts	Bc. Jiøí Sliž		<xslizj00@stud.feec.vutbr.cz>
			Bc. Daniel Davídek	<danieldavidek@gmail.com>
@date		2013_12_07
@brief		Log message class
@descrptn	Class C_CircBuffer implements circular buffer and provides read/write operations.
			Class C_LogMessageA uses C_CircBuffer and provides methods for asynchronous logging using mutex.
***********/
#ifndef __LOGMESSAGEA__
#define __LOGMESSAGEA__

//#include "main.h"
#include "roboArm.h"

#define LENGTH_OF_BUFF	512
#define HMUTEX_SHARED_NAME TEXT("C_LogMessageA_hMutex.Name")

#define SEVERITY_MIN 0
#define SEVERITY_MAX 16
#define SEVERITY_LEVEL 7

//#define LOG_FILE "C:\\mrts\\xslizj00\\cv6\\LogMessage.txt"
#define LOG_FILE "D:\\LogMessage.txt"
#define SHOW_LOG_ON_SCREEN	// comment for hide logs on screen


/****************************************************************************
@class	C_CircBuffer
@brief	circular buffer class
***************/
class C_CircBuffer
{
private:
	char *buf;					// buffer array
	unsigned int Start, End;		// actual start and end positions
	unsigned int freeSpace;		// actual free space

	char ReadOne();				// method for read one symbol
	void WriteOne(char in);		// method for write one symbol
public:
	// Constructor
	C_CircBuffer();
	// Destructor
	~C_CircBuffer();

	unsigned int Write(char *in);
	unsigned int Read(char* out);
	bool IsEmpty();
};

/****************************************************************************
@class	C_LogMessageA
@brief	class provides methods for asynchronous logging using mutex
***************/
class C_LogMessageA
{
//____________________________________________________
// member variables
private:
	HANDLE hMutex;		// Mutex handle
	HANDLE Hfile;		// File handle
	int actSeverity;
	C_CircBuffer *buf;	// Circular buffer
	bool bLogging;		// Flag indicating start/stop (true/false) of logging

public:
//____________________________________________________
// declaration of external defined member functions 
	C_LogMessageA();
	~C_LogMessageA();

	unsigned int PushMessage(char* in, int iSeverity);
	unsigned int WriteBuffToFile();

	void LoggingStart(){bLogging = true;}
	void LoggingStop(){bLogging = false;}
	bool GetState(){return bLogging;}
};

#endif
