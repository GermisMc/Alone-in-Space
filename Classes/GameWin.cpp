#include "GameWin.h"
#include "cocos2d.h" 
#include "SimpleAudioEngine.h" 

USING_NS_CC;

Scene* GameWin::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameWin::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameWin::init() {

	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();

	//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/final.mp3", true);

	auto visibleSize = Director::getInstance()->getWinSize();

	background = Sprite::create("logo/background.png");

	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	this->addChild(background);

	cards = Sprite::create("gui/card.png");

	cards->setPosition(visibleSize.width / 2, visibleSize.height - 50);

	this->addChild(cards);

	labelCards = Label::createWithSystemFont("0", "Arial", 15);

	labelCards->setPosition(cards->getPosition().x + 20, cards->getPosition().y);

	this->addChild(labelCards);

	return true;
}

void GameWin::resultCards(int *card) {

	auto visibleSize = Director::getInstance()->getWinSize();

	char str[100] = { 0 };

	sprintf(str, "%d", *card);
}

void GameWin::menuCloseCallback(Ref* pSender)
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