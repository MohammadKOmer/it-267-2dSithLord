#ifndef __PROJECTILE__
#define __PROJECTILE__
#include "entity.h"

enum Bullet_Size {B_Tiny,B_Small,B_Medium,B_Large,B_Huge,B_Atomic};
Entity *SpawnBullet(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType);
Entity *SpawnLaser(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType,int gravity);


Entity *GetTouchingEnt(Entity *self);

#endif