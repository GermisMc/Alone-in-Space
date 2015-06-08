#include "Projectile.h"
#include "cocos2d.h"
#include "SimpleaudioEngine.h"

Projectile::Projectile(Sprite *character, TMXTiledMap *map, cocos2d::Vector<cocos2d::Sprite *> *_projectiles, Sprite **flash) {

	this->character = character;
	this->map = map;
	this->_projectiles = _projectiles;
	this->flash = flash;
}

void Projectile::projectileMoveFinished(Node *pSender) {

	Sprite *sprite = (Sprite *)pSender;

	map->removeChild(sprite);

	_projectiles->eraseObject(sprite);
}

void Projectile::projectileLogic(Touch *touch, char *proj, char *projsound) {

	const int LOCAL_Z_ORDER_PROJ = 10;

	auto touchLocation = touch->getLocation();
	touchLocation = map->convertToNodeSpace(touchLocation);

	auto projectile = Sprite::createWithSpriteFrameName(proj);
	projectile->setPosition((*flash)->getPosition());

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(projsound);

	map->addChild(projectile, LOCAL_Z_ORDER_PROJ);

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