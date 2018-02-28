#pragma once

#include "cocos2d.h"
#include "globals.h"

using namespace cocos2d;

extern Screen_Shake screen_shake_effect;

#define MAX_SWIPES 5
#define PLAYER_SPRITE 0
#define MAX_TAPS 4

#if PLAYER_SPRITE
struct Player : public cocos2d::Sprite
#else
struct Player : public cocos2d::DrawNode
#endif
{
	enum {LEFT_PLAYER,RIGHT_PLAYER};
	enum {STANDING, JUMPING, SHIFTING, CROUCHING, JUMP_PREP};
	static Player* create(int type = LEFT_PLAYER);

	Player(int type=LEFT_PLAYER);

	virtual bool init();
	virtual void update(float);
	void         fixed_update(float);
	float        get_distance_to_ground();

	void onKeyPressed(EventKeyboard::KeyCode key, Event* ev);
	void onKeyReleased(EventKeyboard::KeyCode key, Event* ev);
	void onTouchesBegan(std::vector<Touch*> touches, Event* ev);
	void onTouchesMoved(std::vector<Touch*> touches, Event* ev);
	void onTouchesEnded(std::vector<Touch*> touches, Event* ev);

	void handle_landing();
	void adjust_position_x();
	void adjust_position_y();
	void reinit();

	void push_tap(Vec2 pos);

	void push_swipe(Swipe swipe);
	void push_next_swipe(Swipe swipe);

	void mypause(){ paused = true; }
	void myresume(){ paused = false; }

	void setColor(const Color3B& color) override { this->color = Color4F(color); DrawNode::setColor(color); }

	float inity;

	real32 x,y;
	real32 velx;
	real32 accx;

	real32 w,h;
	real32 size;

	int type;

	int state;
	int last_state;

	int jumpkey;
	int last_jumpkey;
	int crouchkey;
	int last_crouchkey;

	int gravity_shifted;
	int last_gravity_shifted;

	double elapsed_since_jumpkey_pressed;
	double max_double_press_interval;

	bool missed_shift;

	float jump_prep_timer;

	real32 land_spring_vel;
	float crouch_spring_vel;

	real32 angle;
	int rotation_done;

	real32 crouch_width;
	real32 crouch_timer;

	Color4F color;
	Color4F init_color;

	real32 white_flash_duration;
	real32 white_flash_timer;

	Swipe swipes[MAX_SWIPES];
	int num_swipes;
	//swipe made while the player is performing some action..
	Swipe next_swipes[MAX_SWIPES];
	int num_next_swipes;

	real32 accum_timer;

	int paused;

	float init_distance_over_ground;

	Vec2 taps_while_on_air[MAX_TAPS];
	int num_taps_while_on_air;

	DrawNode* shifting_circle;

	bool nullify_input;
};