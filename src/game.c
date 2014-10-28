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


#define MAXSTATE 1

extern SDL_Surface *screen;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Entity *ThePlayer;
extern SDL_Surface *clipmask;
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

int main(int argc, char *argv[])
{
	int done;
	SDLMod mod;

	printf("Starting Game\n");
	done = 0;
	
	Init_All();
	if(ThePlayer == NULL)SpawnPlayer(128,128);
	SpawnFloor(0,600);
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
	}while(!done);
	printf("Ending Program");
	exit(0);
	return 0;
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
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
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
