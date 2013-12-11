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

#include "main.h"
//#include "roboArm.h"

#define LENGTH_OF_BUFF	256
#define LENGTH_OF_MESSAGE	512
#define LENGTH_OF_MESSAGE_HEAD	512 + 30 //(30 is for tame, date, severity)
#define HMUTEX_SHARED_NAME TEXT("C_LogMessageA_hMutex.Name")

#define SEVERITY_MIN 0
#define SEVERITY_MAX 16
#define SEVERITY_LEVEL 7

//#define LOG_FILE "C:\\mrts\\xslizj00\\cv6\\LogMessage.txt"
#define LOG_FILE "D:\\LogMessage.txt"
#define LOG_SCREEN	// comment for hide logs on screen


/****************************************************************************
@class	C_CircBuffer
@brief	circular buffer class
***************/
class C_CircBuffer
{
private:
	char buf[LENGTH_OF_BUFF][LENGTH_OF_MESSAGE_HEAD];	// buffer array 
	unsigned int Start, End;	// actual start and end positions
	unsigned int freeSpace;		// actual free space

public:
	// Constructor
	C_CircBuffer();
	// Destructor
	~C_CircBuffer();

	unsigned int Write(char *in);
	unsigned int Read(char out[LENGTH_OF_MESSAGE_HEAD]);
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
	// Circular buffer
	C_CircBuffer buf;
	// Flag indicating start/stop (true/false) of logging
	bool bLogging;
public:

	// Constructor
	C_LogMessageA();
	// Destructor
	~C_LogMessageA();

	unsigned int PushMessage(char in[LENGTH_OF_MESSAGE], int iSeverity);

	unsigned int WriteBuffToFile();

	void LoggingStart(){bLogging = true;}

	void LoggingStop(){bLogging = false;}

	bool GetState(){return bLogging;}
};

#endif
