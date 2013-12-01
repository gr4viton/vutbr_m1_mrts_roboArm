//////////////////////////////////////////////////////////////////
//
// roboArm.c - C file
//
// This file was generated using the RTX Application Wizard 
// for Visual Studio.  
//////////////////////////////////////////////////////////////////
    
#include "roboArm.h"

// global Variables
//volatile static DWORD baseAddress = 0;
DWORD baseAddress = 0;

//static HANDLE hLibModule = NULL;
HMODULE hLibModule = NULL;
/*
typedef struct _S_servo {
	DWORD max_val = 1;
	DWORD min_val = 0;
	DWORD mean_vals = 5;
} S_servo;
*/

// mean of c values
DWORD MEAN_adc(int channel, int gain, int c){
	DWORD sum=0;
	//DWORD *vals;
	//vals = (DWORD*)malloc(sizeof(DWORD));
	int i = c;
	for(i;i>=0;i--){
		//vals[c-1] = GET_ADC(channel);
		sum += GET_ADC(channel, gain);
	}
	//free(vals);
	return(sum/c);
}



// main
void _cdecl main(int  argc, char **argv, char **envp)
{
	//double angles[6];
	int ret = 0;
	int i = 0;
	UCHAR pom = 0;
	//DWORD data;
	int num = 5; // number_of_mean_values
	//UCHAR StartPort = ((UCHAR)base);

	printf("--------(:Clean start :)------\n");
	// Load library
	ret = INIT_Library();
	if(ret)printf("no :(\n");		
	else printf("Library was opened successfully :)\n");

	// Initialize ADC
	INIT_ADC();

	
	LARGE_INTEGER interval_one; 
	LARGE_INTEGER interval_zero; 
	LARGE_INTEGER interval_wait; 
	

	interval_one.QuadPart = 10000; // 1000us
	interval_zero.QuadPart = 200000; // 0.02s = 50Hz
	interval_wait.QuadPart = 50000000; // 5s

	int j = 0;
	int max_j = 7;
	for(j = max_j;j>0;j--){
		for(i = 2;i>0;i--)
		{
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 0x80);
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 255); // big
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 127);
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 63);
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 31);
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 15); // servo 
			//RtWritePortUchar((PUCHAR)(baseAddress+DO_High_Byte), 7); // servo 
			//HightByte 1<<7 = servo  1
			RtWritePortUchar((PUCHAR)(baseAddress+DO_Low_Byte), 1<<j);
			RtSleepFt(&interval_one);
			RtWritePortUchar((PUCHAR)(baseAddress+DO_Low_Byte), 0x00);
			RtSleepFt(&interval_zero);
		}
		printf("Servo %i/%i\n", j+1,max_j);
		RtSleepFt(&interval_wait);
	}
	// Reading Data 
	while(1)
	{
		/*
			MEAN_adc(0,0,num);
			//MEAN_adc(1,0,num);
			//MEAN_adc(2,0,num);
		/**/
		RtPrintf("FDBACK[1,2,3] = %5u;\t\t%5u;\t\t%5u;\n",
			MEAN_adc(0,0,num),
			MEAN_adc(1,0,num),
			MEAN_adc(2,0,num)
			);
		//*/
/*
		data = GET_ADC(0);
		RtPrintf("FDBACK1=%5u\t",data);
		data = GET_ADC(1);
		RtPrintf("FDBACK2=%5u\t",data);
		data = GET_ADC(2);

		RtPrintf("FDBACK3=%5u\n",data);	
*/

		RtSleep(100);
	}
	
    ExitProcess(0);
}

DWORD GET_ADC(UCHAR channel, UCHAR gain)
{
	UCHAR ready;									

	UCHAR mp;

	UCHAR ADlow;								
	UCHAR ADhigh;								

	DWORD val;									
	unsigned short c;

	// set gain and mux

	// x|x|MUX3|MUX2|MUX1|MUX0|GAIN1|GAIN0
	// MUX[3-0] = binary number selecting from 0to15 shifted left 2 
	//	0b0000 0000<<2 = 0x00<<2 = 0x00 = 0<<2 = AI0
	//	0b0000 0001<<2 = 0x01<<2 = 0x04 = 1<<2 = AI1
	//	0b0000 0010<<2 = 0x02<<2 = 0x08 = 2<<2 = AI2
	//	0b0000 0100<<2 = 0x03<<2 = 0x0C = 3<<2 = AI3
	// etc
	gain = 0;
	mp = channel<<2 | gain;

	RtWritePortUchar((PUCHAR)(baseAddress+AD_GAIN), mp);

	RtSleep(1);

	RtWritePortUchar((PUCHAR)(baseAddress+AD_SOFT_TRIGGER_START), 0x01);

	for ( c = 0, ready = 0x00 ; (ready == 0x00) && (c <= 10) ; c++ ) {
		ready = RtReadPortUchar((PUCHAR)(baseAddress+AD_STATUS));		
		ready = ready & 0x01;
	}

	if ( c > 10 ) {
		RtPrintf("GetADC: Reading timeout\n");
		return 0;		
	}

	ADlow = RtReadPortUchar((PUCHAR)(baseAddress+AD_LOWBYTE));
	ADhigh = RtReadPortUchar((PUCHAR)(baseAddress+AD_HIGHBYTE));
	val = ADhigh;
	val = val << 8;
	val = val | ADlow;
	/*
	RtPrintf("control byte = 0x%08x;\t\tADlow = %i;\t\tADhigh = %i;\t\tval = %lu\n",
		channel, ADlow, ADhigh, val);
	*/	
	return val;
}

