#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "SDL.h"
#include "graphics.h"
#include "entity.h"
#include "player.h"
#include "projectiles.h"
#include "space.h"
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
void MakeLevel(char level [20][20], char* filename, int Creating);
int main(int argc, char *argv[])
{
	int level;
	int done;
	SDLMod mod;
	int i;
	int j;
	int creating;
	int mapeditmode;
	int editing;
	char lvlArray [20][20];
	
	FILE  *lvl;
	done = 0;
	
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
	Init_All();
	if(mapeditmode==0){
		SpawnPlayer(0,0);
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
			NextFrame(30);
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
	}else if(mapeditmode==1){
		
		lvl = fopen(argv[editing],"r"); /*checking to see if we are editing a level or
										making a new one */

		if( lvl == NULL )
		{
			printf("Creating new level\n");
			creating = 1;
		}else{
			creating = 0;
			for(i = 0 ; i<20;i++){
				for(j=0;j<20;j++){
					lvlArray[i][j]=fgetc(lvl);
				}
				fgetc(lvl); /*to eat up newlines*/
			}
			fclose(lvl);
		}
		MakeLevel(lvlArray,argv[editing],creating);
	}
	printf("Ending Program\n");
	exit(0);
	return 0;
}
void MakeLevel(char level [20][20], char* filename, int Creating){
	int done;
	int x, y,c;
	SDLMod mod;
	FILE *fp;
	if(Creating==0){
		for(x=0;x<20;x++){
			for(y=0;y<20;y++){
				if(level[x][y]!='9'){
					c=level[x][y] - '0';
					SpawnSquare(x*256,y*256,c);
				}
			}
		}
	}
	x=0;y=0;done = 0;
	SpawnPlayer(0,0);
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
		if(x>19)x=19;
		if(y<0)y=0;
		if(y>19)y=19;
		/* 0 is a spawn point
		  1 is a wall
		  2 - 7 are enemies
		  8 is a boss
	   */
		
		if(keys[SDLK_0]== 1){
			level[x][y] = '0';
			SpawnSquare(x*256,y*256,0);
		}
		else if(keys[SDLK_1]== 1){
			level[x][y] = '1';
			SpawnSquare(x*256,y*256,1);
		}
		else if(keys[SDLK_2]== 1){
			level[x][y] = '2';
			SpawnSquare(x*256,y*256,2);
		}
		else if(keys[SDLK_3]== 1){
			level[x][y] = '3';
			SpawnSquare(x*256,y*256, 3 );
		}
		else if(keys[SDLK_4]== 1){
			level[x][y] = '4';
			SpawnSquare(x*256,y*256,4);
		}
		else if(keys[SDLK_5]== 1){
			level[x][y] = '5';
			SpawnSquare(x*256,y*256,5);
		}
		else if(keys[SDLK_6]== 1){
			level[x][y] = '6';
			SpawnSquare(x*256,y*256,6);
		}
		else if(keys[SDLK_7]== 1){
			level[x][y] = '7';
			SpawnSquare(x*256,y*256,7);
		}
		else if(keys[SDLK_8]== 1){
			level[x][y] = '8';
			SpawnSquare(x*256,y*256,8);
		}
		printf("editing location %i   %i\n",x,y);
		/*Think_ALL();*/
		/*Update_ALL();*/
		UpdateCamera();
		ThePlayer->s.x = 256*x;
		ThePlayer->s.y = 256*y;
		NextFrame(100);
	}while(!done);
	fp = fopen(filename,"w+");
	for(x=0;x<20;x++){
			for(y=0;y<20;y++){
				if(level[x][y]>='0'&&level[x][y]<='9'){
					printf("%c \t",level[x][y]);
					fputc(level[x][y],fp);
				}else{
					fputc('9',fp);
					printf("%c \t",'9');
				}
			}
			fputc('\n',fp);
			printf("\n");
	}
}
/*this amounts to a cheap level list */
void StartLevel(char* filename){
	int x;
	int y;
	FILE *lvl;
	char spot;
	spot=' ';
	lvl = fopen(filename,"r");
	for(x = 0 ; x<20;x++){
		for(y=0;y<20;y++){
			spot = fgetc(lvl);
			if(spot=='0'){
				SpawnPlayer(x*256,y*256);
			}
			else if(spot =='1'){
				SpawnFloor(x*256,y*256);
			}
			else if(spot == '2'){
				SpawnSquare(x*256,y*256,2);
			}
			else if(spot == '3'){
				SpawnSquare(x*256,y*256, 3 );
			}
			else if(spot == '4'){
				SpawnSquare(x*256,y*256,4);
			}
			else if(spot == '5'){
				SpawnSquare(x*256,y*256,5);
			}
			else if(spot == '6'){
				SpawnSquare(x*256,y*256,6);
			}
			else if(spot== '7'){
				SpawnSquare(x*256,y*256,7);
			}
			else if(spot == '8'){
				SpawnSquare(x*256,y*256,8);
			}else if(spot== '9'){
				SpawnSquare(x*256,y*256,9);
			}
		}
		fgetc(lvl); /*to eat up newlines*/
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
	InitSpace();
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
