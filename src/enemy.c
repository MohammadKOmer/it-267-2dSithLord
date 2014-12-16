#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "enemy.h"
#include "space.h"
#include "projectiles.h"
#include "graphics.h"
extern Entity *ThePlayer;
void SpawnEnemy(int x, int y, int type){
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)
	{
		printf( "Unable to generate enemy entity; %s",SDL_GetError());
		exit(0);
	}

	newent->sprite = LoadSprite("images/TestSprite.png",256,256);


	newent->player=ThePlayer;
	newent->size.x = newent->sprite->w;
	newent->size.y = newent->sprite->h;
	if(type == ET_TROOPER){
		newent->update = UpdateTrooper;
		newent->think = TrooperThink;
	}else{
		newent->update = UpdateSith;
		newent->think = SithThink;
	}
	newent->UpdateRate = 35;
	newent->ThinkRate = 35;

	newent->takedamage = 1;

	newent->Unit_Type = EC_AI;

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

	newent->Boundingbox.x = 0;
	newent->Boundingbox.y = 0;
	newent->Boundingbox.w = 256;
	newent->Boundingbox.h = 256;  
	newent->origin.x = 128;
	newent->origin.y = 128;
	
	insert(newent,__space);
}

void TrooperThink(Entity *self){
	
	int playerloc;
	if(self->state==ST_LIFTED){
		return;
	}
	playerloc=self->player->s.x;
	self->timeSinceLastAttack+=1;
	if(self->s.x<playerloc){
		self->face=F_RIGHT;
		self->v.x=4;
	}else{
		self->face=F_LEFT;
		self->v.x=-4;
	}
	if(self->timeSinceLastAttack>=100){
		if(self->face==F_LEFT){
			SpawnBullet(self,self->s.x,self->s.y+128,3.1415,20,10,0,0,0,0);
		}else{
			SpawnBullet(self,self->s.x+270,self->s.y+128,0,20,30,0,0,0,0);
		}
		self->timeSinceLastAttack=0;
	}

}

void UpdateTrooper(Entity *self){
	self->timeSinceLastAttack++;
	if(self->state==ST_LIFTED){
		return;
	}
	UpdateEntityPosition(self);
	if(self->grounded == 0)
	{
		self->v.y += 2;     /*gravity at work*/
	}
	if(self->health<=0){
		FreeEntity(self);
	}
}


void SithThink(Entity *self){

	int playerlocx;
	int dist;
		self->player=ThePlayer;
	dist=(self->player->s.x-self->s.x)*(self->player->s.x-self->s.x)+(self->player->s.y-self->s.y)*(self->player->s.y-self->s.y);
	playerlocx=self->player->s.x;
	self->timeSinceLastAttack+=1;
	if(self->s.x<playerlocx){
		self->face=F_RIGHT;
		self->v.x=6;
	}else{
		self->face=F_LEFT;
		self->v.x=-6;
	}
	if(self->player->s.y<self->s.y&&self->grounded == 0){
		self->v.y-=16;
	}
	if(self->timeSinceLastAttack>=80&&dist<(self->size.x*self->size.x)){
		if(self->face==F_LEFT){
			SpawnSaberhit(self,self->s.x-(.5*self->size.x),self->s.y,0,0,50,0,0,Blue_,0);
		}else{
			SpawnSaberhit(self,self->s.x+(.5*self->size.x),self->s.y,0,0,50,0,0,Blue_,0);
		}
		self->timeSinceLastAttack=0;
	}
}

void UpdateSith(Entity *self){
	UpdateEntityPosition(self);
	if(self->grounded == 0)
	{
		self->v.y += 2;     /*gravity at work*/
	}
	printf("%i\n",self->health);
	if(self->health<=0){
		FreeEntity(self);
	}
}
