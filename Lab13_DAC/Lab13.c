// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);

int main(void){
	unsigned long key_current;
	unsigned long key_prev = keyNone;
	float note_current;
	
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz    
  Sound_Init(); // initialize DAC
  Piano_Init();
  EnableInterrupts();
  while(1){    
		key_current = Piano_In();
		if (key_prev != key_current) {
			if (key_current == keyNone) {
				Sound_Off();
			} else {
				note_current = Key_to_Note(key_current);
				Sound_Tone(note_current);
			}
		}
		key_prev = key_current;
		delay(10);
	
		// **Tests**
		// Piano
		//piano_inputs = Piano_In();
		
		// DAC
		//wave_index = (wave_index + 1)&0x0F;
		//DAC_Out(wave_index);
		
		// Sound
		
		//delay(500);
		//Sound_Tone(D_note);
		//delay(500);
		//Sound_Off();
		
		//delay
		//delay(500);
		
	}
            
}



// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles (12.5 ns/cycle)
    msec--;
  }
}


