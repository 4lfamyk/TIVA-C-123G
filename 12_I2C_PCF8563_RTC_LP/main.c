/*
 * Objective : To interface PCF8563 (Real Time Clock) using I2C
 */

/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "utils/uartstdio.c"


/************ CONSTANTS AND DEFINES ****************/

#define NUM_I2C_DATA 17
#define SLAVE_ADDRESS 0x51

/************ FUNCTION DECLARATIONS ****************/

void InitConsole(void);		//Initializes and configures UART Communication
unsigned char BcdToDec(unsigned char);		//Converts a given BCD number to Decimal equivalent
unsigned char DecToBcd(unsigned char value);		//Converts a given Decimal number to BCD equivalent

/**************** MAIN FUNCTION ********************/

int main(void) {
	unsigned char second;
	unsigned char second_old;
	unsigned char minute;
	unsigned char month;
	unsigned char ulDataRx[7];
	char* weekdayname[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	int ulindex, i=0;
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);	//Configure the system clock at 80MHz
	InitConsole();	//Initialize the UART Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	//Enable Port A
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);	//Enable I2C1 module
	GPIOPinConfigure(GPIO_PA6_I2C1SCL);	//Enable I2C Output on PA6
	GPIOPinConfigure(GPIO_PA7_I2C1SDA);	//Enable I2C Output on PA7
	GPIOPinTypeI2C(GPIO_PORTA_BASE,GPIO_PIN_7);	//Configure PA6 as I2C SCL Pin
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE,GPIO_PIN_6);	//Configure PA7 as I2C SDA Pin
	I2CMasterDisable(I2C1_BASE);	//Disable the I2C1 Module
	I2CMasterInitExpClk(I2C1_BASE,SysCtlClockGet(), false);	//Configure the I2C1 Module - Clock and Mode of operation
	I2CMasterEnable(I2C1_BASE);	//Enable the I2C1 module
	UARTprintf("Hello\n");
	while(1)
	{
		I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);	//Initialize I2C Module at System Clock
		I2CMasterEnable(I2C1_BASE);		//Enable I2C Module
		I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, false);		//Setup Module to TX data to RTC
		I2CMasterDataPut(I2C1_BASE, 0x02);		//Send 0x02
		I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);	//Start Sending
		while(I2CMasterBusy(I2C1_BASE));	//Wait till operation completed
		I2CMasterSlaveAddrSet(I2C1_BASE, SLAVE_ADDRESS, true);		//Setup Modules to RX data from RTC
		I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);		//Start RX data
		while(I2CMasterBusy(I2C1_BASE));	//Wait till operation is completed
		ulDataRx[0] = I2CMasterDataGet(I2C1_BASE);		//Get the data Recieved
		second = BcdToDec(((unsigned char)ulDataRx[0]) & 0b01111111);	//Obtain the value of seconds from the RXed data
		UARTprintf("%d ",second);
		if (second != second_old) // Cycle begins only when it has changed
			{
				UARTprintf("1st %d",second);
				second_old = second;	//Store the seconds value
				if (second == 0) // If second value is 0, then changes need to be made in the minute value
				{
					I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);		//Setup I2C Module to Receive a single byte of data
					while(I2CMasterBusy(I2C1_BASE));	//Wait till operation is completed
					ulDataRx[1] = I2CMasterDataGet(I2C1_BASE);		//Receive the data byte
					minute = BcdToDec(((unsigned char)ulDataRx[1]) & 0b01111111);	//Extract Minute value from the data byte
					if (minute != minute_old) // If minute has changed, only then proceed
					{
						minute_old = minute;	//Save the minute
						if (minute == 0) // If minute is zero, then changes need to be made in the hour as well
						{
							I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);		//Setup the I2C Module to receive a single byte
							while(I2CMasterBusy(I2C1_BASE));	//Wait till operation is completed
							ulDataRx[2] = I2CMasterDataGet(I2C1_BASE);		//Receive the data byte
							hour = BcdToDec(((unsigned char)ulDataRx[2]) & 0b01111111);		//Extract the hour value from the byte
							if (hour != hour_old) // Changes need to be made only when hour value has changed
							{
								hour_old = hour;	//Save the hour value
								if (hour == 0) // If hour is zero, other elements might need to be updated
								{
									for(ulindex = 3; ulindex < 7; ulindex++)
									{

										I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);		//Setup module to receive a single byte of data
										while(I2CMasterBusy(I2C1_BASE));	//Wait till operation is completed
										ulDataRx[ulindex] = I2CMasterDataGet(I2C1_BASE);	//Receive the data
									}
									day= BcdToDec(((unsigned char)ulDataRx[3]) & 0b01111111);	//Extract day
									weekday = BcdToDec(((unsigned char)ulDataRx[4]) & 0b01111111);		//Extract weekday
									month = (unsigned char)ulDataRx[5];		//Extract month & century
									century = (month & 0x80);		//Extract century
									month = BcdToDec(month & 0b01111111);	//Extract month
									year=BcdToDec(((unsigned char)ulDataRx[6]) & 0b01111111);	//Extract year
								}
							}
						}
					}
				}
				I2CMasterDisable(I2C1_BASE);	//Disable the I2C Module and print the data:
				if (century)
					UARTprintf(" 21");
				else
					UARTprintf(" 20");
				if (year < 10)
					UARTprintf("0");
				UARTprintf("%d",year);
				UARTprintf("-");
				if (month < 10)
					UARTprintf("0");
					UARTprintf("%d",month);
					UARTprintf("-");
				if (day < 10)
					UARTprintf("0");
					UARTprintf("%d\t",day);
					UARTprintf("");
				if (hour < 10)
					UARTprintf("0");
					UARTprintf("%d",hour);
					UARTprintf(":");
				if (minute < 10) UARTprintf("0");
					UARTprintf("%d",minute);
					UARTprintf(":");
				if (second < 10)
					UARTprintf("0");
					UARTprintf("%d\n",second);
				for(ulindex=0; ulindex<20000;ulindex++);
			}
		}

	return 0;
}

/************ FUNCTION DEFINITIONS ****************/

void InitConsole(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	//Enable PORT A Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);	//Enable UART0 Module
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);		//Configure PA0 and PA1 as UART Pins
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);		//Configure UART0 Module clock source as the Precision Internal OSCillator
	UARTStdioConfig(0, 115200, 16000000);	//Configure the settings of UART0 Module (Buadrate : 115200 ; CLock : 16MHz)
}


unsigned char BcdToDec(unsigned char value)
{
	return ((value / 16) * 10 + value % 16);	//Convert BCD number to Decimal number
}


unsigned char DecToBcd(unsigned char value)
{
	return (value / 10 * 16 + value % 10);		//Convert Decimal Number to BCD number
}
