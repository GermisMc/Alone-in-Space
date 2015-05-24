#include "Guns.h"
#include "cocos2d.h"

Guns::Guns(Sprite *character, TMXTiledMap *map, Sprite **flash) {
	
	this->character = character;
	this->map = map;
	this->flash = flash;
}

void Guns::gunRender(EventKeyboard::KeyCode currentKey, char *guntexture) {

	this->currentKey = currentKey;

	auto actionTo1 = RotateTo::create(0, 0, 180);
	auto actionTo2 = RotateTo::create(0, 0, 0);

	gun = Sprite::createWithSpriteFrameName(guntexture);
	character->addChild(gun, -5);
	gun->setPosition(-5, 15);

	switch (currentKey) {

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:

		character->removeAllChildren();

		gun->runAction(actionTo2);

		character->addChild(gun, 5);

		break;

	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:

		character->removeAllChildren();

		gun->runAction(actionTo1);
		gun->setPosition(30, 15);

		character->addChild(gun, 5);

		break;

	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_W:

		character->removeAllChildren();

		gun->runAction(actionTo2);
		gun->setPosition(-5, 15);

		character->addChild(gun, -5);

		break;

	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_S:

		character->removeAllChildren();

		gun->runAction(actionTo1);
		gun->setPosition(30, 15);

		character->addChild(gun, 5);

		break;
	}
}

void Guns::flashRender() {

	const float flashSpeed = 0.08f;

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

	Vector<SpriteFrame*> animFrames(2);

	char str[100] = { 0 };

	for (int i = 0; i <= 1; i++)
	{
		sprintf(str, "gunflash%d.png", i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.2f);

	*flash = Sprite::createWithSpriteFrameName("gunflash0.png");

	map->addChild(*flash, 15);

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
	auto delay = DelayTime::create(flashSpeed);

	auto callback = CallFunc::create([=]() {

		map->removeChild(*flash);
	});

	auto sequence = Sequence::createWithTwoActions(delay, callback);

	this->runAction(sequence);
}
