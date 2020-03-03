/* The knob interface that sends the data to USB for the RPi */

#include "project.h"

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
	AMux_1_Start();
	ADC_SAR_1_Start();
	USBFS_1_Start(USBFS_DEVICE, USBFS_1_5V_OPERATION);
	
	// declare struct for storing knob values
	uint8 knobs[KNOBCOUNT];	

	for(;;){
		if(triggered){
			// Read the ADC and store to struct
			int i;
			for(i = 0; i < KNOBCOUNT; i++){
				AMux_1_Select(i);
				knobs[i] = ADC_SAR_1_GetResult8();
			}

			USBFS_1_LoadInEP(IN_EP_NUM, knobs, 1);
			triggered = FALSE;
		}
	}
}