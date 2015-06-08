#include "Animations.h"
#include "cocos2d.h"

Animations::Animations(cocos2d::Sprite *character) {

	this->character = character;
}

void Animations::movAnim(char *anim_direct, int frames_num)  {

	const int TAG_ANIMATE = 15;

	const float DELAY_ANIMATION = 0.2f;

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

	Vector<SpriteFrame*> animFrames(15);

	char str[100] = { 0 };
	for (int i = 0; i <= frames_num; i++)
	{
		sprintf(str, anim_direct, i);
		SpriteFrame* frame = cache->getSpriteFrameByName(str);
		animFrames.pushBack(frame);
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, DELAY_ANIMATION);

	auto animate = RepeatForever::create(Animate::create(animation));
	animate->setTag(TAG_ANIMATE);

	character->runAction(animate);
}