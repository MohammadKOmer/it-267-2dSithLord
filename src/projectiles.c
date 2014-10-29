#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "projectiles.h"



extern Uint32 NOW;


Entity**ColideibleList;

/*this will be a generic spawn function that will set some basic info to save code*/
void initProjectiles(){
	ColideibleList=((Entity**) malloc(sizeof(Entity) * 32));

}

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
	newent = SpawnProjectile(sx,sy,angle,speed,0,damage,DT_Energy,kick);
	if(newent == NULL)return NULL;

	newent->sprite = LoadSwappedSprite("images/laser.png",5,5,color,0,0);
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
	int i;
	int j;

	memset(ColideibleList,0,sizeof(Entity) * 32); 

	UpdateEntityPosition(self);
	i=0;
	j=0;
	while((ColideibleList)[i])
	{
		if(  ((ColideibleList)[i]->EntClass==EC_STATIC)  ||  ((ColideibleList)[i]==self->owner)  ||  ((ColideibleList)[i]==self)   ){
			i++;
			
		}else{
			j++;
			DamageTarget(self->owner,self,(ColideibleList)[i],self->damage,self->dtype,self->kick,self->v.x,self->v.y);
			i++;
		}
	}
	if(j!=0){
	FreeEntity(self); 
	}
}


Entity *SpawnForcePush(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType)
{
	Entity *newent = NULL;
	newent = SpawnProjectile(sx,sy,angle,speed,0,damage,DT_Energy,kick);
	if(newent == NULL)return NULL;

	newent->sprite = LoadSwappedSprite("images/forcewave.png",128,256,color,0,0);
	newent->origin.x = 3;
	newent->origin.y = 3;
	newent->Boundingbox.x = 1;
	newent->Boundingbox.y = 1;
	newent->Boundingbox.w = 16;
	newent->Boundingbox.h = 255;
	newent->lifetime=90;
	newent->EntClass=EC_BULLET;
	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
	newent->frame = 0;
	newent->owner = owner;
	newent->update = UpdateForcePush;
	newent->UpdateRate = 30;
	newent->Color = color;
	newent->Unit_Type = UType;
	insert(newent,__quadtreeList);
	return newent;
}


void UpdateForcePush(Entity *self)
{
	int i;
	memset(ColideibleList,0,sizeof(Entity) * 32); 

	PotentialColidables(self, __quadtreeList,ColideibleList, 0);
	UpdateEntityPosition(self);
	i=0;
	while((ColideibleList)[i])
	{
		if( ((ColideibleList)[i]->EntClass==EC_BULLET)  || ((ColideibleList)[i]->EntClass==EC_STATIC)  ||  ((ColideibleList)[i]==self->owner)  ||  ((ColideibleList)[i]==self)   ){
			i++;
			
		}else{
			DamageTarget(self->owner,self,(ColideibleList)[i],self->damage,self->dtype,self->kick,self->v.x,self->v.y);
			(ColideibleList)[i]->pushed.x+=self->v.x;
			(ColideibleList)[i]->pushed.y+=self->v.y;
			i++;
		}
	
	}
	self->lifetime--;
	if(self->lifetime<=0){
		FreeEntity(self); 
	}
	

}


Entity *SpawnSaberhit(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType)
{
	Entity *newent = NULL;
	newent = SpawnProjectile(sx,sy,angle,0,0,damage,DT_Energy,kick);
	if(newent == NULL)return NULL;

	newent->sprite = LoadSwappedSprite("images/TestSprite.png",128,128,color,0,0);
	newent->origin.x = 3;
	newent->origin.y = 3;
	newent->Boundingbox.x = 1;
	newent->Boundingbox.y = 1;
	newent->Boundingbox.w = 128;
	newent->Boundingbox.h = 128;
	newent->lifetime=2;
	newent->EntClass=EC_STATIC;
	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
	newent->frame = 0;
	newent->owner = owner;
	newent->update = UpdateSaberhit;
	newent->UpdateRate = 30;
	newent->Color = color;
	newent->Unit_Type = UType;
	insert(newent,__quadtreeList);
	return newent;
}


void UpdateSaberhit(Entity *self)
{
	int i;
	UpdateEntityPosition(self);
	memset(ColideibleList,0,sizeof(Entity) * 32); 
	PotentialColidables(self, __quadtreeList,ColideibleList, 0);
	
	i=0;
	while((ColideibleList)[i])
	{
		if(  ((ColideibleList)[i]->EntClass==EC_STATIC)  ||  ((ColideibleList)[i]==self->owner)  ||  ((ColideibleList)[i]==self)   ){
			i++;
		}else{
			DamageTarget(self->owner,self,(ColideibleList)[i],self->damage,self->dtype,self->kick,self->v.x,self->v.y);
			i++;
		}
	}
	self->lifetime--;
	if(self->lifetime<=0){
		FreeEntity(self); 
	}
}




