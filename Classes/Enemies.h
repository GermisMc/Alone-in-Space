#ifndef __ENEMIES_H__
#define __ENEMIES_H__

#include "cocos2d.h"

using namespace cocos2d;

class Enemies
{
private:

	Sprite *character;
	Sprite *enemy;

	TMXTiledMap *map;

	TMXLayer *wall;

	TMXObjectGroup *objectGroup;

	cocos2d::Vector<cocos2d::Sprite *> _visibleEnemies;
	cocos2d::Vector<cocos2d::Sprite *> _enemies;
	cocos2d::Vector<cocos2d::Sprite *> _projectiles;
public:

	Enemies(Sprite *character, TMXTiledMap *map, TMXObjectGroup *objectGroup, TMXLayer *wall, cocos2d::Vector<cocos2d::Sprite *> _projectiles);

	void enemyMoveFinished(Node *pSender);

	void animateEnemy(Sprite *enemy);

	void addEnemyAtPos(Point position);

	void spawnEnemy();

	void enemyOnScreen();

	void testCollisions();

	//void stopEnemyAtBlock();

	//Point tileCoordForPosition(Point position);
};

#endif // __ENEMIES_H__