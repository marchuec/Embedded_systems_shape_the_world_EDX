// Specify a delay in msec to wait with systick

#include "timer.h"
#include "tm4c123gh6pm.h"

// Time delay using busy wait.
// The delay parameter is in units of the core clock. 
static void SysTick_Wait_Delay(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

// Time delay using busy wait.
// This assumes 80 MHz system clock.
void SysTick_Wait(unsigned long msec){
  unsigned long i;
  for(i=0; i<msec; i++){
    SysTick_Wait_Delay(80000);
		// wait 1ms (assumes 80 MHz clock)
  }
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000005;          // enable SysTick with core clock
}
