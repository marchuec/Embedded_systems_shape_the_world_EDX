// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

static unsigned long Wave_index_current = 0;	// 0 to 31 Position in the output wave
static unsigned long Wave_indexes[] = {1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};


// **************Sound_Init*********************
// Calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
	DAC_Out(0);
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period (us)
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(float period){
	unsigned long clock_num = 80 * period / 32;	// 80 Mhz, 32 points/cycle
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = clock_num;         // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
	Wave_index_current = 7;
	NVIC_ST_CTRL_R = 0x00000007;          // enable SysTick with core clock and interrupt
}


// **************Sound_Off*********************
// Stop outputing to DAC by disabling SysTick
// Output: none
void Sound_Off(void){
	NVIC_ST_CTRL_R = 0;
	DAC_Out(0);
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	/*
   if (Wave_index == 0) {
		 DAC_Out(++Wave_index);
		 Wave_direction = up;
	 } else if (Wave_index == max_wave_index) {
		 DAC_Out(--Wave_index);
		 Wave_direction = down;
	 } else if (Wave_direction == up) {
		 DAC_Out(++Wave_index);
	 } else if (Wave_direction == down) {
		 DAC_Out(--Wave_index);
	 }
	*/
	
	// Better way (memory instead of computation)
	Wave_index_current = (Wave_index_current + 1) & 0x1F;
	DAC_Out(Wave_indexes[Wave_index_current]);
	 
}
