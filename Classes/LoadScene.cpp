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

	auto visibleSize = Director::getInstance()->getVisibleSize();

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

	// Preload textures
	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("characters/chabi.plist");
	cache->addSpriteFramesWithFile("flash/gunflash.plist");
	cache->addSpriteFramesWithFile("guns/guns.plist");
	cache->addSpriteFramesWithFile("projectiles/projectiles.plist");

	auto labelContinue = Label::createWithSystemFont("Press any key to continue", "Arial", 15);

	labelContinue->setPosition(Vec2(logo->getPosition().x, logo->getPosition().y - 200));

	this->addChild(labelContinue);

	// Blinking
	auto actionBlink = RepeatForever::create(Blink::create(4, 3));
	labelContinue->runAction(actionBlink);

	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {

		auto scene = GameScene::createScene();

		Director::getInstance()->replaceScene(scene);
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

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