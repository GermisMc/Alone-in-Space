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

	const int FONT_SIZE_THANKS = 20;
	const int FONT_SIZE_COPYRIGHT = 15;
	const int COPYRIGHT_POS_X = 15;
	const int COPYRIGHT_POS_Y = 15;

	CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/acdc.mp3", true);

	auto visibleSize = Director::getInstance()->getWinSize();

	background = Sprite::create("logo/background.png");

	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	this->addChild(background);

	labelThanks = Label::createWithSystemFont("Thank you for a well played game!", "Arial", FONT_SIZE_THANKS);
	copyRight = Label::createWithSystemFont("Author Nikita Klimenko \n2015 All rights reserved.", "Arial", FONT_SIZE_COPYRIGHT);

	copyRight->setAnchorPoint(Vec2(0, 0));

	labelThanks->setPosition(visibleSize.width / 2, visibleSize.height - 45);
	copyRight->setPosition(COPYRIGHT_POS_X, COPYRIGHT_POS_Y);

	this->addChild(labelThanks);
	this->addChild(copyRight);

	return true;
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