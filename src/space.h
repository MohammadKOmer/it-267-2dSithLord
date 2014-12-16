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
	SDL_Rect  Bounds;		/*Bounds of the tree */
	struct Quadtree_T* Nodes[4];
}Quadtree;

extern cpSpace* __space; /** head node of the quadtrees */


void clear(Quadtree *node); 
void split(Quadtree *node); /*split tree into 4*/
int getIndex(Entity *ent,Quadtree *node);
void InitSpace();  /*allocates a persistent section of memory */
void PrepareQuadtrees();  /*it amounts to  a memory wipe */
Quadtree* GetQuadtree(int level, float x, float y, float w, float h);
void insert(Entity *ent,cpSpace *space) ;
void PotentialColidables(Entity *ent,Quadtree *node, Entity** out, int cursor);

#endif