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

extern Quadtree* __quadtreeList; /** the list of quadtrees */


void clear(Quadtree *node);
void split(Quadtree *node);
int getIndex(Entity *ent,Quadtree *node);
void InitQuadtrees();
void PrepareQuadtrees();
Quadtree* GetQuadtree(int level, float x, float y, float w, float h);
void insert(Entity *ent,Quadtree *node) ;
void PotentialColidables(Entity *ent,Quadtree *node, Entity***out, int cursor);

#endif