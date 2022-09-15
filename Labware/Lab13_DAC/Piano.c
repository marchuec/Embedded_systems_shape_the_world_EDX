// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014
// Driver to initialize and read piano keys pressed from PE0-3

#include "Piano.h"
#include "..//tm4c123gh6pm.h"


// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PE
	GPIO_PORTE_PDR_R = 0x0F;					// Pull down
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE
  GPIO_PORTE_DIR_R = 0x00;          // 5) PE0-3 inputs
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PE
  GPIO_PORTE_DEN_R = 0x0F;          // 7) enable digital I/O on PE0-3
}
  

// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x00 if no key is pressed
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
	return (GPIO_PORTE_DATA_R & 0x0F);
}

// ***************Key_to_Note***************
// Convert the key to the corresponding note (period)
// Input: key_current The current key
// Output: The corresponding note period in usec
float Key_to_Note(unsigned long key_current) {
	switch (key_current) {
		case key0 :
			return C_note;
		case key1 :
			return D_note;
		case key2 : 
			return E_note;
		case key3 :
			return G_note;
	}
	return C_note;
}

