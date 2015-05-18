#include "Projectile.h"
#include "cocos2d.h"

Projectile::Projectile(Sprite *character, TMXTiledMap *map, cocos2d::Vector<cocos2d::Sprite *> *_projectiles) {

	this->character = character;
	this->map = map;
	this->_projectiles = _projectiles;
}

void Projectile::projectileMoveFinished(Node *pSender) {

	Sprite *sprite = (Sprite *)pSender;

	map->removeChild(sprite);

	_projectiles->eraseObject(sprite);
}

void Projectile::projectileLogic(Touch *touch) {

	auto touchLocation = touch->getLocation();
	touchLocation = map->convertToNodeSpace(touchLocation);

	auto projectile = Sprite::create("projectiles/laserball.png");
	projectile->setPosition(character->getPosition());

	map->addChild(projectile, 10);

	int realX;

	auto diff = touchLocation - character->getPosition();

	if (diff.x > 0) {

		realX = (map->getMapSize().width * map->getTileSize().width) + (projectile->getContentSize().width / 2);

	} else {

		realX = -(map->getMapSize().width * map->getTileSize().width) - (projectile->getContentSize().width / 2);
	}

	float proportion = (float)diff.y / (float)diff.x;

	int realY = ((realX - projectile->getPosition().x) * proportion) + projectile->getPosition().y;

	auto realTarget = Point(realX, realY);

	int offRealX = realX - projectile->getPosition().x;
	int offRealY = realY - projectile->getPosition().y;

	float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
	float velocity = 480 / 1; // 480pixels/1sec
	float realMoveDuration = length / velocity;

	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(Projectile::projectileMoveFinished, this));
	projectile->runAction(Sequence::create(MoveTo::create(realMoveDuration, realTarget), actionMoveDone, NULL));

	_projectiles->pushBack(projectile);
}