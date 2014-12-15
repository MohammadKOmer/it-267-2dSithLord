#ifndef __space__
#define __space__



#include "SDL.h"
/*
 *  This file is entirely unused but it might come in handy for finals
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