#pragma once 

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;

class LoadingScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual void update(float);

	bool onTouchBegan(Touch* touch, Event* ev);

	cocos2d::DrawNode* draw_node;
	ui::Text* loading_text;

	int all_sounds_loaded;
	float check_interval;
	float check_timer;

	CREATE_FUNC(LoadingScene);
};
