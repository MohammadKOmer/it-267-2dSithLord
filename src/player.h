#ifndef __PLAYER__
#define __PLAYER__

#include "entity.h"


#define MAXFORCE  100


enum ForceStates {AS_NULL,AS_COOLDOWN,AS_PUSHING};
typedef struct Player_T
{	int lightSabering;
	int timeTillNestStab;
	int timeTillNextPush;
	int forceState;
	int liftSpeed;
	int  mx,my;               /*the coordinates that the player is in.*/
	int force;				  /*ammount of force power player has */
	Entity *liftingTarget; /*target of force lift*/
}PlayerSpecificVars;

void SpawnPlayer(int x, int y);
void LoadHUD();
void DrawHUD(Entity *self);
void DefaultConfig();
void LoadPlayerScreen();

/*below is the required end of line*/
#endif