/*
 * Objective : To control the intensity of the on-board LED using software PWM
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


/************ FUNCTION DECLARATIONS ****************/

void pin_pwm(unsigned long, unsigned char);		//Increase and Decreases the Intensity (Duty Cycle of the PWM)

/**************** MAIN FUNCTION ********************/

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |	SYSCTL_XTAL_16MHZ);		//Configure the System clock to run at 16MHz
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);		//Enable GPIOF Module
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);		//Configure PF1 as a GPIO pin
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,0);		//Set PF1 as LOW

	while(1){
		pin_pwm(GPIO_PORTF_BASE, GPIO_PIN_1);
	}
	return 0;
}

/************ FUNCTION DEFINITIONS ****************/

void pin_pwm(unsigned long ulPort, unsigned char ucPin)
{
	volatile unsigned long count=255;	//Control the Increments

	for(count=255;count>=1;count--)
	{
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);			//Turn Off the LED
		SysCtlDelay(count*SysCtlClockGet()/100000);			//Control the OFF time
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);		//Turn On the LED
		SysCtlDelay((256-count)*SysCtlClockGet()/100000);	//Control the ON time => Duty Cycle
	}


	for(count=255;count>=1;count--)
	{
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);
		SysCtlDelay((256-count)*SysCtlClockGet()/100000);
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);
		SysCtlDelay(count*SysCtlClockGet()/100000);
	}
}

