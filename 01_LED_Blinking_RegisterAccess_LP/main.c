/*
 * Objective : To blink an LED using Register Access code
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1231h6pm.h"

/************ CONSTANTS AND DEFINES ****************/
#define delay_value 200000		//Set the delay duration

/**************** MAIN FUNCTION ********************/

int main(void) {
	 volatile unsigned long ulLoop;
	 SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;	//Enable clock to GPIO F Module
	 ulLoop = SYSCTL_RCGC2_R;	//A dummy read to wait while peripheral is enabled
	 GPIO_PORTF_DIR_R |= 0x02;	//Set PF1 as Output
	 GPIO_PORTF_DEN_R |= 0x02;	//Enable digital output to the PF1
	 GPIO_PORTF_AFSEL_R = 0x00;	//Select GPIO Function of PORT F
	 GPIO_PORTF_DATA_R |= 0x02;	//Set PF1 to be HIGH
	while(1){
		GPIO_PORTF_DATA_R |= 0x02;	//Set PF1 as HIGH
		for(ulLoop = 0; ulLoop < delay_value; ulLoop++);	//Delay
		GPIO_PORTF_DATA_R &= ~(0x02);	//Set PF1 as LOW
		for(ulLoop = 0; ulLoop < delay_value; ulLoop++);	//Delay
	}
	return 0;
}

