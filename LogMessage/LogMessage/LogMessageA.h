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


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								C_CircBuffer
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								C_LogMessageA
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
public:

	// Constructor
	C_LogMessageA();
	// Destructor
	~C_LogMessageA();

	unsigned int PushMessage(char* in, int iSeverity);

	unsigned int WriteBuffToFile();
};

#endif