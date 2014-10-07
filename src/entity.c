#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "entity.h"



#define   RPixel_W    256
#define   RPixel_H    256



extern SDL_Surface *screen;
extern SDL_Surface *clipmask;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Uint32 NOW;


Entity EntityList[MAXENTITIES];
int NumEnts = 0;


void InitEntityList()
{
	int i,j;
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
	int tsx,tsy;   /*temporary positions*/
	Uint32 start = SDL_GetTicks();
	Uint32 clear = SDL_MapRGB(background->format,0,0,0);
	int xdir = 0;
	int ydir = 0;
	int updated_s = 0;
	float temp = 0;
	double vx,vy = 0;
	
	float fx,fy,gx,gy,hx,hy = 0;
	vx = self->v.x;
	vy = self->v.y;
	if(self->v.x < 0)
	{
		tsx  = ((int)self->s.x + self->Boundingbox.x);
		xdir = 1;
	}
	else if(self->v.x > 0)
	{
		tsx  = ((int)self->s.x + self->Boundingbox.x + self->Boundingbox.w);
		xdir = -1;
	}
	else
	{
		xdir = 0;
		tsx  = ((int)self->s.x + self->Boundingbox.x + (self->Boundingbox.w >> 1));
	}
	if(self->v.y < 0)
	{
		tsy  = ((int)self->s.y + self->Boundingbox.y);
		ydir = 1;
	}
	else if(self->v.y > 0)
	{
		tsy  = ((int)self->s.y + self->Boundingbox.y + self->Boundingbox.h);
		ydir = -1;
	}
	else
	{
		tsy  = ((int)self->s.y + self->Boundingbox.y + (self->Boundingbox.h >> 1));
		ydir = 0;    
	}
	start = SDL_GetTicks();

	if(xdir&ydir)
	{
		if((TraceHit(tsx , tsy, vx, vy, &hx, &hy))&&(updated_s == 0))
		{
			self->s.x += hx;
			self->s.y += hy;
			updated_s = 1;
		}
	}
	if(xdir)
	{
		if((TraceHit(tsx , tsy, vx, 0, &fx, &fy))&&(updated_s == 0))
		{
			hx = vx;
			hy = vy;
			VectorScaleTo(sqrt((fx * fx) + (fy * fy)),&hx,&hy);
			self->s.x += hx;
			self->s.y += hy;
			updated_s = 1;
		}
	}
	if(ydir)
	{
		if((TraceHit(tsx , tsy, 0, vy, &gx, &gy))&&(updated_s == 0))
		{
			hx = vx;
			hy = vy;
			VectorScaleTo(sqrt((gx * gx) + (gy * gy)),&hx,&hy);
			self->s.x += hx;
			self->s.y += hy;
			updated_s = 1;
		}
	}
	if((clipmask != NULL)&&(SDL_MUSTLOCK(clipmask)))SDL_UnlockSurface(clipmask);
	if(updated_s == 0)
	{
		self->s.x += vx;
		self->s.y += vy;
	}

	self->v.x = vx;
	self->v.y = vy;
	if(ydir == -1)
	{
		if(vy <= 0.2)
		{
			self->grounded = 1;
		}
	}

	self->v.x += self->a.x+self->pushed.x;
	self->v.y += self->a.y+self->pushed.y;
}



int TraceHit(float sx, float sy, float vx, float vy, float *fx, float *fy)
{
	Uint32 clear = SDL_MapRGBA(background->format,0,0,0,0);
	Uint32 pixelColor;
	int deltax,deltay;
	float x,y;
	int curpixel;
	float ox,oy;    /*old x, old y*/
	int den,num,numadd,numpixels;
	int xinc1,xinc2,yinc1,yinc2;
	deltax = fabs(vx);        // The difference between the x's
	deltay = fabs(vy);        // The difference between the y's
	x = sx;                       // Start x off at the first pixel
	y = sy;                       // Start y off at the first pixel

	if (vx >= 0)                 // The x-values are increasing
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          // The x-values are decreasing
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (vy >= 0)                 // The y-values are increasing
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          // The y-values are decreasing
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         // There is at least one x-value for every y-value
	{
		xinc1 = 0;                  // Don't change the x when numerator >= denominator
		yinc2 = 0;                  // Don't change the y for every iteration
		den = deltax;
		num = deltax >> 1;
		numadd = deltay;
		numpixels = deltax;         // There are more x-values than y-values
	}
	else                          // There is at least one y-value for every x-value
	{
		xinc2 = 0;                  // Don't change the x for every iteration
		yinc1 = 0;                  // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay >> 1;
		numadd = deltax;
		numpixels = deltay;         // There are more y-values than x-values
	}
	ox = x;
	oy = y;
	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		pixelColor = getpixel(clipmask, x ,y);
		fprintf(stdout,"pixel color: %i, clear color: %i\n",pixelColor,clear);
		if(pixelColor != clear)/*check to see if the pixel is clear or not*/
		{
			*fx = x - sx;
			*fy = y - sy;
			fprintf(stdout,"We hit something\n");
			return 1;/*we hit shit*/
		}
		num += numadd;              // Increase the numerator by the top of the fraction
		if (num >= den)             // Check if numerator >= denominator
		{
			num -= den;               // Calculate the new numerator value
			x += xinc1;               // Change the x as appropriate
			y += yinc1;               // Change the y as appropriate
		}
		x += xinc2;                 // Change the x as appropriate
		y += yinc2;                 // Change the y as appropriate
		ox = x;
		oy = y;
	}
	/*clean trace, nothing hit*/
	return 0;
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



