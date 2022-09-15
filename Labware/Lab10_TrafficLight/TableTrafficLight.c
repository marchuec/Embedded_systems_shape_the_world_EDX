// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Teachers: Daniel Valvano, Jonathan Valvano
// January 15, 2016
//
// Current code : Marc-Antoine Huet 

// *Traffic:*
// East = East/West
// North = North/South
// Walk = pedestrians

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// Initialize the PortF
void PortF_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF
  GPIO_PORTF_DIR_R = 0x0A;          // 5) PF1 and PF3 output
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF
  GPIO_PORTF_DEN_R = 0x0A;          // 7) enable digital I/O on PF1,PF3
}

// Initialize the PortB
void PortB_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog on PB
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PB
  GPIO_PORTB_DIR_R = 0x3F;          // 5) PB0-5 output
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) disable alt funct on PB
  GPIO_PORTB_DEN_R = 0x3F;          // 7) enable digital I/O on PB0-5
}

// Initialize the PortE
void PortE_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PE
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE
  GPIO_PORTE_DIR_R = 0x00;          // 5) PE0-2 inputs
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PE
  GPIO_PORTE_DEN_R = 0x07;          // 7) enable digital I/O on PE0-2
}

struct FSM{
	unsigned long colorsToOutput[3];
	unsigned long delay;
	unsigned long nextState[8];
}; 


// *Legend:*
// go = Green light
// slow = Yellow light
// wait = Red light
// WalkBlink = blinking green light (on/off)
// WaitNorth = Waiting to change traffic (all red), previous state is North

// All states numbers (line)
#define EastGo 		0
#define EastSlow 	1
#define NorthGo 	2 
#define NorthSlow 3
#define WalkGo 		4
#define WalkBlink1 5
#define WalkBlink2 6
#define WalkBlink3 7
#define WalkBlink4 8
#define WalkBlink5 9
#define WaitNorth 10
#define WaitEast 	11
#define WaitWalk 	12
	
// Output to the port for each color
// G_ = green
// R_ = red
// Y_ = yellow
// N_ = none
// *Lights pins:*
#define G_EAST 0x08  // PB3
#define Y_EAST 0x10  // PB4
#define R_EAST 0x20  // PB5
#define G_NORTH 0x01 // PB0
#define Y_NORTH 0x02 // PB1
#define R_NORTH 0x04 // PB2
#define G_WALK 0x08  // PF3
#define R_WALK 0x02  // PF1
#define N_WALK 0x00  
const struct FSM fsm[] = {
	//East    North  	 Walk   |Delay| In= East		North				Walk				East,North	East,Walk		North,Walk	All					None
	{{G_EAST, R_NORTH, R_WALK},	5000,	{EastGo, 		EastSlow, 	EastSlow, 	EastSlow, 	EastSlow, 	EastSlow, 	EastSlow, 	EastSlow}},		// S0 EastGo
	{{Y_EAST, R_NORTH, R_WALK}, 2000, 	{WaitEast, 	WaitEast, 	WaitEast, 	WaitEast, 	WaitEast, 	WaitEast, 	WaitEast, 	WaitEast}},		// S1 EastSlow
	{{R_EAST, G_NORTH, R_WALK}, 5000, {NorthSlow, NorthGo, 		NorthSlow, 	NorthSlow, 	NorthSlow, 	NorthSlow, 	NorthSlow, 	NorthSlow}},	// S2 NorthGo
	{{R_EAST, Y_NORTH, R_WALK},	2000,	{WaitNorth, WaitNorth, 	WaitNorth, 	WaitNorth, 	WaitNorth, 	WaitNorth, 	WaitNorth, 	WaitNorth}},	// S3 NorthSlow
	{{R_EAST, R_NORTH, G_WALK},	3000,	{WalkBlink1,WalkBlink1, WalkGo, 		WalkBlink1, WalkBlink1, WalkBlink1, WalkBlink1, WalkBlink1}},	// S4 WalkGo
	{{R_EAST, R_NORTH, N_WALK},	500,	{WalkBlink2,WalkBlink2, WalkBlink2, WalkBlink2, WalkBlink2, WalkBlink2, WalkBlink2, WalkBlink2}},	// S5 WalkBlink1
	{{R_EAST, R_NORTH, G_WALK},	500,	{WalkBlink3,WalkBlink3, WalkBlink3, WalkBlink3, WalkBlink3, WalkBlink3, WalkBlink3, WalkBlink3}},	// S6 WalkBlink2
	{{R_EAST, R_NORTH, N_WALK},	500,	{WalkBlink4,WalkBlink4, WalkBlink4, WalkBlink4, WalkBlink4, WalkBlink4, WalkBlink4, WalkBlink4}},	// S7 WalkBlink3
	{{R_EAST, R_NORTH, G_WALK},	500,	{WalkBlink5,WalkBlink5, WalkBlink5, WalkBlink5, WalkBlink5, WalkBlink5, WalkBlink5, WalkBlink5}},	// S8 WalkBlink4
	{{R_EAST, R_NORTH, N_WALK},	500,	{WaitWalk,  WaitWalk, 	WaitWalk, 	WaitWalk, 	WaitWalk, 	WaitWalk, 	WaitWalk, 	WaitWalk}},		// S9 WalkBlink5
	{{R_EAST, R_NORTH, R_WALK},	500, 	{EastGo, 		NorthGo, 		WalkGo, 		EastGo, 		WalkGo, 		WalkGo, 		EastGo, 		EastGo}},			// S10 WaitNorth
	{{R_EAST, R_NORTH, R_WALK},	500, 	{EastGo, 		NorthGo, 		WalkGo, 		NorthGo, 		WalkGo, 		NorthGo, 		WalkGo, 		WalkGo}},			// S11 WaitEast
	{{R_EAST, R_NORTH, R_WALK},	2000, 	{EastGo, 		NorthGo,		WalkGo, 		EastGo, 		EastGo, 		NorthGo, 		NorthGo, 		NorthGo}}			// S12 WaitWalk
};
	
// Output the corect colors for each trafic direction.
// Input: lightsColors[3] (u long) - an array with the corresponding port output for East, North and Walk lights
// Output: None
#define EAST_LIGHT 	(*((volatile unsigned long *)0x400050E0)) // PB3-5
#define NORTH_LIGHT	(*((volatile unsigned long *)0x4000501C)) // PB0-2
#define WALK_LIGHT	(*((volatile unsigned long *)0x40025028)) // PF1,PF3
void outputLights(unsigned long* lightsColors){
	EAST_LIGHT = lightsColors[0];
	NORTH_LIGHT = lightsColors[1];
	WALK_LIGHT = lightsColors[2];
}


// Read the 3 switches inputs. This function could be simplified 
// by removing the switch/case by rearranging the column order in the fsm.
// Input: none
// Output: none
// *Sensors pins:*
// PE2 = Walk
// PE1 = North
// PE0 = East
#define SENSORS_INPUTS 	(*((volatile unsigned long *)0x4002401C)) // PE0-2 x111
#define IN_EAST 0	// Corresponding column for each input
#define IN_NORTH 1
#define IN_WALK 2
#define IN_EAST_NORTH 3
#define IN_EAST_WALK 4
#define IN_NORTH_WALK 5
#define IN_ALL 6
#define IN_NONE 7
unsigned long readInput(void){
	unsigned long input;
	switch(SENSORS_INPUTS){
		case 0x00: input = IN_NONE; break;
		case 0x01: input = IN_EAST; break;
		case 0x02: input = IN_NORTH; break;
		case 0x04: input = IN_WALK; break;
		case 0x03: input = IN_EAST_NORTH; break;
		case 0x05: input = IN_EAST_WALK; break;
		case 0x06: input = IN_NORTH_WALK; break;
		case 0x07: input = IN_ALL; break;
		default: input = IN_ALL;
	}
	return input;
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


// ***** 3. Subroutines Section *****
unsigned long state;
unsigned long* colorsToOutput;
unsigned long input;
int main(void){ 
	DisableInterrupts();
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	PortF_Init();
	PortB_Init();
	PortE_Init();
	SysTick_Init();
  EnableInterrupts();
	state = EastGo; // init state
	
  while(1){
		colorsToOutput = (unsigned long*) fsm[state].colorsToOutput;	
		outputLights(colorsToOutput);
		SysTick_Wait(fsm[state].delay);
		input = readInput();
		state = fsm[state].nextState[input];
  }
}
