// main.c
// Runs on LM4F120/TM4C123
// Test main for Lab 11
// January 15, 2016

// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable

#include "UART.h"
#include "TExaS.h"
#include "timer.h"

void EnableInterrupts(void);  // Enable interrupts

unsigned char in_char;
unsigned long in_num;
int main(void){ 
  TExaS_Init();             // initialize grader, set system clock to 80 MHz
  UART_Init_PORTA();      // initialize UART on port A 
	SysTick_Init();
  EnableInterrupts();       // needed for TExaS
	
	//To test :
	// OutChar()
	// OutString()
	// ConvertDistance()
	// OutDistance()
	// ConvertUDec()
	// OutUDec()
	
	SysTick_Wait(3000);
	
	UART_OutChar('A');
	UART_OutChar('\n');

	UART_OutString("TEST");
	UART_OutChar('\n');
	
	UART_OutUDec(1234);
	UART_OutChar('\n');
	UART_OutUDec(123);
	UART_OutChar('\n');
	UART_OutUDec(0);
	UART_OutChar('\n');
	UART_OutUDec(12345);
	UART_OutChar('\n');

	UART_OutDistance(1234);
	UART_OutChar('\n');
	UART_OutDistance(123);
	UART_OutChar('\n');
	UART_OutDistance(0);
	UART_OutChar('\n');
	UART_OutDistance(99999);
	UART_OutChar('\n');
	
	while(1) {
		UART_OutString("Start input testing\n");
		
		UART_OutChar('\n');
		UART_OutString("Enter a caracter: \n");
	  in_char = UART_InChar();
		UART_OutString("Entered caracter: \n");
		UART_OutChar(in_char);
		UART_OutChar('\n');
		SysTick_Wait(500);
		
		UART_OutChar('\n');
		UART_OutString("Enter a 32-bits number: \n");
		in_num = UART_InUDec();
		UART_OutString("Entered numer: \n");
		UART_OutUDec(in_num);
		SysTick_Wait(500);
	}
}

