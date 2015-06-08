#ifndef __GameScene_H__
#define __GameScene_H__

#include "cocos2d.h"
#include "Enemies.h"
#include "Collisions.h"
#include "Animations.h"
#include "Projectile.h"
#include "Guns.h"
#include "Gui.h"
#include "GameWin.h"
#include "LoadScene.h"

using namespace cocos2d;

class GameScene : public cocos2d::Layer
{

public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene *createScene();
private:

	enum priority { plasmagun, revolver };

	char *currentGunTexture;

	std::string gameType;

	int running;
	int ammorevolver;
	int ammoplasmagun;
	int hp;
	int card;
	int priorityGun;
	int score = 0;

	TMXLayer *openDoor;
	TMXLayer *wall;
	TMXLayer *ammo;

	// Notice objects
	Enemies *enemies;
	Collisions *collisions;
	Animations *anim;
	Projectile *projectile;
	Guns *gun;
	Gui *gui;
	GameWin *gamewin;
	LoadScene *load;

	cocos2d::Vector<cocos2d::Sprite *> _projectiles;
	cocos2d::Vector<cocos2d::Sprite *> _projTurret;

	TMXObjectGroup *objectGroup;

	TMXTiledMap *map;

	Sprite *character;
	Sprite *flash;
	Sprite *emotion;

	EventKeyboard::KeyCode currentKey;

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref *pSender);

	void keyboardSupport();

	void onTouchEnded(Touch *touch, Event *event);

	void centerProcessingMove(EventKeyboard::KeyCode keyCode, float dt);

	void setViewPointCenter(cocos2d::Point position);

	void shots(Touch *touch);

	void update(float dt);

	void updtMoving(float dt);

	void gameOverAction();

	void gameWin();

	void emotions();

	void emotionsDone();

	void ScoreArena();

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);
};

#endif // __GameScene_H__
