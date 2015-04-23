#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

using namespace cocos2d;

class GameScene : public cocos2d::Layer
{
private:
	cocos2d::TMXTiledMap *map;
	cocos2d::TMXLayer *floor;
	cocos2d::TMXLayer *wall;
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	void movAnim(char *anim_direct, char *start_anim, int frames_num);

	void keyboardSupport();

	void setViewPointCenter(cocos2d::Point position);

	void safetyCheck();

	void update(float dt);

	void setPlayerPosition(Point position);

	Point tileCoordForPosition(Point position);
    
    // implement the "static create()" method manually
	CREATE_FUNC(GameScene);

	cocos2d::Sprite *character;
};

#endif // __HELLOWORLD_SCENE_H__
