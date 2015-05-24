#include "Collisions.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h" 

Collisions::Collisions(TMXTiledMap *map, Sprite *character, TMXLayer *wall, int *ammorevolver, int *ammoplasmagun) {

	this->map = map;
	this->character = character;
	this->wall = wall;
	this->ammorevolver = ammorevolver;
	this->ammoplasmagun = ammoplasmagun;

	closedDoor = map->getLayer("closed_door");
	ammo = map->getLayer("ammo");
}

bool Collisions::checkBorder(Point position) {

	if ((position.x < (map->getMapSize().width * map->getTileSize().width) - 12 &&
		position.y < (map->getMapSize().height * map->getTileSize().height) - 12 &&
		position.y > 18 &&
		position.x > 12))
	{
		return true;
		// The black hole effect
		//character->setPosition((map->getMapSize().width * map->getTileSize().width) - playerPos.x, (map->getMapSize().height * map->getTileSize().height) - playerPos.y);
	} else {
		return false;
	}
}

Point Collisions::tileCoordForPosition(Point position) {

	int x = position.x / map->getTileSize().width;
	int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;

	return Point(x, y);
}

bool Collisions::collision(Point position, EventKeyboard::KeyCode keyCode) {

	Point tileCoord;
	Point charSize = character->getContentSize();

	switch (keyCode) {

	case EventKeyboard::KeyCode::KEY_A:
		tileCoord = this->tileCoordForPosition(Vec2(position.x - (charSize.x / 2), position.y));
		break;
	case EventKeyboard::KeyCode::KEY_S:
		tileCoord = this->tileCoordForPosition(Vec2(position.x, position.y - (charSize.y / 2)));
		break;
	case EventKeyboard::KeyCode::KEY_D:
		tileCoord = this->tileCoordForPosition(Vec2(position.x + (charSize.x / 2), position.y));
		break;
	case EventKeyboard::KeyCode::KEY_W:
		tileCoord = this->tileCoordForPosition(Vec2(position.x, position.y + (charSize.y / 2)));
		break;
	}

	int tileGidWall = wall->getTileGIDAt(tileCoord);

	if (tileGidWall) {

		auto properties = map->getPropertiesForGID(tileGidWall).asValueMap();

		if (!properties.empty()) {

			auto collision = properties["Collidable"].asString();

			if ("true" == collision) {
				return false;
			} else {
				return true;
			}
		}
	}

	Collisions::openDoors(tileCoord);
	Collisions::pickAmmo(tileCoord);

	return true;
}

void Collisions::openDoors(Point tileCoord) {
	
	const float timerCloseDoor = 5.0f;

	int tileGidCloseDoor = closedDoor->getTileGIDAt(tileCoord);

	if (tileGidCloseDoor) {

		auto properties = map->getPropertiesForGID(tileGidCloseDoor).asValueMap();

		if (!properties.empty()) {

			auto opening = properties["opening"].asString();

			if ("true" == opening) {

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/door.mp3");

				closedDoor->removeTileAt(tileCoord);

				auto delay = DelayTime::create(timerCloseDoor);
				auto callback = CallFunc::create([=]() {

					closedDoor->setTileGID(tileGidCloseDoor, tileCoord);

					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/door.mp3");
				});

				auto sequence = Sequence::createWithTwoActions(delay, callback);

				this->runAction(sequence);
			}
		}
	}
}

void Collisions::pickAmmo(Point tileCoord) {

	int tileGidPickAmmo = ammo->getTileGIDAt(tileCoord);

	if (tileGidPickAmmo) {

		auto properties = map->getPropertiesForGID(tileGidPickAmmo).asValueMap();

		if (!properties.empty()) {

			auto type = properties["type"].asString();

			if ("revolver" == type) {

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/pickupammo.mp3");

				*ammorevolver += 5;

				ammo->removeTileAt(tileCoord);
			}
			else if ("plasmagun" == type) {

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/pickupammo.mp3");

				*ammoplasmagun += 5;

				ammo->removeTileAt(tileCoord);
			}
		}
	}
}

void Collisions::projCollision(cocos2d::Vector<cocos2d::Sprite *> *_projectiles) {

	cocos2d::Vector<Sprite*> projectilesToDelete;

	for (Sprite *projectile : *_projectiles) {

		Point projCoord = this->tileCoordForPosition(projectile->getPosition());

		if (map->getBoundingBox().containsPoint(projectile->getPosition())) {

			int tileGidWall = wall->getTileGIDAt(projCoord);
			int tileGidCloseDoor = closedDoor->getTileGIDAt(projCoord);

			if (tileGidWall || tileGidCloseDoor) {

				projectilesToDelete.pushBack(projectile);
			}
		}
	}

	for (Sprite *projectile : projectilesToDelete) {

		_projectiles->eraseObject(projectile);
		map->removeChild(projectile);
	}
		projectilesToDelete.clear();
}