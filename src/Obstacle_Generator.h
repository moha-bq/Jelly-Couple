#pragma once

#include "cocos2d.h"
#include "globals.h"
#define MAX_OBSTACLES 300
#define USE_OBSTACLE_SPRITE 1
#define OBSTACLE_DEBUG_DRAW 0
#define RENDER_OBSTACLE_GLOW 0

using namespace cocos2d;

extern float obs_spawn_y;

struct Obstacle_Generator;


#if USE_SPRITES==1
struct Obstacle : public Sprite
#else
struct Obstacle : public DrawNode
#endif
{
	enum { JUMP_OBS=0, CROUCH_OBS, SHIFT_OBS };

	static Obstacle* create();
	Obstacle();

	virtual bool init();
	virtual void update(float);
	void fixed_update(float);

	void draw(float);

	void update_graphic();
	void kill();
	void revive();

	void mypause() { paused = true; }
	void myresume() { paused = false; }

	void reinit();

	void set(int t,int side,float x,float y,float w,float h,int generated_by_right=0);

	int type;

	float x, y;
	float w, h;

	bool alive;

	int side; // 1 = right

	float accum_timer;
	int paused;
#if USE_SPRITES == 1 && RENDER_OBSTACLE_GLOW
	Sprite* glow;
#endif

#if OBSTACLE_DEBUG_DRAW
	DrawNode* debug_draw;
#endif

	DrawNode* outline;

	int id;
	int generated_by_right;//generated using right generated
	Obstacle_Generator* generator;
};

struct Obstacle_Generator: public Node
{

#if OLD_GENERATION_MODEL
	float genx[2];

	int generating_side;
	int left_generator;

	float next_obs_distance;
	Obstacle* last_obstacle;

	void generate();
#else
	struct Obstacle_Generator_State
	{
		enum { GENERATING, SHIFTING };
		int generating_side;//shift obs generating side
		int num_remaining_obstacles;
		int did_special_move;
		int state;
		int last_state;
		int saved_state; // saved state between frames
		float last_spike_obs_y; // used to align spikes proprely
		int first_obstacle_generated;
	};
	virtual void new_generate(float dt);


	float right_genx[2];//spawn points
	float left_genx[2];

	void generate_left_obs(int left_desync);
	void generate_right_obs(int right_desync);


	Obstacle_Generator_State left_generator_state;
	Obstacle_Generator_State right_generator_state;

	float last_obstacle_y;
	int first_generation;

#endif

	Obstacle_Generator(float ix,float iy);

	virtual bool init();
	virtual void update(float);

	virtual void reinit();

	void mypause();
	void myresume();

	static Obstacle_Generator* create(float ix,float iy);

	Obstacle* get_free_obs();
	Obstacle* create_shift_spikes(int num, int generating_side, float x, float y,int generated_by_right=0);

	
	float x, y;

	Obstacle* obstacles_pool[MAX_OBSTACLES];
	Sprite* obstacles_glow_sprites[MAX_OBSTACLES];

	float accum_timer;

	int paused;
};
//THIS JUST A HAAACK; the generation code is shit..
struct Tut_Obstacle_Generator : public Obstacle_Generator
{
	static Tut_Obstacle_Generator* create(float ix, float iy);
	Tut_Obstacle_Generator(float ix,float iy):Obstacle_Generator(ix,iy)
	{
		next_obs_type = 0;
		jump_obs = 0;
		crouch_obs = 0;
		shift_obs1 = 0;
		shift_obs2 = 0;
	}
	void new_generate(float dt);
	void generate_obs();
	int next_obs_type;

	Obstacle* jump_obs;
	Obstacle* crouch_obs;
	Obstacle* shift_obs1;
	Obstacle* shift_obs2;

	void reinit()
	{
		Obstacle_Generator::reinit();
		next_obs_type = 0;
		jump_obs = 0;
		crouch_obs = 0;
		shift_obs1 = 0;
		shift_obs2 = 0;
	}
};