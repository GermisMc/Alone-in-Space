#include "Enemies.h"
#include "cocos2d.h"

Enemies::Enemies(Sprite* character, TMXTiledMap* map, TMXObjectGroup *objectGroup, TMXLayer *wall, cocos2d::Vector<cocos2d::Sprite *> _projectiles) {

	this->character = character;
	this->map = map;
	this->objectGroup = objectGroup;
	this->wall = wall;
	this->_projectiles = _projectiles;
	
}

void Enemies::enemyMoveFinished(Node *pSender) {

	Sprite *enemy = (Sprite *)pSender;
	this->animateEnemy(enemy);
}

void Enemies::animateEnemy(Sprite *enemy) {

	auto actionTo1 = RotateTo::create(0, 0, 180);
	auto actionTo2 = RotateTo::create(0, 0, 0);
	auto diff = Point(character->getPosition() - enemy->getPosition());
	if (diff.x < 0) {
		enemy->runAction(actionTo2);
	}
	if (diff.x > 0) {
		enemy->runAction(actionTo1);
	}

	float actualDuration = 0.3f;
	auto position = (character->getPosition() - enemy->getPosition());
	position.normalize();
	auto actionMove = MoveBy::create(actualDuration, position.operator*(10));
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(Enemies::enemyMoveFinished, this));

	enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
}

void Enemies::addEnemyAtPos(Point position) {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Point charPos = character->getPosition();

	auto winSize = Director::getInstance()->getWinSize();

	enemy = Sprite::create("enemies/gost.png");
	enemy->setPosition(position);

	if (position.x < charPos.x - winSize.width / 2 &&
		position.y < charPos.y - winSize.height / 2 &&
		position.x > charPos.x + winSize.width / 2 &&
		position.y > charPos.y + winSize.height / 2)
	{
		this->animateEnemy(enemy);
	} else {
		_visibleEnemies.pushBack(enemy);
	}

	map->addChild(enemy, 10);

	_enemies.pushBack(enemy);
}

void Enemies::enemyOnScreen() {
	
	auto winSize = Director::getInstance()->getWinSize();

	Point charPos = character->getPosition();

	for (Sprite* target : _visibleEnemies) {
		Point positionEnemy = target->getPosition();

		if (positionEnemy.x > charPos.x - winSize.width / 2 &&
			positionEnemy.y > charPos.y - winSize.height / 2 &&
			positionEnemy.x < charPos.x + winSize.width / 2 &&
			positionEnemy.y < charPos.y + winSize.height / 2)
		{
			this->animateEnemy(target);
			_visibleEnemies.eraseObject(target);
			break;
		}
	}
}

void Enemies::spawnEnemy() {

	for (auto& eSpawnPoint : objectGroup->getObjects()) {
		ValueMap& dict = eSpawnPoint.asValueMap();
		if (dict["Enemy"].asInt() == 1) {
			int x = dict["x"].asInt();
			int y = dict["y"].asInt();
			this->Enemies::addEnemyAtPos(Point(x, y));
		}
	}
}

void Enemies::testCollisions() {

	cocos2d::Vector<Sprite*> projectilesToDelete;

	for (Sprite *projectile : _projectiles) {
		auto projectileRect = Rect(
			projectile->getPositionX() - projectile->getContentSize().width / 2,
			projectile->getPositionY() - projectile->getContentSize().height / 2,
			projectile->getContentSize().width,
			projectile->getContentSize().height);

		cocos2d::Vector<Sprite*> targetsToDelete;
		
		for (Sprite *target : _enemies) {
			auto targetRect = Rect(
				target->getPositionX() - target->getContentSize().width / 2,
				target->getPositionY() - target->getContentSize().height / 2,
				target->getContentSize().width,
				target->getContentSize().height);

			if (projectileRect.intersectsRect(targetRect)) {
				targetsToDelete.pushBack(target);
			}
		}


		for (Sprite *target : targetsToDelete) {
			_enemies.eraseObject(target);
			map->removeChild(target);
		}

		if (targetsToDelete.size() > 0) {
			// add the projectile to the list of ones to remove
			projectilesToDelete.pushBack(projectile);
		}
		targetsToDelete.clear();
	}


	for (Sprite *projectile : projectilesToDelete) {
		_projectiles.eraseObject(projectile);
		map->removeChild(projectile);
	}
	projectilesToDelete.clear();
}

// Today it's not working

/*void Enemies::stopEnemyAtBlock() {

	Point position = enemy->getPosition();

	Point tileCoord = this->Enemies::tileCoordForPosition(position);

	int tileGidWall = wall->getTileGIDAt(tileCoord);

	if (tileGidWall) {
		auto properties = map->getPropertiesForGID(tileGidWall).asValueMap();
		if (!properties.empty()) {
			auto collision = properties["Collidable"].asString();
			if ("true" == collision) {
				enemy->stopAllActions();
				enemy->setPosition(character->getPosition());
			}
			else {
				//return true;
			}
		}
	}
}

Point Enemies::tileCoordForPosition(Point position) {

	int x = position.x / map->getTileSize().width;
	int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;

	return Point(x, y);
}*/