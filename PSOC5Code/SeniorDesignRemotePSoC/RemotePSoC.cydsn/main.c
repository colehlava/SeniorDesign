/* The remote PSoC code.  This is the code that runs on the PSoC connected to the arm.
* It will recieve data from the RPi and adjust the PWM outputs accordingly.
* It will also read the temperature sensor voltage and send that back over USB to the RPi.
*/

#include "project.h"
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// USB Device Number
#define USBFS_DEVICE (0u)
// Active endpoints of USB device
#define IN_EP_NUM (1u)


int triggered = FALSE;

CY_ISR(TimerInt){
	triggered = TRUE;
}

int main(void){
	CyGlobalIntEnable; /* Enable global interrupts. */
	TimerInt_StartEx(TimerInt);
	
	LCD_Char_1_Init();

	//start USBFS operation with 5V
	USBFS_1_Start(USBFS_DEVICE, USBFS_1_5V_OPERATION);

	// wait until device is enumerated by host
	int flag = 1;
	while (0u == USBFS_1_GetConfiguration()){
		if(flag){
			LCD_Char_1_PrintString("Waiting for enum");
			flag = 0;
		}
	}
	LCD_Char_1_PrintString("Done with enum");
	
	// Init Components
	Timer_1_Init();
	Timer_1_Start();
	ADC_SAR_1_Start();
	ADC_SAR_1_StartConvert();
	
	ADC_SAR_2_Start();
	ADC_SAR_2_StartConvert();

	int rawTemp = 0;
	float calTempC;
	float calTempF;
	
	int rawHumid = 0;
	float calHumid;
	
	char string[30];
	char readings[12];
	uint8 otherReadings[12];

	int i = 0;
	for(;;){
		if(triggered){
			i++;
			// Read the ADC
			rawTemp = ADC_SAR_1_GetResult16();
			rawHumid = ADC_SAR_2_GetResult16();
			
			// calibrate the temp
			calTempC = (rawTemp-480) / 6.1;
			calTempF = (calTempC * 9/5.0) + 32;

			// calibrate the humidity
			calHumid = ((rawHumid*5/4096.0) - 0.826)/0.0315;
			
			LCD_Char_1_ClearDisplay();
			sprintf(string, "%3dC %3dF", (int)calTempC, (int)calTempF);
			LCD_Char_1_PrintString(string);
			sprintf(string, "%4d RH", (int)calHumid);
			LCD_Char_1_Position(1,0);
			LCD_Char_1_PrintString(string);
			
			triggered = FALSE;
			
			// Send Data
			// Wait until IN buffer become empty (host has read the data)
			while (USBFS_1_IN_BUFFER_EMPTY != USBFS_1_GetEPState(IN_EP_NUM));
			sprintf(readings, "%04d, %04d\n", (int)calTempC, (int)calHumid);
			
			//convert string to uint8
			int loop = 0;
			int i = 0;
			while(readings[loop] != '\0'){
				otherReadings[i++] = (uint8)readings[loop++];
			}
			
			// Put Data into IN endpoint buffer
			USBFS_1_LoadInEP(IN_EP_NUM, otherReadings, 12);
		}
	}
}