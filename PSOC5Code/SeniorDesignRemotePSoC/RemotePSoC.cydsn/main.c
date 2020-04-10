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

	// Init Components
	Timer_1_Init();
	Timer_1_Start();
	ADC_SAR_1_Start();
	ADC_SAR_1_StartConvert();
	
	LCD_Char_1_Init();
	
	// USB code needs work
	USBFS_1_Start(USBFS_DEVICE, USBFS_1_5V_OPERATION);

	// wait until device is enumerated by host
	while (0u == USBFS_1_GetConfiguration());
	

	int rawTemp = 0;
	uint8 calTempC[2];
	uint8 calTempF[2];
	
	char string[30];

	int i = 0;
	for(;;){
		if(triggered){
			i++;
			// Read the ADC
			//ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
			rawTemp = ADC_SAR_1_GetResult16();
			
			// calibrate the temp
			calTempC[0] = (rawTemp-480) / 6.5;
			calTempF[0] = (calTempC[0] * 9/5.0) + 32;

			LCD_Char_1_ClearDisplay();
			sprintf(string, "%3dC %3dF", (int)calTempC[0], (int)calTempF[0]);
			//snprintf(string, 30, "%f", 123.45f);
			LCD_Char_1_PrintString(string);
			
			//USBFS_1_LoadInEP(IN_EP_NUM, &calibratedTemp, 4);
			triggered = FALSE;
			
						// Send Data
			// Wait until IN buffer become empty (host has read the data)
			while (USBFS_1_IN_BUFFER_EMPTY != USBFS_1_GetEPState(IN_EP_NUM));
		
			// Put Data into IN endpoint buffer
			USBFS_1_LoadInEP(IN_EP_NUM, calTempC, 1);
		}
	}
}