#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "entity.h"
#include "projectiles.h"
#include "graphics.h"

#define   RPixel_W    256
#define   RPixel_H    256



extern SDL_Surface *screen;
extern SDL_Surface *clipmask;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Uint32 NOW;


Entity EntityList[MAXENTITIES];
int NumEnts = 0;
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

	if(!defender->takedamage)return;/*lets not hurt anything that can't be hurt*/

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
	int i;
	
	double vx,vy = 0;

	vx = self->v.x;
	vy = self->v.y;
	memset(ColideibleList,0,sizeof(Entity) * 32); 
	PotentialColidables(self, __quadtreeList,ColideibleList, 0);
	i=0;
	while(ColideibleList[i])
	{
		if(ColideibleList[i]->EntClass==EC_STATIC){
			if(ColideibleList[i]->s.y>self->s.y){
				self->v.y=0;
				self->grounded = 1;
			}else{
				if(ColideibleList[i]->s.x>self->s.x&&vx>0){
					self->v.x=0;

				}
				else if(ColideibleList[i]->s.x<self->s.x&&vx<0){
					self->v.x=0;
				}
			}
			
		}
		i++;
	}
	if(self->s.y>9000){  /*lets add a floor to any version of the game world.*/
				self->v.y=0;
				self->grounded = 1;
	}
	printf("self->v.x: %d  \n", self->v.x+self->pushed.x);
	self->s.x += self->v.x+self->pushed.x;
	self->s.y += self->v.y+self->pushed.y;
}



int OnScreen(Entity *self)
{
  if(((self->s.x + self->size.x) >= Camera.x)&&(self->s.x <= (Camera.x + Camera.w))&&((self->s.y + self->size.y) >= Camera.y)&&(self->s.y <= (Camera.y + Camera.h)))
    return 1;
  return 0;
}


int DistanceBetween(Entity *self, Entity *target)
{
  int difx,dify;
  difx = (int)abs((int)self->s.x - (int)target->s.x);
  dify = (int)abs((int)self->s.y - (int)target->s.y);
  return (difx + dify)>>1;
}



int Collide(SDL_Rect box1,SDL_Rect box2)
{
  /*check to see if box 1 and box 2 clip, then check to see if box1 is in box or vice versa*/
  if((box1.x + box1.w >= box2.x) && (box1.x <= box2.x+box2.w) && (box1.y + box1.h >= box2.y) && (box1.y <= box2.y+box2.h))
    return 1;
  return 0;
}


int VectorLength(float vx,float vy)
{
  return (int)((vx * vx) + (vy *vy)) >> 1;
}

void  VectorScaleTo(int magnitude, float *xdir,float *ydir)
{
  ScaleVectors(xdir,ydir);
  *xdir *= magnitude;
  *ydir *= magnitude;  
}

void ScaleVectors(float *vx, float *vy)
{
  double hyp;
  hyp = sqrt((*vx * *vx) + (*vy * *vy));
  if(hyp == 0)return;
  hyp = 1 / hyp;
  *vx = (*vx * hyp);
  *vy = (*vy * hyp);
}



