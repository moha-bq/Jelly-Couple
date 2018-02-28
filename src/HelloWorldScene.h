#pragma once 

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "globals.h"
#include <vector>

struct Player;
struct Obstacle_Generator;
#if ENABLE_EDITOR
struct Editor;
#endif

using namespace cocos2d;

#define MAX_TAPS 4

struct Dead_Anim_Vars
{
	float timer;
	float duration;
	
	float flash_duration;
	float gameover_begin_time;
	float score_begin_time;
	float highscore_begin_time;
	float buttons_begin_time;
	float buttons_delta_time;//time between buttons popping up

	float gameover_scale;
	float gameover_spring_vel;

	float score_scale;
	float score_spring_vel;

	float highscore_scale;
	float highscore_spring_vel;

	float home_butt_scale;
	float home_butt_spring_vel;

	float replay_butt_scale;
	float replay_butt_spring_vel;

	float acheiv_butt_scale;
	float acheiv_butt_spring_vel;

	float leader_butt_scale;
	float leader_butt_spring_vel;

	float twitter_butt_scale;
	float twitter_butt_spring_vel;
};

class HelloWorld : public cocos2d::Layer
{
public:
    enum { PLAYING_LEVEL, LEVEL_TRANSITION, DEAD, PAUSE, RESUMING, TUTORIAL, CONGRATS};
    static cocos2d::Scene* createScene();


	CREATE_FUNC(HelloWorld);

	void onEnterTransitionDidFinish();

    virtual bool init();
    virtual void update(float);
    void fixed_update(float);

    void onKeyPressed(EventKeyboard::KeyCode key, Event* ev);
    void onKeyReleased(EventKeyboard::KeyCode key, Event* ev);

	void onTouchesBegan(std::vector<Touch*> touches, Event* ev);
	void onTouchesEnded(std::vector<Touch*> touches, Event* ev);

    void reinit_game();

	void pause_game();
	void resume_game();


	void show_pause_menu();
	void show_gameover_menu();
	void hide_all_menus();
	void set_gameover_menu_alpha(float a);

    cocos2d::DrawNode* splitter_and_grnd_draw_node;
    cocos2d::DrawNode* black_overlay;
    
    bool last_player_dead;

	Dead_Anim_Vars dead_anim;
	
	float pause_anim_duration;
	float pause_anim_timer;
	float pause_home_butt_scale;
	float pause_home_butt_spring_vel;
	float pause_continue_butt_scale;
	float pause_continue_butt_spring_vel;

	float resume_anim_timer;
	float resume_anim_duration;

    double left_player_last_tap;
    double right_player_last_tap;

    float double_tap_condition_time;//ms

    int state;
	int state_when_paused;

    float accum_timer;

	ui::Button* pause_home_butt;
	ui::Button* pause_continue_butt;

	ui::Button* gameover_home_butt;
	ui::Button* gameover_replay_butt;
	ui::Button* gameover_leader_butt;
	ui::Text* gameover_new_record;
	Sprite* gameover_score_shine;
	float gameover_score_shine_angle;

	//ui::Text* gameover_text;
	ui::Text* gameover_score_text;
	ui::Text* gameover_highscore_text;


	float floating_menu_sine_timer;
	float tut_flashing_region_sine_timer;

	int tut_phase;

	enum { EXIT, NO_TRANSITION, END };
	float tut_text_y;
	int tut_text_transition_state;
	float tut_text_timer;
	ui::Text* tut_label;
	ui::Text* tut_tut_notice;
	ui::Text* tut_goodluck;
	bool tut_tut_notice_enter_anim_finished;
	DrawNode* tut_region_indicator;
	DrawNode* tut_info_region;
	//ui::Button* tut_skip_button;
	Color4F tut_color;
	float tut_text_scale;
	float tut_text_spring_vel;
	bool tut_did_last_step;

	float good_luck_stay_timer;

	ui::Text* score_label_two_points;
	ui::Text* score_label_seconds;
	ui::Text* score_label_miliseconds;

	DrawNode* score_panel_drawnode;

	ParticleSystem* left_trail_emitter;
	bool left_trail_emitter_paused;
	ParticleSystem* right_trail_emitter;
	bool right_trail_emitter_paused;
	float left_trail_init_grativyx;
	float right_trail_init_grativyx;
	float left_trail_init_tangent_accel;
	float right_trail_init_tangent_accel;

	//this stuff is moving down.. we need two instances in order to maintain continuity
	ParticleSystem* background_emitter;
	Sprite* left_backgrounds[2];
	Sprite* right_backgrounds[2];

	Sprite* left_splitter;
	Sprite* right_splitter;

	Sprite* right_ground;
	Sprite* left_ground;

	bool gameover_voice_played;

	float new_record_y;
	float new_record_move_timer;
	int   new_record_state;
	ui::Text* new_record_label;
	DrawNode* new_record_region;

	bool showed_new_record_notice;

	DrawNode* congrats_drawnode;
	DrawNode* congrats_tap_drawnode;
	ui::Text* congrats_text;
	ui::Text* congrats_tap_text;
	ui::Text* congrats_unlock_text;
	ui::Text* congrats_complete_text;
	bool congrats_tap;
	Color_Interpolator congrats_colinter;
	float congrats_show_timer;

#if ENABLE_EDITOR
	ui::Button* edit_butt;
#endif
};
