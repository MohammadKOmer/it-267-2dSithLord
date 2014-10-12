#ifndef __QUADTREE__
#define __QUADTREE__

#include "entity.h"
#include "graphics.h"

/*Quadtree for use in collision detection*/
typedef struct Quadtree_T
{
	int level; /*level of tree node from 0-4*/
	int numObjects;
	Entity* Entities[16];  /*16 entities per node ^5 levels=1024*4 corners*/
	SDL_Rect  Bounds; 
	struct Quadtree_T* Nodes[4];
}Quadtree;

int initTree();
void clear(Quadtree *node);
void split(Quadtree *node);
int getIndex(Entity *ent,Quadtree *node); 
void insert(Entity *ent,Quadtree *node);
Entity *PotentialColidables(Entity *ent,Quadtree *node);


#endif