#ifndef __GUI_H__
#define __GUI_H__

#include "cocos2d.h"

using namespace cocos2d;

class Gui : public cocos2d::Layer
{
private:

	bool test = true;

	DrawNode *blackout;

	Label *countPlasmaGun;
	Label *countHealth;
	Label *wasted;
	Label *countCard;

	Sprite *guiAmmoPlasmagun;
	Sprite *guiAmmoRevolver;
	Sprite *guiCard;
	Sprite *health;
public:

	void countGui(int *ammorevolver, int *ammoplasmagun, int *hp, int *card);

	void gameOver(int *hp, Sprite *character, int *card);

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene *createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	bool init();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref *pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(Gui);
};

#endif // __GUI_H__