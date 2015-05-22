#include "Animations.h"
#include "cocos2d.h"

Animations::Animations(cocos2d::Sprite *character) {

	this->character = character;
}

void Animations::movAnim(char *anim_direct, int frames_num)  {

	//SpriteBatchNode* spritebatch = SpriteBatchNode::create("characters/chabi.png");

	SpriteFrameCache* cache = SpriteFrameCache::getInstance();

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
}