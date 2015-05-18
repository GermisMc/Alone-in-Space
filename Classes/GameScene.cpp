#include "GameScene.h"
#include "SimpleaudioEngine.h"

USING_NS_CC;

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
	
	// Preload sound effects and music
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/Running.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/OpeningDoor.mp3");

	// Create character
	character = Sprite::create("characters/backward0.png");

	// Create tile map
	map = TMXTiledMap::create("map/map.tmx");
	this->addChild(map);
	map->addChild(character, 10);

	wall = map->getLayer("wall");
	openDoor = map->getLayer("open_door");
	openDoor->setLocalZOrder(20);

	// Create objects 
	objectGroup = map->getObjectGroup("Objects");

	// Init object Enemies
	enemies = new Enemies(character, map, objectGroup, wall, &_projectiles);
	enemies->spawnEnemy();

	// Init Collisions
	collisions = new Collisions(map, character, wall);
	GameScene::addChild(collisions);

	// Init Animations
	anim = new Animations(character);

	// Init Projectiles
	projectile = new Projectile(character, map, &_projectiles);

	auto spawnPoint = objectGroup->getObject("SpawnPoint");

	int x = spawnPoint["x"].asInt();
	int y = spawnPoint["y"].asInt();
	
	character->setPosition(x + map->getTileSize().width / 2, y + map->getTileSize().height / 2);

	// Init touch
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch *touch, Event *event)->bool {return true; };
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	GameScene::keyboardSupport();

	this->scheduleUpdate();

	return true;
}

void GameScene::update(float dt) {

	GameScene::setViewPointCenter(character->getPosition());
	enemies->enemyOnScreen();
	enemies->testCollisions();
	//enemies->stopEnemyAtBlock();
}

void GameScene::updtMoving(float dt) {

	GameScene::centerProcessingMove(currentKey, dt);
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

void GameScene::centerProcessingMove(EventKeyboard::KeyCode keyCode, float dt) {

	const int SPEED = 100;

	Point nextCoord;
	Point position = character->getPosition();

	bool isNotCrossBorder;
	bool isNotCollide;

	switch (keyCode) {

	case EventKeyboard::KeyCode::KEY_A:

		nextCoord.x = position.x - SPEED * dt;
		nextCoord.y = position.y;

		isNotCrossBorder = collisions->checkBorder(nextCoord);
		isNotCollide = collisions->collision(position, EventKeyboard::KeyCode::KEY_A);
		break;

	case EventKeyboard::KeyCode::KEY_D:

		nextCoord.x = position.x + SPEED * dt;
		nextCoord.y = position.y;

		isNotCrossBorder = collisions->checkBorder(nextCoord);
		isNotCollide = collisions->collision(position, EventKeyboard::KeyCode::KEY_D);
		break;

	case EventKeyboard::KeyCode::KEY_W:

		nextCoord.x = position.x;
		nextCoord.y = position.y + SPEED * dt;

		isNotCrossBorder = collisions->checkBorder(nextCoord);
		isNotCollide = collisions->collision(position, EventKeyboard::KeyCode::KEY_W);
		break;

	case EventKeyboard::KeyCode::KEY_S:

		nextCoord.x = position.x;
		nextCoord.y = position.y - SPEED * dt;

		isNotCrossBorder = collisions->checkBorder(nextCoord);
		isNotCollide = collisions->collision(position, EventKeyboard::KeyCode::KEY_S);
		break;
	}

	if (isNotCrossBorder && isNotCollide) {

		character->setPosition(nextCoord);

	} else {

		character->stopAllActions();
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);
	}
}

void GameScene::keyboardSupport() {

	running = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/Running.mp3", true);
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event){
	
		switch (keyCode) {

		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:

			anim->movAnim("left%d.png", 3);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			currentKey = EventKeyboard::KeyCode::KEY_A;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break; 
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:

			anim->movAnim("right%d.png", 3);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			currentKey = EventKeyboard::KeyCode::KEY_D;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:

			anim->movAnim("backward%d.png", 2);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			currentKey = EventKeyboard::KeyCode::KEY_W;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:

			anim->movAnim("forward%d.png", 3);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			currentKey = EventKeyboard::KeyCode::KEY_S;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break;
		}
	};

	eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){

		switch (keyCode) {

		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:

			character->stopAllActions();

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			this->unschedule(schedule_selector(GameScene::updtMoving));

			character->setTexture("characters/left0.png");

			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:

			character->stopAllActions();

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			this->unschedule(schedule_selector(GameScene::updtMoving));

			character->setTexture("characters/right0.png");

			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:

			character->stopAllActions();

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			this->unschedule(schedule_selector(GameScene::updtMoving));

			character->setTexture("characters/backward0.png");

			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:

			character->stopAllActions();

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			this->unschedule(schedule_selector(GameScene::updtMoving));

			character->setTexture("characters/forward0.png");

			break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, character);
}

void GameScene::onTouchEnded(Touch *touch, Event *event) {
	
	projectile->projectileLogic(touch);
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
