#include "quadtree.h"

Quadtree* root;


int initTree(){

}

void clear(Quadtree *node){
	int i;
	if(node->level>=5){
		return;
	}
	for(i=0; i<16;i++){
		 if(!node->Entities[i]){
			continue;
		 }
		 FreeEntity(node->Entities[i]);
	}

	for(i=0; i<4;i++){
		if(!node->Nodes[i]){
			continue;
		}
		 clear(node->Nodes[i]);
	}

}

void split(Quadtree *node) {

	int subWidth = (int)(node->Bounds.w / 2);
	int subHeight = (int)(node->Bounds.w / 2);


	node->Nodes[0]->level=node->level++;
	node->Nodes[0]->Bounds.x=node->Bounds.x+subWidth;
	node->Nodes[0]->Bounds.y=node->Bounds.y;
	node->Nodes[0]->Bounds.w=subWidth;
	node->Nodes[0]->Bounds.h=subHeight;

	node->Nodes[1]->level=node->level++;
	node->Nodes[1]->Bounds.x=node->Bounds.x;
	node->Nodes[1]->Bounds.y=node->Bounds.y;
	node->Nodes[1]->Bounds.w=subWidth;
	node->Nodes[1]->Bounds.h=subHeight;

	node->Nodes[2]->level=node->level++;
	node->Nodes[2]->Bounds.x=node->Bounds.x;
	node->Nodes[2]->Bounds.y=node->Bounds.y+ subHeight;
	node->Nodes[2]->Bounds.w=subWidth;
	node->Nodes[2]->Bounds.h=subHeight;

	node->Nodes[3]->level=node->level++;
	node->Nodes[3]->Bounds.x=node->Bounds.x+subWidth;
	node->Nodes[3]->Bounds.y=node->Bounds.y+ subHeight;
	node->Nodes[3]->Bounds.w=subWidth;
	node->Nodes[3]->Bounds.h=subHeight;
}

int getIndex(Entity *ent,Quadtree *node){
	int index = -1;
	double verticalMidpoint = node->Bounds.x + (node->Bounds.w / 2);
	double horizontalMidpoint = node->Bounds.y + (node->Bounds.h / 2);

	// Object can completely fit within the top quadrants
	bool topQuadrant = (ent->Boundingbox.y < horizontalMidpoint && ent->Boundingbox.y +ent->Boundingbox.h < horizontalMidpoint);
	// Object can completely fit within the bottom quadrants
	bool bottomQuadrant = (ent->Boundingbox.y > horizontalMidpoint);

	// Object can completely fit within the left quadrants
	if (ent->Boundingbox.x < verticalMidpoint &&ent->Boundingbox.x+ ent->Boundingbox.w < verticalMidpoint) {
		if (topQuadrant) {
			index = 1;
		}
		else if (bottomQuadrant) {
			index = 2;
		}
	}
	// Object can completely fit within the right quadrants
	else if (ent->Boundingbox.x > verticalMidpoint) {
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
	if(node->numObjects<16){
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
			index=getIndex(node->Entities[i],node);
			if (index != -1) {
				insert(node->Entities[i],node->Nodes[index]);
				node->Entities[i]=NULL;
			}
		}
		index=getIndex(ent,node);
		if (index != -1) {
			insert(ent,node->Nodes[index]);
			node->Entities[i]=NULL;
		}
	}
}

Entity *PotentialColidables(Entity *ent,Quadtree *node){
	int index = getIndex(ent);
   if (index != -1 && node->Nodes[0]) {
     PotentialColidables(ent,node->Nodes[index]);
   }
 
   
   return node->Entities[0];
}