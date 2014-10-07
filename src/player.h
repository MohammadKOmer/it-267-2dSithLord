#ifndef __PLAYER__
#define __PLAYER__

#include "entity.h"


#define MAXFORCE  100



typedef struct PLAYER_T
{
 
  int  mx,my;               /*the coordinates that the player is in.*/
  int force
}PlayerStats;

void SpawnPlayer(int x, int y);
void LoadHUD();
void DrawHUD(Entity *self);
void DefaultConfig();
void LoadPlayerScreen();

/*below is the required end of line*/
#endif