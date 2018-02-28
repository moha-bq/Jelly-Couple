#include "PauseScene.h"
#include "globals.h"

USING_NS_CC;

Scene* PauseScene::createScene()
{
    auto scene = Scene::create();
    auto layer = PauseScene::create();

    scene->addChild(layer);
    return scene;
}

bool PauseScene::init()
{
	if ( !Layer::init() )
        return false;

	draw_node = DrawNode::create();
	draw_node->drawSolidRect(Vec2(0, 0), Vec2(480, 780), Color4F(1, 1, 1, 0.3f));
	draw_node->drawSolidRect(Vec2(100, 100), Vec2(300, 300), Color4F(1, 1, 1, 1));
	

	addChild(draw_node, 1);

	auto touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(PauseScene::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

	scheduleUpdate();

	return true;
}

void PauseScene::update(float dt)
{
}

bool PauseScene::onTouchBegan(Touch* touch, Event* ev)
{
	Vec2 play_button_center = Vec2(200, 200);
	float play_button_radius = 50;
	if ((touch->getLocation() - play_button_center).lengthSquared() < play_button_radius * play_button_radius)
	{
		Director::getInstance()->popScene();
	}

	return true;
}