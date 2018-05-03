// Buffer.c
// Virtual buffer for AirKatz and Game Objects
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date Created: 4/24/2018
// Last Modified: 

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "images.h"
#include "ST7735.h"

#define WIDTH 128
#define HEIGHT 160
#define NUMENEMY 18
#define NUMPBULLET 40
#define NUMEBULLET 20

typedef enum {alive, dead} state_t;
struct object{                        // player, enemy, etc. struct  
  int16_t xpos;
  int16_t ypos;
  int16_t w;
  int16_t h;
  int8_t  dx;
  int8_t  dy;
  uint32_t hp;
  state_t State;
  const uint8_t  *spritePt;
}; typedef struct object obj_t;
volatile obj_t Player;
volatile obj_t DogBoss;
volatile obj_t Enemy[NUMENEMY];
volatile obj_t PBullet[NUMPBULLET];
volatile obj_t EBullet[NUMEBULLET];
volatile obj_t Menu1;
volatile obj_t Menu2;
volatile obj_t Title1;
volatile obj_t Title2;
volatile obj_t TitleTxt;

typedef enum {mob, dog, win, lose} stage_t;
stage_t Stage;

volatile uint8_t buffer[128*160];

void Buffer_BG(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h){
  int j,k;
  int16_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int16_t originalWidth = w;
  int i = w*(h-1);
  int start = 0;
  if((x>=WIDTH) || ((y-h+1) >= HEIGHT) || ((x+w) <= 0) || (y<0)){
    return;                             // image is totally off the screen, do nothing
  }
  if((w>WIDTH) || (h>HEIGHT)){    // image is too wide for the screen, do nothing
    //***This isn't necessarily a fatal error, but it makes the
    //following logic much more complicated, since you can have
    //an image that exceeds multiple boundaries and needs to be
    //clipped on more than one side.
    return;
  }
  if((x+w-1)>=WIDTH){             // image exceeds right of buffer
    skipC = (x+w)-WIDTH;
    w = WIDTH-x;
  }
  if(x<0){                        // image exceeds left of buffer
    w = w+x;
    skipC = -1*x;
    i = i-x;
    x=0;
  }
  if((y-h+1<0)){                  // image exceeds top of screen
    i = i-(h-y-1)*originalWidth;
    h = y+1;
    y = (HEIGHT-1)-y;
  }
  else if(y>=HEIGHT){                  // image exceeds bottom of screen
    h = h-(y-HEIGHT+1);
    y = ((HEIGHT-1)-y)*(-1);
    start = (y-HEIGHT+1)*originalWidth;
  }
  else{
    y = (HEIGHT-1)-y;
  }
  for(j=0; j<h; j++){
    for(k=0; k<w; k++){
      //if((image[start]>>4) != 0x07E){
        buffer[(y*128+x) + (j*128+k)] = image[start];
        start++;
      //}
    }
    start += skipC;
  }
}

void Buffer_Draw(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h){
  int j,k;
  int16_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int16_t originalWidth = w;
  int i = w*(h-1);
  int start = 0;
  int end = w*h;
  if((x>=WIDTH) || ((y-h+1) >= HEIGHT) || ((x+w) <= 0) || (y<0)){
    return;                             // image is totally off the screen, do nothing
  }
  if((w>WIDTH) || (h>HEIGHT)){    // image is too wide for the screen, do nothing
    //***This isn't necessarily a fatal error, but it makes the
    //following logic much more complicated, since you can have
    //an image that exceeds multiple boundaries and needs to be
    //clipped on more than one side.
    return;
  }
  if((x+w-1)>=WIDTH){             // image exceeds right of buffer
    skipC = (x+w)-WIDTH;
    w = WIDTH-x;
  }
  else if(x<0){                        // image exceeds left of buffer
    w = w+x;
    skipC = -1*x;
    start += skipC;
    i = i-x;
    x=0;
  }
  if((y-h+1<0)){                  // image exceeds top of screen
    i = i-(h-y-1)*originalWidth;
    h = y+1;
    y = (HEIGHT-1)-y;
  }
  else if(y>=HEIGHT){                  // image exceeds bottom of screen
    h = h-(y-HEIGHT+1);
    start = (y-HEIGHT+1)*originalWidth;
    y = 0;  // ((HEIGHT-1)-y)*(-1);
  }
  else{
    y = (HEIGHT-1)-y;
  }
  for(j=0; j<h; j++){
    for(k=0; k<w; k++){
      if(start>=end)  break;
      if((!(image[start]==0x1C))&&(!(image[start] == 0x10)))
      {
        buffer[(y*128+x) + (j*128+k)] = image[start];
        start++;
      }
      else if(image[start] == 0x10){
        buffer[(y*128+x) + (j*128+k)] = sky[(y*128+x) + (j*128+k)];
        start++;
      }
      else{
        buffer[(y*128+x) + (j*128+k)] = buffer[(y*128+x) + (j*128+k)];
        start++;
      }
    }
    start += skipC;
  }
}

void Buffer_Menu(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h){
  int j,k;
  int16_t skipC = 0;                      // non-zero if columns need to be skipped due to clipping
  int16_t originalWidth = w;
  int i = w*(h-1);
  int start = 0;
  int end = w*h;
  if((x>=WIDTH) || ((y-h+1) >= HEIGHT) || ((x+w) <= 0) || (y<0)){
    return;                             // image is totally off the screen, do nothing
  }
  if((x+w-1)>=WIDTH){             // image exceeds right of buffer
    skipC = (x+w)-WIDTH;
    w = WIDTH-x;
  }
  else if(x<0){                        // image exceeds left of buffer
    w = w+x;
    skipC = -1*x;
    start += skipC;
    i = i-x;
    x=0;
  }
  if((y-h+1<0)){                  // image exceeds top of screen
    h = y+1;
  }
  if(y>=HEIGHT){                  // image exceeds bottom of screen
    h = h-(y-HEIGHT+1);
    start = (y-HEIGHT+1)*originalWidth;
    y = 0;  // ((HEIGHT-1)-y)*(-1);
  }
  else{
    y = (HEIGHT-1)-y;
  }
  for(j=0; j<h; j++){
    for(k=0; k<w; k++){
      if(start>=end)  break;
      if(!(image[start]==0x1C))
      {
        buffer[(y*128+x) + (j*128+k)] = image[start];
        start++;
      }
      else{
        buffer[(y*128+x) + (j*128+k)] = buffer[(y*128+x) + (j*128+k)];
        start++;
      }
    }
    start += skipC;
  }
}

void Buffer_Update(obj_t bufferObj){
  if(bufferObj.State == dead){
    Buffer_Draw(bufferObj.xpos, bufferObj.ypos, &Green[0], bufferObj.w, bufferObj.h);
  }
  else{
    Buffer_Draw(bufferObj.xpos, bufferObj.ypos, bufferObj.spritePt, bufferObj.w, bufferObj.h);
  }
}

void Buffer_UpdateMenu(obj_t bufferMenu){
  Buffer_Menu(bufferMenu.xpos,bufferMenu.ypos,bufferMenu.spritePt,bufferMenu.w,bufferMenu.h);
}

void Game_CreatePlayer(int16_t x, int16_t y){
  Player.xpos = x;
  Player.ypos = y;
  Player.dx = 0;
  Player.dy = 0;
  Player.w = Catw;
  Player.h = Cath;
  Player.hp = 3;
  Player.State = alive;
  Player.spritePt = &Cat[0];
}

void Game_CreateEnemy(int16_t x, int16_t y, const uint8_t *spritePt){
  uint8_t i;
  for(i = 0; i<NUMENEMY; i++){
    Enemy[i].w = Monsterw;
    Enemy[i].h = Monsterh;
    Enemy[i].dx = 0;
    Enemy[i].dy = 1;
    Enemy[i].hp = 3;
    Enemy[i].State = alive;
    Enemy[i].spritePt = spritePt;
  }
  for(i=0; i<8; i++){
    Enemy[i].xpos = i*16 +1;
  }
  Enemy[3].ypos = -1;
  Enemy[4].ypos = -1;
  Enemy[2].ypos = Enemy[3].ypos - (Enemy[3].h/2);
  Enemy[5].ypos = Enemy[4].ypos - (Enemy[4].h/2);
  Enemy[1].ypos = Enemy[2].ypos - (Enemy[2].h/2);
  Enemy[6].ypos = Enemy[5].ypos - (Enemy[5].h/2);
  Enemy[0].ypos = Enemy[1].ypos - (Enemy[1].h/2);
  Enemy[7].ypos = Enemy[6].ypos - (Enemy[6].h/2);
  
  Enemy[8].xpos = 16*2+1;
  Enemy[8].ypos = Enemy[0].ypos - 90;
  Enemy[9].xpos = 16*3+1+8;
  Enemy[9].ypos = Enemy[8].ypos - Enemy[8].h - 20;
  Enemy[10].xpos = Enemy[9].xpos + 16+1+8;
  Enemy[10].ypos = Enemy[8].ypos + 15;
  Enemy[11].xpos = Enemy[10].xpos + 16+1;
  Enemy[11].ypos = Enemy[10].ypos - 15;
  Enemy[12].xpos = Enemy[11].xpos;
  Enemy[12].ypos = Enemy[11].ypos - Enemy[11].h;
  Enemy[13].xpos = Enemy[10].xpos;
  Enemy[13].ypos = Enemy[9].ypos;
  Enemy[14].xpos = Enemy[11].xpos;
  Enemy[14].ypos = Enemy[13].ypos - 15;
  Enemy[15].xpos = Enemy[11].xpos;
  Enemy[15].ypos = Enemy[14].ypos - Enemy[14].h;
  Enemy[16].xpos = Enemy[10].xpos;
  Enemy[16].ypos = Enemy[15].ypos - 15;
  Enemy[17].xpos = Enemy[8].xpos;
  Enemy[17].ypos = Enemy[8].ypos - Enemy[8].h - 65;
}

void Game_CreateBoss(volatile obj_t *Boss, uint16_t w, uint16_t h, const uint8_t *spritePt){
  (*Boss).xpos = 40;
  (*Boss).ypos = 0;
  (*Boss).dy = 1;
  (*Boss).dx = 1;
  (*Boss).w = w;
  (*Boss).h = h;
  (*Boss).hp = 60;
  (*Boss).State = alive;
  (*Boss).spritePt = spritePt;
}

void Game_CreatePBullets(void){
  uint8_t i;
  for(i=0;i<NUMPBULLET;i++){
    PBullet[i].h = PBullh;
    PBullet[i].w = PBullw;
    PBullet[i].hp = 1;
    PBullet[i].dx = 0;
    PBullet[i].dy = -2;
    PBullet[i].State = dead;
    PBullet[i].spritePt = &PBull[0];
  }
}

void Game_CreateEBullets(stage_t bulletType){
  uint8_t i;
  if(bulletType == mob){
    for(i=0;i<NUMEBULLET;i++){
//      EBullet[i].h = 13;
//      EBullet[i].w = 5;
//      EBullet[i].hp = 1;
//      EBullet[i].dx = 0;
//      EBullet[i].dy = -1;
//      EBullet[i].State = dead;
//      EBullet[i].spritePt = &EBull[0];
    }
  }
  else if(bulletType == dog){   // woof
    EBullet[0].h = DogBullh;
    EBullet[0].w = DogBullw;
    EBullet[0].dy = 1;
    EBullet[0].State = dead;
    EBullet[0].spritePt = &DogBull[0];
  }
}

void Game_DogBossInit(void){
  Game_CreatePBullets();
  Game_CreateEBullets(dog);
  Game_CreateBoss(&DogBoss, Dognicew, Dogniceh, &Dognice[0]);
  Buffer_BG(0,159,&sky[0],128,160);
}

void Game_DogBossCharge(void){
  DogBoss.h = Dogh;
  DogBoss.spritePt = &Dog[0];
}

void Game_DogBossChill(void){
  DogBoss.h = Dogniceh;
  DogBoss.spritePt = &Dognice[0];
}

void Game_DogBossDead(void){
  DogBoss.dy = 0;
  DogBoss.dx = 4;
  DogBoss.spritePt = &Dogdead[0];
}

extern void Delay100ms(uint32_t count);

void Game_Init(void){
  uint8_t i;
  //PF1
  Buffer_BG(0,159,&sky[0],128,160);
  //PF1
  // create enemies here, create player, create boss, draw onto buffer
  Game_CreatePlayer(52, 159);
  Buffer_Update(Player);
  Game_CreateEnemy(0,0, (const uint8_t *) &Monster[0]);
  for(i = 0; i<NUMENEMY; i++){
    Buffer_Update(Enemy[i]);
  }
  Game_CreatePBullets();
  ST7735_DrawBitmap(0,159, (const uint8_t *) &buffer[0],128,160);
  // ST7735_DrawBitmap(0, 159, &sky[0], 128, 160);
}

uint8_t Game_CheckHit(obj_t objOne, obj_t objTwo){
  uint16_t obj1x;
  uint16_t obj1y = 2;
  uint16_t obj2x;
  uint16_t obj2y = 2;

  if(objOne.h>15) obj1y = objOne.h/3;
  if(objTwo.h>15) obj2y = objTwo.h/3;
  if((objOne.w<16)&&(objTwo.w<16)){
    obj1x = 0;
    obj2x = 0;
  }
  else if((objOne.w<16)&&(!(objTwo.w<16))){
    obj1x = 0;
    obj2x = objTwo.w/6;
  }
  else if((!(objOne.w<16)&&(objTwo.w<16))){
    obj1x = objOne.w/6;
    obj2x = 0;
  }
  else{
    obj1x = objOne.w/8;
    obj2x = objTwo.w/8;
  }
  if((objOne.h>40)&&(objOne.w>40)){
    if(objOne.h>15){
      obj2y = 3;
    }
    obj1y = 3;
    obj1x = 3;
  }
  if((objTwo.h>40)&&(objTwo.w>40)){
    if(objOne.h>15){
      obj1y = 3;
    }
    obj2y = 3;
    obj2x = 3;
  }
  if((objTwo.xpos+obj2x<=objOne.xpos+objOne.w-obj1x)&&(objTwo.xpos+obj2x>=objOne.xpos+obj1x)
    &&(objTwo.ypos-obj2y>=objOne.ypos-objOne.h+obj1y)&&(objTwo.ypos-obj2y<=objOne.ypos-obj1y))  return 1;
  if((objOne.xpos+obj1x<=objTwo.xpos+objTwo.w-obj2x)&&(objOne.xpos+obj1x>=objTwo.xpos+obj2x)
    &&(objOne.ypos-obj1y>=objTwo.ypos-objTwo.h+obj2y)&&(objOne.ypos-obj1y<=objTwo.ypos-obj2y))  return 1;
  
  if((objOne.xpos+objOne.w-obj1x>=objTwo.xpos+obj2x)&&(objOne.xpos+objOne.w-obj1x<=objTwo.xpos+objTwo.w-obj2x)
    &&(objOne.ypos-obj1y>=objTwo.ypos-objTwo.h+obj2y)&&(objOne.ypos-obj1y<=objTwo.ypos-obj2y))  return 1;
  if((objTwo.xpos+objTwo.w-obj2x>=objOne.xpos+obj1x)&&(objTwo.xpos+objTwo.w-obj2x<=objOne.xpos+objOne.w-obj1x)
    &&(objTwo.ypos-obj2y>=objOne.ypos-objOne.h+obj1y)&&(objTwo.ypos-obj2y<=objOne.ypos-obj1y))  return 1;
  
  if((objOne.xpos+obj1x<=objTwo.xpos+objTwo.w-obj2x)&&(objOne.xpos+obj1x>=objTwo.xpos+obj2x)
    &&(objOne.ypos-objOne.h+obj1y<=objTwo.ypos-obj2y)&&(objOne.ypos-objOne.h+obj1y>=objTwo.ypos-objTwo.h+obj2y)) return 1;
  if((objTwo.xpos+obj2x<=objOne.xpos+objOne.w-obj1x)&&(objTwo.xpos+obj2x>=objOne.xpos+obj1x)
    &&(objTwo.ypos-objTwo.h+obj2y<=objOne.ypos-obj1y)&&(objTwo.ypos-objTwo.h+obj2y>=objOne.ypos-objOne.h+obj1y)) return 1;
  
  if((objOne.xpos+objOne.w-obj1x>=objTwo.xpos+obj2x)&&(objOne.xpos+objOne.w-obj1x<=objTwo.xpos+objTwo.w-obj2x)
    &&(objOne.ypos-objOne.h+obj1y<=objTwo.ypos-obj2y)&&(objOne.ypos-objOne.h+obj1y>=objTwo.ypos-objTwo.h+obj2y))  return 1;
  if((objTwo.xpos+objTwo.w-obj2x>=objOne.xpos+obj1x)&&(objTwo.xpos+objTwo.w-obj2x<=objOne.xpos+objOne.w-obj1x)
    &&(objTwo.ypos-objTwo.h+obj2y<=objOne.ypos-obj1y)&&(objTwo.ypos-objTwo.h+obj2y>=objOne.ypos-objOne.h+obj1y))  return 1;
  return 0;
}

void Game_Win(void){
  Menu1.xpos = 0;
  Menu1.ypos = 159;
  Menu1.h = Starth;
  Menu1.w = Startw;
  Menu1.dy = -1;
  Menu1.spritePt = &Start[0];
  Menu2.xpos = 0;
  Menu2.h = Starth;
  Menu2.ypos = 159+Menu2.h;
  Menu2.w = Startw;
  Menu2.dy = -1;
  Menu2.spritePt = &Start[0];
  Buffer_Menu(Menu1.xpos,Menu1.ypos,(const uint8_t *)Menu1.spritePt,Menu1.w,Menu1.h);
  Buffer_Menu(15,140,(const uint8_t*) &WinScrn[0],WinScrnw,WinScrnh);
  ST7735_DrawBitmap(0,159,(const uint8_t*)&buffer[0],WIDTH,HEIGHT);
}

void Game_Lose(void){
  ST7735_FillScreen(0);
  ST7735_DrawBitmap(25,124,&GameOver[0],GameOverw,GameOverh);
}

void Start_Init(void){
  Menu1.xpos = 0;
  Menu1.ypos = 159;
  Menu1.h = Starth;
  Menu1.w = Startw;
  Menu1.dy = 1;
  Menu1.spritePt = &Start[0];
  Menu2.xpos = 0;
  Menu2.h = Starth;
  Menu2.ypos = (signed) (Menu1.ypos-Menu1.h+1);
  Menu2.w = Startw;
  Menu2.dy = 1;
  Menu2.spritePt = &Start[0];
  Title1.xpos = 10;
  Title1.h = Titleh;
  Title1.w = Titlew;
  Title1.ypos = 159+Title1.h;
  Title1.dy = -2;
  Title1.spritePt = &TitleScrn[0];
  Title2.xpos = Title1.xpos;
  Title2.h = Titleh;
  Title2.w = Titlew;
  Title2.ypos = Title1.ypos+Title2.h+20;
  Title2.dy = -2;
  Title2.spritePt = &TitleScrn[0];
  TitleTxt.xpos = 90;
  TitleTxt.ypos = 120;
  TitleTxt.h = Meowh;
  TitleTxt.w = Meoww;
  TitleTxt.spritePt = &Meow[0];
  Buffer_Menu(Menu1.xpos,Menu1.ypos,(const uint8_t *)Menu1.spritePt,Menu1.w,Menu1.h);
}
