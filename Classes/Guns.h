#ifndef __GUNS_H__
#define __GUNS_H__

#include "cocos2d.h"
#include "Projectile.h"

using namespace cocos2d;

class Guns : public Node
{
private:

	TMXTiledMap *map;

	EventKeyboard::KeyCode currentKey;

	Sprite *character;
	Sprite *gun;
	Sprite **flash;
public:

	Guns(Sprite *character, TMXTiledMap *map, Sprite **flash);

	void gunRender(EventKeyboard::KeyCode currentKey, char *guntexture);

	void flashRender();
};

#endif // __GUNS_H__