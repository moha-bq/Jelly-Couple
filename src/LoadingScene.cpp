#include "LoadingScene.h"
#include "globals.h"
#include "MenuScene.h"
#include "AudioEngine.h"

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#include "SonarFrameworks.h"
#endif

USING_NS_CC;

Scene* LoadingScene::createScene()
{
    auto scene = Scene::create();
    auto layer = LoadingScene::create();

    scene->addChild(layer);
    return scene;
}

bool LoadingScene::init()
{
	if ( !Layer::init() )
        return false;
	all_sounds_loaded = 0;
	check_timer = 1;
	check_interval = 0.4f;

	draw_node = DrawNode::create();
	draw_node->drawSolidRect(Vec2(0, 0), Vec2(480, designResolutionSize.height), Color4F::BLACK);

	addChild(draw_node);

	auto touch_listener = EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = CC_CALLBACK_2(LoadingScene::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

	Director::getInstance()->getTextureCache()->addImage("game_bg.png");

	scheduleUpdate();

	return true;
}

void LoadingScene::update(float dt)
{
	check_timer += dt;
	if (check_timer > check_interval)
	{
		all_sounds_loaded = 1;
		for (int i = 0; i < NUM_SOUNDS; i++)
		{
			if (sound_loading_status[i] == 0)
			{
				all_sounds_loaded = 0;
				break;
			}
		}
		check_timer = 0;
	}

	if (all_sounds_loaded)
	{
		auto menu_scene = MenuScene::createScene();
		Director::getInstance()->replaceScene(menu_scene);
		if (!music_muted)
		{
			current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true,0.0f);
			music_transtion_timer = music_transtion_duration;
		}
	}
}

bool LoadingScene::onTouchBegan(Touch* touch, Event* ev)
{
	return true;
}