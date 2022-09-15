// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

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


//#include "TExaS.h"
#include "..//tm4c123gh6pm.h"


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

#define output (*(volatile unsigned long *) 0x40004010)
#define input (*(volatile unsigned long *) 0x40004020)
// input from PA3, output from PA2, SysTick interrupts
void PortA_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) activate clock for Port A
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTA_AMSEL_R = 0x00;        // 3) disable analog on PA
  GPIO_PORTA_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PA
  GPIO_PORTA_DIR_R = 0x04;          // 5) PA3 input, PA2 output
  GPIO_PORTA_AFSEL_R = 0x00;        // 6) disable alt funct on PA
  GPIO_PORTA_DEN_R = 0x0C;          // 7) enable digital I/O on PA2-3
	GPIO_PORTA_IS_R &= ~0x08;     		// 8) PA3 is edge-sensitive
  GPIO_PORTA_IBE_R |= 0x08;    			//    PA3 is both edges
  GPIO_PORTA_IEV_R |= 0x08;    			//    PA3 falling/rising edge event
  GPIO_PORTA_ICR_R = 0x08;      		// 9) clear flags 3
  GPIO_PORTA_IM_R |= 0x08;      		// 10) arm interrupt on PA3
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF0F)|0x000000E0; // (g) priority 2 bits 7,6,5
  NVIC_EN0_R |= 0x00000001;      // (h) enable interrupt 0 in NVIC
	output = 0;
}


// Initialize SysTick with interrupt.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x0001631C;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000007;          // enable SysTick and interrupt
}


unsigned char PWM_activated = 0;
unsigned char prev_state = 0;
unsigned long debounce_counter = 0;
#define output_bit 0x04
#define debounce_cst 9	// 10 msec
// Called at 880 Hz. Controls the PWM.  
void SysTick_Handler(void){
	if (PWM_activated) {
		output ^= output_bit;
	} else {
		output = 0;
	}
	
	// Rearm the PortA interrupt after a specified delay to account for switch debouncing
	if (debounce_counter != 0) {
		debounce_counter--;
		if (debounce_counter == 0) {
			GPIO_PORTA_IM_R |= 0x08;	// Arm interrupt
			GPIO_PORTA_ICR_R = 0x08;	// Aknowledge ISR
		}
	}
}

// Reset the debounce counter value to debounce_cst and disable PortA_Handler
void debounce() {
	debounce_counter = debounce_cst;
	GPIO_PORTA_IM_R &= ~0x08;		// Disarm interrupt
}

// Called on both edges
unsigned long edge_counter = 0;
void GPIOPortA_Handler(void) {
	if (!PWM_activated) {
		if (input && !prev_state) {	// First click rising edge, start PWM
			PWM_activated = 1;
			debounce();
		}
		else if (!input && prev_state) { // Second click falling edge, do nothing
			debounce();
		}
	}
	else {
		if (!input && prev_state) {	// First click falling edge, do nothing					
			debounce();;
		}
		else if (input && !prev_state) {	// Second click rising edge, stop PWM
			PWM_activated = 0;
			debounce();
		}
	}
	prev_state = input>>3;
	edge_counter++;
	GPIO_PORTA_ICR_R = 0x08;	// Aknowledge ISR
}


int main(void){// activate grader and set system clock to 80 MHz
  //TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
	DisableInterrupts();
  PortA_Init();
	SysTick_Init();
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
		WaitForInterrupt();
  }
}
