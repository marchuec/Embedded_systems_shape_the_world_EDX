// ***** 0. Documentation Section *****
// main.c for Lab 9
// Runs on LM4F120/TM4C123
// In this lab we are learning functional debugging by dumping
//   recorded I/O data into a buffer
// January 15, 2016

// Lab 9
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define SW1 (*((volatile unsigned long *)0x40025004))	// PF0
#define SW2 (*((volatile unsigned long *)0x40025040))	// PF4
#define LED (*((volatile unsigned long *)0x40025008))	// PF1
#define MASK1 0x13	// PortF-0,1,4

// ***** 2. Global Declarations Section *****

// Contains important data for debugging
struct Dump{
	unsigned long Data2[50];
	unsigned long Time[50];
	unsigned int i;
	unsigned long portF_old;
	unsigned long Time_last;
};

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Dump_init(struct Dump *d);
void Dump_fct(struct Dump *d);

// ***** 3. Subroutines Section *****

/* 
This Lab9 starter project is the same as C9_Debugging example but 
includes the connections to the Lab9 grader. You will make three changes. 
First, make the LED flash at 10 Hz. In other words, make it turn on for 0.05 seconds, 
and then turn off for 0.05 seconds. 
Second, make the LED flash if either switch SW1 or SW2 is pressed 
(this means flash the LED if either PF4 or PF0 is 0). 
Third, record PortF bits 4,1,0 every time the input changes or the output changes. 
For example, if your system detects a change in either PF4 or PF0 input, 
record PortF bits 4,1,0. If your system causes a change in PF1, record PortF bits 4,1,0. 

If both PF4 and PF0 switch are not pressed, the PF1 output should be low.  
If either PF4 or PF0 switches is pressed, the output toggles at 10 Hz (±10%). 
Information collected in the Data array matches the I/O on PortF.
50 data points are collected only on a change in input or a change in output.
This means no adjacent elements in the array should be equal.

*/


void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on Port F
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) disable PCTL on Port F
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF1-3 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on Port F
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on Port F
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000005;          // enable SysTick with core clock
}


// Time delay using busy wait.
// The delay parameter is in units of the core clock. 
void SysTick_Wait_Delay(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

// Time delay using busy wait.
// This assumes 16 MHz system clock.
void SysTick_Wait(unsigned long msec){
  unsigned long i;
  for(i=0; i<msec; i++){
    SysTick_Wait_Delay(16000);
		// wait 1ms (assumes 16 MHz clock)
  }
}

unsigned long Data[50];
int main(void){
	struct Dump dump;
  TExaS_Init(SW_PIN_PF40, LED_PIN_PF1);  // activate grader and set system clock to 16 MHz
  PortF_Init();
  SysTick_Init(); // initialize SysTick, runs at 16 MHz
  EnableInterrupts();           // enable interrupts for the grader
	Dump_init(&dump);
	LED = 0;	// LED off
	Dump_fct(&dump);
  while(1){
		SysTick_Wait(50);
		if(!SW1 || !SW2){
			LED ^= 0x02;	// LED toggle
			Dump_fct(&dump);
		} else {
			LED = 0;	// LED off
			Dump_fct(&dump);
		}
	}
}


// Initialize the dump structure
void Dump_init(struct Dump *dump){
	dump->i = 0;
	dump->portF_old = GPIO_PORTF_DATA_R&MASK1;	// PortF-0,1,4
}

// If PortF value has changed, record the new value
// Input: None
// Output: None
void Dump_fct(struct Dump *dump){
	if((dump->portF_old != (GPIO_PORTF_DATA_R&MASK1)) && ((dump->i)<50)){
		dump->Data2[dump->i] = GPIO_PORTF_DATA_R&MASK1;
		dump->Time[dump->i] = (dump->Time_last - NVIC_ST_CURRENT_R)&0x00FFFFFF;
		dump->Time_last = NVIC_ST_CURRENT_R;
		dump->portF_old = GPIO_PORTF_DATA_R&MASK1;
		Data[dump->i] = GPIO_PORTF_DATA_R&MASK1;	// For corrector only (duplicated)
		dump->i = dump->i + 1;
	}
}


// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06
