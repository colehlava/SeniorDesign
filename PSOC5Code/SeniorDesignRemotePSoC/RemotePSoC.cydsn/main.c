/* The remote PSoC code.  This is the code that runs on the PSoC connected to the arm.
* It will recieve data from the RPi and adjust the PWM outputs accordingly.
* It will also read the temperature sensor voltage and send that back over USB to the RPi.
*/

#include "project.h"
#include <stdio.h>
#define TRUE 1
#define FALSE 0

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

