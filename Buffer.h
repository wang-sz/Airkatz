// Buffer.c
// Virtual buffer for AirKatz
// Runs on TM4C123
// Written by: Anna Sim and Sean Wang
// Date Created: 4/24/2018
// Last Modified: 

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "TExaS.h"
// #include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Sound.h"

#define WIDTH 128
#define HEIGHT 160
#define NUMENEMY 18
#define NUMPBULLET 20
#define NUMEBULLET 20

typedef enum {alive, dead} state_t;
struct object{                  // player, enemy, etc. struct  
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



extern enum stage_t {mob, dog, win, lose} Stage;

void Game_CreatePlayer(int16_t x, int16_t y);

void Game_Init(void);

void Game_DogBossInit(void);

void Game_DogBossCharge(void);

void Game_DogBossChill(void);

void Game_DogBossDead(void);

void Game_CreateEBullets(enum stage_t bulletType);

void Buffer_Update(obj_t objectBuffer);

void Buffer_UpdateMenu(obj_t bufferMenu);

void Buffer_BG(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h);

void Buffer_Draw(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h);

void Buffer_Menu(int16_t x, int16_t y, const uint8_t *image, int16_t w, int16_t h);

uint8_t Game_CheckHit(obj_t objOne, obj_t objTwo);

void Game_Win(void);

void Game_Lose(void);

void Start_Init(void);
