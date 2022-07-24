// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// January 15, 2016

 // Edited by Marc-Antoine Huet

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "MeasurementOfDistance.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "ADC.h"
#include "UART.h" 


//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  return sample*fitting_csts.A + fitting_csts.B;
}


//***********Calibrate**************
// Calibrate the potentiometer for 2 cm in 2 steps
// 1. Place the pot at max (2.000 cm).
// 2. Place the pot at min (0.000 cm).
// Input: None
// Output:
unsigned int PosToPlace;	// Debug
void Calibrate(void) {
	double x1, x2, y1, y2;
	PosToPlace = 0;
	//Nokia5110_SetCursor(0,3);
	//Nokia5110_OutString("*Calib*");
	//Nokia5110_SetCursor(0,4);
	//Nokia5110_OutString("pos. 0 cm");
	UART_OutString("pos. 0 cm\n");
	WaitFor(3000);
	x1 = ADC0_In();
	PosToPlace = 1;
	//Nokia5110_SetCursor(0,4);
	//Nokia5110_OutString("pos. 2 cm");
	UART_OutString("pos. 2 cm\n");
	WaitFor(3000);
	x2 = ADC0_In();
	y1 = 0;	// Calibration value
	y2 = 2000;	// Calibration value
	fitting_csts.A = (y2 - y1) / (x2 - x1);
	fitting_csts.B = y1 - x1*fitting_csts.A; 	// equal 0 
	
	if (x1 > 500 || x2 < 3500) {
		//Nokia5110_SetCursor(0,4);
		//Nokia5110_OutString("failed!    ");
		UART_OutString("failed!\n");
		WaitFor(2000);
		Calibrate();
	} else {
		//Nokia5110_SetCursor(0,3);
		//Nokia5110_OutString("           ");
		//Nokia5110_SetCursor(0,4);
		//Nokia5110_OutString("D = ");
	}
}


// Initialize SysTick interrupts to trigger at 5 Hz. Clock at 16 MHz
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x0004E200;
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000007;          // enable SysTick and interrupt
}

// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	ADCdata = ADC0_In();
	Distance = Convert(ADCdata);
	Flag = 1;	// To output to display
	//LED  ^= 0x08;
}


// Delay function
void WaitFor(unsigned long msec) {
	while (msec) {
		unsigned long Counter1Msec = 16000/3;	// 3 instructions by cycle
		while (Counter1Msec) {
			Counter1Msec--;
		}
		msec--;
	}
}


// Initialize the display with permanent text
void Initialize_Display(void) {
	Nokia5110_Init();
	Nokia5110_SetCursor(0,0);
	Nokia5110_OutString("Lab 14:");
	Nokia5110_SetCursor(0,1);
	Nokia5110_OutString("Measurement");
	Nokia5110_SetCursor(0,2);
	Nokia5110_OutString("of distance");
	WaitFor(2000);
}


// Setup PE3 to flash a LED when ADC is activated
void Setup_PE3(void){ 
	GPIO_PORTE_PCTL_R &= ~0x00000008; 		// 4) disable PCTL on PE3
  GPIO_PORTE_DIR_R |= 0x08;       			// 2) make PE3 output
  GPIO_PORTE_AFSEL_R &= ~0x08;    			// 3) disable alternate function on PE3
  GPIO_PORTE_DEN_R |= 0x08;       			// 4) enable digital I/O on PE3
  GPIO_PORTE_AMSEL_R &= ~0x08;    			// 5) disable analog function on PE3
}


// Update the measurement on the display when ADC has new data
void Update_Display(void) {
	UART_ConvertDistance(Distance);
	//Nokia5110_SetCursor(4,4);
	//Nokia5110_OutString(String);
	//Nokia5110_OutChar('A');
	//Nokia5110_OutUDec(198);
	UART_OutString(String);
	UART_OutString("\r\n");
	Flag = 0;
	LED  ^= 0x08;
}

void EnableInterrupts(void);  // Enable interrupts

int main(void){ 
  volatile unsigned long delay;
  //TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	WaitFor(3000);
	ADC0_Init();
	Setup_PE3();
	Initialize_Display();
	UART_Init_PORTA();
	//Calibrate();
	fitting_csts.A = 0.5;
	fitting_csts.B = 0;
	SysTick_Init();
  EnableInterrupts();

  while(1){ 
		if (Flag) {
			Update_Display();
		}
  }
}


