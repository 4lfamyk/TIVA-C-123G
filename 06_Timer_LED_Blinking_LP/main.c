/*
 * Objective : To blink an LED using the Timer Module
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"


/**************** MAIN FUNCTION ********************/

int main(void) {

			SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);	//Configure clock at 16MHz
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//Enable Port F
			GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);	//Enable PF1,PF2 & PF3 as GPIO Output
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0);	//Put LOW on all three pins
			SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);	//Enable TIMER5 Module
			TimerConfigure(TIMER5_BASE,TIMER_CFG_PERIODIC);	//Configure TIMER5 into a Continuous Mode
			TimerLoadSet(TIMER5_BASE, TIMER_A,80000000 );	//Set the Max Value of the timer
			TimerEnable(TIMER5_BASE,TIMER_A);	//Start the timer
			while(1)
			{
				if(TimerValueGet(TIMER5_BASE,TIMER_A)<40000000)	// Monitor the value of timer and change the value on the pins accordingly
				{
					GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2);
				}
				else
					GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0);
			}
			return 0;
}

