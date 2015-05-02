#include "Game.h"
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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Preload sound effect and music
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/Running.mp3");

	// Create character
	character = Sprite::create("characters/backward0.png");

	// Create tile map
	map = TMXTiledMap::create("map/map.tmx");
	wall = map->getLayer("wall");
	openDoor = map->getLayer("open_door");
	openDoor->setLocalZOrder(20);
	closedDoor = map->getLayer("closed_door");

	this->addChild(map);
	map->addChild(character, 10);

	// Create objects 
	TMXObjectGroup *objectGroup = map->getObjectGroup("Objects");

	for (auto& eSpawnPoint : objectGroup->getObjects()) {
		ValueMap& dict = eSpawnPoint.asValueMap();
		if (dict["Enemy"].asInt() == 1) {
			int x = dict["x"].asInt();
			int y = dict["y"].asInt();
			this->addEnemyAtPos(Point(x, y));
		}
	}

	auto spawnPoint = objectGroup->getObject("SpawnPoint");

	int x = spawnPoint["x"].asInt();
	int y = spawnPoint["y"].asInt();
	
	character->setPosition(x + map->getTileSize().width / 2, y + map->getTileSize().height / 2);

	this->scheduleUpdate();

	GameScene::keyboardSupport();

	return true;
}

void GameScene::update(float dt) {

	GameScene::setViewPointCenter(character->getPosition());
}

void GameScene::updtMoving(float dt) {

	GameScene::centerProcessingMove(currentKey, dt);
}

void GameScene::addEnemyAtPos(Point position) {

	auto enemy = Sprite::create("characters/enemy2.png");
	enemy->setPosition(position);
	this->animateEnemy(enemy);
	map->addChild(enemy, 10);
}

void GameScene::enemyMoveFinished(Node *pSender) {

	Sprite *enemy = (Sprite *)pSender;
	this->animateEnemy(enemy);
}

void GameScene::animateEnemy(Sprite *enemy) {

	auto actionTo1 = RotateTo::create(0, 0, 180);
	auto actionTo2 = RotateTo::create(0, 0, 0);
	auto diff = Point(character->getPosition() - enemy->getPosition());
	if (diff.x < 0) {
		enemy->runAction(actionTo2);
	}
	if (diff.x > 0) {
		enemy->runAction(actionTo1);
	}

	float actualDuration = 0.3f;
	auto position = (character->getPosition() - enemy->getPosition());
	position.normalize();
	auto actionMove = JumpBy::create(actualDuration, position.operator*(10), 10, 1);
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(GameScene::enemyMoveFinished, this));

	enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
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

bool GameScene::safetyCheck(Point position) {

	if ((position.x < (map->getMapSize().width * map->getTileSize().width) - 12 &&
		position.y < (map->getMapSize().height * map->getTileSize().height) - 12 &&
		position.y > 18 &&
		position.x > 12))
	{
		return true;
		//character->setPosition((map->getMapSize().width * map->getTileSize().width) - playerPos.x, (map->getMapSize().height * map->getTileSize().height) - playerPos.y);
	} else {
		return false;
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

void GameScene::centerProcessingMove(EventKeyboard::KeyCode keyCode, float dt) {

	const int speed = 100;

	Point nextCoord;
	Point position = character->getPosition();

	bool isNotCrossBorder;
	bool isNotCollide;

	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_A:
		nextCoord.x = position.x - speed * dt;
		nextCoord.y = position.y;
		isNotCrossBorder = GameScene::safetyCheck(nextCoord);
		isNotCollide = GameScene::collision(position, EventKeyboard::KeyCode::KEY_A);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		nextCoord.x = position.x + speed * dt;
		nextCoord.y = position.y;
		isNotCrossBorder = GameScene::safetyCheck(nextCoord);
		isNotCollide = GameScene::collision(position, EventKeyboard::KeyCode::KEY_D);
		break;
	case EventKeyboard::KeyCode::KEY_W:
		nextCoord.x = position.x;
		nextCoord.y = position.y + speed * dt;
		isNotCrossBorder = GameScene::safetyCheck(nextCoord);
		isNotCollide = GameScene::collision(position, EventKeyboard::KeyCode::KEY_W);
		break;
	case EventKeyboard::KeyCode::KEY_S:
		nextCoord.x = position.x;
		nextCoord.y = position.y - speed * dt;
		isNotCrossBorder = GameScene::safetyCheck(nextCoord);
		isNotCollide = GameScene::collision(position, EventKeyboard::KeyCode::KEY_S);
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
			GameScene::movAnim("left%d.png", "left0.png", 3);
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			currentKey = EventKeyboard::KeyCode::KEY_A;
			this->schedule(schedule_selector(GameScene::updtMoving));
			break; 
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			GameScene::movAnim("right%d.png", "right0.png", 3);
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			currentKey = EventKeyboard::KeyCode::KEY_D;
			this->schedule(schedule_selector(GameScene::updtMoving));
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			GameScene::movAnim("backward%d.png", "backward0.png", 2);
			CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(running);
			currentKey = EventKeyboard::KeyCode::KEY_W;
			this->schedule(schedule_selector(GameScene::updtMoving));
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			GameScene::movAnim("forward%d.png", "forward0.png", 3);
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

Point GameScene::tileCoordForPosition(Point position) {

	int x = position.x / map->getTileSize().width;
	int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;

	return Point(x, y);
}

bool GameScene::collision(Point position, EventKeyboard::KeyCode keyCode) {
	
	Point tileCoord;
	Point charSize = character->getContentSize();

	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_A:
		tileCoord = this->tileCoordForPosition(Vec2(position.x - (charSize.x / 2), position.y));
		break;
	case EventKeyboard::KeyCode::KEY_S:
		tileCoord = this->tileCoordForPosition(Vec2(position.x, position.y - (charSize.y / 2)));
		break;
	case EventKeyboard::KeyCode::KEY_D:
		tileCoord = this->tileCoordForPosition(Vec2(position.x + (charSize.x / 2), position.y));
		break;
	case EventKeyboard::KeyCode::KEY_W:
		tileCoord = this->tileCoordForPosition(Vec2(position.x, position.y + (charSize.y / 2)));
		break;
	}

	int tileGidWall = wall->getTileGIDAt(tileCoord);

	if (tileGidWall) {
		auto properties = map->getPropertiesForGID(tileGidWall).asValueMap();
		if (!properties.empty()) {
			auto collision = properties["Collidable"].asString();
			if ("true" == collision) {
				return false;
			} else {
				return true;
			}
		}
	}

	int tileGidCloseDoor = closedDoor->getTileGIDAt(tileCoord);

	if (tileGidCloseDoor) {
		auto properties = map->getPropertiesForGID(tileGidCloseDoor).asValueMap();
		if (!properties.empty()) {
			auto opening = properties["opening"].asString();
			if ("true" == opening) {
				closedDoor->removeTileAt(tileCoord);
				
				auto delay = DelayTime::create(5);
				auto callback = CallFunc::create([=]() {
					closedDoor->setTileGID(tileGidCloseDoor, tileCoord);
				});

				auto sequence = Sequence::createWithTwoActions(delay, callback);
				this->runAction(sequence);
			}
		}
	}
	return true;
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
