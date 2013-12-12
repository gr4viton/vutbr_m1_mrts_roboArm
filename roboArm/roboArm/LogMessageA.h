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

//#define LOGMSG_DEV

#ifdef LOGMSG_DEV
//	#include "main.h"
#else
	#include "roboArm.h"
#endif

#include "returnCodeDefines.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// macro definitions
//____________________________________________________
// VARIABLES
// #define SEVERITY_LEVEL			7 --> defined in roboArm.h
// buffer dimensions - user choise
#define LENGTH_OF_BUFFER			256
#define MAX_MESSAGE_LENGTH		512

#define LOG_SCREEN		// comment for hide logs on screen

#define LOG_FILE			"C:\\mrts\\LogMessage.txt"
//#define LOG_FILE				"D:\\LogMessage.txt"
#define LOGMSG_LINE_END		"\n"


//____________________________________________________
// CONSTANTS - do not modify
#define HMUTEX_SHARED_NAME				TEXT("C_LogMessageA_hMutex.Name")
//(30 is for tame, date, severity)
#define MESSAGE_HEAD_LENGTH				30
#define MAX_FULL_MESSAGE_LENGTH			MAX_MESSAGE_LENGTH + MESSAGE_HEAD_LENGTH

// Severities
#define SEVERITY_MIN		0
#define SEVERITY_MAX		16



/****************************************************************************
@class	C_CircBuffer
@brief	circular buffer class
***************/
class C_CircBuffer
{
private:
	char buf[LENGTH_OF_BUFFER][MAX_FULL_MESSAGE_LENGTH];	// buffer array 
	unsigned int Start, End;	// actual start and end positions
	unsigned int freeSpace;		// actual free space
	unsigned int strcpySafe(char *dest, char *in);

public:
	C_CircBuffer();
	~C_CircBuffer();

	unsigned int Write(char *inMsg);
	unsigned int Read(char out[MAX_FULL_MESSAGE_LENGTH]);
	bool IsEmpty();
};

/****************************************************************************
@class	C_LogMessageA
@brief	class provides methods fo asynchronous logging using mutex
***************/
class C_LogMessageA
{
//____________________________________________________
// member variables
private:
	HANDLE hMutex;		// Mutex handle
	HANDLE Hfile;		// File handle
	C_CircBuffer buf;	// Circular buffer
	bool bLogging;		// Flag indicating start/stop (true/false) of logging

public:
//____________________________________________________
// declaration of external defined member functions 
	C_LogMessageA();
	~C_LogMessageA();

	unsigned int PushMessage(char in[MAX_MESSAGE_LENGTH], int iSeverity);
	unsigned int WriteBuffToFile();

	void LoggingStart(){bLogging = true;}
	void LoggingStop(){bLogging = false;}
	bool GetState(){return bLogging;}
};

#endif
