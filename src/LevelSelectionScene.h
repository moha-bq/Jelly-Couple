#pragma once 

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "globals.h"

using namespace cocos2d;

struct Level_Page : public Node
{
	static Level_Page* create();
	Level_Page();

	virtual bool init();
	virtual void update(float);

	void set_level_name(char* name);
	void set_best_time(float time);

	float x, y;
	char* level_name;
	float best_time;

	ui::Text* best_time_text;
	ui::Text* level_name_label;


};

class LevelSelectionScene: public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void update(float);

	void onEnterTransitionDidFinish();

	void onKeyReleased(EventKeyboard::KeyCode key, Event* ev);

    cocos2d::DrawNode* draw_node;
#if 0
	ui::Button* veryeasy_butt;
	ui::Button* easy_butt;
    ui::Button* normal_butt;
    ui::Button* hard_butt;
    ui::Button* superhard_butt;
#endif
	ui::Button* right_arrow;
	ui::Button* left_arrow;
	ui::Button* play_button;
	ui::Button* leader_button;
	ui::Button* back_button;


	Level_Page *pages[NUM_DIFFICULTIES];
	int current_page;
	int next_page;
	int transitioning;
    
    float sine_timer;
	
	DrawNode* desc_frame;

	float transition_spring_damp;
	float transition_spring_k;
	float transition_spring_vel;

	float transition_timer;
	float transition_max_duration;

	ParticleSystem* background_emitter;
	Sprite* background;

	float enter_anim_timer;
	float enter_anim_duration;
	float enter_anim_glow_begin_time;

	float enter_anim_ui_scale;
	float enter_anim_ui_spring_vel;

	float enter_anim_glow_scale;
	float enter_anim_glow_spring_vel;

	DrawNode* page_indicator;

	Sprite* very_easy_icon;
	Sprite* easy_icon[2];
	Sprite* normal_icon[3];
	Sprite* hard_icon[4];
	Sprite* very_hard_icon[5];
	Sprite* very_easy_icon_glow;
	Sprite* easy_icon_glow[2];
	Sprite* normal_icon_glow[3];
	Sprite* hard_icon_glow[4];
	Sprite* very_hard_icon_glow[5];

	float icon_rotatin_speed;
	float icon_sine_timer;

	Color3B background_colors[5];
	
	bool enter_anim_finished;

	ui::Text* locked_text;

    CREATE_FUNC(LevelSelectionScene);
};
