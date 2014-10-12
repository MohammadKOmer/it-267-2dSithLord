#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "projectiles.h"

void UpdateBullet(Entity *self);
void UpdateGrenade(Entity *self);
void UpdateRocket(Entity *self);
void UpdateExplode(Entity *self);
void UpdateMissile(Entity *self);
void MissileThink(Entity *self);
void UpdateLaser(Entity *self);
void UpdateFlame(Entity *self);
void UpdateSmoke(Entity *self);
void UpdateCBomb(Entity *self);
void FadingLaser(Entity *self);
void UpdateMine(Entity *self);
void UpdateBlade(Entity *self);

extern Uint32 NOW;


/*this will be a generic spawn function that will set some basic info to save code*/
Entity *SpawnProjectile(int sx,int sy,float angle,float speed,float accel,int damage,int dtype,float kick)
{
  float cosine,sine;
  Entity *newent = NULL;
  newent = NewEntity();
  if(newent == NULL)return NULL;
  cosine = cos(angle);
  sine = sin(angle);
  newent->dtype = dtype;
  newent->kick = kick;
  newent->damage = damage;
  newent->gravityent = 0;
  newent->shown = 1;
  newent->legstate = -1;    /*needed if we don't have separate legs*/
  newent->s.x = sx;
  newent->s.y = sy;
  newent->v.x = speed * cosine;
  newent->v.y = speed * sine;
  if(accel != 0)
  {
    newent->v.x = speed * cosine;
    newent->v.y = speed * sine;
    newent->accel = accel;
  } 
  newent->movespeed = (int)speed;
  /*binding the entity to the engine should happen here as well*/
  return newent;
}


Entity *SpawnBullet(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType)
{
  Entity *newent = NULL;
  newent = SpawnProjectile(sx,sy,angle,speed,0,damage,DT_Physical,kick);
  if(newent == NULL)return NULL;
  newent->maxtraillen = 2;
  switch(size)
  {
    case B_Tiny:
      newent->sprite = LoadSwappedSprite("images/tinybullet.png",5,5,color,0,0);
      newent->origin.x = 3;
      newent->origin.y = 3;
      newent->Boundingbox.x = 1;
      newent->Boundingbox.y = 1;
      newent->Boundingbox.w = 3;
      newent->Boundingbox.h = 3;
      newent->sound[SF_ALERT] = LoadSound("sounds/ric1.wav",SDL_MIX_MAXVOLUME>>4);
      break;
    case B_Small:
      newent->sprite = LoadSwappedSprite("images/smallbullet.png",8,8,color,0,0);
      newent->origin.x = 4;
      newent->origin.y = 4;
      newent->Boundingbox.x = 1;
      newent->Boundingbox.y = 1;
      newent->Boundingbox.w = 6;
      newent->Boundingbox.h = 6;
      newent->sound[SF_ALERT] = LoadSound("sounds/ric2.wav",SDL_MIX_MAXVOLUME>>4);
      break;
    case B_Medium:
      newent->sprite = LoadSwappedSprite("images/bullet.png",12,12,color,0,0);
      newent->origin.x = 6;
      newent->origin.y = 6;
      newent->Boundingbox.x = 2;
      newent->Boundingbox.y = 2;
      newent->Boundingbox.w = 8;
      newent->Boundingbox.h = 8;
      newent->sound[SF_ALERT] = LoadSound("sounds/ric3.wav",SDL_MIX_MAXVOLUME>>3);
      break;
    case B_Large:
      newent->maxtraillen = 4;
      newent->sprite = LoadSwappedSprite("images/Largebullet.png",16,16,color,0,0);
      newent->origin.x = 8;
      newent->origin.y = 8;
      newent->Boundingbox.x = 3;
      newent->Boundingbox.y = 3;
      newent->Boundingbox.w = 10;
      newent->Boundingbox.h = 10;
      newent->sound[SF_ALERT] = LoadSound("sounds/xfire.wav",SDL_MIX_MAXVOLUME>>2);
      break;
    case B_Huge:
      newent->sprite = LoadSwappedSprite("images/Hugebullet.png",24,24,color,0,0);
      newent->origin.x = 12;
      newent->origin.y = 12;
      newent->Boundingbox.x = 4;
      newent->Boundingbox.y = 4;
      newent->Boundingbox.w = 16;
      newent->Boundingbox.h = 16;
      newent->sound[SF_ALERT] = LoadSound("sounds/deadexplode.wav",SDL_MIX_MAXVOLUME>>1);
      newent->maxtraillen = 5;
      break;
    case B_Atomic:
      newent->maxtraillen = 6;
      newent->sprite = LoadSwappedSprite("images/Atomicbullet.png",32,32,color,0,0);
      newent->origin.x = 16;
      newent->origin.y = 16;
      newent->Boundingbox.x = 5;
      newent->Boundingbox.y = 5;
      newent->Boundingbox.w = 22;
      newent->Boundingbox.h = 22;
      newent->sound[SF_ALERT] = LoadSound("sounds/machinexplode.wav",SDL_MIX_MAXVOLUME);
      break;
    default:
      newent->sound[SF_ALERT] = LoadSound("sounds/ric1.wav",SDL_MIX_MAXVOLUME>>4);
  }
  SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
  newent->frame = 0;
  newent->owner = owner;
  newent->update = UpdateBullet;
  newent->UpdateRate = 30;
  newent->Color = color;
  newent->Unit_Type = UType;
  newent->lifespan = 60;
  newent->thick = 1;
  newent->trailhead = 0;
  newent->traillen = 0;
  newent->trail[0].x = sx;
  newent->trail[0].y = sy;
  newent->m.x = (sx + newent->origin.x) >> 6;
  newent->m.y = (sy + newent->origin.y) >> 6;
  AddEntToRegion(newent,newent->m.x,newent->m.y);
  return newent;
}


void UpdateBullet(Entity *self)
{
  Entity *target = NULL;
  self->lifespan--;
  if(self->lifespan <= 0)
  {
    FreeEntity(self);
    return;  
  }
  if(UpdateEntityPosition(self,0))
  {
    if(self->sound[SF_ALERT]->sound != NULL)Mix_PlayChannel(Mix_GroupAvailable(2),self->sound[SF_ALERT]->sound,0);
    FreeEntity(self);
    return;  
  }
  target = GetTouchingEnt(self);
  if(target != NULL)
  {
    DamageTarget(self->owner,self,target,self->damage,self->dtype,self->kick,self->v.x,self->v.y);
    FreeEntity(self);    
  }
}

/*************************************************************
              
                       Basic Laser Projectile

 *************************************************************/

Entity *SpawnLaser(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size, int color,int UType,int gravity)
{
  Entity *newent = NULL;
  newent = SpawnProjectile(sx,sy,angle,speed,0,damage,DT_Energy,kick);
  if(newent == NULL)return NULL;
  strcpy(newent->EntName,"Laser\0");
  newent->sound[SF_ALERT] = LoadSound("sounds/bluehit.wav",SDL_MIX_MAXVOLUME>>3);
  newent->origin.x = 2;
  newent->origin.y = 2;
  newent->owner = owner;
  newent->frame = 0;
  newent->gravityent = gravity;
  newent->update = UpdateLaser;
  newent->UpdateRate = 30;
  newent->Color = color;
  newent->Unit_Type = UType;
  newent->lifespan = 90;
  newent->trailhead = 0;
  newent->traillen = 0;
  newent->trail[0].x = sx;
  newent->trail[0].y = sy;
  newent->maxtraillen = MAXTRAIL - 1;
  newent->Boundingbox.x = 1;
  newent->Boundingbox.y = 1;
  newent->thick = size;
  newent->Boundingbox.w = 2;
  newent->Boundingbox.h = 2;
  newent->m.x = (sx + newent->origin.x) >> 6;
  newent->m.y = (sy + newent->origin.y) >> 6;
  AddEntToRegion(newent,newent->m.x,newent->m.y);
  return newent;
}


void UpdateLaser(Entity *self)
{
  Entity *target = NULL;
  if(UpdateEntityPosition(self,1))Mix_PlayChannel(Mix_GroupAvailable(2),self->sound[SF_ALERT]->sound,0);
  
  if(self->gravityent)self->v.y += 1.5;
  self->lifespan--;
  if(self->lifespan <= 0)
  {
    self->update = FadingLaser;
    self->v.x = 0;
    self->v.y = 0;
    self->a.x = 0;
    self->a.y = 0;
    self->fcount = self->maxtraillen;
    return;
  }
  target = GetTouchingEnt(self);
  if(target != NULL)
  {
    DamageTarget(self->owner,self,target,self->damage,self->dtype,self->kick,self->v.x,self->v.y);
    self->update = FadingLaser;
    self->v.x = 0;
    self->v.y = 0;
    self->a.x = 0;
    self->a.y = 0;
    self->fcount = self->maxtraillen;
  }
}

void FadingLaser(Entity *self)
{
  UpdateEntityPosition(self,0);
  self->fcount--;
  if(self->fcount <= 0)
      FreeEntity(self);
}
  
Entity *GetTouchingEnt(Entity *self)
{
  Entity *target = NULL;
  SDL_Rect bbox;
  bbox.w = self->Boundingbox.w;
  bbox.h = self->Boundingbox.h;
  bbox.x = self->Boundingbox.x + (int)self->s.x;
  bbox.y = self->Boundingbox.y + (int)self->s.y;
  target = GetEntByBox(bbox,self->m.x,self->m.y,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x + 1,self->m.y - 1,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x + 1,self->m.y + 1,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x + 1,self->m.y,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x - 1,self->m.y - 1,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x - 1,self->m.y + 1,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x - 1,self->m.y,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x,self->m.y - 1,self,self->Unit_Type);
  if(target != NULL)return target;
  target = GetEntByBox(bbox,self->m.x,self->m.y + 1,self,self->Unit_Type);
  if(target != NULL)return target;
  return NULL;
}


