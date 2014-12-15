/*#include "chipmunk/chipmunk.h"*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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
void MakeLevel(char level [20][20], char* filename);
int main(int argc, char *argv[])
{
	int level;
	int done;
	SDLMod mod;
	 int i;
	 int j;
	 int mapeditmode;
	 int editing;
	 char lvlArray [20][20];
	 FILE  *lvl;


	 mapeditmode=0;
	printf("Starting Game\n");
	for(i = 1;i < argc;i++)
	{
		if(strcmp("-mapedit",argv[i])== 0)
		{
		  mapeditmode = 1;
		  editing = i+1;
		}
   
	 }
	if(!mapeditmode){
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
			/*if(EnemyPresent==0){
				/*could theoretically stuff a transition here
				level++;
				ClearEntities();
				InitEntityList();
				PrepareQuadtrees();
				LoadHUD();
				StartLevel(level%2);
			}*/
		}while(!done);
	}
	if(mapeditmode){
		lvl = fopen(argv[editing],"r"); /*checking to see if we are editing a level or
											making a new one */
 
	   if( lvl == NULL )
	   {
		  printf("Creating new level");
	   }else{
			for(i = 0 ; i<20;i++){
				for(j=0;j<20;j++){
					lvlArray[i][j]=fgetc(lvl);
				}
			}
			fclose(lvl);
	   }
	   MakeLevel(lvlArray,argv[editing]);
	}
	printf("Ending Program");
	exit(0);
	return 0;
}
void MakeLevel(char level [20][20], char* filename){
	int done;
	int x, y;
	SDLMod mod;

	do
	{
		ResetBuffer();
		SDL_PumpEvents();
		keys = SDL_GetKeyState(NULL);    
		mod = SDL_GetModState();
		Draw_ALL();
		if(keys[SDLK_ESCAPE] == 1)done = 1;
		if(keys[SDLK_DOWN]== 1) y+=1;
		if(keys[SDLK_LEFT]== 1)x-=1;
		if(keys[SDLK_UP]== 1)y-=1;
		if(keys[SDLK_RIGHT]== 1)x+=1;
		if(x<0) x=0;
		if(x>20)x=20;
		if(y<0)y=0;
		if(y>20)y=20;
		Think_ALL();
		Update_ALL();
		NextFrame();
	}while(!done);

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
