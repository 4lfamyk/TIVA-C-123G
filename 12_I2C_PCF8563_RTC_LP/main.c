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
unsigned char ran='0';

/************ FUNCTION DECLARATIONS ****************/

void InitConsole(void);		//Initializes and configures UART Communication
unsigned char BcdToDec(unsigned char);		//Converts a given BCD number to Decimal equivalent
unsigned char DecToBcd(unsigned char value);		//Converts a given Decimal number to BCD equivalent

/**************** MAIN FUNCTION ********************/

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);	//Configure System Clock
	InitConsole();
	UARTprintf("RTC SETUP\n");
	//Declare various variables to be used in the code
    unsigned char second;
	unsigned char second_old;
	unsigned char minute;
	unsigned char minute_old;
	unsigned char hour;
	unsigned char hour_old;
	unsigned char weekday;
	unsigned char day;
	unsigned char month;
	unsigned char year;
	unsigned char century =0;
	unsigned char ulDataTx[17];
	unsigned long ulDataRx[7];
	int ulindex;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);		//Enable I2C0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);	//Enable GPIO B Module

	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);	//Initialize PB2 and PB3 as I2C Pins
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE,GPIO_PIN_2);	//Initialize PB2 as SCL

	 second_old = second = 10; // Second (0-59)
	 minute_old = minute = 52; // Minute (0-59)
	 hour_old = hour = 12 ; // Hour (0-23)
	 weekday = 1; // Day of the week (0-6)
	 day = 20; // Day (1-31)
	 month = 7; // Month (1-12)
	 year = 15; // Year (0-99)

	UARTprintf("RTC SETUP\n");
	UARTprintf("DATE\t");
	UARTprintf("TIME\n");
	UARTprintf("Rate = 100kbps\n\n");

	// Initialize the array to be sent
	ulDataTx[0]=0;
	ulDataTx[1]=0;
	ulDataTx[2]=0;
	ulDataTx[3]=DecToBcd(second);
	ulDataTx[4]=DecToBcd(minute);
	ulDataTx[5]=DecToBcd(hour);
	ulDataTx[6]=DecToBcd(day);
	ulDataTx[7]=DecToBcd(weekday);
	ulDataTx[8]=DecToBcd(month);
	ulDataTx[9]=DecToBcd(year);
	ulDataTx[10]=0b10000000;
	ulDataTx[11]=0b10000000;
	ulDataTx[12]=0b10000000;
	ulDataTx[13]=0b10000000;
	ulDataTx[14]=0b10000000;
	ulDataTx[15]=0;
	ulDataTx[16]=0;

	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);	//Setup Clock to the I2C0 Module
	I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);		//Provide the Slave Address of the RTC in the Write Mode

	UARTprintf("  Sending: '0'\t'%d'  . . . \n " , ulDataTx[0]);
	I2CMasterDataPut(I2C0_BASE, ulDataTx[0]);	//Put the data to be sent in the buffer
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);	//Start sending
	while(I2CMasterBusy(I2C0_BASE))	//Wait until the transaction is complete
	{
	}

	UARTprintf("00 passed");

	//Do the same for all the other data bytes
	for(ulindex = 1; ulindex < NUM_I2C_DATA-1; ulindex++)
	   {

		   UARTprintf("  Sending: '%d'\t'%d'  . . . \n ", ulindex, ulDataTx[ulindex]);

		   I2CMasterDataPut(I2C0_BASE, ulDataTx[ulindex]);
		   I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
		   while(I2CMasterBusy(I2C0_BASE))
		   {
		   }

	   }
	UARTprintf("  Sending: '16'\t'%d'  . . . \n " , ulDataTx[16]);
	I2CMasterDataPut(I2C0_BASE, ulDataTx[16]);
	   I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	   while(I2CMasterBusy(I2C0_BASE))
	   {
	   }

	I2CMasterDisable(I2C0_BASE);

	while(1)
	  {
	    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
	    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);

	    I2CMasterDataPut(I2C0_BASE, 0x02);		// Send 0x02 (Register Address) to the RTC
	    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
	    while(I2CMasterBusy(I2C0_BASE))
	               {
	               }

	   I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, true);	//Send the Slave Address in Read Mode
	   I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	   while(I2CMasterBusy(I2C0_BASE))
	   	   	   	   {
	   	   	   	   }
	   ulDataRx[0] = I2CMasterDataGet(I2C0_BASE);	// Read the data
	   second = BcdToDec(((unsigned char)ulDataRx[0]) & 0b01111111);
	if (second != second_old) // Cycle begins only when it has changed
		{
		//UARTprintf("1st %d",second);
		second_old = second;
		if (second == 0) // If second is zero I need to ask for the minute
		{
			//	UARTprintf("1st %d  %d",second,minute);
	      I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	      while(I2CMasterBusy(I2C0_BASE))
	      	  	  {
	      	  	  }
	      ulDataRx[1] = I2CMasterDataGet(I2C0_BASE);
	      minute = BcdToDec(((unsigned char)ulDataRx[1]) & 0b01111111);
	      if (minute != minute_old) // Cycle begins only when it has changed
	        {
	          minute_old = minute;
	          if (minute == 0) // If minute is zero I need to ask for the hour
	             {
	             I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	   	         while(I2CMasterBusy(I2C0_BASE))
	   	         	 	 {
	   	         	 	 }
	             ulDataRx[2] = I2CMasterDataGet(I2C0_BASE);
	             hour = BcdToDec(((unsigned char)ulDataRx[2]) & 0b01111111);
	             if (hour != hour_old) // Cycle begins only when it has changed
	                {
	                hour_old = hour;
	                if (hour == 0) // If hour is zero I need to ask for other elements
	                	{
	                	for(ulindex = 3; ulindex < 7; ulindex++)
	               	   	   {
	                		I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	                		while(I2CMasterBusy(I2C0_BASE))
	                			{
	                			}
	                		ulDataRx[ulindex] = I2CMasterDataGet(I2C0_BASE);
	               	   	   }
	                	day= BcdToDec(((unsigned char)ulDataRx[3]) & 0b01111111);
	                	weekday = BcdToDec(((unsigned char)ulDataRx[4]) & 0b01111111);
	                	month = (unsigned char)ulDataRx[5];
	                	century = (month & 0x80);
	                	month = BcdToDec(month & 0b01111111);
	                	year=BcdToDec(((unsigned char)ulDataRx[6]) & 0b01111111);
	                	}
	                }
	             }
	        }
		}

		}

	if (century)
	  UARTprintf(" 21");
	else
	  UARTprintf(" 20");
	if (year < 10) UARTprintf("0");
	UARTprintf("%d",year);
	UARTprintf("-");
	if (month < 10) UARTprintf("0");
	UARTprintf("%d",month);
	UARTprintf("-");
	if (day < 10) UARTprintf("0");
	UARTprintf("%d\t",day);
	UARTprintf("");
	if (hour < 10) UARTprintf("0");
	UARTprintf("%d",hour);
	UARTprintf(":");
	if (minute < 10) UARTprintf("0");
	UARTprintf("%d",minute);
	UARTprintf(":");
	if (second < 10) UARTprintf("0");
	UARTprintf("%d\n",second);
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
