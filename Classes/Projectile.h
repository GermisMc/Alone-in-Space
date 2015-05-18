#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "cocos2d.h"
#include "Enemies.h"

using namespace cocos2d;

class Projectile : public Node
{
private:

	Sprite *character;

	TMXTiledMap *map;

	cocos2d::Vector<cocos2d::Sprite *> *_projectiles;
public:

	Projectile(Sprite *character, TMXTiledMap *map, cocos2d::Vector<cocos2d::Sprite *> *_projectiles);

	void projectileMoveFinished(cocos2d::Node *pSender);

	void projectileLogic(Touch *touch);
};

#endif // __PROJECTILE_H__