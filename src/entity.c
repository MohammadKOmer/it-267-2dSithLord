#include <chipmunk/chipmunk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "entity.h"
#include "projectiles.h"
#include "graphics.h"
#include "player.h"
#include "enemy.h"
#define   RPixel_W    256
#define   RPixel_H    256



extern SDL_Surface *screen;
extern SDL_Surface *clipmask;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Uint32 NOW;
extern Entity *ThePlayer;
extern PlayerSpecificVars __Nai;

Entity EntityList[MAXENTITIES];
int NumEnts = 0;
int EnemyPresent;
void Insert(Entity ent){
	
}
void DrawBBoxEntities()
{
  int i;
  int count = 0;
  for(i = 0;i < MAXENTITIES;i++)
  {
    if(EntityList[i].used == 1)
    {
      if(((EntityList[i].s.x + EntityList[i].size.x) >= Camera.x)&&(EntityList[i].s.x <= (Camera.x + Camera.w))&&((EntityList[i].s.y + EntityList[i].size.y) >= Camera.y)&&(EntityList[i].s.y <= (Camera.y + Camera.h)))
      {
        DrawRect(EntityList[i].Boundingbox.x + (int)EntityList[i].s.x - Camera.x,EntityList[i].Boundingbox.y + (int)EntityList[i].s.y - Camera.y, EntityList[i].Boundingbox.w, EntityList[i].Boundingbox.h, IndexColor(EntityList[i].Color), screen);
      }
      count++;
    }
  }
}

void InitEntityList()
{
	int i;
	NumEnts = 0;
	for(i = 0;i < MAXENTITIES; i++)
	{
		EntityList[i].sprite = NULL;
		EntityList[i].owner = NULL;
		EntityList[i].think = NULL;
		EntityList[i].target = NULL;
		EntityList[i].update = NULL;
		EntityList[i].shown = 0;
		EntityList[i].used = 0;
	}

	NewEntity();
}


void DrawEntities()
{
	int i;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(EntityList[i].used == 1)
		{
			if(((EntityList[i].s.x + EntityList[i].size.x) >= Camera.x - 200)&&(EntityList[i].s.x <= (Camera.x + Camera.w + 200))&&((EntityList[i].s.y + EntityList[i].size.y) >= Camera.y - 200)&&(EntityList[i].s.y <= (Camera.y + Camera.h) + 200))
			{
				DrawEntity(&EntityList[i]);
			}
		}
	}
}


void ThinkEntities()
{
	int i;
	int checked = 0;
	EnemyPresent=0;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(EntityList[i].used)
		{
			checked++;
			if(EntityList[i].NextThink < NOW)
			{
				if(EntityList[i].think != NULL)
				{
					EntityList[i].think(&EntityList[i]);
					EntityList[i].NextThink = NOW + EntityList[i].ThinkRate;
				}
					
			}
			if(EntityList[i].Unit_Type==EC_AI){
				EnemyPresent++;
			
			}
		}
	}
}
void UpdateEntities()
{
  int i;
  int checked = 0;
  for(i = 0;i < MAXENTITIES;i++)
  {
      if(EntityList[i].used)
      {
		     checked++;
        if(EntityList[i].NextUpdate < NOW)
        {
          if(EntityList[i].update != NULL)
          {
            EntityList[i].update(&EntityList[i]);
            EntityList[i].NextUpdate = NOW + EntityList[i].UpdateRate;
          }

        }
		
      }
  }
  
}
void DrawEntity(Entity *ent)
{

    if(ent->sprite != NULL){
      DrawSprite(ent->sprite,screen,ent->s.x - Camera.x,ent->s.y - Camera.y ,ent->frame,ent->frameR);
	}
  
}
Entity *NewEntity()
{
	int i;
	if(NumEnts + 1 >= MAXENTITIES)
	{
		return NULL;
	}
	NumEnts++;
	for(i = 0;i < MAXENTITIES;i++)
	{
		if(!EntityList[i].used)break;
	}
	EntityList[i].used = 1;
	return &EntityList[i];
}


void FreeEntity(Entity *ent)
{
	ent->used = 0;
	NumEnts--;
	if(ent->sprite != NULL)FreeSprite(ent->sprite);
	ent->sprite = NULL;
	ent->owner = NULL;
	ent->think = NULL;
	ent->target = NULL;
	ent->update = NULL;
	if(ent->Shape){
		cpSpaceRemoveBody(__space, ent->Shape->body);
		cpBodyFree( ent->Shape->body);
  		cpSpaceRemoveShape(__space,  ent->Shape);
		cpShapeFree( ent->Shape);
	}
}


void ClearEntities()
{
	int i;
	for(i = 0;i < MAXENTITIES;i++)
	{
		FreeEntity(&EntityList[i]);
	}
}



void DamageTarget(Entity *attacker,Entity *inflictor,Entity *defender,int damage,int dtype,float kick,float kickx,float kicky)
{     
	if(defender == attacker){
		
		return;
	}
	if(!defender->takedamage)return;/*lets not hurt anything that can't be hurt*/
	if(defender == ThePlayer){
		if(__Nai.force>damage){
			__Nai.force-=damage;
			return;
		}

	}
	defender->health -= damage;
	if(kick > 1)kick = 1;
	defender->v.x += (kickx * kick);
	defender->v.y += (kicky * kick);
	if(defender->EntClass == EC_AI)defender->state = ST_ALERT;
	if(defender->health <= 0)
	{
		defender->state = ST_DIE;
		defender->takedamage = 0;
	}
}

int GetFace(Entity *self)
{

	if(self->v.x < -2)
	{
		self->face = F_LEFT;
		return F_LEFT;
	}
	else if(self->v.x > 2)
	{
		self->face = F_RIGHT;
		return F_RIGHT;
	}
	return self->face;
}


void UpdateEntityPosition(Entity *self)
{
	
}



int OnScreen(Entity *self)
{
  if(((self->s.x + self->size.x) >= Camera.x)&&(self->s.x <= (Camera.x + Camera.w))&&((self->s.y + self->size.y) >= Camera.y)&&(self->s.y <= (Camera.y + Camera.h)))
    return 1;
  return 0;
}



void SpawnFloor(int x,int y)
{
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)
	{
		printf( "Unable to generate player entity; %s",SDL_GetError());
		exit(0);
	}
	strcpy(newent->EntName,"testsprite\0");
	newent->sprite = LoadSprite("images/TestSprite.png",256,2560);
	newent->size.x = 256;
	newent->size.y = 256;

	newent->takedamage = 0;
	newent->Unit_Type = ET_WorldEnt;

	newent->healthmax = 100;
	newent->health = 100;
	newent->frame = 1;


	newent->frate = 0;
	newent->state = ST_IDLE;
	newent->EntClass=EC_STATIC;


	newent->s.x = x;
	newent->s.y = y;
	newent->v.x = 0;
	newent->v.y = 0;
	newent->maxspeed = 0;
	newent->movespeed = 0;
	newent->accel = 4;

	newent->Boundingbox.x = 0;
	newent->Boundingbox.y = 0;
	newent->Boundingbox.w = 256;
	newent->Boundingbox.h = 256;  
	newent->origin.x = 128;
	newent->origin.y = 128;
	newent->Body = cpSpaceAddBody(__space,cpBodyNewStatic());
	newent->Body->data=newent;
	cpBodySetPos(newent->Body,newent->s);
	newent->Shape =cpBoxShapeNew(newent->Body,256,256);

	
}


void SpawnSquare(int x,int y, int frame)
{
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)
	{
		printf( "Unable to generate player entity; %s",SDL_GetError());
		exit(0);
	}
	strcpy(newent->EntName,"testsprite\0");
	newent->sprite = LoadSprite("images/TestSprite.png",256,2560);
	newent->size.x = 256;
	newent->size.y = 256;

	newent->takedamage = 0;
	newent->Unit_Type = ET_WorldEnt;

	newent->healthmax = 100;
	newent->health = 100;
	newent->frame = frame;


	newent->frate = 0;
	newent->state = ST_IDLE;
	newent->EntClass=EC_STATIC;


	newent->s.x = x;
	newent->s.y = y;
	newent->v.x = 0;
	newent->v.y = 0;
	newent->maxspeed = 0;
	newent->movespeed = 0;
	newent->accel = 4;

	newent->Boundingbox.x = 0;
	newent->Boundingbox.y = 0;
	newent->Boundingbox.w = 256;
	newent->Boundingbox.h = 256;  
	newent->origin.x = 128;
	newent->origin.y = 128;

}

