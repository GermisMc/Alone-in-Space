#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "cocos2d.h"

using namespace cocos2d;

class Animations
{

public:

	Animations(Sprite *character);

	void movAnim(char *anim_direct, int frames_num);
private:

	Sprite *character;
};

#endif // __ANIMATION_H__