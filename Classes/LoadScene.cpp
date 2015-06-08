#include "LoadScene.h"
#include "cocos2d.h"
#include "GameScene.h" 
#include "SimpleAudioEngine.h" 

USING_NS_CC;

Scene* LoadScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = LoadScene::create();
	
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool LoadScene::init() {

	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	const int FONT_SIZE_CLASSIC = 25;
	const int CLASSIC_POS_X = 300;

	auto visibleSize = Director::getInstance()->getWinSize();

	logo = Sprite::create("logo/logo.png");
	logo->setScale(0.2f);
	logo->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	this->addChild(logo);

	// Preload sounds
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/running.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/door.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/revolver.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/plasmagun.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/emptyclip.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/pickupammo.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/punch.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/explosionturret.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/heal.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/wasted.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/acdc.mp3");

	// Preload textures
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("characters/chabi.plist");
	cache->addSpriteFramesWithFile("flash/gunflash.plist");
	cache->addSpriteFramesWithFile("projectiles/projectiles.plist");
	cache->addSpriteFramesWithFile("effects/plasma.plist");
	cache->addSpriteFramesWithFile("gui/revolver.plist");
	cache->addSpriteFramesWithFile("enemies/enemies.plist");

	classic = Label::createWithSystemFont("Classic mode", "Arial", FONT_SIZE_CLASSIC);

	classic->setPosition(CLASSIC_POS_X, visibleSize.height / 4);


	arena = Label::createWithSystemFont("Arena", "Arial", 25);

	arena->setPosition(visibleSize.width - 300, visibleSize.height / 4);

	this->addChild(classic);
	this->addChild(arena);
	

	auto selectGame = EventListenerTouchOneByOne::create();

	selectGame->onTouchBegan = [=](Touch *touch, Event *event) -> bool {

		if (classic->getBoundingBox().containsPoint(touch->getLocation())) {

			UserDefault::getInstance()->setStringForKey("selectedMap", "map/map.tmx");
			UserDefault::getInstance()->setStringForKey("gameType", "classic");

			auto game = GameScene::createScene();
			Director::getInstance()->replaceScene(game);

			return true;
		}
		if (arena->getBoundingBox().containsPoint(touch->getLocation())) {

			UserDefault::getInstance()->setStringForKey("selectedMap", "map/backup.tmx");
			UserDefault::getInstance()->setStringForKey("gameType", "arena");

			auto game = GameScene::createScene();
			Director::getInstance()->replaceScene(game);

			return true;
		}
		return false;
	};

	selectGame->setSwallowTouches(true);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(selectGame, this);
	
	return true;
}

void LoadScene::menuCloseCallback(Ref* pSender)
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