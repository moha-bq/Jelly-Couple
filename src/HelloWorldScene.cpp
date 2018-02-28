#include "HelloWorldScene.h"
#include "Player.h"
#include "Obstacle_Generator.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "MenuScene.h"

#include <fstream>
#include <string>

#include <functional>

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#include "SonarFrameworks.h"
#endif

#if ENABLE_EDITOR
#include "Editor.h"
#endif

USING_NS_CC;

static float score_panel_height = 50;
static int ground_outline = 2;

void HelloWorld::show_pause_menu()
{
	pause_home_butt->setVisible(true);
	pause_continue_butt->setVisible(true);

	gameover_home_butt->setVisible(false);
	gameover_replay_butt->setVisible(false);
	gameover_leader_butt->setVisible(false);
	//gameover_text->setVisible(false);
	gameover_score_text->setVisible(false);
	gameover_score_shine->setVisible(false);
	gameover_highscore_text->setVisible(false);
}

void HelloWorld::show_gameover_menu()
{
	pause_home_butt->setVisible(false);
	pause_continue_butt->setVisible(false);

	gameover_home_butt->setVisible(true);
	gameover_replay_butt->setVisible(true);
	gameover_leader_butt->setVisible(true);
	//gameover_text->setVisible(true);
	gameover_score_text->setVisible(true);
	gameover_score_shine->setVisible(true);
	gameover_highscore_text->setVisible(true);
}

void HelloWorld::hide_all_menus()
{
	pause_home_butt->setVisible(false);
	pause_continue_butt->setVisible(false);

	gameover_home_butt->setVisible(false);
	gameover_replay_butt->setVisible(false);
	gameover_leader_butt->setVisible(false);
	//gameover_text->setVisible(false);
	gameover_score_text->setVisible(false);
	gameover_score_shine->setVisible(false);
	gameover_highscore_text->setVisible(false);

}
void HelloWorld::set_gameover_menu_alpha(float a)
{
	if (state == DEAD)
	{
		gameover_home_butt->setOpacity(a * 255);
		gameover_replay_butt->setOpacity(a * 255);
		gameover_leader_butt->setOpacity(a * 255);
		//gameover_text->setOpacity(a * 255);
		gameover_score_text->setOpacity(a * 255);
		gameover_score_shine->setOpacity(a * 255);
		gameover_highscore_text->setOpacity(a * 255);
	}
}

Scene* HelloWorld::createScene()
{
	auto layer = HelloWorld::create();
	//game_scene_pointer = layer;

	auto scene = Scene::create();

	scene->addChild(layer);
	return scene;
}

bool HelloWorld::init()
{
#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//SonarCocosHelper::AdMob::showBannerAd(0);
#endif

	if (!Layer::init())
		return false;
	game_scene_pointer = this;
	config = levels_data[current_difficulty][0].config;

	auto kb_listener = EventListenerKeyboard::create();
	kb_listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	kb_listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(kb_listener, this);

	auto touch_listener = EventListenerTouchAllAtOnce::create();
	touch_listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	touch_listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

	dead_anim.flash_duration = 0.4;
	dead_anim.gameover_begin_time = 0.3;
	dead_anim.score_begin_time = 0.6;
	dead_anim.highscore_begin_time = 0.9;
	dead_anim.buttons_begin_time = 1.2;
	dead_anim.buttons_delta_time = 0.2;//time between buttons popping u

	dead_anim.duration = dead_anim.buttons_begin_time + 10* dead_anim.buttons_delta_time;

	dead_anim.timer = dead_anim.duration + 1;

	gameover_voice_played = false;
	
	dead_anim.gameover_scale = 0;
	dead_anim.gameover_spring_vel = 0;
	dead_anim.highscore_scale = 0;
	dead_anim.highscore_spring_vel = 0;
	dead_anim.score_scale = 0;
	dead_anim.score_spring_vel = 0;
	dead_anim.home_butt_scale = 0;
	dead_anim.home_butt_spring_vel = 0;
	dead_anim.replay_butt_scale = 0;
	dead_anim.replay_butt_spring_vel = 0;
	dead_anim.acheiv_butt_scale = 0;
	dead_anim.acheiv_butt_spring_vel = 0;
	dead_anim.leader_butt_scale = 0;
	dead_anim.leader_butt_spring_vel = 0;
	dead_anim.twitter_butt_scale = 0;
	dead_anim.twitter_butt_spring_vel = 0;
	
	pause_anim_duration = 10;
	pause_anim_timer = 0;
	pause_home_butt_scale = 0;
	pause_home_butt_spring_vel = 0;
	pause_continue_butt_scale = 0;
	pause_continue_butt_spring_vel = 0;

	resume_anim_timer = 0;
	resume_anim_duration = 0.4;

	left_player = Player::create();
	right_player = Player::create(Player::RIGHT_PLAYER);
	addChild(right_player);
	addChild(left_player);

	left_splitter = Sprite::create("left_ground.png");
	left_splitter->setAnchorPoint(Vec2(0, 0.5));
	left_splitter->setRotation(180);
	left_splitter->setPosition(Vec2(240, designResolutionSize.height / 2 - score_panel_height));
	left_splitter->setBlendFunc(BlendFunc::ADDITIVE);
	left_splitter->setColor(Color3B(0, 255, 0));
	addChild(left_splitter);
	//left_splitter->setVisible(false);

	right_splitter = Sprite::create("left_ground.png");
	right_splitter->setAnchorPoint(Vec2(0, 0.5));
	right_splitter->setPosition(Vec2(240, designResolutionSize.height / 2 - score_panel_height));
	right_splitter->setBlendFunc(BlendFunc::ADDITIVE);
	right_splitter->setColor(Color3B(0, 255, 0));
	addChild(right_splitter);
	//right_splitter->setVisible(false);

	left_ground = Sprite::create("left_ground.png");
	left_ground->setAnchorPoint(Vec2(0, 0.5));
	left_ground->setPosition(Vec2(0, designResolutionSize.height / 2));
	left_ground->setBlendFunc(BlendFunc::ADDITIVE);
	left_ground->setColor(Color3B(0, 255, 0));
	addChild(left_ground);
	//left_ground->setVisible(false);

	right_ground = Sprite::create("left_ground.png");
	right_ground->setAnchorPoint(Vec2(0, 0.5));
	right_ground->setRotation(180);
	right_ground->setPosition(Vec2(480, designResolutionSize.height / 2));
	right_ground->setBlendFunc(BlendFunc::ADDITIVE);
	right_ground->setColor(Color3B(0, 255, 0));
	addChild(right_ground);
	//right_ground->setVisible(false);

	splitter_and_grnd_draw_node = DrawNode::create();
	addChild(splitter_and_grnd_draw_node);


	black_overlay = DrawNode::create();
	black_overlay->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	//black_overlay->drawSolidRect(Vec2(0, 0), Vec2(480, designResolutionSize.height), Color4F(0, 0, 0, 1));
	black_overlay->setVisible(false);

	addChild(black_overlay, 10);


#if OLD_GENERATION_MODEL
	left_obsgen = Obstacle_Generator::create(0, 800);
	left_obsgen->genx[0] = floor_rect_width;
	left_obsgen->genx[1] = 240 - splitter_rect_width / 2;
	left_obsgen->left_generator = 1;
	addChild(left_obsgen);


	right_obsgen = Obstacle_Generator::create(0, 800);
	addChild(right_obsgen);
#else
	obsgen = Obstacle_Generator::create(0, 800);
	addChild(obsgen);

	tut_obsgen = Tut_Obstacle_Generator::create(0, 800);
	addChild(tut_obsgen);
	if (!show_tutorial)
		tut_obsgen->mypause();

#endif

#if ENABLE_EDITOR
	editor = Editor::create();
	editor->setVisible(false);
	addChild(editor);

	edit_butt = ui::Button::create("edit_butt.png");
	edit_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			editor->setVisible(true);
			editor->resume();
			player_dead = true;
			in_editor = true;
		default:
			break;
		}
	});
	edit_butt->setPosition(Vec2(20, 750));
	edit_butt->setScale(0.5f);
	addChild(edit_butt);

	in_editor = false;
#endif


	score_panel_drawnode = DrawNode::create();
	addChild(score_panel_drawnode);
	{
		float score_panel_roof_width = 100;
		float score_panel_base_width = 80;
		float score_panel_base_y = designResolutionSize.height - score_panel_height;
		Vec2 score_panel_polygon[] = { Vec2(240 - score_panel_roof_width,designResolutionSize.height),Vec2(240 + score_panel_roof_width,designResolutionSize.height),
			Vec2(240 + score_panel_base_width,score_panel_base_y),Vec2(240 - score_panel_base_width,score_panel_base_y) };
		Vec2 score_panel_polygon_outline[] = { Vec2(240 - score_panel_roof_width - 4,designResolutionSize.height),Vec2(240 + score_panel_roof_width + 4,designResolutionSize.height),
			Vec2(240 + score_panel_base_width + 4,score_panel_base_y - 4),Vec2(240 - score_panel_base_width - 4,score_panel_base_y - 4) };
		score_panel_drawnode->drawSolidPoly(score_panel_polygon_outline, 4, Color4F(level_themes[current_difficulty].ground_glow));
		score_panel_drawnode->drawSolidPoly(score_panel_polygon, 4, Color4F(level_themes[current_difficulty].ground_obs));
	}
	tut_tut_notice = ui::Text::create("TUTORIAL", "fonts/hemi.ttf", 18);
	tut_tut_notice->setColor(level_themes[current_difficulty].ground_glow);
	tut_tut_notice->setPosition(Vec2(240, designResolutionSize.height - score_panel_height / 2));
	tut_tut_notice->setTextVerticalAlignment(TextVAlignment::CENTER);
	tut_tut_notice->setTextHorizontalAlignment(TextHAlignment::CENTER);
	addChild(tut_tut_notice);
	tut_tut_notice->setVisible(false);

	score_label_two_points = ui::Text::create(":", "fonts/hemi.ttf", 30);
	score_label_two_points->setColor(level_themes[current_difficulty].ground_glow);
	score_label_two_points->setPosition(Vec2(240, designResolutionSize.height - score_panel_height/2));
	score_label_two_points->setTextVerticalAlignment(TextVAlignment::CENTER);
	score_label_two_points->setTextHorizontalAlignment(TextHAlignment::CENTER);
	addChild(score_label_two_points);

	score_label_seconds = ui::Text::create("", "fonts/hemi.ttf", 30);
	score_label_seconds->setColor(level_themes[current_difficulty].ground_glow);
	score_label_seconds->setPosition(Vec2(240 - 38, designResolutionSize.height - score_panel_height / 2));
	score_label_seconds->setTextVerticalAlignment(TextVAlignment::CENTER);
	score_label_seconds->setTextHorizontalAlignment(TextHAlignment::CENTER);
	addChild(score_label_seconds);

	score_label_miliseconds = ui::Text::create("", "fonts/hemi.ttf", 30);
	score_label_miliseconds->setColor(level_themes[current_difficulty].ground_glow);
	score_label_miliseconds->setPosition(Vec2(240 + 38, designResolutionSize.height - score_panel_height / 2));
	score_label_miliseconds->setTextVerticalAlignment(TextVAlignment::CENTER);
	score_label_miliseconds->setTextHorizontalAlignment(TextHAlignment::CENTER);
	addChild(score_label_miliseconds);
	
	if (show_tutorial)
	{
		score_label_two_points->setVisible(false);
		score_label_seconds->setVisible(false);
		score_label_miliseconds->setVisible(false);
	}

	double_tap_condition_time = 500;
	left_player_last_tap = 0;
	right_player_last_tap = 0;

	accum_timer = 0;

	play_time = 0;


	state = PLAYING_LEVEL;
	current_level = 0;
	gameover_score_shine_angle = 0;

	pause_home_butt = ui::Button::create("menu/home_button.png");
	pause_home_butt->setPosition(Vec2(240 - 100, designResolutionSize.height / 2));
	pause_home_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			// cocos does not release the scene when popping..so we reinit the game ourselves
			reinit_game();
			if (!music_muted)
			{

				experimental::AudioEngine::stopAll();
				current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0);
				music_transtion_timer = music_transtion_duration;
			}

			game_scene_pointer = 0;

			if (!music_muted)
			{
				current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
				music_transtion_timer = music_transtion_duration;
			}
			else
			{
				current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
				experimental::AudioEngine::pauseAll();
			}

			auto menu_scene = MenuScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.35f, menu_scene, Color3B(255, 255, 255)));


		}break;
		}
	});

	pause_continue_butt = ui::Button::create("menu/play_button.png");
	pause_continue_butt->setPosition(Vec2(240 + 100, designResolutionSize.height / 2));
	pause_continue_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			state = RESUMING;
		}break;
		}
	});

	gameover_home_butt = ui::Button::create("menu/home_button.png");
	gameover_home_butt->setPosition(Vec2(100, designResolutionSize.height / 2 - 300));
	gameover_home_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			reinit_game();
			experimental::AudioEngine::stopAll();
			if (!music_muted)
			{
				current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.0f);
				music_transtion_timer = music_transtion_duration;
			}
			game_scene_pointer = 0;

			if (!music_muted)
			{
				current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
				music_transtion_timer = music_transtion_duration;
			}
			else
			{
				current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
				experimental::AudioEngine::pauseAll();
			}
			auto menu_scene = MenuScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.35f, menu_scene, Color3B(255, 255, 255)));

		}break;
		}
	});

	gameover_replay_butt = ui::Button::create("menu/replay_button.png");
	gameover_replay_butt->setPosition(Vec2(240, designResolutionSize.height / 2 - 200));
	gameover_replay_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
			reinit_game();
		}break;
		}
	});

	gameover_leader_butt = ui::Button::create("menu/leader_button.png");
	gameover_leader_butt->setPosition(Vec2(380, designResolutionSize.height / 2 - 300));
	gameover_leader_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
		{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
			if (!SonarCocosHelper::GooglePlayServices::isSignedIn())
				SonarCocosHelper::GooglePlayServices::signIn();
			else
			{
				if(highscore[current_difficulty]>=0)
					SonarCocosHelper::GooglePlayServices::submitScore(leaderboard_ids[current_difficulty], (int)(highscore[current_difficulty] * 1000.0));
				SonarCocosHelper::GooglePlayServices::showLeaderboard(leaderboard_ids[current_difficulty]);
			}
#endif
		}break;
		}
	});
	
	/*gameover_text = ui::Text::create("GAME OVER", "fonts/hemi.ttf", 56);
	gameover_text->setColor(Color3B(0, 255, 90));
	gameover_text->setPosition(Vec2(240, 650));
	gameover_text->setScale(0);*/

	gameover_score_text = ui::Text::create("SCORE : ", "fonts/hemi.ttf", 58);
	gameover_score_text->setColor(Color3B(255, 255, 0));
	gameover_score_text->setPosition(Vec2(240, 580));

	gameover_new_record = ui::Text::create("NEW RECORD!", "fonts/hemi.ttf", 15);
	gameover_new_record->setColor(Color3B(255, 255, 0));
	gameover_new_record->setPosition(Vec2(240, 510));
	gameover_new_record->setVisible(false);

	gameover_highscore_text = ui::Text::create("BEST : ", "fonts/hemi.ttf", 22);
	gameover_highscore_text->setColor(Color3B(255, 0, 90));
	gameover_highscore_text->setPosition(Vec2(240, 450));

	gameover_score_shine = Sprite::create("jellycouple_shine.png");
	gameover_score_shine->setColor(Color3B(200, 200, 0));

	//gameover_text->setScale(0);
	gameover_highscore_text->setScale(0);
	gameover_score_text->setScale(0);
	gameover_new_record->setScale(0);
	gameover_score_shine->setScale(0);

	gameover_home_butt->setScale(0);
	gameover_replay_butt->setScale(0);
	gameover_leader_butt->setScale(0);

	pause_home_butt->setScale(0);
	pause_continue_butt->setScale(0);

	addChild(pause_home_butt, 11);
	addChild(pause_continue_butt, 11);
	addChild(gameover_home_butt, 11);
	addChild(gameover_replay_butt, 11);
	addChild(gameover_leader_butt, 11);
	//addChild(gameover_text, 11);
	addChild(gameover_highscore_text, 11);
	addChild(gameover_score_shine, 11);
	addChild(gameover_score_text, 11);
	addChild(gameover_new_record, 11);
	

	hide_all_menus();

	if (show_tutorial)
	{
		tut_tut_notice->setVisible(true);
		tut_tut_notice->setScale(0);
		tut_tut_notice_enter_anim_finished = false;

		left_player->nullify_input = true;
		right_player->nullify_input = true;
		state = TUTORIAL;
		//state = PLAYING_LEVEL;
#if OLD_GENERATION_MODEL
		left_obsgen->mypause();
		right_obsgen->mypause();
#else
		obsgen->mypause();
#endif
		tut_text_scale = 0;
		tut_text_spring_vel = 0;
		tut_flashing_region_sine_timer = 0;
		tut_text_transition_state = NO_TRANSITION;
		tut_did_last_step = false;

		tut_text_timer = 0;
		tut_color = Color4F::WHITE;
		tut_phase = 0;

		tut_text_y = 700;

		tut_goodluck = ui::Text::create("GOOD LUCK!", "fonts/hemi.ttf", 36);
		tut_goodluck->setColor(Color3B::WHITE);
		tut_goodluck->setPosition(Vec2(240, tut_text_y));
		tut_goodluck->setTextVerticalAlignment(TextVAlignment::CENTER);
		tut_goodluck->setTextHorizontalAlignment(TextHAlignment::CENTER);
		addChild(tut_goodluck);
		tut_goodluck->setVisible(false);
		tut_goodluck->enableOutline(Color4B::BLACK, 2);

		tut_label = ui::Text::create(tut_messages[tut_phase], "fonts/hemi.ttf", 18);
		tut_label->setColor(Color3B(tut_color));
		tut_label->setTextVerticalAlignment(TextVAlignment::CENTER);
		tut_label->setTextHorizontalAlignment(TextHAlignment::CENTER);
		tut_label->setPosition(Vec2(240, tut_text_y));
		tut_label->setScale(0);
		tut_label->enableOutline(Color4B::BLACK, 2);

		tut_region_indicator = DrawNode::create();
		tut_region_indicator->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
		addChild(tut_region_indicator);

		tut_info_region = DrawNode::create();
		tut_info_region->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
		tut_info_region->setPosition(Vec2(240, tut_text_y));
		addChild(tut_info_region);

		

		addChild(tut_label);

		/*tut_skip_button = ui::Button::create();
		tut_skip_button->setTitleText("SKIP");
		tut_skip_button->setTitleFontName("fonts/hemi.ttf");
		tut_skip_button->setTitleFontSize(24);
		tut_skip_button->setTitleColor(Color3B(tut_color));
		tut_skip_button->setPosition(Vec2(240, 100));
		tut_skip_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::ENDED:
			{
				tut_phase = 6;
				tut_text_transition_state = EXIT;
				tut_text_timer = 0;
				tut_skip_button->setVisible(false);
			}break;
			}
		});

		addChild(tut_skip_button);*/
	}

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	SonarCocosHelper::AdMob::preLoadFullscreenAd();
#endif

	floating_menu_sine_timer = 0;
	good_luck_stay_timer = 0;

	left_trail_emitter = ParticleSystemQuad::create("trail_ps.plist");
	left_trail_emitter->setPosition(Vec2(240 - splitter_rect_width / 2, left_player->inity));
	left_trail_emitter->setGravity(Vec2(-left_trail_emitter->getGravity().x, left_trail_emitter->getGravity().y));
	left_trail_emitter->setTangentialAccel(-left_trail_emitter->getTangentialAccel());
	left_trail_init_grativyx = left_trail_emitter->getGravity().x;
	left_trail_init_tangent_accel = left_trail_emitter->getTangentialAccel();
	addChild(left_trail_emitter, -1);

	right_trail_emitter = ParticleSystemQuad::create("trail_ps.plist");
	right_trail_emitter->setPosition(Vec2(240 + splitter_rect_width / 2, right_player->inity));
	right_trail_init_grativyx = right_trail_emitter->getGravity().x;
	right_trail_init_tangent_accel = right_trail_emitter->getTangentialAccel();
	addChild(right_trail_emitter, -1);

	left_trail_emitter_paused = false;
	right_trail_emitter_paused = false;

	CCLOG("GAME_5");

	background_emitter = ParticleSystemQuad::create("background_ps.plist");
	background_emitter->setAnchorPoint(Vec2(0, 0));
	background_emitter->setPosition(Vec2(0, 0));
	addChild(background_emitter, -1);

	left_backgrounds[0] = Sprite::create("game_bg.png");
	//left_backgrounds[0]->setAnchorPoint(Vec2(0, 0));
	left_backgrounds[0]->setColor(Color3B(170, 80, 170));
	left_backgrounds[0]->setBlendFunc(BlendFunc::ADDITIVE);
	left_backgrounds[0]->setPosition(Vec2(designResolutionSize.width / 2 - left_backgrounds[0]->getTexture()->getContentSize().width / 2, designResolutionSize.height / 2));
	addChild(left_backgrounds[0], -10);
	//left_backgrounds[0]->setVisible(false);

	left_backgrounds[1] = Sprite::create("game_bg.png");
	//left_backgrounds[1]->setAnchorPoint(Vec2(0, 0));
	left_backgrounds[1]->setColor(Color3B(170, 80, 170));
	left_backgrounds[1]->setPosition(Vec2(designResolutionSize.width / 2 - left_backgrounds[0]->getTexture()->getContentSize().width / 2,
		designResolutionSize.height / 2 + left_backgrounds[0]->getTexture()->getContentSize().height));
	left_backgrounds[1]->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(left_backgrounds[1], -10);
	//left_backgrounds[1]->setVisible(false);

	right_backgrounds[0] = Sprite::create("game_bg.png");
	//right_backgrounds[0]->setAnchorPoint(Vec2(0, 0));
	right_backgrounds[0]->setColor(Color3B(170, 80, 170));
	right_backgrounds[0]->setBlendFunc(BlendFunc::ADDITIVE);
	right_backgrounds[0]->setPosition(Vec2(designResolutionSize.width / 2 + left_backgrounds[0]->getTexture()->getContentSize().width / 2, designResolutionSize.height / 2));
	addChild(right_backgrounds[0], -10);
	//right_backgrounds[0]->setVisible(false);

	right_backgrounds[1] = Sprite::create("game_bg.png");
	//right_backgrounds[1]->setAnchorPoint(Vec2(0, 0));
	right_backgrounds[1]->setColor(Color3B(170, 80, 170));
	right_backgrounds[1]->setPosition(Vec2(designResolutionSize.width / 2 + left_backgrounds[0]->getTexture()->getContentSize().width / 2,
		designResolutionSize.height / 2 + left_backgrounds[0]->getTexture()->getContentSize().height));
	right_backgrounds[1]->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(right_backgrounds[1], -10);
	//right_backgrounds[1]->setVisible(false);


	left_player->setColor(level_themes[current_difficulty].left_player);
	right_player->setColor(level_themes[current_difficulty].right_player);
	splitter_and_grnd_draw_node->clear();
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(240 - splitter_rect_width / 2, 0), Vec2(240 + splitter_rect_width / 2, designResolutionSize.height - score_panel_height), Color4F(level_themes[current_difficulty].ground_glow));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(240 - splitter_rect_width / 2 + ground_outline, 0), Vec2(240 + splitter_rect_width / 2 - ground_outline, designResolutionSize.height - score_panel_height), Color4F(level_themes[current_difficulty].ground_obs));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(480 - floor_rect_width, 0), Vec2(480, designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_glow));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(480 - floor_rect_width + (ground_outline + 1), 0), Vec2(480, designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_obs));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(0, 0), Vec2(floor_rect_width, designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_glow));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(0, 0), Vec2(floor_rect_width - (ground_outline + 1), designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_obs));
	left_splitter->setColor(level_themes[current_difficulty].ground_glow);
	right_splitter->setColor(level_themes[current_difficulty].ground_glow);
	left_ground->setColor(level_themes[current_difficulty].ground_glow);
	right_ground->setColor(level_themes[current_difficulty].ground_glow);
	left_trail_emitter->setStartColor(Color4F(level_themes[current_difficulty].ground_obs));
	left_trail_emitter->setEndColor(Color4F(level_themes[current_difficulty].ground_obs));
	right_trail_emitter->setStartColor(Color4F(level_themes[current_difficulty].ground_obs));
	right_trail_emitter->setEndColor(Color4F(level_themes[current_difficulty].ground_obs));
	left_backgrounds[0]->setColor(level_themes[current_difficulty].right_player);
	left_backgrounds[1]->setColor(level_themes[current_difficulty].right_player);
	right_backgrounds[0]->setColor(level_themes[current_difficulty].left_player);
	right_backgrounds[1]->setColor(level_themes[current_difficulty].left_player);
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obsgen->obstacles_pool[i]->setColor(level_themes[current_difficulty].ground_obs);
		obsgen->obstacles_glow_sprites[i]->setColor(level_themes[current_difficulty].ground_glow);
	}
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		tut_obsgen->obstacles_pool[i]->setColor(level_themes[current_difficulty].ground_obs);
		tut_obsgen->obstacles_glow_sprites[i]->setColor(level_themes[current_difficulty].ground_glow);
	}


	showed_new_record_notice = false;

	new_record_y = 0;
	new_record_move_timer = 0;
	new_record_state = -1;
	new_record_label = ui::Text::create("New Record!","fonts/hemi.ttf",20);
	new_record_label->setAnchorPoint(Vec2(0.5f, 1));
	new_record_label->setPosition(Vec2(240, -100));
	//new_record_label->setColor(Color3B(255-level_themes[current_difficulty].ground_obs.r, 255 - level_themes[current_difficulty].ground_obs.g, 255 - level_themes[current_difficulty].ground_obs.b));
	new_record_label->setColor(level_themes[current_difficulty].ground_glow);
	new_record_label->setTextVerticalAlignment(TextVAlignment::CENTER);
	new_record_label->setTextHorizontalAlignment(TextHAlignment::CENTER);

	new_record_region = DrawNode::create();
	new_record_region->setAnchorPoint(Vec2(0, 1));
	new_record_region->setPosition(Vec2(0, -100));
	new_record_region->clear();
	new_record_region->drawSolidRect(Vec2(-ground_outline, 0), Vec2(100 - splitter_rect_width / 2, 50 + ground_outline +1), Color4F(level_themes[current_difficulty].ground_glow));
	new_record_region->drawSolidRect(Vec2(100 + splitter_rect_width / 2, 0), Vec2(200 + ground_outline, 50 + ground_outline +1), Color4F(level_themes[current_difficulty].ground_glow));
	new_record_region->drawSolidRect(Vec2(0, 0), Vec2(200, 50), Color4F(level_themes[current_difficulty].ground_obs));

	addChild(new_record_region);
	addChild(new_record_label);

	float congrats_h = 120;
	congrats_drawnode = DrawNode::create();
	congrats_drawnode->clear();
	congrats_drawnode->drawSolidRect(Vec2(0, 400 - congrats_h / 2), Vec2(480, 400 + congrats_h / 2), Color4F::BLACK);

	congrats_tap_drawnode = DrawNode::create();
	{
		float tap_panel_roof_width = 160;
		float tap_panel_base_width = 140;
		float tap_panel_base_y = 400 - congrats_h / 2 - 70;
		float tap_panel_roof_y = 400 - congrats_h / 2 - 30;
		Vec2 tap_panel_polygon[] = { Vec2(240 - tap_panel_roof_width,tap_panel_roof_y),Vec2(240 + tap_panel_roof_width,tap_panel_roof_y),
			Vec2(240 + tap_panel_base_width,tap_panel_base_y),Vec2(240 - tap_panel_base_width,tap_panel_base_y) };
		Vec2 tap_panel_polygon_outline[] = { Vec2(240 - tap_panel_roof_width - 4,tap_panel_roof_y+4),Vec2(240 + tap_panel_roof_width + 4,tap_panel_roof_y+4),
			Vec2(240 + tap_panel_base_width + 4,tap_panel_base_y - 4),Vec2(240 - tap_panel_base_width - 4,tap_panel_base_y - 4) };
		congrats_tap_drawnode->drawSolidPoly(tap_panel_polygon_outline, 4, Color4F::WHITE);
		congrats_tap_drawnode->drawSolidPoly(tap_panel_polygon, 4, Color4F::BLACK);
	}

	congrats_text = ui::Text::create("CONGRATULATIONS", "fonts/hemi.ttf", 27);
	congrats_text->setPosition(Vec2(240,400+congrats_h/2-25));
	congrats_text->setColor(Color3B::WHITE);
	congrats_text->setTextVerticalAlignment(TextVAlignment::CENTER);
	congrats_text->setTextHorizontalAlignment(TextHAlignment::CENTER);

	congrats_complete_text = ui::Text::create("LEVEL COMPLETE", "fonts/hemi.ttf", 16);
	congrats_complete_text->setPosition(Vec2(240, 400 + congrats_h / 2 - 50));
	congrats_complete_text->setColor(Color3B::WHITE);
	congrats_complete_text->setTextVerticalAlignment(TextVAlignment::CENTER);
	congrats_complete_text->setTextHorizontalAlignment(TextHAlignment::CENTER);

	congrats_unlock_text = ui::Text::create("", "fonts/hemi.ttf", 16);
	congrats_unlock_text->setPosition(Vec2(240, 400 + congrats_h / 2 - 90));
	congrats_unlock_text->setColor(Color3B::WHITE);
	congrats_unlock_text->setTextVerticalAlignment(TextVAlignment::CENTER);
	congrats_unlock_text->setTextHorizontalAlignment(TextHAlignment::CENTER);

	congrats_tap_text = ui::Text::create("Tap to continue", "fonts/hemi.ttf", 16);
	congrats_tap_text->setPosition(Vec2(240, 400 - congrats_h / 2 - 50));
	congrats_tap_text->setColor(Color3B::WHITE);
	congrats_tap_text->setTextVerticalAlignment(TextVAlignment::CENTER);
	congrats_tap_text->setTextHorizontalAlignment(TextHAlignment::CENTER);


	addChild(congrats_drawnode);
	addChild(congrats_tap_drawnode);
	addChild(congrats_text);
	addChild(congrats_unlock_text);
	addChild(congrats_complete_text);
	addChild(congrats_tap_text);

	congrats_drawnode->setVisible(false);
	congrats_tap_text->setVisible(false);
	congrats_text->setVisible(false);
	congrats_unlock_text->setVisible(false);
	congrats_complete_text->setVisible(false);
	congrats_tap_drawnode->setVisible(false);

	congrats_colinter.init(0.2f);
	congrats_colinter.add_color(Color3B(255, 255, 255));
	congrats_colinter.add_color(Color3B(128, 128, 128));
	
	congrats_show_timer = 0;

	return true;
}

void HelloWorld::onEnterTransitionDidFinish()
{
	scheduleUpdate();
}

void HelloWorld::fixed_update(float dt)
{
}

void HelloWorld::update(float dt)
{
	if (new_record_state >= 0)
	{
		float new_record_move_duration = 1;
		new_record_move_timer += dt;
		if (new_record_state == 0)
		{
			float t = new_record_move_timer / (new_record_move_duration);
			new_record_y = coserp(0, t, 30);
			new_record_label->setPosition(Vec2(240, new_record_y));
			new_record_region->setPosition(Vec2(240 - 100, new_record_y - 50));

			if (new_record_move_timer > (new_record_move_duration))
			{
				new_record_state = 1;
				new_record_move_timer = 0;
			}
		}
		else
		{
			if (new_record_move_timer > 2.5f*new_record_move_duration)
			{
				float t = (new_record_move_timer- 2.5f*new_record_move_duration) / (new_record_move_duration);
				new_record_y = coserp(30, t, 0);
				new_record_label->setPosition(Vec2(240, new_record_y));
				new_record_region->setPosition(Vec2(240 - 100, new_record_y - 50));

				if ((new_record_move_timer- 2.5f*new_record_move_duration) > (new_record_move_duration))
				{
					new_record_move_timer = 0;
					new_record_state = -1;
					new_record_label->setPosition(Vec2(240, -100));
					new_record_region->setPosition(Vec2(0, -100));
				}
			}
		}
	}

	if (state != DEAD && state!=CONGRATS && state!=PAUSE)
	{
		for (int i = 0; i < 2; i++)
		{
			float background_depth = 2.f;
			{
				Vec2 pos = left_backgrounds[i]->getPosition();

				pos.y -= config.obs_vely / background_depth*dt*time_scale;
				if (pos.y + left_backgrounds[i]->getTexture()->getContentSize().height / 2 < 0)
					pos.y += 2 * left_backgrounds[i]->getTexture()->getContentSize().height;
				left_backgrounds[i]->setPosition(pos);
			}
			{
				Vec2 pos = right_backgrounds[i]->getPosition();

				pos.y -= config.obs_vely / background_depth*dt*time_scale;
				if (pos.y + right_backgrounds[i]->getTexture()->getContentSize().height / 2 < 0)
					pos.y += 2 * right_backgrounds[i]->getTexture()->getContentSize().height;
				right_backgrounds[i]->setPosition(pos);
			}
		}
		//make sure that there is no gap between the two backgrounds
		if (right_backgrounds[0]->getPosition().y < right_backgrounds[1]->getPosition().y)
		{
			right_backgrounds[1]->setPosition(right_backgrounds[0]->getPosition() + Vec2(0,right_backgrounds[0]->getTexture()->getContentSize().height));
		}
		if (left_backgrounds[0]->getPosition().y < left_backgrounds[1]->getPosition().y)
		{
			left_backgrounds[1]->setPosition(left_backgrounds[0]->getPosition() + Vec2(0, left_backgrounds[0]->getTexture()->getContentSize().height));
		}
		{
			score_panel_drawnode->clear();
			float score_panel_roof_width = 100;
			float score_panel_base_width = 80;
			float score_panel_base_y = designResolutionSize.height - score_panel_height;
			Vec2 score_panel_polygon[] = { Vec2(240 - score_panel_roof_width,designResolutionSize.height),Vec2(240 + score_panel_roof_width,designResolutionSize.height),
				Vec2(240 + score_panel_base_width,score_panel_base_y),Vec2(240 - score_panel_base_width,score_panel_base_y) };
			Vec2 score_panel_polygon_outline[] = { Vec2(240 - score_panel_roof_width - 4,designResolutionSize.height),Vec2(240 + score_panel_roof_width + 4,designResolutionSize.height),
				Vec2(240 + score_panel_base_width + 4,score_panel_base_y - 4),Vec2(240 - score_panel_base_width - 4,score_panel_base_y - 4) };
			score_panel_drawnode->drawSolidPoly(score_panel_polygon_outline, 4, Color4F(level_themes[current_difficulty].ground_glow));
			score_panel_drawnode->drawSolidPoly(score_panel_polygon, 4, Color4F(level_themes[current_difficulty].ground_obs));
		}
	}
	

	//right_ground->setVisible(false);

	screen_shake_effect.update(dt);

	if (left_player->gravity_shifted==1)
	{
		left_trail_emitter->setGravity(Vec2(-left_trail_init_grativyx, left_trail_emitter->getGravity().y));
		left_trail_emitter->setTangentialAccel(-left_trail_init_tangent_accel);
		left_trail_emitter->setPosition(Vec2(floor_rect_width, right_player->inity));
	}
	else
	{
		left_trail_emitter->setGravity(Vec2(left_trail_init_grativyx, left_trail_emitter->getGravity().y));
		left_trail_emitter->setTangentialAccel(left_trail_init_tangent_accel);
		left_trail_emitter->setPosition(Vec2(240 - splitter_rect_width / 2, left_player->inity));
	}

	if (right_player->gravity_shifted==1)
	{
		right_trail_emitter->setGravity(Vec2(-right_trail_init_grativyx, right_trail_emitter->getGravity().y));
		right_trail_emitter->setTangentialAccel(-right_trail_init_tangent_accel);
		right_trail_emitter->setPosition(Vec2(480 - floor_rect_width, right_player->inity));
	}
	else
	{
		right_trail_emitter->setGravity(Vec2(right_trail_init_grativyx, right_trail_emitter->getGravity().y));
		right_trail_emitter->setTangentialAccel(right_trail_init_tangent_accel);
		right_trail_emitter->setPosition(Vec2(240 + splitter_rect_width / 2, right_player->inity));
	}

	if (left_player->state == Player::STANDING)
	{
		if (left_trail_emitter_paused)
			left_trail_emitter->resetSystem();
		left_trail_emitter_paused = false;
	}
	else if (left_player->state != Player::CROUCHING)
	{
		left_trail_emitter->stopSystem();
		left_trail_emitter_paused = true;
	}

	if (right_player->state == Player::STANDING)
	{
		if (right_trail_emitter_paused)
			right_trail_emitter->resetSystem();
		right_trail_emitter_paused = false;
	}
	else if (right_player->state != Player::CROUCHING)
	{
		right_trail_emitter->stopSystem();
		right_trail_emitter_paused = true;
	}


	if (state == TUTORIAL)
	{
		tut_region_indicator->setVisible(false);
		tut_info_region->setVisible(false);
		if (tut_obsgen->paused || time_scale<1)
		{
			tut_region_indicator->setVisible(true);
			//tut_info_region->setVisible(true);
		}


		dead_anim.timer += dt;
		if (!last_player_dead&&player_dead)
		{
			dead_anim.timer = 0;

			black_overlay->setVisible(true);

			left_trail_emitter->pause();
			right_trail_emitter->pause();

			if (!music_muted)
				experimental::AudioEngine::stopAll();
			if (!sfx_muted)
				experimental::AudioEngine::play2d(DIE_SFX);

			left_player->mypause();
			right_player->mypause();

			tut_obsgen->mypause();
			//erase player actions
			for (int i = 0; i < 2; i++)
				tut_player_crouched[i] = tut_player_jumped[i] = tut_player_shifted[i] = 0;

			time_scale = 1;
		}
		if (player_dead)
		{
			tut_text_transition_state = END;
			tut_region_indicator->setVisible(false);
			tut_info_region->setVisible(false);
			tut_label->setVisible(false);

			if (dead_anim.timer < dead_anim.flash_duration)
			{
				float t = dead_anim.timer / dead_anim.flash_duration;

				float alpha = lerp(1, t, 0);
				float color_value = lerp(1, t, 0.0f);
				black_overlay->clear();
				black_overlay->drawSolidRect(Vec2(0, 0), Vec2(480, designResolutionSize.height), Color4F(color_value, color_value, color_value, alpha));
			}
			else
			{
				reinit_game();
				state = TUTORIAL;
				obsgen->mypause();
				left_player->nullify_input = true;
				right_player->nullify_input = true;
			}
		}

		simulate_spring(dt, ui_spring_damp*1.5f, ui_spring_tightness, 1, &tut_text_scale, &tut_text_spring_vel);
		tut_label->setScale(tut_text_scale);

		if (!tut_tut_notice_enter_anim_finished)
		{
			tut_tut_notice->setScale(tut_text_scale);
			if (fabs(tut_text_spring_vel) < 0.01f)
				tut_tut_notice_enter_anim_finished = true;
		}
		else
			tut_tut_notice->setScale(1);

		tut_region_indicator->clear();
		tut_info_region->clear();

		tut_info_region->drawSolidRect(Vec2(-240, 60), Vec2(240, -60), Color4F(0.6f, 0.6f, 0.6f, 0.6f));
		tut_info_region->drawSolidRect(Vec2(-240, -58), Vec2(240, -60), Color4F(1, 1,1, 1));
		tut_info_region->drawSolidRect(Vec2(-240, 58), Vec2(240, 60), Color4F(1, 1, 1, 1));

		if (tut_text_transition_state == NO_TRANSITION)
		{
			tut_label->setVisible(false);
			if(tut_obsgen->paused || time_scale <1)
				tut_label->setVisible(true);

			tut_flashing_region_sine_timer += dt;
			float sine_period = 1.0f;
			float sin_value = sinf(2 * 3.14 * tut_flashing_region_sine_timer / sine_period);
			sin_value = (sin_value + 1) * 0.5f * 0.7f;
			if (tut_flashing_region_sine_timer > sine_period)
				tut_flashing_region_sine_timer -= sine_period;

			if (tut_phase == 0)
			{
				tut_region_indicator->drawSolidRect(Vec2(3 * 480.f / 4, 0), Vec2(480, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(0, 0), Vec2(480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				if (!tut_obsgen->paused && tut_obsgen->jump_obs && tut_obsgen->jump_obs->y < 300)
				{
					tut_obsgen->mypause();
					left_trail_emitter->pause();
					right_trail_emitter->pause();
					tut_text_scale = 0;
					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}

				if (tut_obsgen->paused && (left_player->state != Player::STANDING || right_player->state != Player::STANDING))
				{
					tut_phase++;
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_obsgen->myresume();
					left_trail_emitter->resume();
					right_trail_emitter->resume();
				}
			}
			else if (tut_phase == 1)
			{
				tut_region_indicator->drawSolidRect(Vec2(480.f / 2, 0), Vec2(3 * 480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(480 / 4, 0), Vec2(480 / 2, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));

				//nullify input as soon as the player gets on ground
				if (left_player->last_state != Player::STANDING && left_player->state == Player::STANDING)
					left_player->nullify_input = true;
				if (right_player->last_state != Player::STANDING && right_player->state == Player::STANDING)
					right_player->nullify_input = true;

				if (!tut_obsgen->paused && tut_obsgen->crouch_obs && tut_obsgen->crouch_obs->y < 300)
				{
					tut_obsgen->mypause();
					left_trail_emitter->pause();
					right_trail_emitter->pause();
					tut_text_scale = 0;
					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}

				if (tut_obsgen->paused && (left_player->state != Player::STANDING || right_player->state != Player::STANDING))
				{
					tut_phase++;
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;

					tut_obsgen->myresume();
					left_trail_emitter->resume();
					right_trail_emitter->resume();
				}
			}
			else if (tut_phase == 2)
			{
				tut_region_indicator->drawSolidRect(Vec2(3 * 480.f / 4, 0), Vec2(480, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(0, 0), Vec2(480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));


				if (left_player->last_state != Player::STANDING && left_player->state == Player::STANDING)
					left_player->nullify_input = true;
				if (right_player->last_state != Player::STANDING && right_player->state == Player::STANDING)
					right_player->nullify_input = true;

				if (!tut_obsgen->paused && tut_obsgen->shift_obs1 && tut_obsgen->shift_obs1->y < 300)
				{
					tut_obsgen->mypause();
					left_trail_emitter->pause();
					right_trail_emitter->pause();
					tut_text_scale = 0;
					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}

				if (tut_obsgen->paused && (left_player->state!=Player::STANDING|| right_player->state != Player::STANDING))
				{
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_phase++;

					tut_obsgen->myresume();
					left_trail_emitter->resume();
					right_trail_emitter->resume();
				}
			}
			else if (tut_phase == 3)
			{
				tut_region_indicator->drawSolidRect(Vec2(3 * 480.f / 4, 0), Vec2(480, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(0, 0), Vec2(480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));

				if (left_player->x<180)
				{
					time_scale = 0.1f;
				}

				if ((left_player->state == Player::SHIFTING || right_player->state == Player::SHIFTING)
					|| (left_player->state == Player::JUMPING && left_player->elapsed_since_jumpkey_pressed>left_player->max_double_press_interval)
					|| (right_player->state == Player::JUMPING && right_player->elapsed_since_jumpkey_pressed>right_player->max_double_press_interval))

				{
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_phase++;
					time_scale = 1;
				}
			}
			else if (tut_phase == 4)
			{
				tut_region_indicator->drawSolidRect(Vec2(480.f / 2, 0), Vec2(3 * 480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(480 / 4, 0), Vec2(480 / 2, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));

				if (left_player->last_state != Player::STANDING && left_player->state == Player::STANDING)
					left_player->nullify_input = true;
				if (right_player->last_state != Player::STANDING && right_player->state == Player::STANDING)
					right_player->nullify_input = true;

				if (!tut_obsgen->paused && tut_obsgen->jump_obs && tut_obsgen->jump_obs->y < 300)
				{
					tut_obsgen->mypause();
					left_trail_emitter->pause();
					right_trail_emitter->pause();
					tut_text_scale = 0;
					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}

				if (tut_obsgen->paused && (left_player->state != Player::STANDING || right_player->state != Player::STANDING))
				{
					tut_phase++;
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_obsgen->myresume();
					left_trail_emitter->resume();
					right_trail_emitter->resume();
				}
			}
			else if (tut_phase == 5)
			{
				tut_region_indicator->drawSolidRect(Vec2(3 * 480.f / 4, 0), Vec2(480, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(0, 0), Vec2(480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				
				if (left_player->last_state != Player::STANDING && left_player->state == Player::STANDING)
					left_player->nullify_input = true;
				if (right_player->last_state != Player::STANDING && right_player->state == Player::STANDING)
					right_player->nullify_input = true;

				if (!tut_obsgen->paused && tut_obsgen->crouch_obs && tut_obsgen->crouch_obs->y < 300)
				{
					tut_obsgen->mypause();
					left_trail_emitter->pause();
					right_trail_emitter->pause();
					tut_text_scale = 0;
					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}

				if (tut_obsgen->paused && (left_player->state != Player::STANDING || right_player->state != Player::STANDING))
				{
					tut_phase++;
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;

					tut_obsgen->myresume();
					left_trail_emitter->resume();
					right_trail_emitter->resume();
				}
			}
			else if (tut_phase == 6)
			{
				tut_region_indicator->drawSolidRect(Vec2(480.f / 2, 0), Vec2(3 * 480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(480 / 4, 0), Vec2(480 / 2, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));

				//nullify input as soon as the player gets on ground
				if(left_player->last_state!=Player::STANDING && left_player->state==Player::STANDING)
					left_player->nullify_input = true;
				if (right_player->last_state != Player::STANDING && right_player->state == Player::STANDING)
					right_player->nullify_input = true;

				
				if (!tut_obsgen->paused && tut_obsgen->shift_obs2 && tut_obsgen->shift_obs2->y < 300)
				{
					tut_obsgen->mypause();
					left_trail_emitter->pause();
					right_trail_emitter->pause();
					tut_text_scale = 0;
					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}

				if (tut_obsgen->paused && (left_player->state != Player::STANDING || right_player->state != Player::STANDING))
				{
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_phase++;

					tut_obsgen->myresume();
					left_trail_emitter->resume();
					right_trail_emitter->resume();
				}
			}
			else if (tut_phase == 7)
			{
				tut_region_indicator->drawSolidRect(Vec2(480.f / 2, 0), Vec2(3 * 480 / 4, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));
				tut_region_indicator->drawSolidRect(Vec2(480 / 4, 0), Vec2(480 / 2, designResolutionSize.height), Color4F(tut_color.r, tut_color.g, tut_color.b, sin_value));

				if (left_player->x>60)
				{
					time_scale = 0.1f;
				}

				if ((left_player->state == Player::SHIFTING || right_player->state == Player::SHIFTING)
					|| (left_player->state == Player::JUMPING && left_player->elapsed_since_jumpkey_pressed>left_player->max_double_press_interval)
					|| (right_player->state == Player::JUMPING && right_player->elapsed_since_jumpkey_pressed>right_player->max_double_press_interval))

				{
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_phase++;
					time_scale = 1;
					tut_goodluck->setScale(0);
					tut_goodluck->setVisible(true);
					good_luck_stay_timer = 0;
				}
			}
			else if (tut_phase == 8)
			{
				good_luck_stay_timer += dt;
				
				if (good_luck_stay_timer>1.f)
				{
					tut_text_transition_state = EXIT;
					tut_text_timer = 0;
					tut_phase++;
					tut_region_indicator->setVisible(false);
					tut_info_region->setVisible(false);

					left_player->nullify_input = false;
					right_player->nullify_input = false;
				}
				if(good_luck_stay_timer<0.5f)
					tut_goodluck->setScale(sqrtf(good_luck_stay_timer*2));
			}
		}
		else if(tut_text_transition_state == EXIT)
		{
			//erase player actions
			for (int i = 0; i < 2; i++)
				tut_player_crouched[i] = tut_player_jumped[i] = tut_player_shifted[i] = 0;

			tut_text_timer += dt;
			float transition_duration = 0.1f;
			float t = tut_text_timer / transition_duration;
			float txt_scale = coserp(tut_text_scale, t, 0);
			tut_label->setScale(txt_scale);
			

			if (tut_phase == NUM_TUTORIAL_PHASES)
			{
				//tut_skip_button->setScale(txt_scale);
				tut_info_region->setScale(txt_scale);
				tut_tut_notice->setScale(txt_scale);
				tut_goodluck->setScale(txt_scale*txt_scale);
			}

			if (tut_text_timer > transition_duration)
			{
				if (tut_phase == NUM_TUTORIAL_PHASES)
				{
					state = PLAYING_LEVEL;
#if OLD_GENERATION_MODEL
					left_obsgen->reinit();
					right_obsgen->reinit();
					left_obsgen->myresume();
					right_obsgen->myresume();
#else
					obsgen->reinit();
					obsgen->myresume();
#endif
					score_label_two_points->setVisible(true);
					score_label_seconds->setVisible(true);
					score_label_miliseconds->setVisible(true);

					tut_tut_notice->setVisible(false);
					tut_label->setVisible(false);
					//tut_skip_button->setVisible(false);

					tut_region_indicator->clear();
					tut_region_indicator->setVisible(false);
					tut_info_region->setVisible(false);
					tut_goodluck->setVisible(false);

					first_game = 0;
					show_tutorial = 0;
					UserDefault::getInstance()->setIntegerForKey("SHOW_TUTORAIL", 0);
				}
				else
				{
					tut_label->setString(tut_messages[tut_phase]);
					tut_text_transition_state = NO_TRANSITION;
					tut_text_timer = 0;
					tut_text_scale = 0;
				}
			}
		}

		//config.obs_vely += 10 * dt;
		float obs_vel_ratio = config.obs_vely / 240.f;
		config.gravity_acc = 1500 * obs_vel_ratio * obs_vel_ratio;
		config.jump_impulsion = 550 * obs_vel_ratio;
		config.crouch_spring_damp = 6.375*config.obs_vely / 170.0f;
		config.crouch_duration = 170 / config.obs_vely;
		config.jump_prep_duration = 38.4f / config.obs_vely;
	}
	else
	{
		if (state == CONGRATS)
		{
			congrats_show_timer += dt;
			float t = congrats_show_timer / 0.5f;
			congrats_drawnode->setVisible(true);
			if (congrats_show_timer < 0.5f)
			congrats_drawnode->setPosition(coserp(-480, t, 0), 0);
			else
			{
				congrats_drawnode->setPosition(0, 0);
				congrats_drawnode->setVisible(true);
				congrats_text->setVisible(true);
				congrats_unlock_text->setVisible(true);
				congrats_complete_text->setVisible(true);
				congrats_tap_drawnode->setVisible(true);
				congrats_tap_text->setVisible(true);
			}
			if (congrats_show_timer < dead_anim.flash_duration)
			{
				float t = congrats_show_timer / dead_anim.flash_duration;

				float alpha = lerp(1, t, 0);
				float color_value = lerp(1, t, 0.0f);
				black_overlay->clear();
				black_overlay->drawSolidRect(Vec2(0, 0), Vec2(480, designResolutionSize.height), Color4F(color_value, color_value, color_value, alpha));
			}

			{
				float congrats_h = 120;
				float tap_panel_roof_width = 160;
				float tap_panel_base_width = 140;
				float tap_panel_base_y = 400 - congrats_h / 2 - 70;
				float tap_panel_roof_y = 400 - congrats_h / 2 - 30;
				Vec2 tap_panel_polygon[] = { Vec2(240 - tap_panel_roof_width,tap_panel_roof_y),Vec2(240 + tap_panel_roof_width,tap_panel_roof_y),
					Vec2(240 + tap_panel_base_width,tap_panel_base_y),Vec2(240 - tap_panel_base_width,tap_panel_base_y) };
				Vec2 tap_panel_polygon_outline[] = { Vec2(240 - tap_panel_roof_width - 4,tap_panel_roof_y + 4),Vec2(240 + tap_panel_roof_width + 4,tap_panel_roof_y + 4),
					Vec2(240 + tap_panel_base_width + 4,tap_panel_base_y - 4),Vec2(240 - tap_panel_base_width - 4,tap_panel_base_y - 4) };
				congrats_tap_drawnode->drawSolidPoly(tap_panel_polygon_outline, 4, Color4F(congrats_colinter.current_color));
				congrats_tap_drawnode->drawSolidPoly(tap_panel_polygon, 4, Color4F::BLACK);
			}

			congrats_colinter.update(dt);
			//congrats_tap_text->setColor(congrats_colinter.current_color);
			congrats_text->setColor(congrats_colinter.current_color);
			congrats_unlock_text->setColor(congrats_colinter.current_color);
			congrats_complete_text->setColor(congrats_colinter.current_color);
			if (congrats_tap)
			{
				games_to_interstetial--;
				reinit_game();
				if (!music_muted)
				{

					experimental::AudioEngine::stopAll();
					current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0);
					music_transtion_timer = music_transtion_duration;
				}

				game_scene_pointer = 0;

				if (!music_muted)
				{
					current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
					music_transtion_timer = music_transtion_duration;
				}
				else
				{
					current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
					experimental::AudioEngine::pauseAll();
				}

				auto menu_scene = MenuScene::createScene();
				Director::getInstance()->replaceScene(TransitionFade::create(0.35f, menu_scene, Color3B(255, 255, 255)));
			}
		}


		if (!player_dead && (state==PLAYING_LEVEL || state==LEVEL_TRANSITION))
			play_time += dt;

		if (state == PLAYING_LEVEL)
		{
			if (play_time > levels_accum_duration[current_difficulty][current_level]&& current_level < NUM_LEVELS - 1)
			{
				state = LEVEL_TRANSITION;
			}
		}
		else if (state == LEVEL_TRANSITION)
		{
			float t = (play_time - levels_accum_duration[current_difficulty][current_level]) / levels_data[current_difficulty][current_level].next_level_transition_time;

			if (t >= 1)
			{
				current_level++;
				config = levels_data[current_difficulty][current_level].config;
				state = PLAYING_LEVEL;
			}
			else
			{
				config = levels_data[current_difficulty][current_level + 1].config;
				config.obs_vely = lerp(levels_data[current_difficulty][current_level].config.obs_vely, t, levels_data[current_difficulty][current_level + 1].config.obs_vely);
			}

		}

		if (!showed_new_record_notice && play_time > highscore[current_difficulty])
		{
			if (UserDefault::getInstance()->getFloatForKey(highscores_store_key[current_difficulty], -1) >= 0)
			{
				if (!sfx_muted)
					experimental::AudioEngine::play2d(NEW_RECORD_SFX);
				new_record_state = 0;
			}

			gameover_new_record->setVisible(true);
			showed_new_record_notice = true;
		}

		if (!last_player_dead&&player_dead)
		{
			//check if congrats
			if (play_time > 60 && highscore[current_difficulty] < 60)
			{
				congrats_tap = false;
				state = CONGRATS;

				if (current_difficulty == 1)
					congrats_unlock_text->setText("YOU UNLOCKED HARD MODE");
				else if (current_difficulty == 2)
					congrats_unlock_text->setText("YOU UNLOCKED SUPER HARD MODE");
				else
					congrats_unlock_text->setText("");
			}
			else
			{
				state = DEAD;
				show_gameover_menu();
				dead_anim.timer = 0;
				games_to_interstetial--;
			}

			black_overlay->setVisible(true);

			left_trail_emitter->pause();
			right_trail_emitter->pause();

			

#if USE_SIMPLE_AUDIO_ENGINE
			CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(DEAD_MUSIC);
			CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0);
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(DIE_SFX);

#else
			if (!music_muted)
			{
				experimental::AudioEngine::stopAll();
				//current_playing_music = experimental::AudioEngine::play2d(DEAD_MUSIC,true,0.0f);
			}

			if(!sfx_muted)
				experimental::AudioEngine::play2d(DIE_SFX);
#endif
			left_player->mypause();
			right_player->mypause();
#if OLD_GENERATION_MODEL
			left_obsgen->mypause();
			right_obsgen->mypause();
#else
			obsgen->mypause();
#endif
			if (play_time > highscore[current_difficulty])
			{
				highscore[current_difficulty] = play_time;
				UserDefault::getInstance()->setFloatForKey(highscores_store_key[current_difficulty], highscore[current_difficulty]);
				int hash_val = std::hash<int>{}((int)(play_time*100));
				UserDefault::getInstance()->setIntegerForKey(highscores_store_key_hash[current_difficulty], hash_val);
#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
				if(SonarCocosHelper::GooglePlayServices::isSignedIn())
					SonarCocosHelper::GooglePlayServices::submitScore(leaderboard_ids[current_difficulty],(int)(highscore[current_difficulty]*1000.0));
#endif
			}

			char buffer[64];
			sprintf(buffer, "BEST:  %.2d:%.2d", (int)highscore[current_difficulty], ((int)(highscore[current_difficulty] *100.f)) % 100);
			gameover_highscore_text->setString(buffer);

			sprintf(buffer, "%.2d:%.2d", (int)play_time, ((int)(play_time*100.f)) % 100);
			gameover_score_text->setString(buffer);

			//config.obs_vely = init_config.obs_vely;
		}

		if (state == DEAD && dead_anim.timer < dead_anim.duration)
		{
			//coloring the background
			{
				splitter_and_grnd_draw_node->clear();

				Color3B the_color;
				float t = dead_anim.timer / dead_anim.duration;
				t = 2.5*t;
				if (t > 1)t = 1;

				the_color = rgb_color_lerp(level_themes[current_difficulty].left_player, t, get_grayscaled_current_color(level_themes[current_difficulty].left_player));
				left_player->setColor(the_color);
				right_backgrounds[0]->setColor(the_color);
				right_backgrounds[1]->setColor(the_color);

				the_color = rgb_color_lerp(level_themes[current_difficulty].right_player, t, get_grayscaled_current_color(level_themes[current_difficulty].right_player));
				right_player->setColor(the_color);
				left_backgrounds[0]->setColor(the_color);
				left_backgrounds[1]->setColor(the_color);

				the_color = rgb_color_lerp(level_themes[current_difficulty].ground_glow, t, get_grayscaled_current_color(level_themes[current_difficulty].ground_glow));
				left_splitter->setColor(the_color);
				right_splitter->setColor(the_color);
				right_ground->setColor(the_color);
				left_ground->setColor(the_color);

				new_record_region->clear();
				new_record_region->drawSolidRect(Vec2(-ground_outline, 0), Vec2(100 - splitter_rect_width / 2, 50 + ground_outline + 1), Color4F(the_color));
				new_record_region->drawSolidRect(Vec2(100 + splitter_rect_width / 2, 0), Vec2(200 + ground_outline, 50 + ground_outline + 1), Color4F(the_color));

				new_record_label->setColor(the_color);
				{
					score_panel_drawnode->clear();
					float score_panel_roof_width = 100;
					float score_panel_base_width = 80;
					float score_panel_base_y = designResolutionSize.height - score_panel_height;
					Vec2 score_panel_polygon_outline[] = { Vec2(240 - score_panel_roof_width - 4,designResolutionSize.height),Vec2(240 + score_panel_roof_width + 4,designResolutionSize.height),
						Vec2(240 + score_panel_base_width + 4,score_panel_base_y - 4),Vec2(240 - score_panel_base_width - 4,score_panel_base_y - 4) };
					score_panel_drawnode->drawSolidPoly(score_panel_polygon_outline, 4, Color4F(the_color));
					score_label_two_points->setColor(the_color);
					score_label_seconds->setColor(the_color);
					score_label_miliseconds->setColor(the_color);
				}

				for (int i = 0; i < MAX_OBSTACLES; i++)
					obsgen->obstacles_glow_sprites[i]->setColor(the_color);

				splitter_and_grnd_draw_node->drawSolidRect(Vec2(240 - splitter_rect_width / 2, 0), Vec2(240 + splitter_rect_width / 2, designResolutionSize.height - score_panel_height), Color4F(the_color));
				splitter_and_grnd_draw_node->drawSolidRect(Vec2(480 - floor_rect_width, 0), Vec2(480, designResolutionSize.height), Color4F(the_color));
				splitter_and_grnd_draw_node->drawSolidRect(Vec2(0, 0), Vec2(floor_rect_width, designResolutionSize.height), Color4F(the_color));


				the_color = rgb_color_lerp(level_themes[current_difficulty].ground_obs, t, get_grayscaled_current_color(level_themes[current_difficulty].ground_obs));
				splitter_and_grnd_draw_node->drawSolidRect(Vec2(240 - splitter_rect_width / 2  +  ground_outline, 0), Vec2(240 + splitter_rect_width / 2 - ground_outline, designResolutionSize.height - score_panel_height), Color4F(the_color));
				splitter_and_grnd_draw_node->drawSolidRect(Vec2(480 - floor_rect_width + ground_outline, 0), Vec2(480, designResolutionSize.height), Color4F(the_color));
				splitter_and_grnd_draw_node->drawSolidRect(Vec2(0, 0), Vec2(floor_rect_width - ground_outline, designResolutionSize.height), Color4F(the_color));
				
				left_trail_emitter->setStartColor(Color4F(the_color));
				left_trail_emitter->setEndColor(Color4F(the_color));
				left_trail_emitter->setColor(the_color);

				right_trail_emitter->setStartColor(Color4F(the_color));
				right_trail_emitter->setEndColor(Color4F(the_color));
				right_trail_emitter->setColor(the_color);

				for (int i = 0; i < MAX_OBSTACLES; i++)
					obsgen->obstacles_pool[i]->setColor(the_color);

				{
					float score_panel_roof_width = 100;
					float score_panel_base_width = 80;
					float score_panel_base_y = designResolutionSize.height - score_panel_height;
					Vec2 score_panel_polygon[] = { Vec2(240 - score_panel_roof_width,designResolutionSize.height),Vec2(240 + score_panel_roof_width,designResolutionSize.height),
						Vec2(240 + score_panel_base_width,score_panel_base_y),Vec2(240 - score_panel_base_width,score_panel_base_y) };
					score_panel_drawnode->drawSolidPoly(score_panel_polygon, 4, Color4F(the_color));
				}

				new_record_region->drawSolidRect(Vec2(0, 0), Vec2(200, 50), Color4F(the_color));
			}

			//TODO: make sure the springs don't blow.. subdivide the dt
			if (dead_anim.timer < dead_anim.flash_duration)
			{
				float t = dead_anim.timer / dead_anim.flash_duration;

				set_gameover_menu_alpha(t);

				float alpha = lerp(1, t, 0);
				float t2 = (2 * t > 1) ? 1 : 2 * t;

				float color_value = lerp(1, t, 0.0f);

				black_overlay->clear();
				black_overlay->drawSolidRect(Vec2(0, 0), Vec2(480, designResolutionSize.height), Color4F(color_value, color_value, color_value, alpha));
			}
			else
			{
				if (!gameover_voice_played)
				{
					if(!sfx_muted)
					experimental::AudioEngine::play2d(GAMEOVER_VOICE);
					gameover_voice_played = true;
				}
				/*if (dead_anim.timer > dead_anim.gameover_begin_time)
				{
					if (fabs(dead_anim.gameover_scale - 1)>EPS || fabs(dead_anim.gameover_spring_vel) > EPS)
					{
						float sa = -(dead_anim.gameover_scale - 1)*ui_spring_tightness - dead_anim.gameover_spring_vel*ui_spring_damp;
						dead_anim.gameover_spring_vel += sa*dt;
						dead_anim.gameover_scale += dead_anim.gameover_spring_vel*dt;
						gameover_text->setScale(dead_anim.gameover_scale);
					}
				}*/

				if (dead_anim.timer > dead_anim.score_begin_time)
				{
					if (fabs(dead_anim.score_scale - 1)>EPS || fabs(dead_anim.score_spring_vel) > EPS)
					{
						simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &dead_anim.score_scale, &dead_anim.score_spring_vel);
						gameover_score_text->setScale(dead_anim.score_scale);
						gameover_new_record->setScale(dead_anim.score_scale);
						gameover_score_shine->setScale(dead_anim.score_scale);

					}
				}

				if (dead_anim.timer > dead_anim.highscore_begin_time)
				{
					if (fabs(dead_anim.highscore_scale - 1)>EPS || fabs(dead_anim.highscore_spring_vel) > EPS)
					{
						simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &dead_anim.highscore_scale, &dead_anim.highscore_spring_vel);
						gameover_highscore_text->setScale(dead_anim.highscore_scale);
					}
				}

				if (dead_anim.timer > dead_anim.buttons_begin_time)
				{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
					if (games_to_interstetial == 0)
					{
						SonarCocosHelper::AdMob::showPreLoadedFullscreenAd();
						games_to_interstetial = GAMES_BEFORE_INTERSTETIAL;
					}
#endif

					if (fabs(dead_anim.replay_butt_scale - 1)>EPS || fabs(dead_anim.replay_butt_spring_vel) > EPS)
					{
						simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &dead_anim.replay_butt_scale, &dead_anim.replay_butt_spring_vel);
						gameover_replay_butt->setScale(dead_anim.replay_butt_scale);
					}
				}
				if (dead_anim.timer > dead_anim.buttons_begin_time + dead_anim.buttons_delta_time)
				{
					if (fabs(dead_anim.home_butt_scale - 1)>EPS || fabs(dead_anim.home_butt_spring_vel) > EPS)
					{
						simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &dead_anim.home_butt_scale, &dead_anim.home_butt_spring_vel);
						gameover_home_butt->setScale(dead_anim.home_butt_scale);
					}
				}
				if (dead_anim.timer > dead_anim.buttons_begin_time + 2*dead_anim.buttons_delta_time)
				{
					if (fabs(dead_anim.leader_butt_scale - 1)>EPS || fabs(dead_anim.leader_butt_spring_vel) > EPS)
					{
						simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &dead_anim.leader_butt_scale, &dead_anim.leader_butt_spring_vel);
						gameover_leader_butt->setScale(dead_anim.leader_butt_scale);
					}
				}
			}
			dead_anim.timer += dt;
		}

		if (state == PAUSE && pause_anim_timer< pause_anim_duration)
		{
			if (fabs(pause_home_butt_scale - 1)>EPS || fabs(pause_home_butt_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &pause_home_butt_scale, &pause_home_butt_spring_vel);
				pause_home_butt->setScale(pause_home_butt_scale);
			}

			if (pause_anim_timer > dead_anim.buttons_delta_time)
			{
				if (fabs(pause_continue_butt_scale - 1) > EPS || fabs(pause_continue_butt_spring_vel) > EPS)
				{
					simulate_spring(dt, ui_spring_damp, ui_spring_tightness, 1, &pause_continue_butt_scale, &pause_continue_butt_spring_vel);
					pause_continue_butt->setScale(pause_continue_butt_scale*0.357f);
				}
			}
			pause_anim_timer += dt;
		}

		if (state == RESUMING && resume_anim_timer < resume_anim_duration)
		{
			if (fabs(pause_continue_butt_scale) > EPS || fabs(pause_continue_butt_spring_vel) > EPS)
			{
				simulate_spring(dt, ui_spring_damp * 2.5f, ui_spring_tightness*2.f, 0, &pause_continue_butt_scale, &pause_continue_butt_spring_vel);
				pause_continue_butt->setScale(pause_continue_butt_scale*0.357f);
			}
			if (resume_anim_timer > dead_anim.buttons_delta_time/2)
			{
				if (fabs(pause_home_butt_scale)>EPS || fabs(pause_home_butt_spring_vel) > EPS)
				{
					simulate_spring(dt, ui_spring_damp * 2.5f, ui_spring_tightness*2.f, 0, &pause_home_butt_scale, &pause_home_butt_spring_vel);
					pause_home_butt->setScale(pause_home_butt_scale);
				}
			}
			resume_anim_timer += dt;

			if (resume_anim_timer >= resume_anim_duration)
				resume_game();
		}


#if ENABLE_EDITOR
		if (in_editor)
			black_overlay->clear();
#endif


		//config.obs_vely += 10 * dt;
		float obs_vel_ratio = config.obs_vely / 240.f;
		config.gravity_acc = 1500 * obs_vel_ratio * obs_vel_ratio;
		config.jump_impulsion = 550 * obs_vel_ratio;
		config.crouch_spring_damp = 6.375*config.obs_vely / 170.0f;
		config.crouch_duration = 170 / config.obs_vely;
		config.jump_prep_duration = 38.4f / config.obs_vely;
		
		{
			int play_time_seconds = (int)play_time;
			int play_time_miliseconds = ((int)(play_time * 100)) % 100;
			char score_display[16];
			sprintf(score_display, "%.2d", play_time_seconds);
			score_label_seconds->setText(score_display);

			sprintf(score_display, "%.2d", ((int)(play_time*100.f))%100);
			score_label_miliseconds->setText(score_display);
		}

#if	(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if (play_time > 60.0)
		{
			if (SonarCocosHelper::GooglePlayServices::isSignedIn())
				SonarCocosHelper::GooglePlayServices::unlockAchievement(achievement_ids[current_difficulty]);
		}
#endif
		obsgen->setPosition(screen_shake_effect.x, screen_shake_effect.y);
		tut_obsgen->setPosition(screen_shake_effect.x, screen_shake_effect.y);
		splitter_and_grnd_draw_node->setPosition(screen_shake_effect.x, screen_shake_effect.y);
		left_splitter->setPosition(240 + screen_shake_effect.x, designResolutionSize.height / 2 + screen_shake_effect.y - score_panel_height);
		right_splitter->setPosition(240 + screen_shake_effect.x, designResolutionSize.height / 2 + screen_shake_effect.y - score_panel_height);
		left_ground->setPosition(screen_shake_effect.x, designResolutionSize.height / 2 + screen_shake_effect.y);
		right_ground->setPosition(480 + screen_shake_effect.x, designResolutionSize.height / 2 + screen_shake_effect.y);
	}

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


	{//floating menu buttons
		floating_menu_sine_timer += dt;
		float sine_period = 2.0f;
		float sin_yoffset = 4 * sinf(2 * 3.14 * floating_menu_sine_timer / sine_period);
		float cos_yoffset = 4 * cosf(2 * 3.14 * floating_menu_sine_timer / sine_period);
		if (floating_menu_sine_timer > sine_period)
			floating_menu_sine_timer -= sine_period;

		pause_home_butt->setPosition(Vec2(240 - 100, designResolutionSize.height / 2 + sin_yoffset));
		pause_continue_butt->setPosition(Vec2(240 + 100, designResolutionSize.height / 2 + cos_yoffset));
		gameover_home_butt->setPosition(Vec2(100, designResolutionSize.height / 2 - 250 + sin_yoffset));
		gameover_replay_butt->setPosition(Vec2(240, designResolutionSize.height / 2 - 100 + cos_yoffset));
		gameover_leader_butt->setPosition(Vec2(380, designResolutionSize.height / 2 - 250 + cos_yoffset));
		//gameover_text->setPosition(Vec2(240, 650 + sin_yoffset));
		gameover_score_text->setPosition(Vec2(240, 660 + cos_yoffset));
		gameover_new_record->setPosition(Vec2(240, 600 + cos_yoffset));
		gameover_score_shine->setPosition(Vec2(240, 660 + cos_yoffset));
		gameover_highscore_text->setPosition(Vec2(240, 500 + sin_yoffset));

		if (state == TUTORIAL)
		{
			tut_label->setPosition(Vec2(tut_label->getPosition().x, tut_text_y + 1.5*sin_yoffset));
			//tut_skip_button->setPosition(Vec2(240, 100 + cos_yoffset));
		}

		gameover_score_shine_angle += 75 * dt;
		gameover_score_shine->setRotation(gameover_score_shine_angle);
		gameover_new_record->setRotation(12 * sinf(0.2f*gameover_score_shine_angle));

		gameover_replay_butt->setColor(level_themes[current_difficulty].ground_obs);
	}

	last_player_dead = player_dead;
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode key, Event* ev)
{
#if ENABLE_EDITOR
	if ((key == EventKeyboard::KeyCode::KEY_R || key == EventKeyboard::KeyCode::KEY_ESCAPE) && !in_editor)
#else
	if ((key == EventKeyboard::KeyCode::KEY_R || key == EventKeyboard::KeyCode::KEY_ESCAPE))
#endif
	{
		if (player_dead)
		{
			reinit_game();
		}
		else
		{
			if (state == PAUSE)
				state = RESUMING;
			else
				pause_game();
		}
	}
}

void HelloWorld::reinit_game()
{
	if (state == TUTORIAL)
	{
		tut_text_scale = 0;
		tut_text_spring_vel = 0;
		tut_flashing_region_sine_timer = 0;
		tut_text_transition_state = NO_TRANSITION;
		tut_tut_notice_enter_anim_finished = false;
		tut_did_last_step = false;

		tut_tut_notice->setScale(0);

		tut_text_timer = 0;
		tut_color = Color4F::WHITE;
		tut_phase = 0;

		tut_label->setString(tut_messages[tut_phase]);

		tut_obsgen->reinit();
		tut_obsgen->myresume();
	}

	new_record_region->clear();
	new_record_region->drawSolidRect(Vec2(-ground_outline, 0), Vec2(100 - splitter_rect_width / 2, 50 + ground_outline + 1), Color4F(level_themes[current_difficulty].ground_glow));
	new_record_region->drawSolidRect(Vec2(100 + splitter_rect_width / 2, 0), Vec2(200 + ground_outline, 50 + ground_outline + 1), Color4F(level_themes[current_difficulty].ground_glow));
	new_record_region->drawSolidRect(Vec2(0, 0), Vec2(200, 50), Color4F(level_themes[current_difficulty].ground_obs));

	new_record_label->setColor(level_themes[current_difficulty].ground_glow);
	new_record_label->setPosition(Vec2(240, -100));
	new_record_region->setPosition(Vec2(0, -100));

	new_record_y = 0;
	new_record_move_timer = 0;
	new_record_state = -1;

	showed_new_record_notice = false;
	gameover_new_record->setVisible(false);

	score_label_two_points->setColor(level_themes[current_difficulty].ground_glow);
	score_label_seconds->setColor(level_themes[current_difficulty].ground_glow);
	score_label_miliseconds->setColor(level_themes[current_difficulty].ground_glow);

	left_player->setColor(level_themes[current_difficulty].left_player);
	right_player->setColor(level_themes[current_difficulty].right_player);

	splitter_and_grnd_draw_node->clear();
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(240 - splitter_rect_width / 2, 0), Vec2(240 + splitter_rect_width / 2, designResolutionSize.height - score_panel_height), Color4F(level_themes[current_difficulty].ground_glow));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(240 - splitter_rect_width / 2 + ground_outline, 0), Vec2(240 + splitter_rect_width / 2 - ground_outline, designResolutionSize.height - score_panel_height), Color4F(level_themes[current_difficulty].ground_obs));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(480 - floor_rect_width, 0), Vec2(480, designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_glow));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(480 - floor_rect_width + (ground_outline + 1), 0), Vec2(480, designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_obs));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(0, 0), Vec2(floor_rect_width, designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_glow));
	splitter_and_grnd_draw_node->drawSolidRect(Vec2(0, 0), Vec2(floor_rect_width - (ground_outline + 1), designResolutionSize.height), Color4F(level_themes[current_difficulty].ground_obs));
	left_splitter->setColor(level_themes[current_difficulty].ground_glow);
	right_splitter->setColor(level_themes[current_difficulty].ground_glow);
	left_ground->setColor(level_themes[current_difficulty].ground_glow);
	right_ground->setColor(level_themes[current_difficulty].ground_glow);
	left_trail_emitter->setStartColor(Color4F(level_themes[current_difficulty].ground_obs));
	left_trail_emitter->setEndColor(Color4F(level_themes[current_difficulty].ground_obs));
	right_trail_emitter->setStartColor(Color4F(level_themes[current_difficulty].ground_obs));
	right_trail_emitter->setEndColor(Color4F(level_themes[current_difficulty].ground_obs));
	left_backgrounds[0]->setColor(level_themes[current_difficulty].right_player);
	left_backgrounds[1]->setColor(level_themes[current_difficulty].right_player);
	right_backgrounds[0]->setColor(level_themes[current_difficulty].left_player);
	right_backgrounds[1]->setColor(level_themes[current_difficulty].left_player);
	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obsgen->obstacles_pool[i]->setColor(level_themes[current_difficulty].ground_obs);
		obsgen->obstacles_glow_sprites[i]->setColor(level_themes[current_difficulty].ground_glow);
	}

	left_trail_emitter->resume();
	right_trail_emitter->resume();

	gameover_score_shine_angle = 0;
	gameover_voice_played = false;

	dead_anim.timer = dead_anim.duration + 1;
	dead_anim.gameover_scale = 0;
	dead_anim.gameover_spring_vel = 0;
	dead_anim.highscore_scale = 0;
	dead_anim.highscore_spring_vel = 0;
	dead_anim.score_scale = 0;
	dead_anim.score_spring_vel = 0;
	dead_anim.home_butt_scale = 0;
	dead_anim.home_butt_spring_vel = 0;
	dead_anim.replay_butt_scale = 0;
	dead_anim.replay_butt_spring_vel = 0;
	dead_anim.acheiv_butt_scale = 0;
	dead_anim.acheiv_butt_spring_vel = 0;
	dead_anim.leader_butt_scale = 0;
	dead_anim.leader_butt_spring_vel = 0;
	dead_anim.twitter_butt_scale = 0;
	dead_anim.twitter_butt_spring_vel = 0;

	//gameover_text->setScale(0);
	gameover_highscore_text->setScale(0);
	gameover_score_text->setScale(0);
	gameover_new_record->setScale(0);
	gameover_score_shine->setScale(0);

	gameover_home_butt->setScale(0);
	gameover_replay_butt->setScale(0);
	gameover_leader_butt->setScale(0);

	pause_home_butt->setScale(0);
	pause_continue_butt->setScale(0);

	pause_anim_timer = pause_anim_duration + 1;
	pause_home_butt_scale = 0;
	pause_home_butt_spring_vel = 0;
	pause_continue_butt_scale = 0;

	pause_continue_butt_spring_vel = 0;
	left_trail_emitter->setGravity(Vec2(left_trail_init_grativyx, left_trail_emitter->getGravity().y));
	left_trail_emitter->setTangentialAccel(left_trail_init_tangent_accel);
	left_trail_emitter->setPosition(Vec2(240 - splitter_rect_width / 2, left_player->inity));
	left_trail_emitter->resetSystem();

	right_trail_emitter->setGravity(Vec2(right_trail_init_grativyx, right_trail_emitter->getGravity().y));
	right_trail_emitter->setTangentialAccel(right_trail_init_tangent_accel);
	right_trail_emitter->setPosition(Vec2(240 + splitter_rect_width / 2, right_player->inity));
	right_trail_emitter->resetSystem();

	current_level = 0;
	config = levels_data[current_difficulty][0].config;

	state = PLAYING_LEVEL;
	hide_all_menus();

	last_player_dead = false;
	player_dead = false;
	left_player->reinit();
	right_player->reinit();

#if OLD_GENERATION_MODEL
	left_obsgen->reinit();
	right_obsgen->reinit();
#else
	obsgen->reinit();
#endif

	experimental::AudioEngine::stopAll();
	if (!music_muted)
	{
		current_playing_music = experimental::AudioEngine::play2d(MAIN_MUSIC, true, 0.0f);
		music_transtion_timer = music_transtion_duration;
	}

	left_player->myresume();
	right_player->myresume();

#if OLD_GENERATION_MODEL
	left_obsgen->myresume();
	right_obsgen->myresume();
#else
	obsgen->myresume();
#endif

	black_overlay->setVisible(false);

	play_time = 0;
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	SonarCocosHelper::AdMob::preLoadFullscreenAd();
	//SonarCocosHelper::AdMob::showBannerAd(0);
#endif
}

void HelloWorld::pause_game()
{
	if (state == TUTORIAL)
	{
		// cocos does not release the scene when popping..so we reinit the game ourselves
		reinit_game();
		if (!music_muted)
		{

			experimental::AudioEngine::stopAll();
			current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0);
			music_transtion_timer = music_transtion_duration;
		}

		game_scene_pointer = 0;

		if (!music_muted)
		{
			current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
			music_transtion_timer = music_transtion_duration;
		}
		else
		{
			current_playing_music = experimental::AudioEngine::play2d(MENU_MUSIC, true, 0.f);
			experimental::AudioEngine::pauseAll();
		}

		auto menu_scene = MenuScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.35f, menu_scene, Color3B(255, 255, 255)));
		return;
	}
	
	tut_obsgen->mypause();

	left_trail_emitter->pause();
	right_trail_emitter->pause();

	show_pause_menu();
#if USE_SIMPLE_AUDIO_ENGINE
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
#else
	experimental::AudioEngine::pauseAll();
#endif
	left_player->mypause();
	right_player->mypause();
#if OLD_GENERATION_MODEL
	left_obsgen->mypause();
	right_obsgen->mypause();
#else
	obsgen->mypause();
#endif

	state_when_paused = state;
	state = PAUSE;

	pause_home_butt->setScale(0);
	pause_continue_butt->setScale(0);

	pause_anim_timer = 0;
	pause_home_butt_scale = 0;
	pause_home_butt_spring_vel = 0;
	pause_continue_butt_scale = 0;

}
void HelloWorld::resume_game()
{
	if (state == TUTORIAL)
	{
		tut_obsgen->myresume();
	}

	resume_anim_timer = 0;

	left_trail_emitter->resume();
	right_trail_emitter->resume();

#if USE_SIMPLE_AUDIO_ENGINE
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MAIN_MUSIC);
#else
	hide_all_menus();

	if (!music_muted)
	{
		experimental::AudioEngine::setVolume(current_playing_music, 0);
		experimental::AudioEngine::resumeAll();
		music_transtion_timer = music_transtion_duration / 3;
	}

#endif

	left_player->myresume();
	right_player->myresume();

	if (state_when_paused != TUTORIAL)
	{
#if OLD_GENERATION_MODEL
		left_obsgen->myresume();
		right_obsgen->myresume();
#else
		obsgen->myresume();
#endif
	}

	state = state_when_paused;
}


void HelloWorld::onKeyReleased(EventKeyboard::KeyCode key, Event* ev)
{
}

void HelloWorld::onTouchesEnded(std::vector<Touch*> touches, Event* ev)
{
	congrats_tap = true;
}


void HelloWorld::onTouchesBegan(std::vector<Touch*> touches, Event* ev)
{
	for (auto touch : touches)
	{
#if 0 // circle control buttons
	{
		Vec2 current_pos = touch->getLocation();
		Vec2 delta1 = current_pos - lp_left_button_pos;
		Vec2 delta2 = current_pos - lp_right_button_pos;
		if (delta1.lengthSquared() < button_radius * button_radius)
		{
			left_player->jumpkey = true;
		}
		if (delta2.lengthSquared() < button_radius * button_radius)
		{
			left_player->crouchkey = true;
		}
	}
	{
		Vec2 current_pos = touch->getLocation();
		Vec2 delta1 = current_pos - rp_left_button_pos;ta
		Vec2 delta2 = current_pos - rp_right_button_pos;
		if (delta1.lengthSquared() < button_radius * button_radius)
		{
			right_player->crouchkey = true;
		}
		if (delta2.lengthSquared() < button_radius * button_radius)
		{
			right_player->jumpkey = true;
		}
	}
#else
		Vec2 current_pos = touch->getLocation();
		if (current_pos.x < 480 / 4)
		{
			left_player->jumpkey = true;
		}
		else if (current_pos.x < 480 / 2)
		{
			left_player->crouchkey = true;
		}
		else if (current_pos.x < 3 * 480 / 4)
		{
			right_player->crouchkey = true;
		}
		else
		{
			right_player->jumpkey = true;
		}
#endif
	}
}
