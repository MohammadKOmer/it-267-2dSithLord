#include <chipmunk/chipmunk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space.h"


int freeIndex;




cpSpace * __space;

void InitSpace()
{
	 cpVect gravity = cpv(0, -10);
	__space = cpSpaceNew();
	cpSpaceSetGravity(__space, gravity);
	 cpSpaceAddCollisionHandler(__space,  C_Static,C_Bullet, bulletToWall, NULL, NULL, NULL, NULL);
	 cpSpaceAddCollisionHandler(__space,  C_Person,C_Bullet, bulletToPerson, NULL, NULL, NULL, NULL);

}
void postStepRemove(cpSpace *space, cpShape *shape, void *unused)
{
	FreeEntity((Entity*)shape->body->data);
 
}

int bulletToWall(cpArbiter *arb, cpSpace *space, void *unused)
{
  
  cpShape *a, *b; 
  cpArbiterGetShapes(arb, &a, &b);
  
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, b, NULL);
  
  return 0;
}

int bulletToPerson(cpArbiter *arb, cpSpace *space, void *unused)
{
  
  cpShape *a, *b; 
  cpArbiterGetShapes(arb, &a, &b);
  Entity* bullet = (Entity*) a->body->data;
  Entity* hit = (Entity*) b->body->data;
  if(bullet->owner->EntClass!= hit->EntClass){
	  hit->health-= bullet->damage;
  }
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, b, NULL);
  
  return 0;
}