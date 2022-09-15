// MeasurementOfDistance.h
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// January 15, 2016

 // Edited by Marc-Antoine Huet

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
#ifndef __MeasurementOfDistance_H__
#define __MeasurementOfDistance_H__

 
unsigned char String[10];
unsigned long Distance = 0;   // value in mm (int) 0-2000 mm
unsigned long ADCdata = 0;    // 12-bit 0 to 4095 sample
unsigned long Flag = 0;       // 1 means Distance available to output to display, 0 means nothing to display
struct fitting_csts_struct {
	double A;	// y = m*A + B
	double B;
};
unsigned char String[10];
struct fitting_csts_struct fitting_csts;

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample);


//***********Calibrate**************
// Calibrate the potentiometer for 2 cm in 2 steps
// 1. Place the pot at max (2.000 cm).
// 2. Place the pot at min (0.000 cm).
// Input: None
// Output:
void Calibrate(void);

// Setup PE3 to flash a LED when ADC is activated
#define LED	(*((volatile unsigned long *)0x40024020))
void Setup_PE3(void);

// Update the measurement on the display when ADC has new data
void Update_Display(void);

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms. Clock at 16 MHz
void SysTick_Init(void);


// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void);


// Delay function
void WaitFor(unsigned long msec);


// Initialize the display with permanent text
void Initialize_Display(void);


//-----------------------ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void ConvertDistance(unsigned long n);

#endif
