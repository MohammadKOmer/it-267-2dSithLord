#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "graphics.h"
#include "entity.h"
#include "player.h"
#include "projectiles.h"
#include "quadtree.h"
#include "level.h"
#include "enemy.h"

#define MAXSTATE 1

extern SDL_Surface *screen;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Entity *ThePlayer;
extern SDL_Surface *clipmask;
extern int EnemyPresent;
int drawents = 1,drawboxes = 0;
int windowed = 1;
int mapeditmode = 0;
int bucketdraw = 0;


Uint8 *keys;


void Init_All();
void Update_ALL();
int Think_ALL();
void Draw_ALL();
void DrawSplashScreen();
void GiveInfo();
void PopUpWindow(Sprite *sprite,char *text,Uint32 Color,int pwait);
void UpdateCamera();
void UpdateMapCamera();
void StartLevel(int i);
int main(int argc, char *argv[])
{
	int level;
	int done;
	SDLMod mod;

	printf("Starting Game\n");
	done = 0;
	level=0;
	Init_All();
	StartLevel(level);
	
	do
	{
		ResetBuffer();
		PrepareQuadtrees();
		SDL_PumpEvents();
		keys = SDL_GetKeyState(NULL);    
		mod = SDL_GetModState();
		Draw_ALL();
		if(keys[SDLK_ESCAPE] == 1)done = 1;
		Think_ALL();
		Update_ALL();
		NextFrame();
		if(EnemyPresent==0){
			/*could theoretically stuff a transition here*/
			level++;
			ClearEntities();
			InitEntityList();
			PrepareQuadtrees();
			LoadHUD();
			StartLevel(level%2);
		}
	}while(!done);
	printf("Ending Program");
	exit(0);
	return 0;
}

/*this amounts to a cheap level list */
void StartLevel(int i){
	if(i==0){
		SpawnPlayer(256,1428);
		SpawnFloor(0,1600);
		SpawnWall(0,1000);
		SpawnFloor(1600,1150);
		SpawnWall(3160,1000);
		SpawnWall(3160,0);
		SpawnEnemy(256,1200,ET_TROOPER);
		SpawnEnemy(1256,1200,ET_SITH);
	}
	if(i==1){
		SpawnPlayer(1000,1200);
		SpawnFloor(0,1600);
		SpawnWall(1600,1000);
		SpawnWall(0,1000);
		SpawnEnemy(1256,1200,ET_SITH);
	}
	
}
void CleanUpAll()
{
	CloseSprites();
	ClearEntities();
	PrepareQuadtrees();

	SDL_FreeSurface(clipmask);
	/*any other cleanup functions can be added here*/ 
}

void Init_All()
{
	Init_Graphics(windowed);
	InitSpriteList();
	
	atexit(CleanUpAll);
	InitEntityList();
	InitQuadtrees();
	initProjectiles();
	LoadHUD();
	DrawLevel();


}
/*calls all of the update functions for everything*/
void Update_ALL()
{
	UpdateEntities();
	UpdateCamera();
}

/*calls all of the think function for everything*/
int Think_ALL()
{
	int done = 0;
	ThinkEntities();
	return done;
}

/*calls all of the draw functions for everything*/
void Draw_ALL()
{

	if(drawents)
	{
		DrawEntities();
		if((ThePlayer != NULL))DrawEntity(ThePlayer);
	}
	if(drawboxes)DrawBBoxEntities();

	DrawHUD(ThePlayer);


}



void UpdateCamera()
{
	Camera.x = (int)ThePlayer->s.x - (Camera.w >> 1);
	Camera.y = (int)ThePlayer->s.y - (Camera.h >> 1);
	

}
