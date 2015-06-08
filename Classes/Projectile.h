#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "cocos2d.h"

using namespace cocos2d;

class Projectile : public Node
{

public:

	Projectile(Sprite *character, TMXTiledMap *map, cocos2d::Vector<cocos2d::Sprite *> *_projectiles, Sprite **flash);

	void projectileLogic(Touch *touch, char *proj, char *projsound);
private:

	Sprite *character;
	Sprite **flash;

	TMXTiledMap *map;

	cocos2d::Vector<cocos2d::Sprite *> *_projectiles;

	void projectileMoveFinished(cocos2d::Node *pSender);
};

#endif // __PROJECTILE_H__