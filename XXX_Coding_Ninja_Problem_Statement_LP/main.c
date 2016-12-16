/*
 * Objective : Embedded C Ninja Level Problem Statement
 * Author :	Anshuman Mishra
 */

/*
 * 	Pin Map:
 * 	- S1 : PF0
 * 	- S2 : PF4
 *	- 4 digit SSD
 *		A:	 PB7
 *		B:	 PB6
 *		C:	 PB5
 *		D:	 PB4
 *		E:	 PB3
 *		F:	 PB2
 *		G:	 PB1
 *		DP:  PB0
 *		CC1: PD0
 *		CC2: PD2
 *		CC3: PD1
 *		CC4: PD3
 *	- Analog Input (Pot) : PE1
 *	- 7555 frequency input : PC4
 */



/******************* INCLUDES **********************/
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"

/************ CONSTANTS AND DEFINES ****************/
int mode = 1;
int ssdnum[4] = {0x00,0x00,0x00,0x00};
int ssdmap[16] = {0xFC,0x0C,0xDA,0x9E,0x2E,0xB6,0xF6,0x1C,0xFE,0xBE,0x7E,0xE6,0xF0,0xCE,0xF2,0x72};
int ssd_digit = 0;
int modemap[][4] = {{0x01,0x00,0x00,0x00},{0x00,0x01,0x00,0x00},{0x00,0x00,0x01,0x00},{0x00,0x00,0x00,0x01},{0x01,0x01,0x01,0x01}};
int hex_count = 0;
int i=0;
int adcValue[2];
int fast_flag=0;
int freq=0;
int edgecount=0;

/************ FUNCTION DECLARATIONS ****************/
void modeselect(void);	//Selects the operating mode
void mode1set();		//Initializes and enables peripherals for Mode 1
void mode2set();		//Initializes and enables peripherals for Mode 2
void mode3set();		//Initializes and enables peripherals for Mode 3
void mode4set();		//Initializes and enables peripherals for Mode 4
void mode5set();		//Initializes and enables peripherals for Mode 5
void mode1unset();		//Disables peripherals for Mode 3
void mode2unset();		//Disables peripherals for Mode 3
void mode3unset();		//Disables peripherals for Mode 3
void mode4unset();		//Disables peripherals for Mode 3
void mode5unset();		//Disables peripherals for Mode 3
void mode1();			//Generate Mode 1 output
void mode2();			//Generate Mode 2 output
void mode3();			//Generate Mode 3 output
void mode4();			//Generate Mode 4 output
void mode5();			//Generate Mode 5 output
void ssdset(int);		//Maps a given decimal integer to the seven segment display
void ssdsetHex(int);	//Maps a given hexadecimal integer to the seven segment display
void ssdmux(void);		//ISR controlling the refresh of the SSD
void fast(void);		//ISR Enables fast update in Mode 2
void freqfind(void);	//Calculates the 7555 frequency

/**************** MAIN FUNCTION ********************/

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);	//Configure System Clock
//SSD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);	//Enable Port D
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);	//Enable Port B
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,0x0F);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,0xFF);
//S1 and S2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//Enable Port F
//Mode 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);		//Enable ADC0 Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);	//Enable Port E
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);	//Select ADC Function of PE 1
	ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);	//Configure ADC0 Sequencer
	ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);	//Configure the step of the sequencer
//Mode 2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);	//Enable TIMER1 Module
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);	//Set PF4 as Input
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);		//Configuring the PF4
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_BOTH_EDGES);		//Setting Interrupt to trigger on both edges
	TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC);	//Configure TIMER1 into a Continuous Mode
	TimerIntRegister(TIMER1_BASE, TIMER_A, fast);	//Register interrupt if PF4 pressed for more than 1s
//Mode 3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);	//Enable TIMER4 Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);	//Enable WTIMER0 Module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);	//Enable Port C
	GPIOPinConfigure(GPIO_PC4_WT0CCP0);
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_4);	//Set PC4 as a Timer Capture pin
	TimerIntRegister(TIMER4_BASE,TIMER_A,freqfind);	//Register a timer interrupt for TIMER4
	TimerConfigure(TIMER4_BASE,TIMER_CFG_PERIODIC);	//Configure Timer4 in continuous mode
	TimerConfigure(WTIMER0_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_CAP_COUNT);
	TimerControlEvent(WTIMER0_BASE,TIMER_A,TIMER_EVENT_POS_EDGE);	//Configure WTIMER0 for Positive Edge Capture

	IntMasterEnable();
	GPIOPinWrite(GPIO_PORTB_BASE,0xFF,0x00);	//Initialize SSD to 0x00
	GPIOPinWrite(GPIO_PORTD_BASE, 0x0F, 0x00);	//Turn off all the digits of the SSD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);	//Enable TIMER0 Module
	TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);	//Configure TIMER0 into a Continuous Mode
	TimerIntRegister(TIMER0_BASE, TIMER_A, ssdmux);	//Register ISR for TIMER0 Interrupt to update SSD
	TimerLoadSet(TIMER0_BASE, TIMER_A,SysCtlClockGet()/3000);	//Set the refresh rate of the SSD
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);	//Enable the timer interrupt
	TimerEnable(TIMER0_BASE,TIMER_A);	//Start the timer

	HWREG(GPIO_PORTF_BASE|GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE|GPIO_O_CR) = GPIO_PIN_0;	//Unlock PF0 from the NMI mode
	HWREG(GPIO_PORTF_BASE|GPIO_O_LOCK) = 0;

	ssdset(0);
	mode1set();

	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);	//Setting PF0 to Input
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);		//Configuring the pins
	GPIOIntRegister(GPIO_PORTF_BASE, modeselect);		//Register Interrupt for Port F with ISR modeselect()
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);		//Clear any existing interrupts
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);		//Setting Interrupt to trigger on falling edges
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);		//Enable the GPIO Interrupts on Port F
	IntEnable(INT_GPIOF);	//Enable Interrupts on Port F

	while(1){
		switch(mode)	//Select operation according to mode
		{
		case 1: mode1();
				break;
		case 2: ssdsetHex(hex_count);
				if(fast_flag)
				{
					mode2();
					SysCtlDelay(SysCtlClockGet()/300);
				}
				break;
		case 3: mode3(); break;
		case 4: mode1(); break;
		case 5: ssdsetHex(hex_count);
				if(fast_flag)
				{
					mode2();
					SysCtlDelay(SysCtlClockGet()/300);
				} break;
				}
		SysCtlDelay(SysCtlClockGet()/3000);
	}
	return 0;
}

/************ FUNCTION DEFINITIONS ****************/
void modeselect(void)
{
	if(GPIOIntStatus(GPIO_PORTF_BASE,false) & GPIO_INT_PIN_0)
	{
		GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_0);
		for(i=0;i<2000;i++);
		if (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0)
		{
			mode++;
			if (mode==6)
				mode = 1;
			switch(mode)	//According to mode, initialize peripherals and disable other peripherals
			{
			case 1:	mode2unset(); mode3unset();	mode4unset();	mode5unset();	mode1set();	break;
			case 2: mode1unset(); mode3unset();	mode4unset();	mode5unset();	mode2set();	break;
			case 3: mode2unset(); mode1unset();	mode4unset();	mode5unset();	mode3set();	break;
			case 4: mode2unset(); mode1unset();	mode3unset();	mode5unset();	mode4set();	break;
			case 5: mode2unset(); mode1unset();	mode4unset();	mode3unset();	mode5set();	break;
			}
		}
	}
	if((GPIOIntStatus(GPIO_PORTF_BASE,false)&GPIO_INT_PIN_4)&& (mode==2||mode==5))
	{
		GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_4);
		if(!GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4))
		{
			mode2();
			TimerLoadSet(TIMER1_BASE, TIMER_A,SysCtlClockGet()/3);	//Set the Max Value of the timer
			TimerEnable(TIMER1_BASE,TIMER_A);	//Start the timer
		}
		else
		{
			TimerDisable(TIMER1_BASE,TIMER_A);
			fast_flag=0;
		}

	}
}

void ssdmux(void)
{
	volatile int l ;
	volatile data[4];
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	GPIOPinWrite(GPIO_PORTD_BASE, 0x0F, 0x00);
	GPIOPinWrite(GPIO_PORTB_BASE,0xFF,0x00);
	ssd_digit++;
	ssd_digit=ssd_digit%4;
	for(i=0;i<4;i++)
		{
			data[i] = ssdnum[i]|modemap[mode-1][i];	//Incorporate mode indicators
		}
	GPIOPinWrite(GPIO_PORTB_BASE,0xFF,data[ssd_digit]);
	GPIOPinWrite(GPIO_PORTD_BASE, 0x0F, 1<<ssd_digit);

}

void mode1set()
{
	ADCSequenceEnable(ADC0_BASE, 2);	//Enable ADC0 Sequencer
	ADCIntClear(ADC0_BASE, 2);		//Clear Existing Interrupts
}

void mode2set()
{
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);		//Clear any existing interrupts
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);		//Enable the GPIO Interrupts on Port F
	TimerLoadSet(TIMER1_BASE, TIMER_A,SysCtlClockGet()/3);	//Set the Max Value of the timer
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);	//Enable Timer1 Interrupt
}

void mode3set()
{
	edgecount=0;
	IntEnable(INT_TIMER4A);
	TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	TimerLoadSet(TIMER4_BASE,TIMER_A,SysCtlClockGet()/10);	//Setup TIMER4 for 100ms count
	TimerLoadSet(WTIMER0_BASE,TIMER_A,10000 );	//Setup WTIMER0 to count from 10000
	TimerEnable(WTIMER0_BASE, TIMER_A);
	TimerEnable(TIMER4_BASE, TIMER_A);	//Enable timers
}

void mode4set()
{
	mode1set();
	mode3set();
}

void mode5set()
{
	mode2set();
	mode3set();
}

void mode1unset()
{
	ADCIntClear(ADC0_BASE, 2);		//Clear Existing Interrupts
	ADCSequenceDisable(ADC0_BASE, 2);	//Enable ADC0 Sequencer
}

void mode2unset()
{
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);		//Clear any existing interrupts
	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);		//Enable the GPIO Interrupts on Port F
	IntDisable(INT_TIMER1A);
	TimerIntDisable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
}

void mode3unset()
{
	IntDisable(INT_TIMER4A);
	TimerIntDisable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	TimerDisable(WTIMER0_BASE, TIMER_A);
	TimerDisable(TIMER4_BASE, TIMER_A);
}

void mode4unset()
{
	mode1unset();
	mode3unset();
	TimerLoadSet(TIMER0_BASE, TIMER_A,SysCtlClockGet()/3000);
}

void mode5unset()
{
	mode2unset();
	mode3unset();
	TimerLoadSet(TIMER0_BASE, TIMER_A,SysCtlClockGet()/3000);
}

void mode1()	//Obtain samples from ADC
{
	ADCProcessorTrigger(ADC0_BASE, 2);		// Start Sampling
	while(!ADCIntStatus(ADC0_BASE, 2, false));	// Wait until sampling is done
	ADCIntClear(ADC0_BASE, 2);	//Clear Interrupt
	ADCSequenceDataGet(ADC0_BASE, 2, adcValue);	//Obtain the sample
	ssdset(adcValue[0]);
}



void mode2()
{
	hex_count++;
	ssdsetHex(hex_count);
}

void mode3()
{
	ssdset(freq);
}

void ssdset(int data)
{
	data = data%10000;
	ssdnum[3] = ssdmap[(int)data/1000];
	data = data%1000;
	ssdnum[2] = ssdmap[(int)data/100];
	data = data%100;
	ssdnum[1] = ssdmap[(int)data/10];
	data = data%10;
	ssdnum[0] = ssdmap[(int)data];
}

void ssdsetHex(int data)
{

	ssdnum[0] = ssdmap[(int)data%16];
	data/= 16;
	ssdnum[1] = ssdmap[(int)data%16];
	data/= 16;
	ssdnum[2] = ssdmap[(int)data%16];
	data/= 16;
	ssdnum[3] = ssdmap[(int)data%16];
}

void fast(void)
{
	TimerIntClear(TIMER1_BASE,TIMER_A);
	fast_flag=1;
	TimerDisable(TIMER1_BASE,TIMER_A);
}

void freqfind(void)
{
	TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	edgecount=TimerValueGet(WTIMER0_BASE,TIMER_A);
	freq = (10000 - edgecount) * 10;
	if(mode==4||mode==5)
		TimerLoadSet(TIMER0_BASE, TIMER_A,SysCtlClockGet()/freq);	//Set the Max Value of the timer
    TimerLoadSet(WTIMER0_BASE, TIMER_A,10000);
}
