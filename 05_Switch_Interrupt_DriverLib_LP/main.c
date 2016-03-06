/*
 * Objective : To use a switch as an interrupt source
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/systick.h"

/************ CONSTANTS AND DEFINES ****************/
volatile unsigned long g_ulcount = 0;
unsigned long i;

/************ FUNCTION DECLARATIONS ****************/

void Pin_Int(void);		//The ISR declaration

/**************** MAIN FUNCTION ********************/

int main(void) {
		SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);	//Configuring system clock to 16MHz
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//Enabling Port F
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 );	//Setting PF2 to Output
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);	//Setting PF4 to Input
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 , 0 );		//Set PF0 to LOW
		GPIOIntRegister(GPIO_PORTF_BASE, Pin_Int);		//Register Interrupt for Port F with ISR Pin_Int()
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);		//Clear any existing interrupts
		GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);		//Configuring the pins
		GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);		//Setting Interrupt to trigger on falling edges
		GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);		//Enable the GPIO Interrupts on Port F
		IntEnable(INT_GPIOF);	//Enable Interrupts on Port F
		IntMasterEnable();	//Enable Global interrupts
		while(1);
	return 0;
}

/************ FUNCTION DEFINITIONS ****************/

void Pin_Int(void)
{
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);		//Clear the generated interrupt
	for(i=0;i<=2000;i++);		//Provide a small delay to provide for bouncing
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0) //If the button is still pressed
		{
			if(g_ulcount==0)		//If g_ulcount is 0 then put HIGH on PF2
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
				g_ulcount =1;
			}
			else				//Otherwise put LOW on PF2
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
				g_ulcount=0;
			}
		}
	}
