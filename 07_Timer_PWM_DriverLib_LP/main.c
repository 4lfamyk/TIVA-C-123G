/*
 * Objective : Configure Timer module in PWM mode and drive on-board LEDs with it
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1231h6pz.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"


/************ CONSTANTS AND DEFINES ****************/

unsigned long i;

/************ FUNCTION DECLARATIONS ****************/

void low_to_high(void);		//Increases the duty cycle of the PWM from 0 to 100%
void high_to_low(void);		//Decreases the duty cycle of the PWM from 100% to 0

/**************** MAIN FUNCTION ********************/

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);			//Configure Clock to run at 16MHz
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);		//Enable TIMER1 Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);		//Enable Port F
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);		//Configure PF2 as a Timer output pin
	GPIOPinConfigure(GPIO_PF2_T1CCP0);			//Set Pin Configuration to Timer Module pin
	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);	//Configure TIMER A of TIMER 1 Module in Split Pair PWM mode
	TimerLoadSet(TIMER1_BASE, TIMER_A, 50000);		//Load Maximum value to the counter to be 50000
	TimerEnable(TIMER1_BASE, TIMER_A);		//Start the Timer

	while(1){
		low_to_high();
		high_to_low();
	}
	return 0;
}

/************ FUNCTION DEFINITIONS ****************/

void low_to_high(void)
{
	for(i=19;i>1;i--)
	{
		TimerMatchSet(TIMER1_BASE, TIMER_A, TimerLoadGet(TIMER1_BASE, TIMER_A)*i/20);	//Load the compare value of the Timer (Sets the duty cycle)
		TimerEnable(TIMER1_BASE, TIMER_A);	//Start the timer
		SysCtlDelay(SysCtlClockGet()/50);	//Provide delay between consecutive changes to the duty cycle
  	}
}

void high_to_low(void)
{
	for(i=1;i<20;i++)
	{
		TimerMatchSet(TIMER1_BASE, TIMER_A, TimerLoadGet(TIMER1_BASE, TIMER_A)*i/20);
		TimerEnable(TIMER1_BASE, TIMER_A);
		SysCtlDelay(SysCtlClockGet()/50);
   }
}
