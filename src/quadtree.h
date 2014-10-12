#ifndef __QUADTREE__
#define __QUADTREE__

#include "entity.h"
#include "graphics.h"

/*Quadtree for use in collision detection*/
typedef struct Quadtree_T
{
	int level; /*level of tree node from 0-4*/
	int numObjects;
	Entity* Entities[4];  /*4 entities per node ^5 levels =1024*/
	SDL_Rect  Bounds; 
	struct Quadtree_T* Nodes[4];
	
}Quadtree;

#endif