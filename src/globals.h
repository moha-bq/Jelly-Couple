#pragma once

typedef double real32;

#define COCOS2D_DEBUG 0

#define EPS 0.00001f

#define MAX_INTERPOLATOR_COLORS 64

#define ENABLE_EDITOR 0
#define OLD_GENERATION_MODEL 0
#define USE_SIMPLE_AUDIO_ENGINE 0

#define NUM_SOUNDS 10
#define SFX_VOLUME 0.7f

#define JUMP_SFX "sound/jmp.mp3"
#define SHIFT_SFX "sound/shift.mp3"
#define LAND_SFX "sound/land.mp3"
#define DIE_SFX "sound/die.mp3"
#define CROUCH_SFX "sound/crouch.mp3"
#define CROUCH_END_SFX "sound/crouch_end.mp3"
#define NEW_RECORD_SFX "sound/new_record.mp3"

#define MAIN_MUSIC "sound/main_music.mp3"
#define MENU_MUSIC "sound/menu.mp3"

#define GAMEOVER_VOICE "sound/gameover_voice.mp3"

#define NUM_LEVELS 3
#define NUM_DIFFICULTIES 5

#include "cocos2d.h"

#define FIXED_TIMESTEP (1.0/60.0)

#define USE_SPRITES 1

struct Player;
struct Obstacle_Generator;
struct Tut_Obstacle_Generator;
class HelloWorld;

#if ENABLE_EDITOR
struct Editor;
#endif 
#define GAMES_BEFORE_INTERSTETIAL 5


extern char* leaderboard_ids[NUM_DIFFICULTIES];
extern char* achievement_ids[NUM_DIFFICULTIES];

struct Level_Theme
{
	cocos2d::Color3B ground_obs;
	cocos2d::Color3B ground_glow;
	cocos2d::Color3B left_player;
	cocos2d::Color3B right_player;
};

extern Level_Theme level_themes[5];

extern int games_to_interstetial;

extern float ui_spring_tightness;
extern float ui_spring_damp;

extern cocos2d::Size designResolutionSize;
extern cocos2d::Size smallResolutionSize;
extern cocos2d::Size mediumResolutionSize;
extern cocos2d::Size largeResolutionSize;
extern cocos2d::Size xLargeResolutionSize;

extern HelloWorld* game_scene_pointer;

extern int jump_sfx_id;
extern int shift_sfx_id;
extern int land_sfx_id;
extern int die_sfx_id;
extern int crouch_sfx_id;
extern int main_music;
extern int dead_music;
extern int current_playing_music;
extern int sound_loading_status[NUM_SOUNDS];
extern float music_transtion_timer;
extern float music_transtion_duration;
extern int music_muted;
extern int sfx_muted;
extern int current_playing_sfx;

struct Config
{
    float gravity_acc;
    float jump_impulsion;
    float max_velx;
    float land_spring_tightness;
    float land_spring_damp;
    float crouch_spring_tightness;
    float crouch_spring_damp;
    float obs_vely;
    float unit;
    int   obs_dist_in_batch_u;
    int   obs_lag_min_u;
    int   obs_lag_max_u;
    int   next_obs_distance_min_u;
    int   next_obs_distance_max_u;
    int   num_obs_per_batch_min;
    int   num_obs_per_batch_max;
    float jump_obs_width;
    float crouch_obs_width;
    float obs_height;
    float velocity_squash_amount;
    float rot_vel;
    float crouch_duration;
    float jump_prep_duration;
    float min_shift_velx;
    int   collision_enabled;
    int   first_obs_spike_lag_min_u;
    int   first_obs_spike_lag_max_u;

	int obs_after_spike_decrement;
	float shift_obs_height;
	float shift_obs_width;
};

extern float time_scale;
extern float splitter_rect_width;
extern float floor_rect_width;
extern Player* left_player;
extern Player* right_player;
extern bool player_dead;
extern Config config;
extern float obs_desync_probabilities[6];
extern float highscore[NUM_DIFFICULTIES];


#if ENABLE_EDITOR
extern Editor* editor;
extern bool in_editor;
extern int num_editor_props;
extern int editor_prop_types[];
extern const char* editor_prop_names[];
#endif

#if OLD_GENERATION_MODEL
extern Obstacle_Generator* left_obsgen;
extern Obstacle_Generator* right_obsgen;
#else
extern Obstacle_Generator* obsgen;
extern Tut_Obstacle_Generator* tut_obsgen;
#endif

struct Level_Data
{
    Config config;
    float duration;
    float next_level_transition_time;    
    float obs_desync_probabilities[6];
};

#define NUM_TUTORIAL_PHASES 9
extern int finished_difficulties[NUM_DIFFICULTIES];
extern int show_tutorial;
extern int first_game;
extern int entred_game;
extern char* tut_messages[NUM_TUTORIAL_PHASES];
//used_for_tutorial
extern int tut_player_jumped[2];
extern int tut_player_crouched[2];
extern int tut_player_shifted[2];



extern Level_Data levels_data[NUM_DIFFICULTIES][NUM_LEVELS];
extern float levels_accum_duration[NUM_DIFFICULTIES][NUM_LEVELS];
extern int current_level;
extern int current_difficulty;

extern char *highscores_store_key[NUM_DIFFICULTIES];
extern char *highscores_store_key_hash[NUM_DIFFICULTIES];

extern float play_time;


struct Screen_Shake
{
	Screen_Shake()
	{
		x = y = timer = amplitude = amplitude_decrease_rate = 0;
	}
	float x, y;
	float timer;
	float amplitude;
	float amplitude_decrease_rate;

	void reset_timer(float time,float max_amplitude);
	void update(float  dt);
};

double get_time();
float lerp(float a, float t, float b);
float coserp(float a, float t, float b);
bool box_intersect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

int random_selector(float* probabilities, int num_probabilities);

struct Swipe
{
    enum { UP, DOWN, LEFT, RIGHT };
    int dir;
    cocos2d::Vec2 start_pos;
};

void simulate_spring(float dt, float damp, float thightness, float target_x, float* spring_x, float* spring_vel);

cocos2d::Color3B get_grayscaled_current_color(cocos2d::Color3B color);

float frand();
int irand(int a, int b);


bool point_in_rect(float x, float y, float w, float h, float px, float py);
bool point_in_tri(float tx0, float ty0, float tx1, float ty1, float tx2, float ty2, float px, float py);

cocos2d::Color3B hsv_color_lerp(cocos2d::Color3B a, float t, cocos2d::Color3B b);
cocos2d::Color3B rgb_color_lerp(cocos2d::Color3B a, float t, cocos2d::Color3B b);

void rgb_to_hsv(cocos2d::Color3B color, float* h, float* s, float *b);
cocos2d::Color3B hsv_to_rgb(float h, float s, float b);


struct Color_Interpolator
{
	Color_Interpolator(float lerping_period=0, int ping_pong = 0);

	void init(float lerping_period, int ping_pong =0, int use_hsv_lerp =0);
	void update(float dt);
	void inverse(Color_Interpolator* colinter);

	void add_color(cocos2d::Color3B c);

	cocos2d::Color3B get_grayscaled_current_color();

	int num_colors;
	cocos2d::Color3B colors[MAX_INTERPOLATOR_COLORS];
	
	float timer;
	float period;
	int ping_pong;

	cocos2d::Color3B current_color;

	int use_hsv_lerp;
};

#if ENABLE_EDITOR
void save_editor_config_to_file(char* path);
#endif
