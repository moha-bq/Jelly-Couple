#include "MenuScene.h"
#include "globals.h"
#include "HelloWorldScene.h"
#include "LevelSelectionScene.h"
#include "AudioEngine.h"
#include <cmath>

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#include "SonarFrameworks.h"
#endif


USING_NS_CC;

Scene* MenuScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MenuScene::create();

    scene->addChild(layer);
    return scene;
}

bool MenuScene::init()
{
	if ( !Layer::init() )
        return false;

	auto kb_listener = EventListenerKeyboard::create();
	kb_listener->onKeyReleased = CC_CALLBACK_2(MenuScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(kb_listener, this);

	jellycouple_shine_angle = 0;

	jellycouple_colinter.init(1.5, 1, 1);
	jellycouple_colinter.add_color(Color3B(255, 0, 4));
	jellycouple_colinter.add_color(Color3B(255, 0, 180));


	background = Sprite::create("game_bg.png");
	background->setPosition(Vec2(240, designResolutionSize.height / 2));

	jellycouple = Sprite::create("jellycouple.png");
	jellycouple->setAnchorPoint(Vec2(0.5f, 0.5f));
	jellycouple->setPosition(Vec2(240, designResolutionSize.height - 170));

	jellycouple_shine = Sprite::create("jellycouple_shine.png");
	jellycouple_shine->setAnchorPoint(Vec2(0.5f, 0.5f));
	jellycouple_shine->setPosition(Vec2(240, designResolutionSize.height - 170));

	play_butt = ui::Button::create("menu/play_button.png");
	play_butt->setPosition(Vec2(250, designResolutionSize.height - 250));
	play_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
			case ui::Widget::TouchEventType::ENDED:
			{
				auto level_selection = LevelSelectionScene::createScene();

				/*if (!music_muted)
				{
					experimental::AudioEngine::stopAll();
					current_playing_music = experimental::AudioEngine::play2d(MAIN_MUSIC, true, 0.0f);
					music_transtion_timer = music_transtion_duration;
				}
				entred_game = 1;*/

				Director::getInstance()->replaceScene(TransitionFade::create(0.35f, level_selection, Color3B(255, 255, 255)));
				//Director::getInstance()->pushScene(level_selection);

			}break;
		}
	});
	//play_butt->setVisible(false);

	acheiv_butt = ui::Button::create("menu/acheiv_button.png");
	acheiv_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
			if (!SonarCocosHelper::GooglePlayServices::isSignedIn())
				SonarCocosHelper::GooglePlayServices::signIn();
			else
			{
				for (int i = 0; i < 5; i++)
				{
					if (highscore[i] > 60.f)
					{
						SonarCocosHelper::GooglePlayServices::unlockAchievement(achievement_ids[i]);
					}
				}
				SonarCocosHelper::GooglePlayServices::showAchievements();
			}
#endif
		}break;
		}
	});
	//acheiv_butt->setVisible(false);

	mute_music_checkbox = ui::CheckBox::create("menu/music_button.png", "menu/cross.png");
	if (music_muted)
		mute_music_checkbox->setSelected(true);
	
	mute_music_checkbox->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			music_muted = !music_muted;
			if (music_muted)
			{
				experimental::AudioEngine::pauseAll();
			}
			else
			{
				experimental::AudioEngine::resumeAll();
				music_transtion_timer = music_transtion_duration/2;
			}

		}break;
		}
	});
	//mute_music_checkbox->setVisible(false);

	mute_sfx_checkbox = ui::CheckBox::create("menu/sfx_button.png", "menu/cross.png");
	if (sfx_muted)
		mute_sfx_checkbox->setSelected(true);
	mute_sfx_checkbox->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			sfx_muted = !sfx_muted;
			if (sfx_muted)
			{
				experimental::AudioEngine::stop(current_playing_sfx);
			}
		}break;
		}
	});
	//mute_sfx_checkbox->setVisible(false);

	addChild(background);
	addChild(play_butt);
	addChild(acheiv_butt);
	addChild(mute_music_checkbox);
	addChild(mute_sfx_checkbox);
	addChild(jellycouple_shine);
	addChild(jellycouple);
	

	sine_timer = 0;

	enter_anim_button_delta_time = 0.15f;
	enter_anim_duration = 13*enter_anim_button_delta_time;

	enter_anim_timer = 0;
	play_butt_scale = 0;
	play_butt_spring_vel = 0;
	acheiv_butt_scale = 0;
	acheiv_butt_spring_vel = 0;
	mute_music_checkbox_scale = 0;
	mute_music_checkbox_spring_vel = 0;
	mute_sfx_checkbox_scale = 0;
	mute_sfx_checkbox_spring_vel = 0;
	jellycouple_scale = 0;
	jellycouple_spring_vel = 0;
	jellycouple_shine_scale =0;
	jellycouple_shine_spring_vel = 0;
	
	play_butt->setScale(0);
	mute_music_checkbox->setScale(0);
	mute_sfx_checkbox->setScale(0);
	acheiv_butt->setScale(0);
	jellycouple->setScale(0);
	jellycouple_shine->setScale(0);

#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SonarCocosHelper::AdMob::hideBannerAd(0);
#endif

	by_beqqi_benjadi = ui::Text::create("By Beqqi&Benjadi", "fonts/hemi.ttf", 12);
	by_beqqi_benjadi->setAnchorPoint(Vec2(0.5f, 1));
	by_beqqi_benjadi->setPosition(Vec2(240, -100));
	by_region = DrawNode::create();
	by_region->setAnchorPoint(Vec2(0, 1));
	by_region->setPosition(Vec2(0, -100));
	addChild(by_region);
	addChild(by_beqqi_benjadi);

	by_y = 0;
	by_showup_timer = 0;
	by_showup_interval = 3;
	by_move_timer = 0;
	by_state = 0;

	google_game_connect_grey = ui::Button::create("games_controller_white.png");
	google_game_connect_grey->setScale(0.8f);
	google_game_connect_grey->setAnchorPoint(Vec2(0, 1));
	google_game_connect_grey->setPosition(Vec2(0, 800));
	google_game_connect_grey->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{

#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			if (!SonarCocosHelper::GooglePlayServices::isSignedIn())
				SonarCocosHelper::GooglePlayServices::signIn();

#endif
		}break;
		}
	}); 
	google_game_connect_grey->setVisible(false);
	addChild(google_game_connect_grey);

	google_game_connect_green = Sprite::create("games_controller.png");
	google_game_connect_green->setScale(0.8f);
	google_game_connect_green->setAnchorPoint(Vec2(0, 1));
	google_game_connect_green->setPosition(Vec2(0, 800));
	addChild(google_game_connect_green);
	google_game_connect_green->setVisible(false);

	

	return true;
}

void MenuScene::onEnterTransitionDidFinish()
{
	scheduleUpdate();
}

void MenuScene::update(float dt)
{
#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SonarCocosHelper::AdMob::hideBannerAd(0);

	if (!SonarCocosHelper::GooglePlayServices::isSignedIn())
	{
		google_game_connect_green->setVisible(false);
		google_game_connect_grey->setVisible(true);
	}
	else
	{
		google_game_connect_green->setVisible(true);
		google_game_connect_grey->setVisible(false);
	}
#endif

	by_showup_timer += dt;
	if (by_showup_timer > by_showup_interval)
	{
		if (by_state == 0)
		{
			float by_move_duration = 1;
			by_move_timer += dt;

			float t = by_move_timer / by_move_duration;
			by_y = coserp(0, t, 25);
			by_beqqi_benjadi->setPosition(Vec2(240, by_y));
			by_region->setPosition(Vec2(240 - 100, by_y - 50));
			
			if (by_move_timer > by_move_duration)
			{
				by_state = 1;
				by_move_timer = 0;
			}
		}
		else
		{
			if (by_showup_timer > by_showup_interval + 5.0f)
			{
				float by_move_duration = 1;
				by_move_timer += dt;

				float t = by_move_timer / by_move_duration;
				by_y = coserp(25, t, 0);
				by_beqqi_benjadi->setPosition(Vec2(240, by_y));
				by_region->setPosition(Vec2(240 - 100, by_y - 50));

				if (by_move_timer > by_move_duration)
				{
					by_showup_timer = 0;
					by_state = 0;
					by_move_timer = 0;
					by_showup_interval = frand() * 25 + 10;
				}
			}
		}
	}

	by_region->clear();
	by_region->drawSolidRect(Vec2(0, 0), Vec2(200, 50), Color4F(jellycouple_colinter.current_color));

	if (music_transtion_timer > 0)
	{
		float t = 1 - music_transtion_timer / music_transtion_duration;
		t = t*t;

#if xUSE_SIMPLE_AUDIO_ENGINE
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(t);
#else
		experimental::AudioEngine::setVolume(current_playing_music, t);
#endif

		music_transtion_timer -= dt;
	}
	sine_timer += dt;
	float sine_period = 2.0f;
	float play_yoffset = 6 * sinf(2 * 3.14 * sine_timer / sine_period);
	float others_yoffset = 6 * cosf(2*3.14 * sine_timer/sine_period);
	if (sine_timer > sine_period)
		sine_timer -= sine_period;

	play_butt->setPosition(Vec2(260, designResolutionSize.height/2 + play_yoffset));

	acheiv_butt->setPosition(Vec2(100, 200 + play_yoffset));
	mute_music_checkbox->setPosition(Vec2(250, 200 + others_yoffset));
	mute_sfx_checkbox->setPosition(Vec2(400, 200 + others_yoffset));
	

	background->setPosition(Vec2(240, designResolutionSize.height/2 + play_yoffset * 0.4f));

	if (enter_anim_timer < enter_anim_duration)
	{
		if (fabs(play_butt_scale - 1)>EPS || fabs(play_butt_spring_vel) > EPS)
		{
			simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &play_butt_scale, &play_butt_spring_vel);
			play_butt->setScale(play_butt_scale);
		}

		if (enter_anim_timer > 0.2f)
		{
			if (fabs(jellycouple_scale - 1) > EPS || fabs(jellycouple_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &jellycouple_scale, &jellycouple_spring_vel);
				jellycouple->setScale(jellycouple_scale);
			}

		}

		if (enter_anim_timer > enter_anim_button_delta_time + 0.2)
		{
			if (fabs(acheiv_butt_scale - 1) > EPS || fabs(acheiv_butt_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &acheiv_butt_scale, &acheiv_butt_spring_vel);
				acheiv_butt->setScale(acheiv_butt_scale);
			}
		}
		if (enter_anim_timer > 2*enter_anim_button_delta_time + 0.2)
		{
			if (fabs(mute_music_checkbox_scale - 1) > EPS || fabs(mute_music_checkbox_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &mute_music_checkbox_scale, &mute_music_checkbox_spring_vel);
				mute_music_checkbox->setScale(mute_music_checkbox_scale);
			}
		}
		if (enter_anim_timer > 3 * enter_anim_button_delta_time + 0.2)
		{
			if (fabs(mute_sfx_checkbox_scale - 1) > EPS || fabs(mute_sfx_checkbox_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &mute_sfx_checkbox_scale, &mute_sfx_checkbox_spring_vel);
				mute_sfx_checkbox->setScale(mute_sfx_checkbox_scale);
			}

			if (fabs(jellycouple_shine_scale - 1) > EPS || fabs(jellycouple_shine_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &jellycouple_shine_scale, &jellycouple_shine_spring_vel);
				jellycouple_shine->setScale(jellycouple_shine_scale);
			}
		}

		enter_anim_timer += dt;
	}

	jellycouple_colinter.update(dt);
	jellycouple_shine_angle += 75 * dt;
	jellycouple_shine->setRotation(jellycouple_shine_angle);
	jellycouple_shine->setColor(jellycouple_colinter.current_color);
	jellycouple->setColor(jellycouple_colinter.current_color);

}

void MenuScene::onKeyReleased(EventKeyboard::KeyCode key, Event* ev)
{
	if (key == EventKeyboard::KeyCode::KEY_ESCAPE || key == EventKeyboard::KeyCode::KEY_BACK)
		Director::getInstance()->end();
}