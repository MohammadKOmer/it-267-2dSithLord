#ifndef __QUADTREE__
#define __QUADTREE__

#include "entity.h"
#include "graphics.h"


extern cpSpace* __space; /** head node of the quadtrees */


void InitSpace();  /*allocates a persistent section of memory */

void postStepRemove(cpSpace *space, cpShape *shape, void *unused);
int bulletToWall(cpArbiter *arb, cpSpace *space, void *unused);
int bulletToPerson(cpArbiter *arb, cpSpace *space, void *unused);

#endif