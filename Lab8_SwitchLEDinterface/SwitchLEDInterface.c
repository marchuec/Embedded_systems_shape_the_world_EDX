// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
unsigned long button;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void PortB_Init(void);
void Wait(unsigned long ms);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  PortE_Init();
	PortB_Init();
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
		GPIO_PORTB_DATA_R |= 0x02;	// LED ON
		Wait(100);
		//button = ;	// PB0
		while(GPIO_PORTE_DATA_R&0x01){
			GPIO_PORTB_DATA_R ^= 0x02;	// LED toggle
			Wait(100);
		}
  }
}

// Wait the number of ms asked
// Input : number of ms to wait
// Output : None
void Wait(unsigned long ms){
	while(ms){
		unsigned long count = 15500;
		while(count){;
			count--;
		}
		ms--;
	}
}

// Initialize port E. PE0 input.
// Input : None
// Outpout : None
void PortE_Init(void) {
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;      // 1) E clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTE_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x01;         // 4 PE0 input 
  GPIO_PORTE_AFSEL_R &= 0x00;        // 5) no alternate function   
  GPIO_PORTE_DEN_R |= 0x01;          // 7) enable digital pins PE0
}

// Initialize port B. PB0 output.
// Input : None
// Outpout : None
void PortB_Init(void) {
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002;      // 1) B clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTB_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R |= 0x02;         // 4 PB1 output
  GPIO_PORTB_AFSEL_R &= 0x00;        // 5) no alternate function   
  GPIO_PORTB_DEN_R |= 0x02;          // 7) enable digital pins PB0
}

