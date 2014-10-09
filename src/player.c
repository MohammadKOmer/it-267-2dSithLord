#include <stdlib.h>
#include <string.h>
#include "player.h"


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
}

/*
* Think Function handles giving the entity new orders and changing states
*/

void PlayerThink(Entity *self)
{
	float t;
	char text[40];
	Uint8 *keys = SDL_GetKeyState(NULL);
	
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
				__Nai.liftingTarget->v.y+=__Nai.liftSpeed;

			} else if(keys[KeyButtons[PI_MovUp]])
			{     
				__Nai.liftingTarget->v.y=-__Nai.liftSpeed;

			}else{  /* if released*/
				__Nai.liftingTarget->v.x=0;
			}
			
		
		}

	}
}

/*updates the entity's postition and handles animation*/

void UpdatePlayer(Entity *self)
{
	int Goframe = 0;
	UpdateEntityPosition(self);

	if(self->grounded == 0)
	{
		if(self->v.y < 20);
		self->v.y += 2;     /*gravity at work*/
	}

	if(self->state == ST_DIE)
	{
		self->fcount = 10;
		self->state = ST_DEAD;
		return;
	}
	else if(self->state == ST_DEAD)
	{
		self->fcount--;
		if(self->fcount <= 0)
		{
			FreeEntity(self);
			exit(0);
		}

		return;
	}
	else
	{
		self->state = ST_IDLE;
	}
	if(self->fcount <= 0)
	{
		Goframe = 1;
		self->fcount = self->frate;
	}
	else
	{
		self->fcount--;
	}
	if(!Goframe)return;
	else GetFace(self);/*check out movement vector to see what direction we are facing.*/
	/*only frame animations should take place next*/
	switch(self->face)
	{
	case F_NULL:
		self->legframe = 0;
		break;
	case F_North:
		self->legframe++;
		if((self->legframe > 9)||(self->legframe < 7))self->legframe = 7;
		break;
	case F_South:
		self->legframe++;
		if((self->legframe > 3)||(self->legframe < 1))self->legframe = 1;
		break;
	case F_SW:
	case F_NW:
	case F_West:
		self->legframe++;
		if((self->legframe > 6)||(self->legframe < 4))self->legframe = 4;
		break;
	case F_SE:
	case F_NE:
	case F_East:
		self->legframe++;
		if((self->legframe > 12)||(self->legframe < 10))self->legframe = 10;
		break;


	}
	if(self->state == ST_ATTACK1)
	{
		self->frame++;
		switch(self->aimdir)
		{
		case F_South:
			if(self->frame > 2)self->frame = 0;
			break;
		case F_SW:
			if((self->frame > 5)||(self->frame < 3))self->frame = 3;
			break;
		case F_West:
			if((self->frame > 8)||(self->frame < 6))self->frame = 6;
			break;
		case F_NW:
			if((self->frame > 11)||(self->frame < 9))self->frame = 9;
			break;
		case F_North:
			if((self->frame > 14)||(self->frame < 12))self->frame = 12;
			break;
		case F_NE:
			if((self->frame > 17)||(self->frame < 15))self->frame = 15;
			break;
		case F_East:
			if((self->frame > 20)||(self->frame < 18))self->frame = 18;
			break;
		case F_SE:
			if((self->frame > 23)||(self->frame < 21))self->frame = 21;
			break;
		default:
			self->frame = 0;
		}
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
	sprintf(text,"Score: %i",self->Score);
	DrawText(text,screen,5,5,IndexColor(self->Color),F_Medium);
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






