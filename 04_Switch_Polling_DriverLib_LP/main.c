/*
 * Objective : To poll a switch using Tivaware functions
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"

/**************** MAIN FUNCTION ********************/

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);	//Setup system clock at 80MHz
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//Enable GPIOF

/* If you want to use the PF0 as the switch then unlock the pin using the following commands */
	//HWREG(GPIO_PORTF_BASE | GPIO_O_LOCK) = GPIO_LOCK_KEY;
	//HWREG(GPIO_PORTF_BASE | GPIO_O_CR) = GPIO_PIN_0;
	//HWREG(GPIO_PORTF_BASE | GPIO_O_LOCK) = 0;

	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);	//Set PF4 as the input pin
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	// Configure the input pin PF4

    /*Setting Pin2 of PortF as Output*/
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2);	//Set PF2 as output
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0); //Set PF2 LOW

	while(1)
	{

		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0)	//If the button connected to PF4 is pressed
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);	//Put PF2 HIGH
		else	//otherwise
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);	//Set PF2 LOW

	}
	
	return 0;
}
