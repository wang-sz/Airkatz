// AirKatz.c
// Runs on TM4C123
// Anna Sim and Sean Wang
// Project for EE319K Lab 10
// Date Created: 4/23/2018
// Last Modified:

// Hardware Connections
// 

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "TExaS.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Sound.h"
#include "Buffer.h"
#include "UART.h"

#define ATKTIME 100
#define SHOOTRATE 10

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Switch_Init(void);

extern const uint16_t WinScrnh;
extern const uint16_t WinScrnw;
extern void Delay10ms(uint32_t count);
extern volatile uint8_t buffer[];
static uint8_t bufferFlag;      // buffer flag
extern obj_t Player;            // player object
extern obj_t Enemy[];
extern obj_t PBullet[NUMPBULLET];
extern obj_t EBullet[NUMEBULLET];
extern obj_t DogBoss;
volatile uint8_t killct = 0;
volatile uint8_t score = 0;
extern const unsigned char WinScrn[];
extern obj_t Menu1;
extern obj_t Menu2;
extern obj_t Title1;
extern obj_t Title2;
extern obj_t TitleTxt;
extern const unsigned char sky[];
extern const unsigned char scoreLet[];
extern const unsigned char one[];
extern const unsigned char two[];
extern const unsigned char three[];

extern int32_t specialCD;

uint8_t ATKmode;
uint16_t ATKtime;
uint8_t ATKone;
uint8_t ATKtwo;
uint8_t PBullIdx;
uint8_t waitShoot;

uint32_t data[2];

uint32_t sysPrev;


void PortF_Init(void){
  volatile uint8_t delay;
  SYSCTL_RCGCGPIO_R |= 0x20;
  delay = SYSCTL_RCGCGPIO_R;
  delay++;
  delay++;
  GPIO_PORTF_DIR_R |= 0x0E;
  GPIO_PORTF_DEN_R |= 0x0E;
}

void SysTick_Init(){
  NVIC_ST_CTRL_R &= ~(0x01);	// disable enable bit
  NVIC_ST_RELOAD_R = 0x0028B0AA;	// set reload value
  NVIC_SYS_PRI3_R |= 0x60000000; // set priority to 3
  NVIC_ST_CURRENT_R = 0;	// clear current 
  NVIC_ST_CTRL_R = 0x00000007; //Enable SysTick, or 7 if interrupt bit is enabled
}

int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);           // set bus clock to 80 MHz
  ST7735_InitR(INITR_REDTAB);   // initialize LCD black screen
  PortF_Init();                 // init Port F for heartbeat
  SysTick_Init();
  Sound_Init();
  Switch_Init();
  ADC_Init56();
  UART_Init();
  Random_Init(1);
  ATKtime = ATKTIME;                // try to make random somehow
  waitShoot = 1;
  PBullIdx = 0;
  ATKmode = 0;
  bufferFlag = 0;
  Stage = mob;
  Start_Init();
  while((GPIO_PORTE_DATA_R&0x30)==0){
    Delay10ms(2);
    Menu1.ypos += Menu1.dy;
    Menu2.ypos += Menu2.dy;
    Buffer_UpdateMenu(Menu1);
    Buffer_UpdateMenu(Menu2);
    
    Title1.ypos += Title1.dy;
    Title2.ypos += Title2.dy;
    Buffer_UpdateMenu(Title1);
    Buffer_UpdateMenu(Title2);
    
    if((signed)(Menu1.ypos-Menu1.h+1)>159){
      Menu1.ypos = (signed)(Menu2.ypos-Menu2.h+1);
    }
    if((signed)(Menu2.ypos-Menu2.h+1)>159){
      Menu2.ypos = (signed)(Menu1.ypos-Menu1.h+1);
    }
    if(Title1.ypos<0){
      Title1.ypos = Title2.ypos+Title1.h+60;
    }
    if(Title2.ypos<0){
      Title2.ypos = Title1.ypos+Title2.h+60;
    }
    Buffer_UpdateMenu(TitleTxt);
    ST7735_DrawBitmap(0,159,(const uint8_t*)&buffer[0],WIDTH,HEIGHT);
  }
  Game_Init();
  Delay10ms(100);
  EnableInterrupts();
  while(1){
    
    while(!(bufferFlag)){
    }
    //sysPrev = NVIC_ST_CURRENT_R;
    //NVIC_ST_CTRL_R &= ~(0x01);
    ST7735_DrawBitmap(0,159, (const uint8_t *) &buffer[0],128,160);
    
    bufferFlag = 0;
    //ST7735_DrawBitmap(0, 159, &sky[0], 128, 160);
    if(Stage == win){
      NVIC_EN0_R &= ~0x0010;         // disable interrupt 4 (Port E) in NVIC
      NVIC_ST_CTRL_R &= ~(0x01);
      Game_Win();
      while(1){
        Delay10ms(2);
        Menu1.ypos += Menu1.dy;
        if(Menu1.ypos<159-Menu2.h){
          Menu1.ypos = HEIGHT+Menu1.h;
        }
        Menu2.ypos += Menu2.dy;
        if(Menu2.ypos<159-Menu1.h){
          Menu2.ypos = HEIGHT+Menu2.h;
        }
        Buffer_UpdateMenu(Menu1);
        Buffer_UpdateMenu(Menu2);
        Buffer_Menu(118,158,(const uint8_t*) &scoreLet[0],7,27);
        if(Player.hp == 1)  Buffer_Menu(118,130, (const uint8_t*) &one[0],7,4);
        else if(Player.hp == 2) Buffer_Menu(118,130,(const uint8_t*) &two[0],7,4);
        else if(Player.hp == 3) Buffer_Menu(118,130,(const uint8_t*) &three[0],7,4);
        Buffer_Menu(15,140,(const uint8_t*) &WinScrn[0],WinScrnw,WinScrnh);
        ST7735_DrawBitmap(0,159,(const uint8_t*)&buffer[0],WIDTH,HEIGHT);
      }
    }
    else if(Stage == lose){
      NVIC_EN0_R &= ~0x0010;         // disable interrupt 4 (Port E) in NVIC
      NVIC_ST_CTRL_R &= ~(0x01);
      Game_Lose();
      while(1){};
    }
  }
}


void SysTick_Handler(void){
  uint8_t i;
  uint8_t j;
  specialCD--;
  Buffer_BG(0,159,&sky[0],128,160);
  // PF2
  ADC_In56(data);
  // player movement
  if((data[1]<0x01F)&&(Player.ypos-Player.h>-1)) 
  {
    Player.dy = -2;
    if((Stage == dog)&&(data[1]<0x01F)&&(Player.ypos-Player.h==60)){
      Player.dy = 0;
    }
    else if((Stage == dog)&&(data[1]<0x01F)&&(Player.ypos-Player.h<=60)){
      Player.dy = 2;
    }
  }
  else if((data[1]>0x090)&&(Player.ypos<HEIGHT+1)) Player.dy = 2;    
  else  Player.dy = 0;

  if((data[0]<0x01F)&&(Player.xpos>-1)) Player.dx = -2;                       
  else if((data[0]>0xFC0)&&(Player.xpos+Player.w<WIDTH+2)) Player.dx = 2; 
  else  Player.dx = 0;

  Player.xpos += Player.dx;
  Player.ypos += Player.dy;
  
  if((Player.State == alive)&&(GPIO_PORTE_DATA_R&0x10)&&(PBullet[PBullIdx].State == dead)&&(waitShoot == 0)){
    PBullet[PBullIdx].State = alive;
    PBullet[PBullIdx].xpos = Player.xpos + 13;
    PBullet[PBullIdx].ypos = Player.ypos - Player.h + 2;
    Sound_Shoot();
    PBullIdx = (PBullIdx+1)%NUMPBULLET;
    waitShoot = SHOOTRATE;
  }
  else if(waitShoot>0){
    waitShoot--;
  }
  else{
    waitShoot = SHOOTRATE;
  }
  
  // stage of game
  if(Stage == mob){
    if(killct == NUMENEMY){
      Game_DogBossInit();
      Stage = dog;
      killct = 0;
      UART_OutChar(0x02);
      UART_OutChar(0x30);
      UART_OutChar(0x30);
      UART_OutChar(0x30);
      UART_OutChar(0x30);
      UART_OutChar(0x30);
      UART_OutChar(0x30);
      UART_OutChar(0x03);
    }
    else{
      // PF2
      for(i = 0; i<NUMPBULLET; i++){
        if(PBullet[i].State == alive){
          PBullet[i].ypos += PBullet[i].dy;
          for(j = 0; j<NUMENEMY; j++){
            if((Enemy[j].State == alive)&&Game_CheckHit(PBullet[i],Enemy[j])){
              Enemy[j].hp--;
              PBullet[i].State = dead;
              Buffer_Update(PBullet[i]);
              PBullet[i].xpos = -1;
              PBullet[i].ypos = -1;
              if(Enemy[j].hp == 0){
                score++;
                Enemy[j].State = dead;
                Buffer_Update(Enemy[j]);
                Enemy[j].xpos = -1;
                Enemy[j].ypos = -1;
                killct++;
              }
            }
          }
        }
        if(PBullet[i].ypos < 0){
          PBullet[i].State = dead;
          Buffer_Update(PBullet[i]);
          PBullet[i].xpos = -1;
          PBullet[i].ypos = -1;
        }
        Buffer_Update(PBullet[i]);
      }
      for(i = 0; i<NUMENEMY; i++){
        if(Enemy[i].State == alive){
          Enemy[i].xpos += Enemy[i].dx;
          Enemy[i].ypos += Enemy[i].dy;
          if(Game_CheckHit(Player,Enemy[i])){
            Player.hp--;
            Sound_Damage();
            if(Player.hp == 0){
              Sound_Death();
              Stage = lose;
              UART_OutChar(0x02);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x03);
            }
            Enemy[i].State = dead;
            Buffer_Update(Enemy[i]);
            Enemy[i].xpos = -1;
            Enemy[i].ypos = -1;
            killct++;
          }
          if(Enemy[i].ypos-Enemy[i].h>HEIGHT){
            Enemy[i].State = dead;
            killct++;
          }
        }
        Buffer_Update(Enemy[i]);
      }
    }
  }
  else if(Stage == dog){
    if(DogBoss.ypos < 55){
      DogBoss.ypos += DogBoss.dy;
    }
    else{
      if(ATKmode == 0){
        ATKone = Random32()%4;
      }
      if(ATKtime != 0){
        DogBoss.xpos += DogBoss.dx;
        if(DogBoss.xpos <= 4) DogBoss.dx *= -1;
        else if(DogBoss.xpos+DogBoss.w >= 124) DogBoss.dx *= -1;
        ATKtime--;
      }
      else if(ATKone==1 && ATKtime==0){
        DogBoss.dx = 0;
        Game_DogBossCharge();
        if((DogBoss.ypos < 150)&&(ATKmode < 25)){
          DogBoss.dy = 4;
          ATKmode++;
          if(Game_CheckHit(Player, DogBoss)){
            Game_DogBossChill();
            Player.hp--;
            Sound_Damage();
            if(Player.hp == 0){
              Sound_Death();
              Stage = lose;
              UART_OutChar(0x02);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x31);
              UART_OutChar(0x03);
            }
            DogBoss.dy = -1;
            ATKmode = 50;
          }
        }
        else{
          DogBoss.dy = -1;
          if(DogBoss.ypos == 55){
            Game_DogBossChill();
            DogBoss.dy = 0;
            DogBoss.dx = 1;
            ATKone = 0;
            ATKmode = 0;
            ATKtime = ATKTIME;
          }
        }
        DogBoss.ypos += DogBoss.dy;
      }
      else if(((ATKone==2 || ATKone==3) && ATKtime==0)&&(EBullet[0].State == dead)){
        //DogBoss.spritePt = woof
        EBullet[0].xpos = DogBoss.xpos+22;
        EBullet[0].ypos = DogBoss.ypos+EBullet[0].h/2;
        EBullet[0].State = alive;
        ATKone = 0;
        ATKtime = ATKTIME;
      }
      else{
        DogBoss.xpos += DogBoss.dx;
        if(DogBoss.xpos <= 4) DogBoss.dx *= -1;
        else if(DogBoss.xpos+DogBoss.w >= 124) DogBoss.dx *= -1;
      }
    }
    for(i = 0; i<NUMPBULLET; i++){
      if(PBullet[i].State == alive){
        PBullet[i].ypos += PBullet[i].dy;
        if(Game_CheckHit(PBullet[i],EBullet[0]) && (EBullet[0].State==alive)&&(DogBoss.ypos == 55)){
          PBullet[i].State = dead;
          Buffer_Update(PBullet[i]);
          PBullet[i].xpos = -1;
          PBullet[i].ypos = -1;
        }
        if(Game_CheckHit(PBullet[i],DogBoss)){
          PBullet[i].State = dead;
          Buffer_Update(PBullet[i]);
          PBullet[i].xpos = -1;
          PBullet[i].ypos = -1;
          DogBoss.hp--;
          if(DogBoss.hp == 0){
            score++;
            Game_DogBossDead();
            while(DogBoss.xpos <= WIDTH){
              Delay10ms(2);
              DogBoss.xpos += DogBoss.dx;
              Buffer_BG(0,159,&sky[0],128,160);
              Buffer_Update(Player);
              Buffer_Update(DogBoss);
              ST7735_DrawBitmap(0,159,(const uint8_t*)&buffer[0],WIDTH,HEIGHT);
            }
            DogBoss.State = dead;
            Sound_Win();
            Stage = win;
            Player.xpos = -1;
            Player.ypos = -1;
            Buffer_Update(Player);
            UART_OutChar(0x02);
            UART_OutChar(0x31);
            UART_OutChar(0x31);
            UART_OutChar(0x31);
            UART_OutChar(0x31);
            UART_OutChar(0x31);
            UART_OutChar(0x31);
            UART_OutChar(0x03);
          }
        }
      }
      if(PBullet[i].ypos < 0){
        PBullet[i].State = dead;
        Buffer_Update(PBullet[i]);
        PBullet[i].xpos = -1;
        PBullet[i].ypos = -1;
      }
      Buffer_Update(PBullet[i]);
    }
    if(EBullet[0].State == alive){
      EBullet[0].ypos += EBullet[0].dy;
      if(Game_CheckHit(Player,EBullet[0])){
        Player.hp--;
        Sound_Damage();
        if(Player.hp == 0){
          Sound_Death();
          Stage = lose;
          UART_OutChar(0x02);
          UART_OutChar(0x31);
          UART_OutChar(0x31);
          UART_OutChar(0x31);
          UART_OutChar(0x31);
          UART_OutChar(0x31);
          UART_OutChar(0x31);
          UART_OutChar(0x03);
        }
        EBullet[0].State = dead;
        Buffer_Update(EBullet[0]);
        EBullet[0].xpos = -1;
        EBullet[0].ypos = -1;
      }
      if(EBullet[0].ypos-EBullet[0].h > HEIGHT){
        EBullet[0].State = dead;
      }
    }
    
    Buffer_Update(EBullet[0]);
    Buffer_Update(DogBoss);
  }
  Buffer_Update(Player);
  // PF2
  bufferFlag = 1;
}

