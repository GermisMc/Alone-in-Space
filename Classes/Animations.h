#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "cocos2d.h"

using namespace cocos2d;

class Animations
{
private:

	Sprite *character;
public:

	Animations(Sprite *character);
	
	void movAnim(char *anim_direct, char *start_anim, int frames_num);
};

#endif // __ANIMATION_H__