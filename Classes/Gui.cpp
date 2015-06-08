#include "Gui.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h" 

USING_NS_CC;

Scene* Gui::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Gui::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Gui::init() {

	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	// Create gui for revolver ammo
	guiAmmoRevolver = Sprite::createWithSpriteFrameName("revolver0.png");

	auto garSize = guiAmmoRevolver->getContentSize();

	this->addChild(guiAmmoRevolver);

	guiAmmoRevolver->setPosition(garSize.width + 10, garSize.height + 10);
	guiAmmoRevolver->setScale(2);
	guiAmmoRevolver->setOpacity(200);

	// Create gui for plasmagun ammo
	guiAmmoPlasmagun = Sprite::create("gui/plasmagun.png");

	this->addChild(guiAmmoPlasmagun);

	auto revolverPos = guiAmmoRevolver->getPosition();

	guiAmmoPlasmagun->setPosition(revolverPos.x + 50, revolverPos.y);
	guiAmmoPlasmagun->setScale(2);
	guiAmmoPlasmagun->setOpacity(200);

	// Create gui for health
	health = Sprite::create("gui/health.png");

	this->addChild(health);

	auto plasmagunPos = guiAmmoPlasmagun->getPosition();

	health->setPosition(plasmagunPos.x + 50, plasmagunPos.y);
	health->setScale(0.2f);
	health->setOpacity(200);

	// Create gui for cards
	guiCard = Sprite::create("gui/card.png");

	this->addChild(guiCard);

	auto healthPos = health->getPosition();

	guiCard->setPosition(healthPos.x + 80, healthPos.y);
	guiCard->setOpacity(200);

	return true;
}

void Gui::countGui(int *ammorevolver, int *ammoplasmagun, int *hp, int *card) {

	const float GUI_FONT_SIZE = 15;

	// Create count revolver label
	this->removeChild(guiAmmoRevolver);

	if (*ammorevolver > 7) {

		guiAmmoRevolver = Sprite::createWithSpriteFrameName("revolver7.png");

		auto garSize = guiAmmoRevolver->getContentSize();

		this->addChild(guiAmmoRevolver);

		guiAmmoRevolver->setPosition(garSize.width + 10, garSize.height + 10);
		guiAmmoRevolver->setScale(2);
		guiAmmoRevolver->setOpacity(200);

	} else {

		char str[100] = { 0 };

		sprintf(str, "revolver%d.png", *ammorevolver);

		guiAmmoRevolver = Sprite::createWithSpriteFrameName(str);

		auto garSize = guiAmmoRevolver->getContentSize();

		this->addChild(guiAmmoRevolver);

		guiAmmoRevolver->setPosition(garSize.width + 10, garSize.height + 10);
		guiAmmoRevolver->setScale(2);
		guiAmmoRevolver->setOpacity(200);
	}

	// Create count plasmagun label
	this->removeChild(countPlasmaGun);

	char strPlasma[100] = { 0 };

	sprintf(strPlasma, "%d", *ammoplasmagun);

	countPlasmaGun = Label::createWithSystemFont(strPlasma, "Arial", GUI_FONT_SIZE);

	countPlasmaGun->setPosition(guiAmmoPlasmagun->getPosition().x + 20, guiAmmoPlasmagun->getPosition().y);

	this->addChild(countPlasmaGun);

	// Create count health label
	this->removeChild(countHealth);

	char strHealth[100] = { 0 };

	sprintf(strHealth, "%d%%", *hp);

	countHealth = Label::createWithSystemFont(strHealth, "Arial", GUI_FONT_SIZE);

	countHealth->setPosition(health->getPosition().x + 40, health->getPosition().y);

	this->addChild(countHealth);

	// Create count cards label
	this->removeChild(countCard);

	char strCard[100] = { 0 };

	sprintf(strCard, "%d/10", *card);

	countCard = Label::createWithSystemFont(strCard, "Arial", GUI_FONT_SIZE);

	countCard->setPosition(guiCard->getPosition().x + 40, guiCard->getPosition().y);

	this->addChild(countCard);
}

void Gui::gameOver(int *hp, Sprite *character, int *card) {

	const float RGBA_RED = 192;
	const float RGBA_GREEN = 192;
	const float RGBA_BLUE = 192;
	const float RGBA_ALPHA = 0.04f;

	const int ACTION_TIME_DRAW = 25;
	const int FONT_SIZE_WASTED = 100;
	const int OUTLINE_SIZE_WASTED = 5;

	auto visibleSize = Director::getInstance()->getWinSize();

	_eventDispatcher->removeAllEventListeners();

	character->setTexture("characters/dead.png");
	character->setLocalZOrder(20);

	auto delayDraw = DelayTime::create(0.1f);
	auto callbackDraw = CallFunc::create([=]() {

		blackout = DrawNode::create();

		blackout->drawSolidRect(Vec2(0, 0), Vec2(visibleSize.width, visibleSize.height), Color4F(RGBA_RED, RGBA_GREEN, RGBA_BLUE, RGBA_ALPHA));

		addChild(blackout);
	});

	auto sequenceDraw = Sequence::createWithTwoActions(delayDraw, callbackDraw);

	this->runAction(Repeat::create(sequenceDraw, ACTION_TIME_DRAW));

	auto delayWasted = DelayTime::create(2.5f);
	auto callbackWasted = CallFunc::create([=]() {

		wasted = Label::createWithTTF("WASTED", "fonts/pricedown bl.ttf", FONT_SIZE_WASTED);

		wasted->enableOutline(Color4B::BLACK, OUTLINE_SIZE_WASTED);

		wasted->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 1.5));

		this->addChild(wasted);
	});

	auto sequenceWasted = Sequence::createWithTwoActions(delayWasted, callbackWasted);

	this->runAction(sequenceWasted);

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/wasted.mp3");
}

void Gui::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}