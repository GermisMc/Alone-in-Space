#include "Enemies.h"
#include "cocos2d.h"
#include "SimpleaudioEngine.h"

Enemies::Enemies(Sprite* character, TMXTiledMap *map, TMXLayer *wall, cocos2d::Vector<cocos2d::Sprite *> *_projectiles, int *ammoplasmagun, cocos2d::Vector<cocos2d::Sprite *> *_projTurret, int *hp) {

	this->character = character;
	this->map = map;
	this->wall = wall;
	this->_projectiles = _projectiles;
	this->_projTurret = _projTurret;
	this->ammoplasmagun = ammoplasmagun;
	this->hp = hp;
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
	
	auto position = (character->getPosition() - enemy->getPosition());

	position.normalize();
	auto enemySpeed = Enemies::slowDownEnemy(enemy);
	auto actionMove = MoveBy::create(enemySpeed, position.operator*(10));
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(Enemies::enemyMoveFinished, this));

	enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
}

void Enemies::turretMoveFinished(Node *pSender) {

	Sprite *turret = (Sprite *)pSender;

	this->animateTurret(turret);
}

void Enemies::animateTurret(Sprite *turret) {

	Point charPos = character->getPosition();
	Point turrPos = turret->getPosition();

	float distance = turrPos.getDistance(charPos);
	float activeDistance = 400;

	Point shootVector = turrPos - charPos;

	float shootAngle = shootVector.getAngle() + CC_DEGREES_TO_RADIANS(360);
	float cocosAngle = CC_RADIANS_TO_DEGREES(-1 * shootAngle);

	double rotateSpeed = 0.3 / M_PI;

	float rotateDuration = fabs(shootAngle * rotateSpeed);

	auto actionMove = RotateTo::create(rotateDuration, cocosAngle);
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(Enemies::turretMoveFinished, this));
	auto turretact = CallFunc::create(CC_CALLBACK_0(Enemies::turretProjectile, this));

	turret->runAction(Sequence::create(actionMove, actionMoveDone, NULL));

	if (distance < activeDistance) {

		Enemies::turretProjectile();
	}
}

void Enemies::turretProjectile() {

	auto projectile = Sprite::create("projectiles/turret.png");
	projectile->setPosition(turret->getPosition()); 

	map->addChild(projectile, 5);

	int realX;

	auto diff = character->getPosition() - turret->getPosition();

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
	float velocity = 300 / 1; // 300pixels/1sec
	float realMoveDuration = length / velocity;

	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(Enemies::projectileMoveFinished, this));
	projectile->runAction(Sequence::create(MoveTo::create(realMoveDuration, realTarget), actionMoveDone, NULL));

	_projTurret->pushBack(projectile);
}

void Enemies::projectileMoveFinished(Node *pSender) {

	Sprite *sprite = (Sprite *)pSender;

	map->removeChild(sprite);

	_projTurret->eraseObject(sprite);
}

void Enemies::addEnemyAtPos(Point position) {

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

void Enemies::addTurretAtPos(Point position) {

	turret = Sprite::create("enemies/turret.png");

	turret->setPosition(position);
	turret->setName("turret");
	map->addChild(turret, 10);

	this->animateTurret(turret);
	
	_enemies.pushBack(turret);
}

void Enemies::enemyOnScreen() {
	
	auto winSize = Director::getInstance()->getWinSize();

	Point charPos = character->getPosition();

	for (Sprite *target : _visibleEnemies) {

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

	objectGroup = map->getObjectGroup("Enemies");

	for (auto &enemySpawnPoint : objectGroup->getObjects()) {

		ValueMap &dict = enemySpawnPoint.asValueMap();
		
		if (dict["Enemy"].asString() == "1") {

			int x = dict["x"].asInt();
			int y = dict["y"].asInt();

			_spawnCoord.push_back(Point(x, y));

			this->Enemies::addEnemyAtPos(Point(x, y));
		}
		if (dict["Turret"].asInt() == 1) {

			int x = dict["x"].asInt();
			int y = dict["y"].asInt();

			this->Enemies::addTurretAtPos(Point(x, y));
		}
	}
}

void Enemies::spawnerTimer() {

	const float TIMER_SPAWN_ENEMY = 5.0f;

	auto delay = DelayTime::create(TIMER_SPAWN_ENEMY);

	auto callback = CallFunc::create([=]() {

		this->Enemies::spawner();
		this->stopAllActions();
	});

	auto sequence = Sequence::createWithTwoActions(delay, callback);

	this->runAction(sequence);
}

void Enemies::spawner() {

	auto winSize = Director::getInstance()->getWinSize();

	Point charPos = character->getPosition();

	for (Point spawn : _spawnCoord) {

		if (spawn.x > charPos.x - winSize.width / 2 &&
			spawn.y > charPos.y - winSize.height / 2 &&
			spawn.x < charPos.x + winSize.width / 2 &&
			spawn.y < charPos.y + winSize.height / 2)
		{
			this->Enemies::addEnemyAtPos(spawn);
		}
	}
}

void Enemies::projCollisionEnemy() {

	cocos2d::Vector<Sprite*> projectilesToDelete;

	for (Sprite *projectile : *_projectiles) {

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

				if (target->getName() == "turret" && *ammoplasmagun == 0) {

					break;
				}
				else { targetsToDelete.pushBack(target); }

				if (*ammoplasmagun > 0) {

					Enemies::projExplosion(projectile);
				}
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

		_projectiles->eraseObject(projectile);
		map->removeChild(projectile);
	}
	projectilesToDelete.clear();
}

void Enemies::enemyCollisionCharacter() {

	auto characterRect = Rect(
		character->getPositionX() - character->getContentSize().width / 2,
		character->getPositionY() - character->getContentSize().height / 2,
		character->getContentSize().width,
		character->getContentSize().height);

	for (Sprite *target : _enemies) {
		
		auto targetRect = Rect(
			target->getPositionX() - target->getContentSize().width / 2,
			target->getPositionY() - target->getContentSize().height / 2,
			target->getContentSize().width,
			target->getContentSize().height);

		if (targetRect.intersectsRect(characterRect)) {
			
			this->schedule(schedule_selector(Enemies::delayAttackEnemy), 1);
		}
	}
}

void Enemies::delayAttackEnemy(float dt) {

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/punch.mp3");

	*hp -= 5;
	unschedule(schedule_selector(Enemies::delayAttackEnemy));
}

void Enemies::projExplosion(Sprite *projectile) {

	const float EXPLOSION_SPEED = 0.08f;
	const char *EXPLOSION_NAME = "explosion";

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

	Vector<SpriteFrame*> animFrames(2);

	char str[100] = { 0 };

	for (int i = 0; i <= 1; i++)
	{
		sprintf(str, "explosion%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.4f);

	explosion = Sprite::createWithSpriteFrameName("explosion0.png");

	map->addChild(explosion, 15, EXPLOSION_NAME);

	explosion->setPosition(projectile->getPosition());

	auto animate = Animate::create(animation);
	auto delay = DelayTime::create(EXPLOSION_SPEED);

	auto callback = CallFunc::create([=]() {

		map->removeChildByName(EXPLOSION_NAME);
	});

	auto sequence = Sequence::create(animate, delay, callback, nullptr);

	explosion->runAction(sequence);
}

double Enemies::slowDownEnemy(Sprite *enemy) {

	Point position = enemy->getPosition();
	Point tileCoord = this->Enemies::tileCoordForPosition(position);

	int tileGidWall = wall->getTileGIDAt(tileCoord);

	if (tileGidWall) {

		auto properties = map->getPropertiesForGID(tileGidWall).asValueMap();

		if (!properties.empty()) {

			auto collision = properties["Collidable"].asString();

			if ("true" == collision) {

				return 0.8;
			}
		}
	}
	return 0.3;
}

	Point Enemies::tileCoordForPosition(Point position) {

	int x = position.x / map->getTileSize().width;
	int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;

	return Point(x, y);
	}