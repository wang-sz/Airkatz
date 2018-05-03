// Switch.c
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date Created: 4/23/2018
// Last Modified: 4/24/2018
// Hardware Connections
// PE4 shoot, PE5 special

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Buffer.h"
#define SPECIALCD 400
#define WIDTH 128
#define HEIGHT 160
#define NUMENEMY 18
#define NUMPBULLET 20


extern void Sound_Laser(void);
extern void Buffer_Update(obj_t bufferObj);
extern void Buffer_Draw(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h);


volatile int32_t specialCD;

extern volatile uint8_t killct;
extern volatile uint8_t buffer[];
extern const unsigned char Laser1[];
extern const unsigned char Laser2[];
extern const unsigned char Laser3[];
extern const unsigned char LaserBegin[];
extern const unsigned char Laser[];
extern const uint16_t Laser1d;
extern const uint16_t Laser2d;
extern const uint16_t Laser3d;
extern const uint16_t LaserBeginh;
extern const uint16_t Laserw;
extern const uint16_t Laserh;
extern const unsigned char Green[];
extern const unsigned char LaserBeginGreen[];
extern volatile uint8_t buffer[];

extern obj_t Player;
extern obj_t PBullet[];
extern obj_t EBullet[];
extern obj_t Enemy[];
extern obj_t DogBoss;
void Delay10ms(uint32_t count);

// ***** Switch_Init *****
// Initializes PE4 and PE5 for switches
// Input: none
// Output: none
void Switch_Init(void){
  volatile uint32_t delay;
  specialCD = 200;
  SYSCTL_RCGCGPIO_R |= 0x10;    // activate clock for Port E
  delay = SYSCTL_RCGCGPIO_R;    // wait for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTE_DIR_R &= ~0x30;    // PE4 and PE5 inputs
  GPIO_PORTE_AFSEL_R &= ~0x30;  // disable alternate functions
  GPIO_PORTE_DEN_R |= 0x30;     // enable digital IO
  GPIO_PORTE_AMSEL_R = 0;       // disable analog mode
  GPIO_PORTE_IS_R &= ~0x20;     // PE5 is edge sensitive
  GPIO_PORTE_IBE_R &= ~0x20;    // PE5 is not both edges
  GPIO_PORTE_IEV_R |= 0x20;     // PE5 is rising edge event
  GPIO_PORTE_ICR_R = 0x20;      // clear flag5
  GPIO_PORTE_IM_R |= 0x20;      // arm interrupt on PE5
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000020;  //set priority (bits 7-5) (priority 1)
  NVIC_EN0_R |= 0x0010;         // enable interrupt 4 (Port E) in NVIC
}

void GPIOPortE_Handler(void){
  int16_t j = 3;
  uint8_t i;
  int8_t dir = 1;
  volatile uint8_t space = LaserBeginh;
  GPIO_PORTE_ICR_R = 0x20;      // acknowledge flag5
  if(specialCD <= 0){
    Buffer_Update(Player);
    Buffer_Draw(Player.xpos+11,Player.ypos-Player.h+1,&Laser1[0],Laser1d,Laser1d);
    ST7735_DrawBitmap(0,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
    Delay10ms(10);
    Buffer_Draw(Player.xpos+7,Player.ypos-Player.h+1,&Laser2[0],Laser2d,Laser2d);
    ST7735_DrawBitmap(0,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
    Delay10ms(10);
    Buffer_Draw(Player.xpos+5,Player.ypos-Player.h+1,&Laser3[0],Laser3d,Laser3d);
    ST7735_DrawBitmap(0,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
    Delay10ms(10);
    Buffer_Draw(Player.xpos-8,Player.ypos-Player.h+1,&LaserBegin[0],Laserw,LaserBeginh);
    ST7735_DrawBitmap(0,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
    Sound_Laser();
    Delay10ms(1);
    while(Player.ypos-space >= 0){
      Buffer_Draw(Player.xpos-8,Player.ypos-Player.h+1-space,&Laser[0],Laserw,Laserh);
      space += Laserh;
      ST7735_DrawBitmap(j,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
      if(j>4){
        dir = -1;
      }
      else if(j<-4){
        dir = 1;
      }
      j += dir*2;
      Delay10ms(1);
    }
    for(i=0; i<NUMPBULLET; i++){
      if((PBullet[i].State == alive)&&(PBullet[i].ypos<Player.ypos-Player.h-3)&&((PBullet[i].xpos>=Player.xpos-8+Laserw)||(PBullet[i].xpos<=Player.xpos-8))){
        PBullet[i].State = dead;
      }
    }
    if((DogBoss.State == alive)&&(DogBoss.ypos<Player.ypos-Player.h-3)&&((DogBoss.xpos>=Player.xpos-8+Laserw)||(DogBoss.xpos<=Player.xpos-8))){
      DogBoss.hp /= 2;
    }
    for(i = 0; i<10; i++){
      if(j>4){
        dir = -1;
      }
      else if(j<-4){
        dir = 1;
      }
      j += dir*2;
      Delay10ms(1);
    }
    space = LaserBeginh;
    Buffer_Draw(Player.xpos-8,Player.ypos-Player.h+1,&LaserBeginGreen[0],Laserw,LaserBeginh);
    ST7735_DrawBitmap(j,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
    if(j>4){
      dir = -1;
    }
    else if(j<-4){
      dir = 1;
    }
    j += dir*2;
    Delay10ms(1);
    while(Player.ypos-space >= 0){
      Buffer_Draw(Player.xpos-8,Player.ypos-Player.h+1-space,&Green[0],Laserw,Laserh);
      space += Laserh;
      for(i = 0; i<NUMENEMY; i++){
        if((Enemy[i].State == alive)&&(Enemy[i].ypos<Player.ypos-Player.h)&&((Enemy[i].xpos<=Player.xpos-8+Laserw)||(Enemy[i].xpos>=Player.xpos-8))&&(Enemy[i].ypos>0)){
          Enemy[i].State = dead;
          killct++;
        }
      }
      if((EBullet[0].State == alive)&&(EBullet[0].ypos<Player.ypos-Player.h)&&((EBullet[0].xpos<=Player.xpos-8+Laserw)||(EBullet[0].xpos>=Player.xpos-8))){
        EBullet[0].State = dead;
        Buffer_Update(EBullet[0]);
        EBullet[0].xpos = -1;
        EBullet[0].ypos = -1;
      }
      ST7735_DrawBitmap(j,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
      if(j>4){
        dir = -1;
      }
      else if(j<-4){
        dir = 1;
      }
      j += dir*2;
      Delay10ms(1);
    }
    for(i = 0; i<NUMENEMY; i++){
      Buffer_Update(Enemy[i]);
      if(Enemy[i].State == dead){
        Enemy[i].xpos = -1;
        Enemy[i].ypos = -1;
      }
    }
    if(DogBoss.State == alive){
      Buffer_Update(DogBoss);
    }
    ST7735_DrawBitmap(0,159,(const uint8_t *)buffer,WIDTH,HEIGHT);
    Delay10ms(10);
    specialCD = SPECIALCD;
    TIMER0_CTL_R = 0x00000000;  // disable TIMER0A if at end of array
  }
}


void Delay10ms(uint32_t count){
  uint32_t volatile time;
  while(count>0){
    time = 72724; //727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
