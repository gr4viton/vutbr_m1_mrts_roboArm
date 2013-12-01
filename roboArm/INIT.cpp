
#include "roboArm.h"


void INIT_ADC()
{
	UCHAR data;

	// Reset
	data = RtReadPortUchar((PUCHAR)(baseAddress));
	data = data | 0x01;
	RtWritePortUchar((PUCHAR)(baseAddress), data);
	
	// Set software triggering
	RtWritePortUchar((PUCHAR)(baseAddress+AD_MODE_CONTROLL), 0xf1);

	RtSleep(100);
}



int INIT_Library()
{
	LPCSTR lpLibFileName = "dac_dll.rtdll";
	LPCSTR lpProcName = "GetPIO821BaseAddress";
	FARPROC functionPointer = NULL;
	// load Library 
	hLibModule = LoadLibrary(lpLibFileName);
	// check if loadLibrary returned correctly 
	if(hLibModule== NULL) {
		/* ERROR */
		printf("Error:\tCould not load the library.\n");
		return 1;
	}
	// Get function from Rtdll 
	functionPointer = GetProcAddress( hLibModule, lpProcName) ;
	// check if function was found 
	if(functionPointer == NULL) 
	{	// ERROR 
		printf("Error:\tCould not find address.\n");
		FreeLibrary(hLibModule);
		return 2;
	}
	// Call function
	baseAddress = functionPointer();
	printf("base = %i = hex = %x \n", baseAddress, baseAddress);
	// Free Library 
	FreeLibrary(hLibModule);
	return 0;
}