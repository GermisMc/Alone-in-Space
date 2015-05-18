#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "cocos2d.h"

using namespace cocos2d;

class Collisions : public cocos2d::Node
{
private:
	TMXTiledMap *map;

	Sprite *character;

	TMXLayer *wall;
	TMXLayer *closedDoor;
public:

	Collisions(TMXTiledMap *map, Sprite *character, TMXLayer *wall);

	bool checkBorder(Point position);

	bool collision(Point position, EventKeyboard::KeyCode keyCode);

	void openDoors(Point tileCoord);

	Point tileCoordForPosition(Point position);
};

#endif // __COLLISIONS_H__