#pragma once 

#include "cocos2d.h"

using namespace cocos2d;

class PauseScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual void update(float);

	bool onTouchBegan(Touch* touch, Event* ev);

	cocos2d::DrawNode* draw_node;

	CREATE_FUNC(PauseScene);
};
