/*
 * Objective :
 * Author :
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/************ CONSTANTS AND DEFINES ****************/

uint32_t adcValue[2];

/************ FUNCTION DECLARATIONS ****************/


/**************** MAIN FUNCTION ********************/

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //Configuring System Clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);		// Enable ADC0 Module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);	// Enable GPIO B Module
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);	// Select ADC Function of PB 5
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);	// Configure ADC0 Sequencer
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH11 | ADC_CTL_IE | ADC_CTL_END);	// Configure the steps of the sequencer
    ADCSequenceEnable(ADC0_BASE, 2);	//Enable ADC0 Sequencer
    ADCIntClear(ADC0_BASE, 2);		//Clear Existing Interrupts

    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 2);		// Start Sampling
        while(!ADCIntStatus(ADC0_BASE, 2, false));	// Wait until sampling is done
        ADCIntClear(ADC0_BASE, 2);	//Clear Interrupt
        ADCSequenceDataGet(ADC0_BASE, 2, adcValue);	//Obtain the sample
        SysCtlDelay(SysCtlClockGet() / 12);
    }
    return 0;
}


/************ FUNCTION DEFINITIONS ****************/
