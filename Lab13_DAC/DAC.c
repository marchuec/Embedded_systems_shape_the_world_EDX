// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// DAC uses PB0-3. Port B0 is the second lowest level

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog on PB
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PB
  GPIO_PORTB_DIR_R = 0x0F;          // 5) PB0-3 output
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) disable alt funct on PB
  GPIO_PORTB_DEN_R = 0x0F;          // 7) enable digital I/O on PB0-3
}


// **************DAC_Out*********************
// Output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & 0xFFFFFFF0) | data;
}
