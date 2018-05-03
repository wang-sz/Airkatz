// DAC.c
// Configures 4-bit DAC output
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date Created: 4/23/2018
// Last Modified: 4/23/2018
// Hardware Connections
// PB0-PB3 for DAC

#include <stdint.h>

// ***** DAC_Init *****
// Initialize 4-bit DAC
// Input: none
// Output: none
void DAC_Init(void);

// ***** DAC_Out *****
// Output to DAC
// Input: 4-bit data, 0 to 15
// Output: none
void DAC_Out(uint32_t data);
