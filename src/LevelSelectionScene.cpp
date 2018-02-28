#include "LevelSelectionScene.h"
#include "globals.h"
#include "HelloWorldScene.h"
#include "AudioEngine.h"
#include "MenuScene.h"
#include <cmath>

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#include "SonarFrameworks.h"
#endif

USING_NS_CC;

static float desc_frame_height = 2;
static float desc_frame_y = 750;
static float desc_height = 100;


Scene* LevelSelectionScene::createScene()
{
    auto scene = Scene::create();
    auto layer = LevelSelectionScene::create();

    scene->addChild(layer);
    return scene;
}

bool LevelSelectionScene::init()
{
    if ( !Layer::init() )
        return false;

	enter_anim_finished = 0;

	current_page = 0;
	next_page = 0;
	transitioning = 0;

	icon_rotatin_speed = 140;
	icon_sine_timer = 0;

    sine_timer = 0;
    transition_spring_damp = 16.f;
    transition_spring_k = 320;
    transition_spring_vel = 0;
    transition_max_duration = 10.0f;
    transition_timer = 0;

	auto kb_listener = EventListenerKeyboard::create();
	kb_listener->onKeyReleased = CC_CALLBACK_2(LevelSelectionScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(kb_listener, this);

	play_button = ui::Button::create("menu/play_button.png");
	play_button->setAnchorPoint(Vec2(0.5f, 0.5f));
	play_button->setPosition(Vec2(250, 290));
	play_button->setColor(Color3B(0, 255, 0));
	play_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			current_difficulty = next_page;
			auto game_scene = HelloWorld::createScene();

			if (!music_muted)
			{
				experimental::AudioEngine::stopAll();
				current_playing_music = experimental::AudioEngine::play2d(MAIN_MUSIC, true, 0.f);
				music_transtion_timer = music_transtion_duration;
			}
			entred_game = 1;
			Director::getInstance()->replaceScene(TransitionFade::create(0.35f, game_scene, Color3B(255, 255, 255)));
		}break;
		}
	});
	addChild(play_button);

	leader_button = ui::Button::create("menu/leader_button.png");
	leader_button->setAnchorPoint(Vec2(0.5f, 0.5f));
	leader_button->setPosition(Vec2(480 - 120, 150));
	leader_button->setColor(Color3B(255, 0, 0));
	leader_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
			if (!SonarCocosHelper::GooglePlayServices::isSignedIn())
				SonarCocosHelper::GooglePlayServices::signIn();
			else
			{
				if(highscore[next_page]>=0)
					SonarCocosHelper::GooglePlayServices::submitScore(leaderboard_ids[next_page], (int)(highscore[next_page] * 1000.0));
				SonarCocosHelper::GooglePlayServices::showLeaderboard(leaderboard_ids[next_page]);
			}
#endif
		}break;
		}
	});
	addChild(leader_button);

	back_button = ui::Button::create("menu/home_button.png");
	back_button->setAnchorPoint(Vec2(0.5f, 0.5f));
	back_button->setPosition(Vec2(120, 150));
	back_button->setColor(Color3B(255, 0, 0));
	back_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			auto menu_scene = MenuScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.35f, menu_scene, Color3B(255, 255, 255)));
		}break;
		}
	});
	addChild(back_button);


    left_arrow = ui::Button::create("menu_arrow.png");
    left_arrow->setPosition(Vec2(50, 290));
    left_arrow->setColor(Color3B(0, 255, 0));
    left_arrow->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        switch (type)
        {
        case ui::Widget::TouchEventType::ENDED:
        {
			if (transitioning)
			{
				pages[current_page]->x = 480;
				pages[next_page]->x = 0;
			}
			current_page = next_page;
			next_page = current_page - 1;
			if (next_page < 0)
				next_page = NUM_DIFFICULTIES - 1;
			pages[next_page]->x = -480;
			transitioning = 1;
			transition_timer = 0;
        }break;
        }
    });
    addChild(left_arrow);

    right_arrow = ui::Button::create("menu_arrow.png");
    right_arrow->setPosition(Vec2(480-50, 290));
    right_arrow->setColor(Color3B(0, 255, 0));
    right_arrow->setRotation(180);
    right_arrow->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
        switch (type)
        {
        case ui::Widget::TouchEventType::ENDED:
        {
			if (transitioning)
			{
				pages[current_page]->x = -480;
				pages[next_page]->x = 0;
			}
			current_page = next_page;
			next_page = current_page + 1;
			if (next_page >= NUM_DIFFICULTIES)
				next_page = 0;
			pages[next_page]->x = 480;
			transitioning = 1;
			transition_timer = 0;
        }break;
        }
    });
    addChild(right_arrow);

    background_emitter = ParticleSystemQuad::create("background_ps.plist");
    background_emitter->setAnchorPoint(Vec2(0, 0));
    background_emitter->setPosition(Vec2(0, 0));
    addChild(background_emitter, -1);
    //background_emitter[0]->setVisible(false);

    background = Sprite::create("game_bg.png");
    //left_backgrounds[0]->setAnchorPoint(Vec2(0, 0));
    background->setColor(Color3B(170, 80, 170));
    background->setBlendFunc(BlendFunc::ADDITIVE);
    background->setPosition(Vec2(240,400));
    addChild(background, -10);

	/*background_colors[0] = Color3B(170, 80, 170);
	background_colors[1] = Color3B(255, 0, 0);
	background_colors[2] = Color3B(255, 170, 0);
	background_colors[3] = Color3B(0, 255, 170);
	background_colors[4] = Color3B(170, 0, 255);*/
	for (int i = 0; i < 5; i++)
		background_colors[i] = level_themes[i].right_player;
	background_colors[1] = level_themes[1].left_player;
	background_colors[4] = Color3B(255, 170, 0);
	if (highscore[2] < 60)
		background_colors[4] = Color3B(200, 200, 200);
	if (highscore[1] < 60)
		background_colors[3] = Color3B(200, 200, 200);

    desc_frame = DrawNode::create();
    desc_frame->setAnchorPoint(Vec2(0.5f, 0.5f));
    desc_frame->setPosition(240, desc_frame_y - desc_frame_height - desc_height/2);
    addChild(desc_frame);

	page_indicator = DrawNode::create();
	page_indicator->setPosition(240, desc_frame_y + 20);
	addChild(page_indicator);

	float icon_y = 510;
	float icon_width = config.jump_obs_width;
	float icon_spacing = 22;

	very_easy_icon      = Sprite::create("spike.png");
	very_easy_icon->setAnchorPoint(Vec2(0.5f, 0.5f));
	very_easy_icon->setPosition(Vec2(240, icon_y));
	very_easy_icon_glow = Sprite::create("spike_glow.png");
	very_easy_icon_glow->setAnchorPoint(Vec2(0.5f, 0.5f));
	very_easy_icon_glow->setPosition(Vec2(240, icon_y));

	for (int i = 0; i < 2; i++)
	{
		float w = icon_width * 2 + icon_spacing;
		easy_icon[i] = Sprite::create("spike.png");
		easy_icon[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		easy_icon[i]->setPosition(Vec2(240 - w/2 + i*(icon_width+icon_spacing) + icon_width / 2, icon_y));
		easy_icon_glow[i] = Sprite::create("spike_glow.png");
		easy_icon_glow[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		easy_icon_glow[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width / 2, icon_y));
	}
	for (int i = 0; i < 3; i++)
	{
		float w = icon_width * 3 + icon_spacing*2;
		normal_icon[i] = Sprite::create("spike.png");
		normal_icon[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		normal_icon[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width / 2, icon_y));
		normal_icon_glow[i] = Sprite::create("spike_glow.png");
		normal_icon_glow[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		normal_icon_glow[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width / 2, icon_y));
	}
	for (int i = 0; i < 4; i++)
	{
		float w = icon_width * 4 + icon_spacing * 3;
		hard_icon[i] = Sprite::create("spike.png");
		hard_icon[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		hard_icon[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width/2, icon_y));
		hard_icon_glow[i] = Sprite::create("spike_glow.png");
		hard_icon_glow[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		hard_icon_glow[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width / 2, icon_y));
	}
	for (int i = 0; i < 5; i++)
	{
		float w = icon_width * 5 + icon_spacing * 4;
		very_hard_icon[i] = Sprite::create("spike.png");
		very_hard_icon[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		very_hard_icon[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width / 2, icon_y));
		very_hard_icon_glow[i] = Sprite::create("spike_glow.png");
		very_hard_icon_glow[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
		very_hard_icon_glow[i]->setPosition(Vec2(240 - w/2 + i*(icon_width + icon_spacing) + icon_width / 2, icon_y));
	}

    for (int i = 0; i < NUM_DIFFICULTIES; i++)
    {
        pages[i] = Level_Page::create();
        addChild(pages[i]);
    }

    pages[0]->set_level_name("VERY EASY");
    pages[0]->set_best_time(highscore[0]);
    pages[0]->x = 0;
	pages[0]->addChild(very_easy_icon);
	pages[0]->addChild(very_easy_icon_glow);

    pages[1]->set_level_name("EASY");
    pages[1]->set_best_time(highscore[1]);
	for (int i = 0; i < 2; i++)
	{
		pages[1]->addChild(easy_icon[i]);
		pages[1]->addChild(easy_icon_glow[i]);
	}

    pages[2]->set_level_name("NORMAL");
    pages[2]->set_best_time(highscore[2]);
	for (int i = 0; i < 3; i++)
	{
		pages[2]->addChild(normal_icon[i]);
		pages[2]->addChild(normal_icon_glow[i]);
	}

    pages[3]->set_level_name("HARD");
    pages[3]->set_best_time(highscore[3]);
	for (int i = 0; i < 4; i++)
	{
		pages[3]->addChild(hard_icon[i]);
		pages[3]->addChild(hard_icon_glow[i]);
	}

    pages[4]->set_level_name("SUPER HARD");
    pages[4]->set_best_time(highscore[4]);
	for (int i = 0; i < 5; i++)
	{
		pages[4]->addChild(very_hard_icon[i]);
		pages[4]->addChild(very_hard_icon_glow[i]);
	}

    enter_anim_timer = 0;
    enter_anim_glow_begin_time = 1;

    enter_anim_duration = 10;
    enter_anim_ui_scale = 0;
    enter_anim_ui_spring_vel = 0;
    enter_anim_glow_scale = 0;
    enter_anim_glow_spring_vel = 0;

    left_arrow->setScale(0);
    right_arrow->setScale(0);
	play_button->setScale(0);
	back_button->setScale(0);
	leader_button->setScale(0);

    desc_frame->setScale(0);

    pages[current_page]->level_name_label->setScale(0);
	pages[current_page]->best_time_text->setScale(0);

	very_easy_icon_glow->setColor(background_colors[0]);
	for (int i = 0; i < 2; i++)
		easy_icon_glow[i]->setColor(background_colors[1]);
	for (int i = 0; i < 3; i++)
		normal_icon_glow[i]->setColor(background_colors[2]);
	for (int i = 0; i < 4; i++)
		hard_icon_glow[i]->setColor(background_colors[3]);
	for (int i = 0; i < 5; i++)
		very_hard_icon_glow[i]->setColor(background_colors[4]);


	for (int i = 0; i < NUM_DIFFICULTIES; i++)
	{
		pages[i]->level_name_label->setColor(background_colors[0]);
		pages[i]->best_time_text->setColor(background_colors[0]);

	}
	background->setColor(Color3B(background_colors[0].r*0.8f, background_colors[0].g*0.8f, background_colors[0].b*0.8f));
	play_button->setColor(background_colors[0]);
	right_arrow->setColor(background_colors[0]);
	left_arrow->setColor(background_colors[0]);
	back_button->setColor(background_colors[0]);
	leader_button->setColor(background_colors[0]);
	desc_frame->clear();
	desc_frame->drawSolidRect(Vec2(-300, desc_height / 2), Vec2(300, -desc_height / 2),
		Color4F(background_colors[0].r / 255.f*0.2f, background_colors[0].g / 255.f*0.2f, background_colors[0].b / 255.f*0.2f, 0.7f));
	desc_frame->drawSolidRect(Vec2(-300, desc_height / 2), Vec2(300, desc_height / 2 + desc_frame_height), Color4F(background_colors[0]));
	desc_frame->drawSolidRect(Vec2(-300, -desc_height / 2), Vec2(300, -desc_height / 2 - desc_frame_height), Color4F(background_colors[0]));

#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//SonarCocosHelper::AdMob::showBannerAd(0);
#endif
	
	locked_text = ui::Text::create("LOCKED", "fonts/hemi.ttf", 32);
	locked_text->setPosition(Vec2(240, 290));
	locked_text->setColor(Color3B(200,200,200));
	locked_text->setTextVerticalAlignment(TextVAlignment::CENTER);
	locked_text->setTextHorizontalAlignment(TextHAlignment::CENTER);
	locked_text->setVisible(false);
	addChild(locked_text);

	return true;
}

void LevelSelectionScene::onEnterTransitionDidFinish()
{
    scheduleUpdate();
}

void LevelSelectionScene::update(float dt)
{
    background_emitter->update(dt);

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

#if 0
    veryeasy_butt->setPosition(Vec2(240, designResolutionSize.height - 70 + sin_yoffset));
    easy_butt->setPosition(Vec2(120, designResolutionSize.height - 200 + sin_yoffset));
    normal_butt->setPosition(Vec2(480 - 120, designResolutionSize.height - 200 + cos_yoffset));
    hard_butt->setPosition(Vec2(120, 200 + sin_yoffset));
    superhard_butt->setPosition(Vec2(480 - 120, 200 + cos_yoffset));
#endif
    if (transitioning != 0)
    {
        transition_timer += dt;
        if ((fabs(pages[next_page]->x) > 1e-2 || fabs(transition_spring_vel) > 1e-2) && transition_timer < transition_max_duration)
        {
			float fixed_dt = FIXED_TIMESTEP;
			float acc = dt;
			while (acc > fixed_dt)
			{
				float sa = -transition_spring_k*pages[next_page]->x - transition_spring_damp*transition_spring_vel;
				transition_spring_vel += sa*fixed_dt;
				pages[next_page]->x += transition_spring_vel*fixed_dt;
				pages[current_page]->x += transition_spring_vel*fixed_dt;
				acc -= fixed_dt;
			}
			{
				float sa = -transition_spring_k*pages[next_page]->x - transition_spring_damp*transition_spring_vel;
				transition_spring_vel += sa*acc;
				pages[next_page]->x += transition_spring_vel*acc;
				pages[current_page]->x += transition_spring_vel*acc;
			}
        }
        else
        {
			pages[next_page]->x = 0;
			pages[current_page]->x = 480;
			current_page = next_page;
			transitioning = 0;
			transition_spring_vel = 0;
#if 0
			if (transitioning < 0)
				transitioning++;
			else
				transitioning--;

			if (transitioning == 0)
			{
				
			}
			else if (transitioning>0)
			{
				transition_timer = 0;
				next_page = current_page + 1;
				if (next_page == NUM_DIFFICULTIES)
					next_page = 0;
				pages[next_page]->x = 480;
			}
			else
			{
				transition_timer = 0;
				next_page = current_page - 1;
				if (next_page < 0)
					next_page = NUM_DIFFICULTIES - 1;
				pages[next_page]->x = -480;
			}
#endif
        }
    }

	if (enter_anim_timer < enter_anim_duration)
	{
		if (fabs(enter_anim_ui_scale - 1)>0.01f || fabs(enter_anim_ui_spring_vel) > 0.01f)
		{
			simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &enter_anim_ui_scale, &enter_anim_ui_spring_vel);

			left_arrow->setScale(enter_anim_ui_scale*0.7f);
			right_arrow->setScale(enter_anim_ui_scale*0.7f);
			play_button->setScale(enter_anim_ui_scale*0.6f);
			back_button->setScale(enter_anim_ui_scale);
			leader_button->setScale(enter_anim_ui_scale);
			desc_frame->setScale(enter_anim_ui_scale);
			pages[current_page]->level_name_label->setScale(enter_anim_ui_scale);
			pages[current_page]->best_time_text->setScale(enter_anim_ui_scale);

			very_easy_icon->setScale(enter_anim_ui_scale);
			very_easy_icon_glow->setScale(enter_anim_ui_scale);
			for (int i = 0; i < 2; i++)
			{
				easy_icon[i]->setScale(enter_anim_ui_scale);
				easy_icon_glow[i]->setScale(enter_anim_ui_scale);
			}
			for (int i = 0; i < 3; i++)
			{
				normal_icon[i]->setScale(enter_anim_ui_scale);
				normal_icon_glow[i]->setScale(enter_anim_ui_scale);
			}
			for (int i = 0; i < 4; i++)
			{
				hard_icon[i]->setScale(enter_anim_ui_scale);
				hard_icon_glow[i]->setScale(enter_anim_ui_scale);
			}
			for (int i = 0; i < 5; i++)
			{
				very_hard_icon[i]->setScale(enter_anim_ui_scale);
				very_hard_icon_glow[i]->setScale(enter_anim_ui_scale);
			}

		}
		else
			enter_anim_finished = true;

		if (enter_anim_timer > enter_anim_glow_begin_time)
		{
			if (fabs(enter_anim_glow_scale - 1) > EPS || fabs(enter_anim_glow_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp * 2, ui_spring_tightness, 1, &enter_anim_glow_scale, &enter_anim_glow_spring_vel);
			}
		}

		enter_anim_timer += dt;
	}

    sine_timer += dt;
    float sine_period = 2.0f;
	float sine_amp = 5;
	float sin_yoffset  = sinf(2 * 3.14 * sine_timer / sine_period);
	float sin_yoffset2 = sinf(4 * 3.14 * sine_timer / sine_period);
	float cos_yoffset  = cosf(2 * 3.14 * sine_timer / sine_period);
	float cos_yoffset2 = cosf(4 * 3.14 * sine_timer / sine_period);
    if (sine_timer > sine_period)
        sine_timer -= sine_period;

    //right_arrow->setPosition(Vec2(480 - 50, 210 + sine_amp*sin_yoffset));
    //left_arrow->setPosition(Vec2(50, 210 + sine_amp*cos_yoffset));
	//play_button->setPosition(Vec2(240, 190 + sine_amp*cos_yoffset2));
	back_button->setPosition(Vec2(120, 150 + sine_amp*cos_yoffset));
	leader_button->setPosition(Vec2(480 - 120, 150 + sine_amp*sin_yoffset));
	
	play_button->setScale(0.1*cos_yoffset + 0.6f);
	locked_text->setScale(0.1*cos_yoffset + 0.6f);

	for (int i = 0; i < NUM_DIFFICULTIES; i++)
	{
		pages[i]->y = 0.3f*(sine_amp*sin_yoffset);
		pages[i]->level_name_label->setColor(background_colors[next_page]);
		pages[i]->best_time_text->setColor(background_colors[next_page]);
		//pages[i]->best_time_text->setScale(0.1*sin_yoffset + 1);

	}
	background->setColor(Color3B(background_colors[next_page].r*0.8f, background_colors[next_page].g*0.8f, background_colors[next_page].b*0.8f));
	play_button->setColor(background_colors[next_page]);
	right_arrow->setColor(background_colors[next_page]);
	left_arrow->setColor(background_colors[next_page]);
	back_button->setColor(background_colors[next_page]);
	leader_button->setColor(background_colors[next_page]);
	desc_frame->clear();
	desc_frame->drawSolidRect(Vec2(-300, desc_height / 2), Vec2(300, -desc_height / 2),
		Color4F(background_colors[next_page].r/255.f*0.2f, background_colors[next_page].g / 255.f*0.2f, background_colors[next_page].b / 255.f*0.2f,0.7f));
	desc_frame->drawSolidRect(Vec2(-300, desc_height / 2), Vec2(300, desc_height / 2 + desc_frame_height), Color4F(background_colors[next_page]));
	desc_frame->drawSolidRect(Vec2(-300, -desc_height / 2), Vec2(300, -desc_height / 2 - desc_frame_height), Color4F(background_colors[next_page]));

	{
		page_indicator->clear();
		float but_dim = 5;
		float but_spacing = 5;
		float page_indicator_width = but_dim*NUM_DIFFICULTIES + but_spacing*(NUM_DIFFICULTIES - 1);
		for (int i = 0; i < NUM_DIFFICULTIES; i++)
		{
			float but_x = i*(but_dim + but_spacing) - page_indicator_width / 2;
			if (i == next_page)
			{
				page_indicator->drawSolidRect(Vec2(but_x, 0), Vec2(but_x + but_dim, but_dim), Color4F(1, 1, 1, 1));
			}
			else
			{
				page_indicator->drawSolidRect(Vec2(but_x, 0), Vec2(but_x + but_dim, but_dim), Color4F(0.5, 0.5, 0.5, 1));
			}
		}
	}

	icon_sine_timer += dt;
	float icon_sine_from = 1.08f;
	float icon_sine_to = 1.5f;
	float icon_sine_period = 1.8f;
	float icon_scale = (icon_sine_to - icon_sine_from) * sinf(2 * 3.14 * icon_sine_timer / icon_sine_period) + icon_sine_from;
	if (icon_sine_timer > icon_sine_period)
		icon_sine_timer -= icon_sine_period;

	very_easy_icon->setRotation(very_easy_icon->getRotation() + icon_rotatin_speed*dt);
	very_easy_icon_glow->setRotation(very_easy_icon_glow->getRotation() + icon_rotatin_speed*dt);
	very_easy_icon->setScale(icon_scale);
	very_easy_icon_glow->setScale(icon_scale);

	for (int i = 0; i < 2; i++)
	{
		int dir = (i & 1) ? 1 : -1;
		easy_icon[i]->setRotation(easy_icon[i]->getRotation() + dir*icon_rotatin_speed*dt);
		easy_icon_glow[i]->setRotation(easy_icon_glow[i]->getRotation() + dir*icon_rotatin_speed*dt);
		easy_icon[i]->setScale(icon_scale);
		easy_icon_glow[i]->setScale(icon_scale);
	}
	for (int i = 0; i < 3; i++)
	{
		int dir = (i & 1) ? 1 : -1;
		normal_icon[i]->setRotation(normal_icon[i]->getRotation() + dir*2.5f*icon_rotatin_speed*dt);
		normal_icon_glow[i]->setRotation(normal_icon_glow[i]->getRotation() + dir*2.5f*icon_rotatin_speed*dt);
		normal_icon[i]->setScale(icon_scale);
		normal_icon_glow[i]->setScale(icon_scale);
	}
	for (int i = 0; i < 4; i++)
	{
		int dir = (i & 1) ? 1 : -1;
		hard_icon[i]->setRotation(hard_icon[i]->getRotation() + dir*4*icon_rotatin_speed*dt);
		hard_icon_glow[i]->setRotation(hard_icon_glow[i]->getRotation() + dir*4*icon_rotatin_speed*dt);
		hard_icon[i]->setScale(icon_scale);
		hard_icon_glow[i]->setScale(icon_scale);
	}
	for (int i = 0; i < 5; i++)
	{
		int dir = (i & 1) ? 1 : -1;
		very_hard_icon[i]->setRotation(very_hard_icon[i]->getRotation() + dir*6*icon_rotatin_speed*dt);
		very_hard_icon_glow[i]->setRotation(very_hard_icon_glow[i]->getRotation() + dir*6*icon_rotatin_speed*dt);
		very_hard_icon[i]->setScale(icon_scale);
		very_hard_icon_glow[i]->setScale(icon_scale);
	}

	if (next_page == 4)
	{
		if (highscore[2] < 60)
		{
			play_button->setVisible(false);
			locked_text->setVisible(true);
			pages[next_page]->best_time_text->setText("COMPLETE NORMAL TO UNLOCK");
			pages[next_page]->best_time_text->setFontSize(16);
		}
		else
		{
			locked_text->setVisible(false);
			play_button->setVisible(true);
			pages[next_page]->set_best_time(highscore[next_page]);
			pages[next_page]->best_time_text->setFontSize(32);
		}
	}
	else if (next_page == 3)
	{
		if (highscore[1] < 60)
		{
			play_button->setVisible(false);
			locked_text->setVisible(true);
			pages[next_page]->best_time_text->setText("COMPLETE EASY TO UNLOCK");
			pages[next_page]->best_time_text->setFontSize(16);
		}
		else
		{
			locked_text->setVisible(false);
			play_button->setVisible(true);
			pages[next_page]->set_best_time(highscore[next_page]);
			pages[next_page]->best_time_text->setFontSize(32);
		}
	}
	else
	{
		locked_text->setVisible(false);
		play_button->setVisible(true);
		pages[next_page]->best_time_text->setVisible(true);
	}

}

Level_Page::Level_Page()
{
}

Level_Page* Level_Page::create()
{
    Level_Page* lvl_page= new Level_Page();
    if (lvl_page && lvl_page->init())
    {
        lvl_page->autorelease();
        return lvl_page;
    }

    delete lvl_page;
    return nullptr;
}

bool Level_Page::init()
{
    if (!Node::init())
        return false;

    setAnchorPoint(Vec2(0, 0));

    level_name = 0;
    best_time = 0;

    x = 480;
    y = 0;
    setPosition(Vec2(x, y));

    level_name_label = ui::Text::create("", "fonts/hemi.ttf", 38);
    level_name_label->setColor(Color3B(255, 0, 144));
    level_name_label->setTextHorizontalAlignment(TextHAlignment::CENTER);
    level_name_label->setTextVerticalAlignment(TextVAlignment::CENTER);
    level_name_label->setPosition(Vec2(240, desc_frame_y - desc_height/2));
    addChild(level_name_label);

	best_time_text = ui::Text::create("", "fonts/hemi.ttf", 32);
	best_time_text->setColor(Color3B(20, 200, 200));
	best_time_text->setTextHorizontalAlignment(TextHAlignment::CENTER);
	best_time_text->setTextVerticalAlignment(TextVAlignment::CENTER);
	best_time_text->setAnchorPoint(Vec2(0.5f, 0.5f));
	best_time_text->setPosition(Vec2(240, 410));
	addChild(best_time_text);

    scheduleUpdate();

    return true;
}

void Level_Page::update(float dt)
{
    setPosition(Vec2(x, y));
}

void Level_Page::set_level_name(char* name)
{
    level_name_label->setString(name);
}

void Level_Page::set_best_time(float time)
{
    char buffer[64]="";
	if (time >= 0)
	{
		sprintf(buffer, "BEST:  %.2d:%.2d", (int)time, ((int)(time * 100.f)) % 100);
	}
	best_time_text->setString(buffer);
}

void LevelSelectionScene::onKeyReleased(EventKeyboard::KeyCode key, Event* ev)
{
	if (key == EventKeyboard::KeyCode::KEY_ESCAPE || key == EventKeyboard::KeyCode::KEY_BACK)
	{
		auto menu_scene = MenuScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.35f, menu_scene, Color3B(255, 255, 255)));
	}
}