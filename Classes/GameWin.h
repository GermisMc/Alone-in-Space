#ifndef __GAMEWIN_H__
#define __GAMEWIN_H__

#include "cocos2d.h"

using namespace cocos2d;

class GameWin : public cocos2d::Layer
{
private:

	Label *labelCards;

	Sprite *background;
	Sprite *cards;
public:

	void resultCards(int *card);

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene *createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref *pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(GameWin);
};

#endif // __GAMEWIN_H__