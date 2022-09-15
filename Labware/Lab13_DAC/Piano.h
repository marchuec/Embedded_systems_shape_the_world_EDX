// Piano.h
// Runs on TM4C123
// edX lab 13
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Driver to initialize and read piano keys pressed from PE0-3


// Piano key 3: G generates a sinusoidal DACOUT at 783.991 Hz
// Piano key 2: E generates a sinusoidal DACOUT at 659.255 Hz
// Piano key 1: D generates a sinusoidal DACOUT at 587.330 Hz
// Piano key 0: C generates a sinusoidal DACOUT at 523.251 Hz
#define keyNone 0
#define key0 0x01 
#define key1 0x02
#define key2 0x04 
#define key3 0x08
#define C_note 1911.128693f	// Periods constants for the 4 notes in usec
#define D_note 1702.620333f
#define E_note 1516.863733f
#define G_note 1275.524847f

// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void); 


// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void);

// ***************Key_to_Note***************
// Convert the key to the corresponding note (period)
// Input: key_current The current key
// Output: The corresponding note period in usec
float Key_to_Note(unsigned long key_current);
