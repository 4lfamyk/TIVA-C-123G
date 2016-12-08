/*
 * Objective : To perform a UART Echo (Transmitting the received data as it is) on the TIVA Launchpad
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.c"

/**************** MAIN FUNCTION ********************/

int main(void) {
		SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //Calibrate System Clock at 80MHz
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	//Enable Port A
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	//Enable UART0 Module
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	//Enable UART Function on PA0 and PA1
		UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);	//Configure UART0 to use Precision Internal Oscillator(PIOSC) as its clock (16MHz)
		UARTStdioConfig(0, 9600, 16000000);	//Setting up UART1 at 9600 baudrate and 16Meg clock

	    while(1)
	    {
	    	if(UARTCharsAvail(UART0_BASE))
	    	   	UARTCharPut(UART0_BASE,(unsigned char)(UARTCharGet(UART0_BASE))); //If there is any data available on the UART0 RX then put it on the TX
	    }

	return 0;
}
