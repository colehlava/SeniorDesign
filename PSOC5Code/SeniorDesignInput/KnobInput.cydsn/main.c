/* The knob interface that sends the data to USB for the RPi */

#include "project.h"
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// how many potentiometers
#define KNOBCOUNT 6

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
	AMux_1_Start();
	ADC_SAR_1_Start();
	ADC_SAR_1_StartConvert();
	
	LCD_Char_1_Init();
	
	// USB code needs work
	USBFS_1_Start(USBFS_DEVICE, USBFS_1_5V_OPERATION);

	// wait until device is enumerated by host
	while (0u == USBFS_1_GetConfiguration());
	
	// declare struct for storing knob values
	uint8 knobs[KNOBCOUNT];
	char string1[15];
	char string2[15];

	int j;
	for(;;){
		if(triggered){
			// Read the ADC and store to struct
			int i;
			for(i = 0; i < KNOBCOUNT; i++){
				AMux_1_Select(i);
				for(j = 0; j < 2<<6; j++);
				ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
				knobs[i] = ADC_SAR_1_GetResult16()>>4;
			}
			sprintf(string1, "%3d %3d %3d", knobs[5], knobs[4], knobs[3]);
			sprintf(string2, "%3d %3d %3d", knobs[2], knobs[1], knobs[0]);
			LCD_Char_1_ClearDisplay();
			LCD_Char_1_PrintString(string1);
			LCD_Char_1_Position(1,0);
			LCD_Char_1_PrintString(string2);
			
			triggered = FALSE;
		
			// Send Data
			// Wait until IN buffer become empty (host has read the data)
			while (USBFS_1_IN_BUFFER_EMPTY != USBFS_1_GetEPState(IN_EP_NUM));
		
			// Put Data into IN endpoint buffer
			USBFS_1_LoadInEP(IN_EP_NUM, knobs, KNOBCOUNT);
		}
	}
}