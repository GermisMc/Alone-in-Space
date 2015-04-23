#include "Game.h"
#include "SimpleaudioEngine.h"

USING_NS_CC;
using namespace cocos2d;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Preload sound effect and music
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/Running.mp3");

	// Create character
	character = Sprite::create("characters/backward0.png");

	// Create tile map
	map = TMXTiledMap::create("map/map.tmx");
	floor = map->getLayer("floor");
	wall = map->getLayer("wall");

	this->addChild(map);

	TMXObjectGroup *objectGroup = map->getObjectGroup("Objects");

	CCASSERT(NULL != objectGroup, "'Objects' object group not found");

	auto spawnPoint = objectGroup->getObject("SpawnPoint");

	int x = spawnPoint["x"].asInt();
	int y = spawnPoint["y"].asInt();
	
	character->setPosition(x + map->getTileSize().width / 2, y + map->getTileSize().height / 2);
	addChild(character);

	this->scheduleUpdate();

	GameScene::keyboardSupport();
	
	return true;
}

void GameScene::update(float dt) {

	GameScene::setViewPointCenter(character->getPosition());
	GameScene::safetyCheck();
	GameScene::setPlayerPosition(character->getPosition());
}

void GameScene::setViewPointCenter(Point position) {

	auto winSize = Director::getInstance()->getWinSize();

	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	
	x = MIN(x, (map->getMapSize().width * map->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (map->getMapSize().height * map->getTileSize().height) - winSize.height / 2);
	auto actualPosition = Point(x, y);

	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	auto viewPoint = centerOfView - actualPosition;
	this->setPosition(viewPoint);
}

void GameScene::safetyCheck() {

	Point playerPos = character->getPosition();

	if (!(playerPos.x <= (map->getMapSize().width * map->getTileSize().width) &&
		playerPos.y <= (map->getMapSize().height * map->getTileSize().height) &&
		playerPos.y >= 0 &&
		playerPos.x >= 0))
	{
		//character->setPosition((map->getMapSize().width * map->getTileSize().width) - playerPos.x, (map->getMapSize().height * map->getTileSize().height) - playerPos.y);
		character->stopAllActions;
	}
}

void GameScene::movAnim(char *anim_direct, char *start_anim, int frames_num)  {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	SpriteBatchNode* spritebatch = SpriteBatchNode::create("characters/chabi.png");

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("characters/chabi.plist");

	// Create a new sprite
	/*character = Sprite::createWithSpriteFrameName(start_anim);
	spritebatch->addChild(character);
	addChild(spritebatch);*/

	Vector<SpriteFrame*> animFrames(15);

	char str[100] = { 0 };
	for (int i = 0; i <= frames_num; i++)
	{
		sprintf(str, anim_direct, i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
	character->runAction(RepeatForever::create(Animate::create(animation)));

	//character->setPosition(Point((visibleSize.width / 2) + origin.x, (visibleSize.height / 2) + origin.y));
}

void GameScene::keyboardSupport() {

	const int actionMoveForwardId = 8;
	const int actionMoveRightId = 6;
	const int actionMoveBackwardId = 2;
	const int actionMoveLeftId = 4;

	int running = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/Running.mp3", true);
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event){

		Vec2 loc = event->getCurrentTarget()->getPosition();
		

		auto actionMoveForward = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(0, 80)));
		actionMoveForward->setTag(actionMoveForwardId);
		
		auto actionMoveRight = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(80, 0)));
		actionMoveRight->setTag(actionMoveRightId); 

		auto actionMoveBackward = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(0, -80)));
		actionMoveBackward->setTag(actionMoveBackwardId);

		auto actionMoveLeft = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(-80, 0)));
		actionMoveLeft->setTag(actionMoveLeftId);
	
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			character->runAction(actionMoveLeft);
			GameScene::movAnim("left%d.png", "left0.png", 3);
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			break; 
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			character->runAction(actionMoveRight);
			GameScene::movAnim("right%d.png", "right0.png", 3);
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			character->runAction(actionMoveForward);
			GameScene::movAnim("backward%d.png", "backward0.png", 2);
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			character->runAction(actionMoveBackward);
			GameScene::movAnim("forward%d.png", "forward0.png", 3 );
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			break;
		}
	};

	eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){
		Vec2 loc = event->getCurrentTarget()->getPosition();

		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			character->stopAllActions();
			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);
			character->setTexture("characters/left0.png");
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			character->stopAllActions();
			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);
			character->setTexture("characters/right0.png");
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			character->stopAllActions();
			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);
			character->setTexture("characters/backward0.png");
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			character->stopAllActions();
			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);
			character->setTexture("characters/forward0.png");
			break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, character);
}

Point GameScene::tileCoordForPosition(Point position) {

	int x = position.x / map->getTileSize().width;
	int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;

	return Point(x, y);
}

void GameScene::setPlayerPosition(Point position) {
	
	Point tileCoord = this->tileCoordForPosition(position);
	int tileGid = wall->tileGIDAt(tileCoord);

	if (tileGid) {
		auto properties = map->getPropertiesForGID(tileGid).asValueMap();
		if (!properties.empty()) {
			auto collision = properties["Collidable"].asString();
			if ("true" == collision) {
				character->setPosition(500,500);
			}
		}
	}
	character->setPosition(position);
}

void GameScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
