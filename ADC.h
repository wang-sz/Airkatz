// ADC.c
// Initialize and Sample ADC inputs on PD2 (Ain5) and PD1 (Ain6)
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date created: 4/23/2018
// Last modified: 4/23/2018
// Hardware connections
// L/R joystick input on PD2
// U/D joystick input on PD1

#include <stdint.h>

// ***** ADC_Init *****
// Initialize PD2 and PD1 for analog input
// Input: none
// Output: none
void ADC_Init56(void);

// ***** ADC_In *****
// Sample L/R and U/D inputs on joystick
// Input: pointer to array/pointer to 2 globals
// Output: none
void ADC_In56(uint32_t data[2]);
