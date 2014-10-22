#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "quadtree.h"
#include "projectiles.h"

extern SDL_Surface *screen;
extern SDL_Joystick *joy;

enum PlayerInputs {PI_MovDown,PI_MovDownLeft,PI_MovLeft,PI_MovUpLeft,PI_MovUp,PI_MovUpRight,PI_MovRight,PI_MovDownRight,
	PI_Lightsaber,PI_Push,PI_Lift,PI_Jump,PI_NULL};


Uint32 KeyButtons[PI_NULL]; /* okay thats clever*/

Entity *ThePlayer = NULL;
SDL_Surface *titlebar = NULL;
int attackleft = SDLK_LEFT;
void UpdatePlayer(Entity *self);
void PlayerThink(Entity *self);
void FinalOutput();
void UpdateInput();

PlayerSpecificVars __Nai;


void SpawnPlayer(int x,int y)
{
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)
	{
		fprintf(stderr,"Unable to generate player entity; %s",SDL_GetError());
		exit(0);
	}
	strcpy(newent->EntName,"Player\0");
	newent->sprite = LoadSprite("images/NaieraSheet.png",256,256);



	newent->size.x = newent->sprite->w;
	newent->size.y = newent->sprite->h;
	newent->update = UpdatePlayer;
	newent->think = PlayerThink;
	newent->UpdateRate = 35;
	newent->ThinkRate = 35;

	newent->takedamage = 1;

	newent->Unit_Type = ET_Player;

	newent->healthmax = 100;
	newent->health = 100;
	newent->frame = 0;


	newent->frate = 1;
	newent->state = ST_IDLE;

	newent->s.x = x;
	newent->s.y = y;
	newent->v.x = 0;
	newent->v.y = 0;
	newent->maxspeed = 12;
	newent->movespeed = 0;
	newent->accel = 4;

	newent->Boundingbox.x = 10;
	newent->Boundingbox.y = 10;
	newent->Boundingbox.w = 28;
	newent->Boundingbox.h = 50;  
	newent->origin.x = 24;
	newent->origin.y = 32;
	__Nai.liftSpeed = 10;
	__Nai.force=100;
	UpdatePlayer(newent);
	ThePlayer = newent;
	atexit(FinalOutput);
	insert(newent,__quadtreeList);
}

/*
* Think Function handles giving the entity new orders and changing states
*/

void PlayerThink(Entity *self)
{
	float t;
	char text[40];
	Uint8 *keys = SDL_GetKeyState(NULL);
	__Nai.timeTillNestStab--;
	__Nai.timeTillNextPush--;
	/*Capture Input*/
	if((self->state != ST_DEAD)&&(self->state != ST_DIE))
	{

		if(self->state != ST_LIFTING){
			/*catch horrizonal input, this moves the player unless they happen to be lifting something*/
			if(keys[KeyButtons[PI_MovLeft]]&&keys[KeyButtons[PI_MovRight]]){
				/*lets kill input if it catches both so that neither out prioritize the other, i should make this more robust but class project*/
				self->state=ST_IDLE;
			}
			else if(keys[KeyButtons[PI_MovLeft]])  /*there is a middleman i could probably cut out but hey, future functionality*/
			{     
				self->state=ST_MOVINGH;
				self->face=F_LEFT;

			} else if(keys[KeyButtons[PI_MovRight]])
			{     
				self->state=ST_MOVINGH;
				self->face=F_RIGHT;

			}else{  /* if released*/
				self->state=ST_IDLE;
			}
		} else{
			/*cease lifting the thing*/
			if(!keys[KeyButtons[PI_Lift]]){
					self->state=ST_IDLE;
			}

			/*this is where we move enemies that are being lifted, player cannot move while lifting things*/
			if(keys[KeyButtons[PI_MovLeft]]&&keys[KeyButtons[PI_MovRight]]){
				/*lets kill input if it catches both so that neither out prioritize the other, i should make this more robust but class project*/
				
			}
			else if(keys[KeyButtons[PI_MovLeft]])  /*there is a middleman i could probably cut out but hey, future functionality*/
			{     
				__Nai.liftingTarget->v.x+=__Nai.liftSpeed;

			} else if(keys[KeyButtons[PI_MovRight]])
			{     
				__Nai.liftingTarget->v.x=-__Nai.liftSpeed;

			}else{  /* if released*/
				__Nai.liftingTarget->v.x=0;
			}
			if(keys[KeyButtons[PI_MovUp]]&&keys[KeyButtons[PI_MovDown]]){
				/*lets kill input if it catches both so that neither out prioritize the other, i should make this more robust but class project*/
				
			}
			else if(keys[KeyButtons[PI_MovUp]])  /*there is a middleman i could probably cut out but hey, future functionality*/
			{     
				__Nai.liftingTarget->v.y-=__Nai.liftSpeed;

			} else if(keys[KeyButtons[PI_MovUp]])
			{     
				__Nai.liftingTarget->v.y=+__Nai.liftSpeed;

			}else{  /* if released*/
				__Nai.liftingTarget->v.x=0;
			}	
		}
		if(keys[KeyButtons[PI_Lightsaber]]&&__Nai.timeTillNestStab<0){
			__Nai.timeTillNestStab=80;
			if(self->face==F_LEFT){
				SpawnSaberhit(self,self->s.x-(.5*self->size.x),self->s.y,0,0,50,0,0,Blue_,0);
			}else{
				SpawnSaberhit(self,self->s.x+(.5*self->size.x),self->s.y,0,0,50,0,0,Blue_,0);
			}
		}
		if(keys[KeyButtons[PI_Push]]&&__Nai.timeTillNextPush<0&&__Nai.force>20){
			__Nai.timeTillNextPush=40;
			__Nai.force-=20;
			if(self->face==F_LEFT){
				SpawnForcePush(self,self->s.x-16,self->s.y,180,40,0,0,0,0,0);
			}else{
				SpawnForcePush(self,self->s.x+256+16,self->s.y,0,40,0,0,0,0,0);
			}
		
		}
		if(keys[KeyButtons[PI_Jump]]&&self->grounded==1){
			self->v.y+=16;
		}

	}
}

/*updates the entity's postition and handles animation*/

void UpdatePlayer(Entity *self)
{
	
	UpdateEntityPosition(self);
	if(self->state==ST_IDLE||self->state==ST_LIFTING){
		self->fcount=4;
		if(self->face==F_LEFT){
			self->frameR=2;
		}else{
			self->frameR=3;
		}
			
	}else{
		self->fcount=14;
		if(self->face==F_LEFT){
			self->frameR=0;
		}else{
			self->frameR=1;
		}
	}
	if(self->grounded == 0)
	{
		self->v.y += 2;     /*gravity at work*/
	}

}



void DefaultConfig()
{

	/*{PI_MovDown,PI_MovDownLeft,PI_MovLeft,PI_MovUpLeft,PI_MovUp,PI_MovUpRight,PI_MovRight,PI_MovDownRight,
	PI_Lightsaber,PI_Push,PI_Lift,PI_Jump,PI_NULL}*/
	KeyButtons[PI_MovDown] = SDLK_DOWN;
	KeyButtons[PI_MovLeft] = SDLK_LEFT;
	KeyButtons[PI_MovUp] = SDLK_UP;
	KeyButtons[PI_MovRight] = SDLK_RIGHT;
	KeyButtons[PI_Lightsaber] = SDLK_z;
	KeyButtons[PI_Push] = SDLK_x;
	KeyButtons[PI_Jump] = SDLK_c;
	KeyButtons[PI_Lift] = SDLK_a;

}




void CloseHUD()
{
	if(titlebar != NULL)
	{
		SDL_FreeSurface(titlebar);
		titlebar = NULL;
	}
}

void LoadHUD()
{
	SDL_Surface *temp;
	temp = IMG_Load("images/titlebar1.png");
	if(temp == NULL)
	{
		fprintf(stderr,"Unable to load title bar: %s\n",SDL_GetError());
		exit(0);
	}
	titlebar = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_SetColorKey(titlebar, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(titlebar->format,0,0,0));
	atexit(CloseHUD);
}

void DrawHUD(Entity *self)
{
	SDL_Rect window;
	char text[60];
	window.x = 0;
	window.y = 0;
	if(titlebar == NULL)
	{
		window.w = screen->w;
		window.h = 32;
		SDL_FillRect(screen, &window, IndexColor(DarkBlue));
	}
	else
	{
		SDL_BlitSurface(titlebar,NULL,screen,NULL);
	}
//	sprintf(text,"Score: %i",self->Score);
	//DrawText(text,screen,5,5,IndexColor(self->Color),F_Medium);
	window.x = screen->w - 120;
	window.w = 100;
	window.y = 5;
	window.h = 16;
	SDL_FillRect(screen, &window, IndexColor(Red));
	if(self->health >= 0)
		window.w = ((float)self->health / (float)self->healthmax) * 100;
	else window.w = 0;
	SDL_FillRect(screen, &window, IndexColor(Green));
}






