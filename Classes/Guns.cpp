#include "Guns.h"
#include "cocos2d.h"

Guns::Guns(Sprite *character, TMXTiledMap *map, Sprite **flash) {
	
	this->character = character;
	this->map = map;
	this->flash = flash;
}

void Guns::gunRender(EventKeyboard::KeyCode currentKey, char *guntexture) {

	const int LOCAL_Z_ORDER_GUN = 5;
	const int GUN_POS_X = -5;
	const int GUN_POS_Y = 15;

	this->currentKey = currentKey;

	auto actionTo1 = RotateTo::create(0, 0, 180);
	auto actionTo2 = RotateTo::create(0, 0, 0);

	gun = Sprite::create(guntexture);

	character->addChild(gun, -LOCAL_Z_ORDER_GUN);

	gun->setPosition(GUN_POS_X, GUN_POS_Y);

	switch (currentKey) {

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:

		character->removeAllChildren();

		gun->runAction(actionTo2);

		character->addChild(gun, LOCAL_Z_ORDER_GUN);

		break;

	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:

		character->removeAllChildren();

		gun->runAction(actionTo1);
		gun->setPosition(GUN_POS_X + 35, GUN_POS_Y);

		character->addChild(gun, LOCAL_Z_ORDER_GUN);

		break;

	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_W:

		character->removeAllChildren();

		gun->runAction(actionTo2);
		gun->setPosition(GUN_POS_X, GUN_POS_Y);

		character->addChild(gun, -LOCAL_Z_ORDER_GUN);

		break;

	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_S:

		character->removeAllChildren();

		gun->runAction(actionTo1);
		gun->setPosition(GUN_POS_X + 35, GUN_POS_Y);

		character->addChild(gun, LOCAL_Z_ORDER_GUN);

		break;
	}
}

void Guns::flashRender() {

	const float FLASH_SPEED = 0.08f;
	const float DELAY_ANIMATION = 0.2f;

	const char *FLASH_NAME = "flash";

	const int LOCAL_Z_ORDER_FLASH = 15;

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

	Vector<SpriteFrame*> animFrames(2);

	char str[100] = { 0 };

	for (int i = 0; i <= 1; i++)
	{
		sprintf(str, "gunflash%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, DELAY_ANIMATION);

	*flash = Sprite::createWithSpriteFrameName("gunflash0.png");

	map->addChild(*flash, LOCAL_Z_ORDER_FLASH, FLASH_NAME);

	(*flash)->setPosition(Vec2(character->getPosition().x - gun->getContentSize().width - 5, character->getPosition().y));

	switch (currentKey) {

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:

		(*flash)->setPosition(Vec2(character->getPosition().x - gun->getContentSize().width - 5, character->getPosition().y));

		break;

	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:

		(*flash)->setPosition(Vec2(character->getPosition().x + gun->getContentSize().width + 5, character->getPosition().y));

		break;

	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_W:

		(*flash)->setPosition(Vec2(character->getPosition().x - gun->getContentSize().width - 5, character->getPosition().y));

		break;

	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_S:

		(*flash)->setPosition(Vec2(character->getPosition().x + gun->getContentSize().width + 5, character->getPosition().y));

		break;
	}

	(*flash)->runAction(Animate::create(animation));

	// Remove flash
	auto delay = DelayTime::create(FLASH_SPEED);

	auto callback = CallFunc::create([=]() {

		map->removeChildByName(FLASH_NAME);
	});

	auto sequence = Sequence::createWithTwoActions(delay, callback);

	this->runAction(sequence);
}
