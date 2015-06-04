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

using namespace cocos2d;

class GameScene : public cocos2d::Layer
{
private:

	char *currentGunTexture;

	int running;
	int ammorevolver;
	int ammoplasmagun;
	int hp;
	int card;

	TMXLayer *openDoor;
	TMXLayer *wall;

	// Notice objects
	Enemies *enemies;
	Collisions *collisions;
	Animations *anim;
	Projectile *projectile;
	Guns *gun;
	Gui *gui;
	GameWin *gamewin;
public:

	cocos2d::Vector<cocos2d::Sprite *> _projectiles;
	cocos2d::Vector<cocos2d::Sprite *> _projTurret;

	TMXObjectGroup *objectGroup;

	TMXTiledMap *map;

	Sprite *character;
	Sprite *flash;
	Sprite *emotion;

	EventKeyboard::KeyCode currentKey;
	
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene *createScene();

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
    
    // implement the "static create()" method manually
	CREATE_FUNC(GameScene);
};

#endif // __GameScene_H__
