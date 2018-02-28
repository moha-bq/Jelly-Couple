#pragma once 

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "globals.h"

using namespace cocos2d;

class MenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual void update(float);

	void onKeyReleased(EventKeyboard::KeyCode key, Event* ev);

	void onEnterTransitionDidFinish();

	ui::Button* play_butt;
	ui::Button* acheiv_butt;

	ui::CheckBox* mute_music_checkbox;
	ui::CheckBox* mute_sfx_checkbox;

	Sprite* background;
	Sprite* jellycouple;
	Sprite* jellycouple_shine;
	ui::Button* google_game_connect_grey;
	Sprite* google_game_connect_green;

	float sine_timer;

	float enter_anim_timer;
	float enter_anim_duration;
	
	float play_butt_scale;
	float play_butt_spring_vel;

	float acheiv_butt_scale;
	float acheiv_butt_spring_vel;

	float mute_sfx_checkbox_scale;
	float mute_sfx_checkbox_spring_vel;

	float mute_music_checkbox_scale;
	float mute_music_checkbox_spring_vel;

	float jellycouple_scale;
	float jellycouple_spring_vel;

	float jellycouple_shine_scale;
	float jellycouple_shine_spring_vel;

	float enter_anim_button_delta_time;

	Color_Interpolator jellycouple_colinter;
	float jellycouple_shine_angle;

	ui::Text* by_beqqi_benjadi;
	DrawNode* by_region;

	float by_y;
	float by_showup_timer;
	float by_showup_interval;
	float by_move_timer;
	int by_state;

	CREATE_FUNC(MenuScene);
};
