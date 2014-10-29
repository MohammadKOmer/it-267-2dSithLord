#ifndef _ENTITY_
#define _ENTITY_

#include "graphics.h"


#define MAXENTITIES   1024
#define MAXSTATES     10


/*random functions stolen shamelessly from quake2*/
#define random()  ((rand () & 0x7fff) / ((float)0x7fff))
#define crandom() (2.0 * (random() - 0.5))

enum ENTTYPES   {ET_Temp,ET_WorldBullet,ET_WorldEnt,ET_PlayerBullet,ET_Player};
enum ENTCLASS   {EC_NULL,EC_AI,EC_PLAYER,EC_BULLET,EC_STATIC};
enum DAMAGE_T   {DT_Saber,DT_Energy,DT_Crushing};
enum STATES     {ST_IDLE,ST_MOVINGH, ST_DIE, ST_DEAD,ST_ALERT,ST_ATTACK1,ST_FORCE,ST_LIFTING,ST_LIFTED};
enum FACE       {F_LEFT,F_RIGHT};
enum AttackFaces {AS_UP,AS_DOWN,AS_FACE};




typedef struct Point_T
{
	int x,y;
}Point;

typedef struct Coord_T
{
	float x,y;
}Coord;


typedef struct Entity_T
{
	int used;                 /*used is for keeping track of what entities are free and should NEVER be touched.*/
	Uint32 LastDrawn;         /*another one that should never be touched by anything but the maintainence functions*/


	struct Entity_T *player;  /*this might be a bad idea by i want everything aware of player entity*/


	int EntClass;             
	int Unit_Type;            /*ET_* for hit detection masking*/
	char EntName[40];         /*the name of the entity*/
	Sprite *sprite;           /*the sprite for the main part of the entity*/


	struct Entity_T *owner;   /*for bullets, drones and other things spawned by other entities*/
	struct Entity_T *target;  /*used so that the stormtroppers and sith know what they are aiming at*/
	void (*think) (struct Entity_T *self);    /*called by the engine every so often to handle input and make decisions*/
	void (*update) (struct Entity_T *self);   /*called by the engine every so often to update the position and check for collisions*/
	Uint32 NextThink;         /*used for how often the entity thinks*/
	Uint16 ThinkRate;         /*used for incrementing above*/
	Uint32 NextUpdate;        /*used for how often the entity is updated, updating is merely animations*/
	Uint16 UpdateRate;        /*used for incrementing above*/
	
	int timeSinceLastAttack;
	int gravityent;           /*if we are affected by gravity*/
	int grounded;             /*set to 1 when the entity is standing on the ground*/
	int Color;                /*the index color for bullets and trails and such*/
	int shown;                /*if 1 then it will be rendered when it is on screen*/
	int frame;                /*current frame to render*/
	int fcount;               /*used for animation, the loop variable*/
	int frameR;				  /*Which row of the spritesheet is being looped over*/
	int frate;                /*how often we update our frames*/
	Uint32 framedelay;        /*ammount of delay between frames*/
	int face;                 /*the direction we are moving*/
	int aimdir;               /*the direction we are shooting at*/
	int state;                /*making each entity a finite state machine.*/
	SDL_Rect  Boundingbox;    /*the bounding box for collision detection*/
	SDL_Rect  ClipBox;        /*the bounding box for hit detection*/
	
	Coord s;                  /*screen coordinates*/
	Coord a;                  /*acceleration*/
	Coord v;                  /*vector values*/
	Coord pushed;			  /* any pushing forces on entity*/
	Point m;                  /*Map coordinate*/
	Point size;               /*mirrors the sprite width and height*/
	Point origin;             /*offset of the center from s*/
	int movespeed;            /*current movement speed*/
	int maxspeed;             /*maximum movement speed*/
	int ordercount;           /*AI use counting variable*/


	int sightrange;           /*how big my sigh radius is for detection of the player*/
	int accel;                /*how much our movement is increased when we accellerate*/
	int kick;                 /*for weapons, how much we can push something*/
	int damage;               /*how much damage I can inflict for things that can deal damage*/

	int dtype;                /*what type of Damage I will inflict*/
	int lifetime;			  /*Used for lightsaber only in the midterm build*/
	
	/*               armor and health                 */
	
	int health,healthmax;     /*everything will need health.  Resources will use it for how much of a resource it has left*/
	int takedamage;           /*a flag as to weather or not we can take damage*/

}Entity;

extern int EnemyPresent;
/*startup and clean up*/
void InitEntityList();
void ClearEntities();

/*creation and destruction*/
Entity *NewEntity();
void FreeEntity(Entity *ent);

/*update functions*/
void DrawEntity(Entity *ent);
void DrawEntities();
void UpdateEntities();
void ThinkEntities();


void DrawBBoxEntities();/*draw only the bounding box of the entity*/



/*generic entity support functions*/
int OnScreen(Entity *self);
void DamageTarget(Entity *attacker,Entity *inflictor,Entity *defender,int damage,int dtype,float kick,float kickx,float kicky);
void DamageRadius(Entity *attacker,Entity *inflictor,int damage,int radius,int dtype,float kick);
Entity *GetClosestEntity(Entity *self,int rx,int ry,int depth);
Entity *GetNextEntByRad(Entity *self,int radius,int rx,int ry,Entity *start,int ETMask);
int MoveUnit(Entity *self);
int GetNextCount(Entity *self);
void  VectorScaleTo(int magnitude, float *xdir,float *ydir);
void ScaleVectors(float *vx, float *vy);
int VectorLength(float vx,float vy);    /*only useful for integer comparisons, bu faster than float answer*/
int Collide(Entity *ent1,Entity *ent2);/*returns 1 if the two entities are clipping*/


int GetFace(Entity *self);



void UpdateEntityPosition(Entity *self);
int DistanceBetween(Entity *self, Entity *target);/*returns the diference between two entities.*/
int AimAtTarget(Entity *self,Entity *target);
Coord AddVectors(Coord v1,Coord v2);
Coord FastAddVectors(Coord v1,Coord v2);

/*collision detection functions*/
int TraceHit(float sx, float sy, float vx, float vy, float *fx, float *fy);
void InitRegionMask(int sizex,int sizey); /*based on size of map*/
void ClearRegionMask();   /*de-allocate all allocated memory*/
Entity *GetEntByBox(SDL_Rect bbox,int rx,int ry,Entity *ignore,int ETMask);
Entity *GetNextEntByBox(SDL_Rect bbox,int rx,int ry,Entity *start,int ETMask);
int AddEntToRegion(Entity *ent,int rx,int ry);
void RemoveEntFromRegion(Entity *ent,int rx,int ry);
void DrawBuckets();
void AdjustOrbit(Entity *self);


void SpawnFloor(int x,int y);
void SpawnWall(int x,int y);

#endif