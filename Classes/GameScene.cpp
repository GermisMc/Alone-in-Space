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

	const int LOCAL_Z_ORDER_CHARACTER = 10;
	const int LOCAL_Z_ORDER_EMOTIONS = 15;
	
	// Default
	ammoplasmagun = 0;
	ammorevolver = 0;
	card = 0;
	hp = 100;
	currentGunTexture = "guns/plasmagun.png";
	priorityGun = plasmagun;
	
	// Create character
	character = Sprite::create("characters/backward0.png");

	// Create tile map
	std::string selectedMap = UserDefault::getInstance()->getStringForKey("selectedMap");

	map = TMXTiledMap::create(selectedMap);
	map->addChild(character, LOCAL_Z_ORDER_CHARACTER);
	this->addChild(map);

	wall = map->getLayer("wall");
	openDoor = map->getLayer("open_door");
	ammo = map->getLayer("ammo");

	openDoor->setLocalZOrder(20);

	// Create objects 
	objectGroup = map->getObjectGroup("Objects");

	// Init Collisions
	collisions = new Collisions(map, character, wall, &ammorevolver, &ammoplasmagun, &hp, &card, ammo);
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
	enemies = new Enemies(character, map, wall, &_projectiles, &ammoplasmagun, &_projTurret, &hp, &priorityGun, ammo);
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
	map->addChild(emotion, LOCAL_Z_ORDER_EMOTIONS);
	GameScene::emotions();

	// Init Arena Scores
	GameScene::ScoreArena();

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

	if (!(isNotCrossBorder && isNotCollide)) {

		character->stopAllActions();
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);
	}
}

void GameScene::keyboardSupport() {

	const int TAG_FORWARD = 8;
	const int TAG_RIGHT = 6;
	const int TAG_BACKWARD = 2;
	const int TAG_LEFT = 4;
	const int TAG_ANIMATE = 15;
	const int FRAME_NUM_ANIMATION = 3;

	running = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/running.mp3", true);
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event){

		auto actionMoveForward = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(0, 80)));
		actionMoveForward->setTag(TAG_FORWARD);

		auto actionMoveRight = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(80, 0)));
		actionMoveRight->setTag(TAG_RIGHT);

		auto actionMoveBackward = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(0, -80)));
		actionMoveBackward->setTag(TAG_BACKWARD);

		auto actionMoveLeft = RepeatForever::create(cocos2d::MoveBy::create(1, cocos2d::Vec2(-80, 0)));
		actionMoveLeft->setTag(TAG_LEFT);

		gun->gunRender(keyCode, currentGunTexture);
	
		switch (keyCode) {

		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:

			anim->movAnim("left%d.png", FRAME_NUM_ANIMATION);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			character->runAction(actionMoveLeft);

			currentKey = EventKeyboard::KeyCode::KEY_A;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break; 
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:

			anim->movAnim("right%d.png", FRAME_NUM_ANIMATION);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			character->runAction(actionMoveRight);

			currentKey = EventKeyboard::KeyCode::KEY_D;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:

			anim->movAnim("backward%d.png", FRAME_NUM_ANIMATION - 1);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			character->runAction(actionMoveForward);

			currentKey = EventKeyboard::KeyCode::KEY_W;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:

			anim->movAnim("forward%d.png", FRAME_NUM_ANIMATION);

			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);

			character->runAction(actionMoveBackward);

			currentKey = EventKeyboard::KeyCode::KEY_S;

			this->schedule(schedule_selector(GameScene::updtMoving));

			break;

		case EventKeyboard::KeyCode::KEY_SPACE:

			if (priorityGun == plasmagun) {

				priorityGun = revolver;

				currentGunTexture = "guns/revolver.png";

			} else {

				priorityGun = plasmagun;

				currentGunTexture = "guns/plasmagun.png";
			}

			break;
		}
	};

	eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){

		switch (keyCode) {

		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			character->stopActionByTag(TAG_ANIMATE);
			character->stopActionByTag(TAG_LEFT);
			character->setTexture("characters/left0.png");

			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			character->stopActionByTag(TAG_ANIMATE);
			character->stopActionByTag(TAG_RIGHT);
			character->setTexture("characters/right0.png");

			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			character->stopActionByTag(TAG_ANIMATE);
			character->stopActionByTag(TAG_FORWARD);
			character->setTexture("characters/backward0.png");

			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:

			CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

			character->stopActionByTag(TAG_ANIMATE);
			character->stopActionByTag(TAG_BACKWARD);
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

	switch (priorityGun) {

	case plasmagun:

		if (ammoplasmagun > 0) {

			gun->gunRender(currentKey, "guns/plasmagun.png");
			gun->flashRender();

			projectile->projectileLogic(touch, "projplasmagun.png", "sounds/plasmagun.mp3");

			currentGunTexture = "guns/plasmagun.png";

			ammoplasmagun--;

		} else { CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/emptyclip.mp3"); }

		break;

	case revolver:

		if (ammorevolver > 0) {

			gun->gunRender(currentKey, "guns/revolver.png");
			gun->flashRender();

			projectile->projectileLogic(touch, "projrevolver.png", "sounds/revolver.mp3");

			currentGunTexture = "guns/revolver.png";

			ammorevolver--;

		} else { CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/emptyclip.mp3"); }

		break;
	}
}

void GameScene::gameOverAction() {

	const int FONT_SIZE_RESTART = 20;
	const int FONT_SIZE_SCORE = 40;

	if (hp <= 0) {

		auto winSize = Director::getInstance()->getWinSize();

		character->stopAllActions();

		CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(running);

		this->unschedule(schedule_selector(GameScene::updtMoving));
		this->unscheduleUpdate();
		this->stopAllActions();
		
		enemies->gameOverEnemy();

		gui->gameOver(&hp, character, &card);

		auto eventListener = EventListenerKeyboard::create();

		eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {

			auto labelRestart = Label::createWithSystemFont("wait to restart...", "Arial", FONT_SIZE_RESTART);

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

		if ("arena" == gameType) {

			int *tmp = &score;

			char str[100] = { 0 };
			sprintf(str, "%d", *tmp);
		
			auto labelScore = Label::createWithSystemFont(str, "Arial", FONT_SIZE_SCORE);

			labelScore->setPosition(Vec2(winSize.width / 2, winSize.height - 100));

			gui->addChild(labelScore);
		}

		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
	}
}

void GameScene::ScoreArena() {

	gameType =  UserDefault::getInstance()->getStringForKey("gameType", "classic");

	if ("arena" == gameType) {
	
		auto scoreCounter = CallFunc::create([&]() {

			score++;
		});

		auto delay = DelayTime::create(1);

		this->runAction(RepeatForever::create(Sequence::createWithTwoActions(scoreCounter, delay)));
	}
}

void GameScene::gameWin() {

	auto gameWin = objectGroup->getObject("gameWin");

	int x = gameWin["x"].asInt();
	int y = gameWin["y"].asInt();
	int width = gameWin["width"].asInt();
	int height = gameWin["height"].asInt();

	auto gameWinRect = Rect(x, y, width, height);

	if ((gameWinRect.containsPoint(character->getPosition())) && (card == 10)) {

		auto scene = GameWin::createScene();

		Director::getInstance()->replaceScene(scene);
	}
}

void GameScene::emotions() {

	const int DELAY_TIME_MIN = 10;
	const int DELAY_TIME_MAX = 60;
	const int FRAME_NUM_EMOTIONS = 6;

	auto showEmotion = CallFunc::create([=]() {

		emotion->setLocalZOrder(15);
	});

	auto removeEmotion = CallFunc::create([=]() {

		emotion->setLocalZOrder(-5);
	});

	auto emotionsDone = CallFunc::create(CC_CALLBACK_0(GameScene::emotionsDone, this));

	srand(time(0));

	auto delay = DelayTime::create(DELAY_TIME_MIN + rand() % DELAY_TIME_MAX);

	char str[100] = { 0 };
	sprintf(str, "emotions/emotion%d.png", rand() % FRAME_NUM_EMOTIONS + 1);

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
