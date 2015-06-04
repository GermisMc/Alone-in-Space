#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "cocos2d.h"

using namespace cocos2d;

class Collisions : public cocos2d::Node
{
private:


	int *ammorevolver;
	int *ammoplasmagun;
	int *hp;
	int *card;

	TMXTiledMap *map;

	Sprite *character;

	TMXLayer *wall;
	TMXLayer *closedDoor;
	TMXLayer *ammo;
	TMXLayer *items;
public:

	Collisions(TMXTiledMap *map, Sprite *character, TMXLayer *wall, int *ammorevolver, int *ammoplasmagun, int *hp, int *card);

	bool checkBorder(Point position);

	bool collision(Point position, EventKeyboard::KeyCode keyCode);

	void openDoors(Point tileCoord);

	void pickAmmo(Point tileCoord);

	void pickItems(Point tileCoord);

	void projCollision(cocos2d::Vector<cocos2d::Sprite *> *_projectiles, cocos2d::Vector<cocos2d::Sprite *> *_projTurret);

	void projCollisionLogic(cocos2d::Vector<cocos2d::Sprite *> *projectiles, char *type);

	Point tileCoordForPosition(Point position);
};

#endif // __COLLISIONS_H__