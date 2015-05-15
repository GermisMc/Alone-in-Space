#include "Collisions.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h" 

Collisions::Collisions(TMXTiledMap *map, Sprite *character, TMXLayer *wall) {

	this->map = map;
	this->character = character;
	this->wall = wall;
	this->closedDoor = closedDoor;
	this->wall = wall;

	closedDoor = map->getLayer("closed_door");
}

bool Collisions::checkBorder(Point position) {

	if ((position.x < (map->getMapSize().width * map->getTileSize().width) - 12 &&
		position.y < (map->getMapSize().height * map->getTileSize().height) - 12 &&
		position.y > 18 &&
		position.x > 12))
	{
		return true;
		//character->setPosition((map->getMapSize().width * map->getTileSize().width) - playerPos.x, (map->getMapSize().height * map->getTileSize().height) - playerPos.y);
	}
	else {
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
			}
			else {
				return true;
			}
		}
	}
	Collisions::openDoors(tileCoord);

	return true;
}

void Collisions::openDoors(Point tileCoord) {

	int tileGidCloseDoor = closedDoor->getTileGIDAt(tileCoord);

	if (tileGidCloseDoor) {
		auto properties = map->getPropertiesForGID(tileGidCloseDoor).asValueMap();
		if (!properties.empty()) {
			auto opening = properties["opening"].asString();
			if ("true" == opening) {

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/OpeningDoor.mp3");
				closedDoor->removeTileAt(tileCoord);

				auto delay = DelayTime::create(5);
				auto callback = CallFunc::create([=]() {
					closedDoor->setTileGID(tileGidCloseDoor, tileCoord);
					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/OpeningDoor.mp3");
				});

				auto sequence = Sequence::createWithTwoActions(delay, callback);
				this->runAction(sequence);
			}
		}
	}
}