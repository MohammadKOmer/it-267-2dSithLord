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


Entity *SpawnBullet(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int color,int UType)
{
	Entity *newent = NULL;
	newent = SpawnProjectile(sx,sy,angle,speed,0,damage,DT_Energy,kick);
	if(newent == NULL)return NULL;

	newent->sprite = LoadSwappedSprite("images/tinybullet.png",5,5,color,0,0);
	newent->origin.x = 3;
	newent->origin.y = 3;
	newent->Boundingbox.x = 1;
	newent->Boundingbox.y = 1;
	newent->Boundingbox.w = 3;
	newent->Boundingbox.h = 3;

	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
	newent->frame = 0;
	newent->owner = owner;
	newent->update = UpdateBullet;
	newent->UpdateRate = 30;
	newent->Color = color;
	newent->Unit_Type = UType;
	insert(newent,__quadtreeList);
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
  if(UpdateEntityPosition(self))
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


void FadingLaser(Entity *self)
{
  UpdateEntityPosition(self,0);
  self->fcount--;
  if(self->fcount <= 0)
      FreeEntity(self);
}
  
Entity *GetTouchingEnt(Entity *self, Entity* (*out)[])
{
	int i;

	PotentialColidables(self, __quadtreeList,out, 0);
	while((*out)[i]){
		if(Collide((*out)[i]->Boundingbox,self->Boundingbox)){
			return ((*out)[i]);
		}
	}
}


