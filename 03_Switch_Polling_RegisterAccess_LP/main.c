/*
 * Objective : Interface the onboard switch of the Launchpad using Register Access
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"


/**************** MAIN FUNCTION ********************/

int main(void) {
	
    volatile unsigned long ulLoop;	//A dummy variable
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;	//Enable Port F
    ulLoop = SYSCTL_RCGC2_R;	//Wait till peripheral is enabled (dummy read)
    GPIO_PORTF_DIR_R |= 0x02;	//Set PF1 as OUTPUT
    GPIO_PORTF_DEN_R |= 0x02;	//Enable Digital Outputs on PF1
    GPIO_PORTF_DIR_R &= ~(0x10);	//Set PF4 as Input
    GPIO_PORTF_DEN_R |= 0x10;	//Enable Digital Outputs on PF4
    GPIO_PORTF_AFSEL_R = 0x00;	//Select GPIO Functionality for PORT F
    GPIO_PORTF_PUR_R |= 0x10;	//Enabling Internal Pullup for PF4

    while(1)
    {
        // If the button is pressed, put HIGH on PF1 else put LOW on PF1
    	if(((GPIO_PORTF_DATA_R) & (0x10)) != (0X10))
    		GPIO_PORTF_DATA_R |= 0x02;
    	else
    		GPIO_PORTF_DATA_R &= ~(0x02);
    }
    return 0;
}

