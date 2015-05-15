#include "Animations.h"
#include "cocos2d.h"

Animations::Animations(cocos2d::Sprite *character) {

	this->character = character;
}

void Animations::movAnim(char *anim_direct, char *start_anim, int frames_num)  {

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