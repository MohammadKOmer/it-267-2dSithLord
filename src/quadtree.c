#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "quadtree.h"

#define __maxDepth 3
#define __maxQuadtrees (1024) /* depth of 0, 1, 2, 3 respectively */


int freeIndex;


Quadtree* root;

Quadtree* __quadtreeList;

void InitQuadtrees()
{


	__quadtreeList = (Quadtree*) malloc(sizeof(Quadtree) * __maxQuadtrees); /* allocate the required memory to hold all the quadtrees */

	if(__quadtreeList == NULL)
	{
		printf( "InitQuadtrees: FATAL: cannot allocate quadtree list\n");
		exit(-1);
		return;
	}

	freeIndex = 0;

	memset(__quadtreeList,0,sizeof(Quadtree) * __maxQuadtrees); /* make sure the memory is wiped before using it */
}

void PrepareQuadtrees()
{
	freeIndex = 0;
	memset(__quadtreeList,0,sizeof(Quadtree) * __maxQuadtrees);
}

Quadtree* GetQuadtree(int level, float x, float y, float w, float h)
{
	Quadtree* quad;
	if(freeIndex >= __maxQuadtrees) return NULL;

	quad = &__quadtreeList[freeIndex];

	quad->level = level;

	quad->Bounds.x = x;
	quad->Bounds.y = y;
	quad->Bounds.w = w;
	quad->Bounds.h = h;

	freeIndex++;

	return quad;
}


void split(Quadtree *node) {

	int subWidth = (int)(node->Bounds.w / 2);
	int subHeight = (int)(node->Bounds.w / 2);

	node->Nodes[0] = GetQuadtree(node->level + 1, node->Bounds.x+subWidth,	node->Bounds.y,				subWidth,subHeight);
	node->Nodes[1] = GetQuadtree(node->level + 1, node->Bounds.x,			node->Bounds.y,				subWidth,subHeight);
	node->Nodes[2] = GetQuadtree(node->level + 1, node->Bounds.x,			node->Bounds.y+ subHeight,	subWidth,subHeight);
	node->Nodes[3] = GetQuadtree(node->level + 1, node->Bounds.x+subWidth,	node->Bounds.y+ subHeight,	subWidth,subHeight);

}

int getIndex(Entity *ent,Quadtree *node){
	int index = -1;
	double verticalMidpoint = node->Bounds.x + (node->Bounds.w / 2);
	double horizontalMidpoint = node->Bounds.y + (node->Bounds.h / 2);
	int topQuadrant;
	int bottomQuadrant;
	topQuadrant = (ent->s.y < horizontalMidpoint && ent->s.y +ent->Boundingbox.h < horizontalMidpoint);

	bottomQuadrant = (ent->s.y > horizontalMidpoint);


	if (ent->s.x < verticalMidpoint &&ent->s.x+ ent->Boundingbox.w < verticalMidpoint) {
		if (topQuadrant) {
			index = 1;
		}
		else if (bottomQuadrant) {
			index = 2;
		}
	}
	// Object can completely fit within the right quadrants
	else if (ent->s.x > verticalMidpoint) {
		if (topQuadrant) {
			index = 0;
		}
		else if (bottomQuadrant) {
			index = 3;
		}
	}

	return index;

}

void insert(Entity *ent,Quadtree *node) {
	int i,j,index;


	if (node->Nodes[0]) {
		index = getIndex(ent,node);

		if (index != -1) {
			insert(ent,node->Nodes[index]);
			return;
		}
	}
	if(node->numObjects>=15){
		printf( "Quadtree: Num Objects 15 or greater");
		exit(-1);
	}
	if(node->numObjects < 5){
		j=0;
		node->numObjects+=1;
		while(node->Entities[j]){
			j++;
		}
		node->Entities[j]=ent;
	}else{
		if (!(node->Nodes[0])) {
			split(node);
		}
		for(i=0;i<16;i++){
			if(!node->Entities[i]){
				continue;
			}
			index=getIndex(node->Entities[i],node);
			if (index != -1) {
				insert(node->Entities[i],node->Nodes[index]);
				node->Entities[i]=NULL;
				node->numObjects-=1;
			}
		}
	}
}



void PotentialColidables(Entity *ent,Quadtree *node, Entity**out, int cursor){
	int i;  
	int index;

	for(i=0;i<16;i++)
	{	
		if(node->Entities[i]){
			if(Collide(node->Entities[i]->Boundingbox,ent->Boundingbox)){
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