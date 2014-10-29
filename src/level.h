#ifndef __space__
#define __space__



#include "SDL.h"
/*
 *  This file will define the data structures and function prototypes needed for rendering and animating
 *  the scrolling backgrounds for this game.
 *  The functions will create teh entities needed for an interactive level.
 */

#define SPACE_W     128
#define SPACE_H     128
#define MAX_OBJ     128
#define MAX_ENT     128


typedef struct LEVEL_STRUCT
{
  Uint8   tilemap[SPACE_H][SPACE_W];
  Uint16  width,height;
  int spawncount;
  Uint8 tileset;
  char levelname[40];
  char bgimage[40];
  char nextLevelFile[40];
}Level;

void GenerateLevel(char *filename);
void LoadLevel(char *filename);
void SaveLevel(char *filename);
void DrawLevel();
void CloseLevel();

#endif