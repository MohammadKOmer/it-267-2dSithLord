#include <stdlib.h>
#include <string.h>
#include "player.h"


extern SDL_Surface *screen;
extern SDL_Joystick *joy;

enum PlayerInputs {PI_MovDown,PI_MovDownLeft,PI_MovLeft,PI_MovUpLeft,PI_MovUp,PI_MovUpRight,PI_MovRight,PI_MovDownRight,
                   PI_Lightsaber,PI_Push,PI_Lift,PI_Jump,PI_NULL};


Uint32 KeyButtons[PI_NULL];

int PlayerCommands[3]; 
Entity *ThePlayer = NULL;
SDL_Surface *titlebar = NULL;
int attackleft = SDLK_LEFT;
void UpdatePlayer(Entity *self);
void PlayerThink(Entity *self);
void FinalOutput();
void UpdateInput();

PlayerStats ThisPlayer_Stats;


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
  newent->sprite = LoadSwappedSprite("images/core1.png",48,48,LightGreen,White,Grey);
  


  newent->size.x = newent->sprite->w;
  newent->size.y = newent->sprite->h;
  newent->update = UpdatePlayer;
  newent->think = PlayerThink;
  newent->UpdateRate = 35;
  newent->ThinkRate = 35;
  newent->legstate = 1;
  newent->takedamage = 1;

  newent->Unit_Type = ET_Player;

  newent->healthmax = 100;
  newent->health = 100;
  newent->frame = 0;
  newent->Color = LightBlue;

  newent->frate = 1;
  newent->legframe = 0;
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
    /*testing (cheating)*/
   if(PlayerCommands[0] == PI_MovUpLeft)
    {     
      t = self->accel * 1.41; 
      self->grounded = 0;
      self->v.y -= t;
      self->v.x -= t;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      if(self->movespeed > self->maxspeed)
      {
        self->v.y = self->maxspeed * 1.41 * -1;
        self->v.x = self->v.y;
        self->movespeed = self->maxspeed;
      }
      
    }
    else if(PlayerCommands[0] == PI_MovUpRight)
    {     
      t = self->accel * 1.41; 
      self->v.y -= t;
      self->v.x += t;
      self->grounded = 0;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      if(self->movespeed > self->maxspeed)
      {
        self->v.x = self->maxspeed * 1.41;
        self->v.y = self->v.x * -1;
        self->movespeed = self->maxspeed;
      }
      
    }
    else if(PlayerCommands[0] == PI_MovDownRight)
    {     
      t = self->accel * 1.41; 
      self->v.y += t;
      self->v.x += t;
      self->grounded = 0;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      if(self->movespeed > self->maxspeed)
      {
        self->v.y = self->maxspeed * 1.41;
        self->v.x = self->v.y;
        self->movespeed = self->maxspeed;
      }
    
    }
    else if(PlayerCommands[0] == PI_MovDownLeft)
    {     
      t = self->accel * 1.41; 
      self->v.y += t;
      self->v.x -= t;
      self->grounded = 0;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      if(self->movespeed > self->maxspeed)
      {
        self->v.y = self->maxspeed * 1.41;
        self->v.x = self->v.y * -1;
        self->movespeed = self->maxspeed;
      }
     
    }
    else if(PlayerCommands[0] == PI_MovUp)
    {      
      self->v.y -= self->accel;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      self->grounded = 0;
      if(self->movespeed > self->maxspeed)
      {
        self->v.x = 0;
        self->v.y = self->maxspeed * -1;
        self->movespeed = self->maxspeed;
      }
      
    }
    else if(PlayerCommands[0] == PI_MovDown)
    {      
      self->v.y += self->accel;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      self->grounded = 0;
      if(self->movespeed > self->maxspeed)
      {
        self->v.x = 0;
        self->v.y = self->maxspeed;
        self->movespeed = self->maxspeed;
      }
    
    }
    else if(PlayerCommands[0] == PI_MovLeft)
    {      
      self->v.x -= self->accel;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      self->grounded = 0;
      if(self->movespeed > self->maxspeed)
      {
        self->v.y = 0;
        self->v.x = self->maxspeed * -1;
        self->movespeed = self->maxspeed;
      }
     
    }
    else if(PlayerCommands[0] == PI_MovRight)
    {      
      self->v.x += self->accel;
      self->movespeed = VectorLength(self->v.x,self->v.y);
      self->grounded = 0;
      if(self->movespeed > self->maxspeed)
      {
        self->v.y = 0;
        self->v.x = self->maxspeed;
        self->movespeed = self->maxspeed;
      }
     
    }

    switch(self->state)
    {
      case ST_IDLE: 
        if(PlayerCommands[1] == PI_FireUltimate)
        {
          self->frame = 12;
          self->state = ST_ATTACK1;
          self->aimdir = F_North;
          self->Ls.x = -5;
          self->Ls.y = -26;
          FireUltimateWeapon(self);
        }
        else if(keys[SDLK_r])
        {
          SpawnOrbitMine(self,self->s.x + self->origin.x,self->s.y - 5,20,10,50,0,self->Color,self->Unit_Type);
          self->frame = 12;
          self->state = ST_ATTACK1;
          self->aimdir = F_North;
          self->Ls.x = -5;
          self->Ls.y = -26;
          self->Cooldown = 3;                    
        }
        else if(PlayerCommands[1] == PI_FireUpLeft)
        {
          self->frame = 9;
          self->state = ST_ATTACK1;
          self->aimdir = F_NW;
          self->Ls.x = -6;
          self->Ls.y = -26;
          FireWeapon(self,F_NW);
        }
        else if(PlayerCommands[1] == PI_FireUp)
        {
          self->frame = 12;
          self->state = ST_ATTACK1;
          self->aimdir = F_North;
          self->Ls.x = -5;
          self->Ls.y = -26;
          FireWeapon(self,F_North);
        }    
        else if(PlayerCommands[1] == PI_FireUpRight)
        {
          self->frame = 15;
          self->state = ST_ATTACK1;
          self->aimdir = F_NE;
          self->Ls.x = 0;
          self->Ls.y = -26;
          FireWeapon(self,F_NE);
        }    
        else if(PlayerCommands[1] == PI_FireLeft)
        {
          self->frame = 6;
          self->state = ST_ATTACK1;
          self->aimdir = F_West;
          self->Ls.x = -8;
          self->Ls.y = -28;
          FireWeapon(self,F_West);
        }    
        else if(PlayerCommands[1] == PI_FireRight)
        {
          self->frame = 18;
          self->state = ST_ATTACK1;
          self->aimdir = F_East;
          self->Ls.x = 4;
          self->Ls.y = -28;
          FireWeapon(self,F_East);
        }    
        else if(PlayerCommands[1] == PI_FireDownLeft)
        {
          self->frame = 3;
          self->state = ST_ATTACK1;
          self->aimdir = F_SW;
          self->Ls.x = -5;
          self->Ls.y = -14;
          FireWeapon(self,F_SW);
        }    
        else if(PlayerCommands[1] == PI_FireDown)
        {
          self->frame = 0;
          self->state = ST_ATTACK1;
          self->aimdir = F_South;
          self->Ls.x = -6;
          self->Ls.y = -14;
          FireWeapon(self,F_South);
        }    
        else if(PlayerCommands[1] == PI_FireDownRight)
        {
          self->frame = 21;
          self->state = ST_ATTACK1;
          self->aimdir = F_SE;
          self->Ls.x = 4;
          self->Ls.y = -14;
          FireWeapon(self,F_SE);
        }    
        
        break;
      case ST_ATTACK1:
      break;
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






