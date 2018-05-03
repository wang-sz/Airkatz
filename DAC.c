// DAC.c
// Configures 8-bit DAC output
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date Created: 4/23/2018
// Last Modified: 4/23/2018
// Hardware Connections
// PB0-PB7 for DAC

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ***** DAC_Init *****
// Initialize 8-bit DAC
// Input: none
// Output: none
void DAC_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x02;    // activate clock for Port B
  delay = SYSCTL_RCGCGPIO_R;    // wait for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTB_AMSEL_R = 0x00;    // disable analog mode
  GPIO_PORTB_AFSEL_R = 0x00;    // disable alternate functions
  GPIO_PORTB_DIR_R |= 0xFF;     // make PB0-PB7 output
  GPIO_PORTB_DEN_R |= 0xFF;     // enable digital I/O on PB0-PB7
}

// ***** DAC_Out *****
// Output to DAC
// Input: 8-bit data, 0 to 255
// Output: none
void DAC_Out(uint32_t data){
  GPIO_PORTB_DATA_R = data&0x0F;
}
