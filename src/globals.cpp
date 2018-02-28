#include "globals.h"
#include <chrono>


#if ENABLE_EDITOR
#include "Editor.h"
#endif

int finished_difficulties[NUM_DIFFICULTIES] = {};

char* leaderboard_ids[NUM_DIFFICULTIES] ={
	"CgkI79DInLoQEAIQBg",
	"CgkI79DInLoQEAIQBw",
	"CgkI79DInLoQEAIQCA",
	"CgkI79DInLoQEAIQCQ",
	"CgkI79DInLoQEAIQCg"
};
char* achievement_ids[NUM_DIFFICULTIES] = {
	"CgkI79DInLoQEAIQAQ",
	"CgkI79DInLoQEAIQAg",
	"CgkI79DInLoQEAIQAw",
	"CgkI79DInLoQEAIQBA",
	"CgkI79DInLoQEAIQBQ"
};


Level_Theme level_themes[5];

int games_to_interstetial = GAMES_BEFORE_INTERSTETIAL;

float ui_spring_tightness = 80.f;
float ui_spring_damp = 8.f;

HelloWorld* game_scene_pointer = 0;

int first_game = 1;
int show_tutorial = 1;
int entred_game = 0;

char* tut_messages[NUM_TUTORIAL_PHASES] = {
    "TAP HERE TO JUMP",
    "TAP HERE TO CROUCH",
    "TAP HERE TO JUMP",
	"TAP NOW TO FLIP GRAVITY",
	"TAP HERE TO JUMP",
	"TAP HERE TO CROUCH",
	"TAP HERE TO JUMP",
	"TAP NOW TO FLIP GRAVITY",
	"GOOD LUCK!"};

int tut_player_jumped[2] = {};
int tut_player_crouched[2] = {};
int tut_player_shifted[2] = {};

int music_muted = 0;
int sfx_muted = 0;

int jump_sfx_id;
int shift_sfx_id;
int land_sfx_id;
int die_sfx_id;
int crouch_sfx_id;
int dead_music;
int main_music;
int current_playing_music = -1;
int current_playing_sfx = -1;

int sound_loading_status[NUM_SOUNDS];

float time_scale = 1;

float music_transtion_timer = 0;
float music_transtion_duration = 5;

float splitter_rect_width = 16;
float floor_rect_width = 8;

float highscore[NUM_DIFFICULTIES];

Player* left_player;
Player* right_player;
Config config;

float obs_desync_probabilities[6] = { 1.f / 6,1.f / 6,1.f / 6,1.f / 6,1.f / 6,1.f / 6 };
//float obs_desync_probabilities[6] = {0,0,0,0,1,1};
Level_Data levels_data[NUM_DIFFICULTIES][NUM_LEVELS];
float levels_accum_duration[NUM_DIFFICULTIES][NUM_LEVELS];
char* highscores_store_key[NUM_DIFFICULTIES] = { "STORE0", "STORE1", "STORE2", "STORE3", "STORE4" };
char* highscores_store_key_hash[NUM_DIFFICULTIES] = {"STORE0X", "STORE1X", "STORE2X", "STORE3X", "STORE4X" };

int current_level = 0;
int current_difficulty = 0;
float play_time = 0;
bool player_dead = false;
Screen_Shake screen_shake_effect;

cocos2d::Size designResolutionSize = cocos2d::Size(480, 800);
cocos2d::Size smallResolutionSize = cocos2d::Size(240, 400);// /2
cocos2d::Size mediumResolutionSize = cocos2d::Size(480, 800);// *1
cocos2d::Size largeResolutionSize = cocos2d::Size(768, 1280);// *1.6
cocos2d::Size xLargeResolutionSize = cocos2d::Size(1536, 2560);// *3.2

#if ENABLE_EDITOR
Editor* editor;
bool in_editor;
#endif

#if OLD_GENERATION_MODEL
Obstacle_Generator* left_obsgen;
Obstacle_Generator* right_obsgen;
#else
Obstacle_Generator* obsgen;
Tut_Obstacle_Generator* tut_obsgen;
#endif

void simulate_spring(float dt, float damp, float tightness, float target_x, float* spring_x, float* spring_vel)
{
	//integrate over dt, using fixed_dt as timestep
	float fixed_dt = FIXED_TIMESTEP;
	while (dt > fixed_dt)
	{
		float sa = -(*spring_x - target_x)*tightness - (*spring_vel)*damp;
		*spring_vel += sa*fixed_dt;
		*spring_x += *spring_vel*fixed_dt;
		dt -= fixed_dt;
	}
	//remaining dt
	{
		float sa = -(*spring_x - target_x)*tightness - (*spring_vel)*damp;
		*spring_vel += sa*dt;
		*spring_x += *spring_vel*dt;
	}
}

void Screen_Shake::reset_timer(float time,float max_amplitude)
{
    timer = time;
    amplitude = max_amplitude;
    amplitude_decrease_rate = 10.0f;
}

void Screen_Shake::update(float  dt)
{
    if (timer > 0)
    {
        amplitude -= amplitude_decrease_rate *dt;
        if (amplitude < 0)
            amplitude = 0;

        x = amplitude * frand() - amplitude / 2;
        y = amplitude * frand() - amplitude / 2;

        timer -= dt;
    }
    else
    {
        x = 0;
        y = 0;
    }
}

double get_time()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

float lerp(float a, float t, float b)
{

    return (b*t + (1.0 - t)*a);
}

float coserp(float a, float t, float b)
{
    float tt;

    tt = (1 - cos(t*3.1415926f)) / 2;
    return(a*(1 - tt) + b*tt);
}

bool box_intersect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    if (x1>x2 + w2 || x1 + w1<x2 || y1>y2 + h2 || y1 + h1<y2)
        return false;
    return true;
}

unsigned int myrand()
{
    static unsigned int x = 123456789, y = 234567891, z = 345678912, w = 456789123, c = 0;

    int t;

    y ^= (y << 5); y ^= (y >> 7); y ^= (y << 22);

    t = z + w + c; z = w; c = t < 0; w = t & 2147483647;

    x += 1411392427;

    return x + y + w;
}


float frand()
{
    //return ((float)myrand() / (float)4294967168U);
    return CCRANDOM_0_1();
}

int irand(int a, int b)
{
    return (int)((b - a + 1)*frand() + a);
}

int random_selector(float* probabilities, int num_probabilities)
{
    float accum = probabilities[0];
    float rnd = frand();
    for (int i = 0; i < num_probabilities - 1; i++)
    {
        if (rnd < accum)
            return i;

        accum += probabilities[i + 1];
    }

    return num_probabilities - 1;
}

#if ENABLE_EDITOR
void save_editor_config_to_file(char* path)
{

    FILE* wfile = fopen(path, "w");
    if (wfile)
    {
        for (int i = 0; i < num_editor_props - 2; i++)
        {
            fprintf(wfile, "%s %s\n", editor_prop_names[i], editor->inputs[i]->getText());
        }

        fclose(wfile);
    }
    else
    {
        log("failed to save editor config to  file %s", path);
    }
}

int num_editor_props = 29;

int editor_prop_types[] = {//0 : int ; 1: float
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0 };


const char* editor_prop_names[] = {
    "gravity_acceleration",
    "jump_speed",
    "max_speed",
    "salabat_nabid_1",
    "mo3amil_i7tikak_nabid_1",
    "salabat_nabid_2",
    "mo3amil_i7tikak_nabid_2",
    "obs_speed",
    "unite",
    "dist_mabin_obsat_u",
    "decalage_min_u",
    "decalage_max_u",
    "dist_chok_jay_min_u",
    "dist_chok_jay_max_u",
    "3adad_dlesobs_fchok_min",
    "3adad_dlesobs_fchok_max",
    "tol_obs_tn9az",
    "tol_obs_7ni",
    "3rd_dles_obs",
    "t3baj_dsor3a",
    "sor3a_dyal_doran",
    "moda_dl7ni",
    "moda_prep_tn9az",
    "sor3a_minmum_fach_t9bl",
    "collision",
    "decalage_3la_chok_min_u",
    "decalage_3la_chok_max_u",
    "config_save_name",
    "load_config"};

#endif

bool point_in_rect(float x, float y, float w, float h, float px, float py)
{
    if (px<x || px>x + w || py<y || py>y + h)
        return false;

    return true;
}

bool point_in_tri(float tx0, float ty0, float tx1, float ty1, float tx2, float ty2, float px, float py)
{
    //compute (u,v) barycentric coordinates for (px,py)
    // (px,py) is inside the tri if and only if u>=0, v>=0, u+v<1

    float v0x = tx2 - tx0;
    float v0y = ty2 - ty0;

    float v1x = tx1 - tx0;
    float v1y = ty1 - ty0;

    float v2x = px - tx0;
    float v2y = py - ty0;

    float dot00 = v0x*v0x + v0y*v0y;
    float dot01 = v0x*v1x + v0y*v1y;
    float dot02 = v0x*v2x + v0y*v2y;
    float dot11 = v1x*v1x + v1y*v1y;
    float dot12 = v1x*v2x + v1y*v2y;

    float inv_denom = 1.f / (dot00*dot11 - dot01*dot01);
    float u = (dot11 * dot02 - dot01*dot12) * inv_denom;
    float v = (dot00 * dot12 - dot01*dot02) * inv_denom;

    return (u >= 0 && v >= 0 && u + v < 1);
}

float mymax(float a, float b)
{
    return ((a > b) ? a : b);
}
float mymin(float a, float b)
{
    return ((a < b) ? a : b);
}

void rgb_to_hsv(cocos2d::Color3B color, float *hue, float *sat, float *bri)
{
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;
    float max = mymax(mymax(r, g), b);
    float min = mymin(mymin(r, g), b);
    float delta = max - min;
    *hue = 0;
    *bri = max;
    *sat = (max == 0) ? 0 : (max - min) / max;

    if (delta != 0) {
        if (r == max) {
            *hue = (g - b) / delta;
        }
        else {
            if (g == max) {
                *hue = 2 + (b - r) / delta;
            }
            else {
                *hue = 4 + (r - g) / delta;
            }
        }
        *hue *= 60;
        if (*hue < 0) *hue += 360;
    }
}

cocos2d::Color3B hsv_to_rgb(float hue, float sat, float bri)
{
    float r, g, b;
    if (sat == 0) {
        r = g = b = bri;
    }
    else {
        if (hue == 360) hue = 0;
        hue /= 60;
        int i = (int)hue;
        float f = hue - i;
        float p = bri * (1 - sat);
        float q = bri * (1 - sat * f);
        float t = bri * (1 - sat * (1 - f));
        switch (i) {
        case 0:
            r = bri;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = bri;
            b = p;
            break;
        case 2:
            r = p;
            g = bri;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = bri;
            break;
        case 4:
            r = t;
            g = p;
            b = bri;
            break;
        case 5:
        default:
            r = bri;
            g = p;
            b = q;
            break;
        }
    }
    return cocos2d::Color3B((int)(r * 255 + 0.5), (int)(g * 255 + 0.5), (int)(b * 255 + 0.5));
}

cocos2d::Color3B rgb_color_lerp(cocos2d::Color3B a, float t, cocos2d::Color3B b)
{
    cocos2d::Color3B result;

    result.r = lerp((float)a.r, t, (float)b.r);
    result.g = lerp((float)a.g, t, (float)b.g);
    result.b = lerp((float)a.b, t, (float)b.b);

    return result;
}

cocos2d::Color3B hsv_color_lerp(cocos2d::Color3B a, float t, cocos2d::Color3B b)
{
    float ahue, asat, abri;
    float bhue, bsat, bbri;
    float result_hue, result_sat, result_bri;
    cocos2d::Color3B result;

    rgb_to_hsv(a, &ahue, &asat, &abri);
    rgb_to_hsv(b, &bhue, &bsat, &bbri);

    result_hue = lerp(ahue, t, bhue);
    result_sat = lerp(asat, t, bsat);
    result_bri = lerp(abri, t, bbri);

    result = hsv_to_rgb(result_hue, result_sat, result_bri);

    return result;
}

Color_Interpolator::Color_Interpolator(float lerping_period,int ping_pong)
{
    timer = 0;
    period = lerping_period;
    this->ping_pong = ping_pong;
	num_colors = 0;
}

void Color_Interpolator::init(float lerping_period, int ping_poing,int use_hsv_lerp)
{
    timer = 0;
    period = lerping_period;
    this->ping_pong = ping_pong;
    this->use_hsv_lerp = use_hsv_lerp;
	num_colors = 0;
}

void Color_Interpolator::update(float dt)
{
    if (timer < (num_colors - 1)*period)
    {
        int from = (timer / period);
        int to = from + 1;
        float t = (timer - from*period) / period;
        assert(t <= 1);
        if (use_hsv_lerp)
            current_color = hsv_color_lerp(colors[from], t, colors[to]);
        else
            current_color = rgb_color_lerp(colors[from], t, colors[to]);

    }
    else if(timer < (2*num_colors -1) * period)//reverse order if ping_pong, or go to first color
    {
        if (ping_pong)
        {
            float reverse_timer = timer - (num_colors - 1)*period;
            int reverse_from = (reverse_timer / period);
            int reverse_to = reverse_from + 1;
            float t = (reverse_timer - reverse_from*period) / period;
            assert(t <= 1);
            if(use_hsv_lerp)
                current_color = hsv_color_lerp(colors[reverse_from], t, colors[reverse_to]);
            else
                current_color = rgb_color_lerp(colors[reverse_from], t, colors[reverse_to]);

        }
        else
        {
            if (timer < num_colors*period)
            {
                int from = num_colors - 1;
                int to = 0;
                float t = (timer - (num_colors - 1)*period) / period;
                assert(t <= 1);
                if (use_hsv_lerp)
                    current_color = hsv_color_lerp(colors[from], t, colors[to]);
                else
                    current_color = rgb_color_lerp(colors[from], t, colors[to]);
            }
            else
            {
                timer -= num_colors*period;
            }
        }
    }
    else
    {
        timer -= (2 * num_colors - 1) * period;
    }

    timer += dt;
}

void Color_Interpolator::inverse(Color_Interpolator* colinter)
{
    num_colors = colinter->num_colors;
    for (int i = 0; i < num_colors; i++)
    {
        colors[i].r = 255 - colinter->colors[i].r;
        colors[i].g = 255 - colinter->colors[i].g;
        colors[i].b = 255 - colinter->colors[i].b;
    }

    period = colinter->period;
    timer = colinter->timer;
    ping_pong = colinter->ping_pong;
}

void Color_Interpolator::add_color(cocos2d::Color3B c)
{
    if (num_colors == MAX_INTERPOLATOR_COLORS)
    {
        assert(!"no more place for colors in the interpolator");
        return;
    }

    colors[num_colors++] = c;
}

cocos2d::Color3B Color_Interpolator::get_grayscaled_current_color()
{
	//TODO: search grayscaling methods
	int avg = (current_color.r + current_color.g + current_color.b) / 3;
	return cocos2d::Color3B(avg, avg, avg);
}
cocos2d::Color3B get_grayscaled_current_color(cocos2d::Color3B color)
{
	//TODO: search grayscaling methods
	int avg = (color.r + color.g + color.b) / 3;
	return cocos2d::Color3B(avg, avg, avg);
}

/*
void simulate_spring(float dt, float spring_rest, float spring_tightness, float spring_damp,float* spring_vel,float* spring_x)
{
    float accum_timer = dt;
    float spring_acc;
    while(accum_timer > FIXED_TIMESTEP)
    {
        spring_acc = spring_tightness*(*spring_x - spring_rest) - spring_damp*(*spring_vel);
        *spring_vel += spring_acc*FIXED_TIMESTEP;
        *spirng_x += spring_vel*FIXED_TIMESTEP;
        accum_timer -= FIXED_TIMESTEP;
    }
    spring_acc = spring_tightness*(*spring_x - spring_rest) - spring_damp*(*spring_vel);
    *spring_vel += spring_acc*accum_timer;
    *spirng_x += spring_vel*accum_timer;
}
*/