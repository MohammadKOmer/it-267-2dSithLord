#ifndef __PROJECTILE__
#define __PROJECTILE__
#include "entity.h"
#include "quadtree.h"

void initProjectiles();
Entity *SpawnBullet(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType);
       
void UpdateBullet(Entity *self);

Entity *SpawnForcePush(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType);
void UpdateForcePush(Entity *self);

Entity *SpawnSaberhit(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType);
void UpdateSaberhit(Entity *self);
Entity *GetTouchingEnt(Entity *self, Entity ***out);

#endif