#include <chipmunk/chipmunk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space.h"

#define __maxDepth 3
#define __maxQuadtrees (1024) /* depth of 0, 1, 2, 3 respectively */


int freeIndex;




cpSpace * __space;

void InitSpace()
{
	 cpVect gravity = cpv(0, -100);
	__space = cpSpaceNew();
	cpSpaceSetGravity(__space, gravity);
	 
}

void insert(Entity *ent,cpSpace *space) {
	
}



void PotentialColidables(Entity *ent,Quadtree *node, Entity**out, int cursor){
	int i;  
	int index;

	for(i=0;i<16;i++)
	{	
		if(node->Entities[i]){
			/*printf("Possible colidible found %s with %s\n",node->Entities[i]->EntName,ent->EntName);*/
				if(node->Entities[i]!=ent&&Collide(node->Entities[i],ent)){
					(out)[cursor] = node->Entities[i];
					cursor++;
			}
		}
	}

	index = getIndex(ent,node);
	if (index != -1 && node->Nodes[0])
	{
		PotentialColidables(ent,node->Nodes[index],out,cursor);
	}
}