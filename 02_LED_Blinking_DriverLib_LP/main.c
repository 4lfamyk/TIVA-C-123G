/*
 * Objective : To blink an LED (connected on PF2) on the TIVA Launchpad
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


/**************** MAIN FUNCTION ********************/

int main(void) {

		SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);	//Calibrating System Clock for 16MHz
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//Enabling Port F as GPIO
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);	//Enabling GPIO Output for PF2
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,0); // Putting a LOW on PF2

	    while(1)
	    {
	    	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,GPIO_PIN_2);	//Putting a HIGH on PF2
	        SysCtlDelay(SysCtlClockGet()/3);	// Inserting a delay for 1s
	        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,0); //Putting a LOW on PF2
	        SysCtlDelay(SysCtlClockGet()/3); // Inserting a delay for 1s

	    }
		return 0;
}

