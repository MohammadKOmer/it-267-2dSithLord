#ifndef _ENEMY_
#define _ENEMY_

#include "entity.h"

enum EnemyTypes {ET_NULL,ET_TROOPER,ET_SITH};
/*Enemy spawning and think functions */
void SpawnEnemy(int x, int y, int type);
void TrooperThink(Entity *self);
void UpdateTrooper(Entity *self);
void SithThink(Entity *self);
void UpdateSith(Entity *self);
#endif