#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Gui.h"
#include "GameWin.h"
#include <ctime>

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

	// Init Gui scene
	Gui *gui = new Gui();
	gui->init();
	scene->addChild(gui);
	layer->gui = gui;
	
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
	
	// Default
	ammoplasmagun = 0;
	ammorevolver = 0;
	card = 0;
	hp = 100;
	currentGunTexture = "guns/plasmagun.png";
	
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

	// Init Collisions
	collisions = new Collisions(map, character, wall, &ammorevolver, &ammoplasmagun, &hp, &card);
	GameScene::addChild(collisions);

	// Init Animations
	anim = new Animations(character);
	
	// Init Projectiles
	projectile = new Projectile(character, map, &_projectiles, &flash);

	auto spawnPoint = objectGroup->getObject("SpawnPoint");

	int x = spawnPoint["x"].asInt();
	int y = spawnPoint["y"].asInt();
	
	character->setPosition(x + map->getTileSize().width / 2, y + map->getTileSize().height / 2);

	// Init Enemies
	enemies = new Enemies(character, map, wall, &_projectiles, &ammoplasmagun, &_projTurret, &hp);
	GameScene::addChild(enemies);
	enemies->spawnEnemy();
	enemies->turretProjReady();
	
	// Init gun
	gun = new Guns(character, map, &flash);
	GameScene::addChild(gun);
	gun->gunRender(currentKey, "guns/plasmagun.png");

	// Init GameWin scene
	gamewin = new GameWin();

	// Init Emotions
	emotion = Sprite::create("emotions/emotion0.png");
	map->addChild(emotion, 15);
	GameScene::emotions();

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
	GameScene::gameOverAction();
	GameScene::gameWin();

	enemies->enemyOnScreen();
	enemies->projCollisionEnemy();
	enemies->enemyCollisionCharacter();
	enemies->spawnerTimer();
	
	collisions->projCollision(&_projectiles, &_projTurret);

	gui->countGui(&ammorevolver, &ammoplasmagun, &hp, &card);

	emotion->setPosition(character->getPosition().x, character->getPosition().y + 30);
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

	running = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/running.mp3", true);
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event){

		gun->gunRender(keyCode, currentGunTexture);
	
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
	
	GameScene::shots(touch);
}

void GameScene::shots(Touch *touch) {

	if (ammoplasmagun > 0) {

		gun->gunRender(currentKey, "guns/plasmagun.png");
		gun->flashRender();

		projectile->projectileLogic(touch, "projplasmagun.png", "sounds/plasmagun.mp3");

		ammoplasmagun--;
	}
	else if (ammorevolver > 0) {

		gun->gunRender(currentKey, "guns/revolver.png");
		gun->flashRender();

		projectile->projectileLogic(touch, "projrevolver.png", "sounds/revolver.mp3");

		ammorevolver--;
	}
	else { CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/emptyclip.mp3"); }
}

void GameScene::gameOverAction() {

	if (hp <= 0) {

		auto winSize = Director::getInstance()->getWinSize();

		character->stopAllActions();

		CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

		this->unschedule(schedule_selector(GameScene::updtMoving));
		this->unscheduleUpdate();

		enemies->gameOverEnemy();

		gui->gameOver(&hp, character, &card);

		auto eventListener = EventListenerKeyboard::create();

		eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {

			auto labelRestart = Label::createWithSystemFont("wait to restart...", "Arial", 20);

			labelRestart->setPosition(Vec2(winSize.width / 2, winSize.height / 6));

			gui->addChild(labelRestart);

			auto delay = DelayTime::create(2.5f);
			auto callback = CallFunc::create([=]() {

				auto scene = GameScene::createScene();

				Director::getInstance()->replaceScene(scene);
			});

			auto sequence = Sequence::createWithTwoActions(delay, callback);

			this->runAction(sequence);
		};

		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
	}
}

void GameScene::gameWin() {

	auto gameWin = objectGroup->getObject("gameWin");

	int x = gameWin["x"].asInt();
	int y = gameWin["y"].asInt();
	int width = gameWin["width"].asInt();
	int height = gameWin["height"].asInt();

	auto gameWinRect = Rect(x, y, width, height);

	if ((gameWinRect.containsPoint(character->getPosition())) && (card = 10)) {
		
		auto scene = GameWin::createScene();
		gamewin->resultCards(&card);
		Director::getInstance()->replaceScene(scene);
		gamewin->resultCards(&card);
	}
}

void GameScene::emotions() {

	auto showEmotion = CallFunc::create([=]() {

		
		emotion->setLocalZOrder(15);
		
		
	});

	auto removeEmotion = CallFunc::create([=]() {


		emotion->setLocalZOrder(-5);


	});

	auto emotionsDone = CallFunc::create(CC_CALLBACK_0(GameScene::emotionsDone, this));

	srand(time(0));

	auto delay = DelayTime::create(10 + rand() % 60);

	char str[100] = { 0 };
	sprintf(str, "emotions/emotion%d.png", rand() % 6);

	emotion->setTexture(str);

	this->runAction(Sequence::create(removeEmotion, delay, showEmotion, delay, emotionsDone, nullptr));
}

void GameScene::emotionsDone() {

	this->emotions();
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
