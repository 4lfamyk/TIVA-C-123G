/*
 * Objective :To control the intensity of the on-board LED using PWM Generator
 *
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"


/**************** MAIN FUNCTION ********************/

int main(void) {
		SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_OSC|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // 80MHz, System Clock Set up
		SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);		//Enable PWM Module 1
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//Enable GPIO Port F
		SysCtlPWMClockSet(SYSCTL_PWMDIV_16);			//Setup PWM Clock Frequency (to 80MHz)
		GPIOPinConfigure(GPIO_PF2_M1PWM6);				//Enables PWM Mode on the GPIO Pin
		GPIOPinTypePWM(GPIO_PORTF_BASE,GPIO_PIN_2);
		//Configure the PWM Generator Settings - Down Counting and Independent Channels
		PWMGenConfigure(PWM1_BASE,PWM_GEN_3,PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);
		//Sets the time period of one PWM cycle in terms of number of PWM clock cycles
		PWMGenPeriodSet(PWM1_BASE,PWM_GEN_3, 1500);
		//Sets the ON time (and therefore the duty cycle) of the PWM output
		PWMPulseWidthSet(PWM1_BASE,PWM_OUT_6, 300);
		//Enables the PWM on to the output pins and starts the generator
		PWMGenEnable(PWM1_BASE, PWM_GEN_3);
		PWMOutputState(PWM1_BASE,PWM_OUT_6_BIT,true);
		while(1){

		}
		return 0;
}

