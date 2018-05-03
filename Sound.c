// Sound.c
// Sound assets for AirKatz game
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date Created: 4/23/2018
// Last Modified: 4/23/2018
// Hardware Connections
// PB0-PB7 for DAC.c

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "DAC.h"
#include "Timer0.h"
#include "sounds.h"

extern volatile uint8_t repeat;
// check how much mem space this is (had 15435 bytes of sound info)


// ***** Sound_Init *****
// Initialize digital outputs
// Called once, sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();
  Timer0_Init(7256); // set sound frequency, 8kHz
}


void Sound_Shoot(void){
  Timer0A_SetSound(&shoot[0], 1651);
}
void Sound_Damage(void){
  Timer0A_SetSound(&damage[0], 2638);
}

void Sound_Laser(void){
  Timer0A_SetSound(&laser[0], 1651);
}

void Sound_Death(void){
  Timer0A_SetSound(&death[0], 9887);
}

void Sound_Win(void){
  Timer0A_SetSound(&win[0], 5661);
}
