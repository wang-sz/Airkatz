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
#include "../inc/tm4c123gh6pm.h"

// ***** ADC_Init *****
// Initialize PD2 and PD1 for analog input
// Input: none
// Output: none
void ADC_Init56(void){
  volatile uint32_t delay;
  SYSCTL_RCGCADC_R |= 0x01;   // activate ADC0 clock
  SYSCTL_RCGCGPIO_R |= 0x08;  // activate clock for Port D
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;  // wait for clock to stabilize
  GPIO_PORTD_DIR_R &= ~0x06;  // make PD2 PD1 input
  GPIO_PORTD_AFSEL_R |= 0x06; // enable alternate function on PD2 PD1
  GPIO_PORTD_DEN_R &= ~0x06;  // disable digital IO on PD2 PD1
  //GPIO_PORTD_PCTL_R = GPIO_PORTD_PCTL_R&0xFFFFF00F;
  GPIO_PORTD_AMSEL_R |= 0x06; // enable analog mode on PD2 PD1
  
  //ADC0_PC_R &= ~0xF;          // clear max sample rate field
  ADC0_PC_R |= 0x01;           // set sample rate to 125K samples/sec (maybe change for higher speed?? idk, needs testing)
  ADC0_SSPRI_R = 0x3210;      // set sequencer 0 at highest priority (0x(SS3)(SS2)(SS1)(SS0))
  ADC0_ACTSS_R &= ~0x04;      // disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;     // set SS2 to software trigger
  ADC0_SSMUX2_R = 0x0056;    // set Ain channels for SS2
  ADC0_SSCTL2_R |= 0x0060; //(ADC0_SSCTL2_R & 0xFFFFFF00) + 0x0060;     // IE1 and END1, 2nd sample interrupt enable, 2nd sample is end of sequence
  ADC0_IM_R &= ~0x04;         // disable SS2 interrupts
  ADC0_ACTSS_R  |= 0x04;      // enable sample sequencer 2
}

// ***** ADC_In *****
// Sample L/R and U/D inputs on joystick
// Input: pointer to array/pointer to 2 globals
// Output: none
void ADC_In56(uint32_t data[2]){
  ADC0_PSSI_R = 0x0004;           // initiate SS2
  while((ADC0_RIS_R&0x04)==0){};  // wait for conversion to be done
  data[1] = ADC0_SSFIFO2_R&0xFFF; // read first result (U/D) (up if < 0x020) (0x020 < down if < 0xC00) 
  data[0] = ADC0_SSFIFO2_R&0xFFF; // read second result (L/R) (left if < 0x020) (0x020 < right if < 0xC20)
  ADC0_ISC_R = 0x0004;            // acknowledge
}
