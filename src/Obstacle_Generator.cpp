#include "Obstacle_Generator.h"
#include "Player.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "HelloWorldScene.h"

float obs_spawn_y = 1200;

Obstacle_Generator::Obstacle_Generator(float ix, float iy)
{
    x = ix;
    y = iy;
}


Obstacle_Generator* Obstacle_Generator::create(float ix,float iy)
{
    Obstacle_Generator* obsgen = new Obstacle_Generator(ix,iy);
    if (obsgen && obsgen->init())
    {
        obsgen->autorelease();
        return obsgen;
    }

    delete obsgen;
    return nullptr;
}

bool Obstacle_Generator::init()
{
    if (!Node::init())
        return false;

    accum_timer = 0;

#if OLD_GENERATION_MODEL
    genx[0] = 240 + splitter_rect_width / 2;
    genx[1] = 480 - floor_rect_width;

    next_obs_distance = 500;
    last_obstacle = NULL;

    generating_side = 0;
    left_generator = 0;

#else

    first_generation = 1;
    last_obstacle_y = 0;

    left_generator_state.generating_side = 0;
    //left_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
    left_generator_state.num_remaining_obstacles = irand(4, 6);//first_generation
    left_generator_state.state = Obstacle_Generator_State::GENERATING;
    left_generator_state.last_state = Obstacle_Generator_State::SHIFTING;
    left_generator_state.did_special_move = 0;
	left_generator_state.first_obstacle_generated = 0;

    right_generator_state.generating_side = 1;
    //right_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
    right_generator_state.num_remaining_obstacles = irand(4, 6);
    right_generator_state.state = Obstacle_Generator_State::GENERATING;
    right_generator_state.last_state = Obstacle_Generator_State::SHIFTING;
    right_generator_state.did_special_move = 0;
	right_generator_state.first_obstacle_generated = 0;

    right_genx[0] = 240 + splitter_rect_width / 2;
    right_genx[1] = 480 - floor_rect_width;

    left_genx[0] = floor_rect_width;
    left_genx[1] = 240 - splitter_rect_width / 2;

    right_generator_state.last_spike_obs_y = obs_spawn_y;
    left_generator_state.last_spike_obs_y = obs_spawn_y;

#endif

	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obstacles_glow_sprites[i] = Sprite::create();
		obstacles_glow_sprites[i]->setAnchorPoint(Vec2(0, 0));
		obstacles_glow_sprites[i]->setVisible(false);
		obstacles_glow_sprites[i]->setBlendFunc(BlendFunc::ADDITIVE);
		obstacles_glow_sprites[i]->setPosition(Vec2(0, obs_spawn_y));
		//obstacles_glow_sprites[i]->setColor(Color3B(255,0,0));
		addChild(obstacles_glow_sprites[i],1);
	}

	for (int i = 0; i < MAX_OBSTACLES; i++)
	{
		obstacles_pool[i] = Obstacle::create();
		obstacles_pool[i]->id = i;
		obstacles_pool[i]->generator = this;
		obstacles_pool[i]->kill();
		obstacles_pool[i]->setPosition(Vec2(0, obs_spawn_y));
		addChild(obstacles_pool[i],0);
	}

    paused = false;
    scheduleUpdate();
    return true;
}

void Obstacle_Generator::update(float dt)
{
	dt *= time_scale;

    if (!paused)
    {
#if OLD_GENERATION_MODEL
        generate();
#else
        last_obstacle_y += config.obs_vely*dt;
        new_generate(dt);
#endif
    }
}


#if OLD_GENERATION_MODEL


void Obstacle_Generator::generate()
{
    if (!last_obstacle || (obs_spawn_y - (last_obstacle->y + last_obstacle->h)) > next_obs_distance)
    {
        int num_obs_per_batch = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
        float obsy = obs_spawn_y + irand(config.first_obs_spike_lag_min_u, config.first_obs_spike_lag_max_u)*config.unit;
        Obstacle* obs;
        for (int i = 0; i < num_obs_per_batch; i++)
        {
            obs = get_free_obs();
            obs->revive();
            obs->reinit();

            float rnd = frand();

            if (rnd < 0.5)
            {
                obs->set(Obstacle::JUMP_OBS, !generating_side, genx[!generating_side], obsy, config.jump_obs_width, config.obs_height);
            }
            else
            {
                if (left_generator)
                {
                    if (!generating_side)
                        obs->set(Obstacle::CROUCH_OBS, !generating_side, genx[!generating_side] - 20, obsy, config.crouch_obs_width, config.obs_height);
                    else
                        obs->set(Obstacle::CROUCH_OBS, !generating_side, genx[!generating_side] + 20, obsy, config.crouch_obs_width, config.obs_height);
                }
                else
                {
                    if (!generating_side)
                        obs->set(Obstacle::CROUCH_OBS, !generating_side, genx[!generating_side] - 20, obsy, config.crouch_obs_width, config.obs_height);
                    else
                        obs->set(Obstacle::CROUCH_OBS, !generating_side, genx[!generating_side] + 20, obsy, config.crouch_obs_width, config.obs_height);

                }
            }
            obs->update_graphic();
            float extra_dist = irand(config.obs_lag_min_u, config.obs_lag_max_u) * config.unit;
            obsy += config.obs_dist_in_batch_u * config.unit + extra_dist;
        }

        float safety_dist = config.unit;
        float spikes_length = obsy - obs_spawn_y + safety_dist;

        obs = get_free_obs();
        obs->revive();
        obs->reinit();
        obs->type = Obstacle::SHIFT_OBS;
        obs->set(Obstacle::SHIFT_OBS, generating_side, genx[generating_side], obs_spawn_y, config.obs_height, spikes_length);
        obs->update_graphic();
        last_obstacle = obs;//last obstacle is the spike
        next_obs_distance = irand(config.next_obs_distance_min_u, config.next_obs_distance_max_u)*config.unit;

        generating_side = !generating_side;
    }
}

#else

void Obstacle_Generator::generate_left_obs(int left_desync)
{
	//!!!!!!!!!!!!!!!!!!
	//NOTE : if we're on the first level, this method generates the left and right obstacles..
	//!!!!!!!!!!!!!!!!!!

    int after_obs_end_spikes = config.obs_dist_in_batch_u;

    float y_axis_translation = left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit - obs_spawn_y;
	if (left_generator_state.first_obstacle_generated == 0)
	{
		y_axis_translation = 0;
		left_generator_state.first_obstacle_generated = 1;
	}

    //don't desync if this is the first obstacle in a batch
    if (left_generator_state.last_state == Obstacle_Generator_State::SHIFTING)
        left_desync = 0;

    int generating_side = left_generator_state.generating_side;
    float special_move_probability = 1;
    
    Obstacle* obs;

    if (left_generator_state.last_state == Obstacle_Generator_State::SHIFTING && left_generator_state.num_remaining_obstacles == 0)///zero obstacles to generate (special case)
    {
#define SPECIAL_MOVE_KEEP_SHIFT_STEP_ZERO_CASE 0
#if SPECIAL_MOVE_KEEP_SHIFT_STEP_ZERO_CASE
        if (frand() < special_move_probability)
        {
            after_obs_end_spikes = config.obs_dist_in_batch_u*2;
            left_generator_state.did_special_move = 1;
        }

        left_generator_state.state = Obstacle_Generator_State::SHIFTING;
#else   //remove shift step
        if (frand() < special_move_probability)
        {
            left_generator_state.did_special_move = 1;
            left_generator_state.state = Obstacle_Generator_State::GENERATING;
            left_generator_state.saved_state = Obstacle_Generator_State::SHIFTING;
            left_generator_state.generating_side = !left_generator_state.generating_side;
            left_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
        }

#endif
     
		int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
		create_shift_spikes(spikes_number, generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation);

		if(current_difficulty == 0)
		{
			int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
			create_shift_spikes(spikes_number, !generating_side, right_genx[!generating_side], obs_spawn_y + y_axis_translation,1);
		}

    }
    else
    {
        if (left_generator_state.did_special_move)// there should'nt be an obstacle
        {
            left_generator_state.did_special_move = 0;

			int spikes_number = config.obs_dist_in_batch_u;
			create_shift_spikes(spikes_number, generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation);

			if (current_difficulty == 0)
			{
				int spikes_number = config.obs_dist_in_batch_u;
				create_shift_spikes(spikes_number, !generating_side, right_genx[!generating_side], obs_spawn_y + y_axis_translation,1);
			}

        }
        else
        {
            obs = get_free_obs();
            obs->reinit();
            obs->revive();

            float rnd = frand();
            if (rnd < 0.5)
            {
                obs->set(Obstacle::JUMP_OBS, !generating_side, left_genx[!generating_side], obs_spawn_y + left_desync + y_axis_translation, config.jump_obs_width, config.obs_height);
				obs->update_graphic();
				if (current_difficulty == 0)
				{
					obs = get_free_obs();
					obs->reinit();
					obs->revive();
					
					if (play_time < 50)
					{
						int special_right_desync = 0;
						if (play_time > 35)
						{
							float rnd2 = frand();
							if (rnd2 < 0.3f)
								special_right_desync = 1;
							else if (rnd2 < 0.6f)
								special_right_desync = -1;
						}

						obs->set(Obstacle::JUMP_OBS, generating_side, right_genx[generating_side], obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit
							, config.jump_obs_width, config.obs_height,1);
					}
					else
					{
						if (frand() < 0.5f)
						{
							int special_right_desync = 0;

							float rnd2 = frand();
							if (rnd2 < 0.20f)
								special_right_desync = 1;
							else if (rnd2 < 0.40f)
								special_right_desync = -1;
							else if (rnd2 < 0.60f)
								special_right_desync = 2;
							else if (rnd2 < 0.80f)
								special_right_desync = -2;

							obs->set(Obstacle::JUMP_OBS, generating_side, right_genx[generating_side], obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit
								, config.jump_obs_width, config.obs_height,1);
						}
						else
						{
							int special_right_desync = 0;

							float rnd2 = frand();
							if (rnd2 < 0.20f)
								special_right_desync = 1;
							else if (rnd2 < 0.40f)
								special_right_desync = -1;
							else if (rnd2 < 0.60f)
								special_right_desync = 2;
							else if (rnd2 < 0.80f)
								special_right_desync = -2;

							if (generating_side)
								obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[!generating_side],
									obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit, config.crouch_obs_width, config.obs_height,1);
							else
								obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[generating_side] + 20,
									obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit, config.crouch_obs_width, config.obs_height,1);

						}
					}

					obs->update_graphic();
				}
            }
            else
            {
                if (!generating_side)
                    obs->set(Obstacle::CROUCH_OBS, !generating_side, left_genx[generating_side], obs_spawn_y + left_desync + y_axis_translation, config.crouch_obs_width, config.obs_height);
                else
                    obs->set(Obstacle::CROUCH_OBS, !generating_side, left_genx[!generating_side] + 20, obs_spawn_y + left_desync + y_axis_translation, config.crouch_obs_width, config.obs_height);
				obs->update_graphic();

				if (current_difficulty == 0)
				{
					obs = get_free_obs();
					obs->reinit();
					obs->revive();

					if (play_time < 50)
					{
						int special_right_desync = 0;
						if (play_time > 35)
						{
							float rnd2 = frand();
							if (rnd2 < 0.3f)
								special_right_desync = 1;
							else if (rnd2 < 0.6f)
								special_right_desync = -1;
						}

						if (generating_side)
							obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[!generating_side],
								obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit, config.crouch_obs_width, config.obs_height,1);
						else
							obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[generating_side] + 20,
								obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit, config.crouch_obs_width, config.obs_height,1);
					}
					else
					{
						if (frand() < 0.5f)
						{
							int special_right_desync = 0;
							float rnd2 = frand();
							if (rnd2 < 0.20f)
								special_right_desync = 1;
							else if (rnd2 < 0.40f)
								special_right_desync = -1;
							else if (rnd2 < 0.60f)
								special_right_desync = 2;
							else if (rnd2 < 0.80f)
								special_right_desync = -2;

							obs->set(Obstacle::JUMP_OBS, generating_side, right_genx[generating_side], obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit
								, config.jump_obs_width, config.obs_height,1);
						}
						else
						{
							int special_right_desync = 0;

							float rnd2 = frand();
							if (rnd2 < 0.20f)
								special_right_desync = 1;
							else if (rnd2 < 0.40f)
								special_right_desync = -1;
							else if (rnd2 < 0.60f)
								special_right_desync = 2;
							else if (rnd2 < 0.80f)
								special_right_desync = -2;

							if (generating_side)
								obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[!generating_side],
									obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit, config.crouch_obs_width, config.obs_height,1);
							else
								obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[generating_side] + 20,
									obs_spawn_y + left_desync + y_axis_translation + special_right_desync*config.unit, config.crouch_obs_width, config.obs_height,1);

						}
					}
					obs->update_graphic();
				}
            }
            



            left_generator_state.num_remaining_obstacles--; 
            if (left_generator_state.num_remaining_obstacles <= 0)
            {
#define SPECIAL_MOVE_KEEP_SHIFT_STEP 0
#if SPECIAL_MOVE_KEEP_SHIFT_STEP
                if (frand() < special_move_probability)
                {
                    after_obs_end_spikes  = config.obs_dist_in_batch_u * 2;
                    left_generator_state.did_special_move = 1;
                }

                left_generator_state.state = Obstacle_Generator_State::SHIFTING;
#else   //remove shift step
                if (frand() < special_move_probability)
                {
                    left_generator_state.did_special_move = 1;
                    left_generator_state.state = Obstacle_Generator_State::GENERATING;
                    left_generator_state.saved_state = Obstacle_Generator_State::SHIFTING;
                    left_generator_state.generating_side = !left_generator_state.generating_side;
                    left_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
                }

#endif
				int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
				create_shift_spikes(spikes_number, generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation);

				if (current_difficulty == 0)
				{

					int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
					create_shift_spikes(spikes_number, !generating_side, right_genx[!generating_side], obs_spawn_y + y_axis_translation,1);
				}
            }
            else
            {
                //spikes; not last obstacle in batch..generate a full spikes segment
				int spikes_number = config.obs_dist_in_batch_u;
				create_shift_spikes(spikes_number, generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation);

				if (current_difficulty == 0)
				{
					int spikes_number = config.obs_dist_in_batch_u;
					create_shift_spikes(spikes_number, !generating_side, right_genx[!generating_side], obs_spawn_y + y_axis_translation,1);
				}
            }
        }
    }
}

void Obstacle_Generator::generate_right_obs(int right_desync)
{
    int after_obs_end_spikes = config.obs_dist_in_batch_u;
    float y_axis_translation = left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit - obs_spawn_y;
	if (right_generator_state.first_obstacle_generated == 0)
	{
		y_axis_translation = 0;
		right_generator_state.first_obstacle_generated = 1;
	}
    //don't desync if this is the first obstacle in a batch
    if (right_generator_state.last_state == Obstacle_Generator_State::SHIFTING)
        right_desync = 0;

    int generating_side = right_generator_state.generating_side;
    float special_move_probability = 1;
    Obstacle* obs;


    if (right_generator_state.last_state == Obstacle_Generator_State::SHIFTING && right_generator_state.num_remaining_obstacles == 0)
    {
#if SPECIAL_MOVE_KEEP_SHIFT_STEP_ZERO_CASE
        if (frand() < special_move_probability)
        {
            after_obs_end_spikes = config.obs_dist_in_batch_u * 2;
            right_generator_state.did_special_move = 1;
        }

        right_generator_state.state = Obstacle_Generator_State::SHIFTING;
#else   //remove shift step
        if (frand() < special_move_probability)
        {
            right_generator_state.did_special_move = 1;
            right_generator_state.state = Obstacle_Generator_State::GENERATING;
            right_generator_state.saved_state = Obstacle_Generator_State::SHIFTING;
            right_generator_state.generating_side = !right_generator_state.generating_side;
            right_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
        }

#endif

		int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
		create_shift_spikes(spikes_number, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation,1);
    }
    else
    {
        if (right_generator_state.did_special_move)// there should'nt be an obstacle
        {
            right_generator_state.did_special_move = 0;

			int spikes_number = config.obs_dist_in_batch_u;
			create_shift_spikes(spikes_number, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation,1);
        }
        else
        {
            obs = get_free_obs();
            obs->reinit();
            obs->revive();

            float rnd = frand();
            if (rnd < 0.5)
            {
                obs->set(Obstacle::JUMP_OBS, !generating_side, right_genx[!generating_side], obs_spawn_y + right_desync + y_axis_translation, config.jump_obs_width, config.obs_height,1);
            }
            else
            {
                if (!generating_side)
                    obs->set(Obstacle::CROUCH_OBS, !generating_side, right_genx[generating_side], obs_spawn_y + right_desync + y_axis_translation,
						config.crouch_obs_width, config.obs_height,1);
                else
                    obs->set(Obstacle::CROUCH_OBS, !generating_side, right_genx[!generating_side] + 20, obs_spawn_y + right_desync + y_axis_translation,
						config.crouch_obs_width, config.obs_height,1);


            }
            obs->update_graphic();



            right_generator_state.num_remaining_obstacles--;
            if (right_generator_state.num_remaining_obstacles <= 0)
            {
#if SPECIAL_MOVE_KEEP_SHIFT_STEP
                if (frand() < special_move_probability)
                {
                    after_obs_end_spikes = config.obs_dist_in_batch_u * 2;
                    right_generator_state.did_special_move = 1;
                }

                right_generator_state.state = Obstacle_Generator_State::SHIFTING;
#else   //remove shift step
                if (frand() < special_move_probability)
                {
                    right_generator_state.did_special_move = 1;
                    right_generator_state.state = Obstacle_Generator_State::GENERATING;
                    right_generator_state.saved_state = Obstacle_Generator_State::SHIFTING;
                    right_generator_state.generating_side = !right_generator_state.generating_side;
                    right_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
                }

#endif
				int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
				create_shift_spikes(spikes_number, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation,1);
            }
            else
            {
				int spikes_number = config.obs_dist_in_batch_u;
				create_shift_spikes(spikes_number, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation,1);
            }
        }
    }
}

void Obstacle_Generator::new_generate(float dt)
{
    left_generator_state.last_spike_obs_y -= config.obs_vely*dt;

    if (first_generation ||
        ( (left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit - config.obs_vely*dt) < obs_spawn_y )
        )
    {
        
        int left_desync;

        if(current_difficulty == 0){
            left_desync = 0;
        }
        else{
            left_desync = random_selector(levels_data[current_level][current_difficulty].obs_desync_probabilities, 6) * config.unit;
        }
        int right_desync = left_desync;

        left_generator_state.saved_state = left_generator_state.state;
        right_generator_state.saved_state = right_generator_state.state;

        if (left_generator_state.state == Obstacle_Generator_State::GENERATING
            && right_generator_state.state == Obstacle_Generator_State::GENERATING)
        {
            int left_to_desync = (frand() < 0.5f);
            if (left_to_desync)
                right_desync = 0;
            else
                left_desync = 0;

            //left
            {
                generate_left_obs(left_desync);
            }

            //right
            {
				//the left generates right obstacle in order to make exact duplicates
				if(current_difficulty!=0)
                generate_right_obs(right_desync);
            }
        }
        else if (left_generator_state.state == Obstacle_Generator_State::SHIFTING
            && right_generator_state.state == Obstacle_Generator_State::GENERATING)
        {
			if (current_difficulty != 0)
			{
				left_generator_state.state = Obstacle_Generator_State::GENERATING;
				left_generator_state.generating_side = !left_generator_state.generating_side;
				left_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);

				//right
				{
					generate_right_obs(right_desync);
				}
			}
        }
        else if (left_generator_state.state == Obstacle_Generator_State::GENERATING
            && right_generator_state.state == Obstacle_Generator_State::SHIFTING)
        {
			if (current_difficulty != 0)
			{
				right_generator_state.state = Obstacle_Generator_State::GENERATING;
				right_generator_state.generating_side = !right_generator_state.generating_side;
				right_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
				//left
				{
					generate_left_obs(left_desync);
				}
			}
        }
        else
        {
            right_generator_state.state = Obstacle_Generator_State::GENERATING;
            right_generator_state.generating_side = !right_generator_state.generating_side;
            right_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);

            left_generator_state.state = Obstacle_Generator_State::GENERATING;
            left_generator_state.generating_side = !left_generator_state.generating_side;
            left_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
        }
        
        left_generator_state.last_state = left_generator_state.saved_state;
        right_generator_state.last_state = right_generator_state.saved_state;

		if(!first_generation)
        left_generator_state.last_spike_obs_y = left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit;

        first_generation = 0;

		if(current_difficulty == 0)
		{
			right_generator_state = left_generator_state;
		}
    }
}
#endif


Obstacle* Obstacle_Generator::get_free_obs()
{
    int obs_index = 0;
    for (obs_index = 0; obs_index < MAX_OBSTACLES; obs_index++)
    {
        if (!obstacles_pool[obs_index]->alive)break;
    }
    if (obs_index == MAX_OBSTACLES)
        assert(!"NOT ENOUGH PLACES IN OBSTACLES POOL");

    return obstacles_pool[obs_index];
}

void Obstacle_Generator::mypause()
{
    paused = true;
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles_pool[i]->mypause();
    }
}

void Obstacle_Generator::myresume()
{
    paused = false;
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles_pool[i]->myresume();
    }
}

void Obstacle_Generator::reinit()
{
    
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles_pool[i]->kill();
    }
#if OLD_GENERATION_MODEL
    last_obstacle = NULL;
    float rnd = frand();
    if (rnd < 0.5f)
    {
        if (left_generator)
            generating_side = 0;
        else
            generating_side = 1;
    }
    else
    {
        if (left_generator)
            generating_side = 1;
        else
            generating_side = 0;
    }
#else
    left_generator_state.last_state = Obstacle_Generator_State::SHIFTING;
    right_generator_state.last_state = Obstacle_Generator_State::SHIFTING;
    first_generation = 1;
    last_obstacle_y = 0;

    right_generator_state.last_spike_obs_y = obs_spawn_y;
    left_generator_state.last_spike_obs_y = obs_spawn_y;

    left_generator_state.did_special_move = 0;
    right_generator_state.did_special_move = 0;

    left_generator_state.generating_side = 0;
    right_generator_state.generating_side = 1;

    left_generator_state.num_remaining_obstacles = irand(4, 6);//first_generation
    right_generator_state.num_remaining_obstacles = irand(4, 6);//first_generation
    
    left_generator_state.state = Obstacle_Generator_State::GENERATING;
    right_generator_state.state = Obstacle_Generator_State::GENERATING;

	left_generator_state.first_obstacle_generated = 0;
	right_generator_state.first_obstacle_generated = 0;

#endif

    accum_timer = 0;
    paused = false;
}

Obstacle* Obstacle_Generator::create_shift_spikes(int num,int generating_side,float x,float y,int generated_by_right)
{
	int spikes_num = (int)(config.unit / config.shift_obs_height) * num;
	Obstacle* first = 0;
	for (int i = 0; i < spikes_num; i++)
	{
		Obstacle* obs = get_free_obs();
		obs->reinit();
		obs->revive();
		obs->type = Obstacle::SHIFT_OBS;
		obs->set(Obstacle::SHIFT_OBS, generating_side, x, y + i*config.shift_obs_height,
				config.shift_obs_width, config.shift_obs_height,generated_by_right);
		obs->update_graphic();
		if (i == 0)
			first = obs;
	}
	return first;
}

void Tut_Obstacle_Generator::generate_obs()
{
	int after_obs_end_spikes = config.obs_dist_in_batch_u;

	float y_axis_translation = left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit - obs_spawn_y;
	if (left_generator_state.first_obstacle_generated == 0)
	{
		y_axis_translation = 0;
		left_generator_state.first_obstacle_generated = 1;
	}

	int generating_side = left_generator_state.generating_side;
	float special_move_probability = 1;

	Obstacle* obs;
	obs = get_free_obs();
	obs->reinit();
	obs->revive();

	if (next_obs_type==0 || next_obs_type == 3)
	{
		obs->set(Obstacle::JUMP_OBS, !generating_side, left_genx[!generating_side], obs_spawn_y + y_axis_translation, config.jump_obs_width, config.obs_height);
		obs->update_graphic();
		obs = get_free_obs();
		obs->reinit();
		obs->revive();

		obs->set(Obstacle::JUMP_OBS, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation, config.jump_obs_width, config.obs_height, 1);
		obs->update_graphic();

		int spikes_number = config.obs_dist_in_batch_u;
		create_shift_spikes(spikes_number, generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation);
		create_shift_spikes(spikes_number, !generating_side, right_genx[!generating_side], obs_spawn_y + y_axis_translation, 1);
		
		jump_obs = obs;
	}
	else if(next_obs_type == 1 || next_obs_type == 4)
	{
		if (!generating_side)
			obs->set(Obstacle::CROUCH_OBS, !generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation, config.crouch_obs_width, config.obs_height);
		else
			obs->set(Obstacle::CROUCH_OBS, !generating_side, left_genx[!generating_side] + 20, obs_spawn_y + y_axis_translation, config.crouch_obs_width, config.obs_height);
		obs->update_graphic();

		obs = get_free_obs();
		obs->reinit();
		obs->revive();
		if (generating_side)
			obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[!generating_side],
				obs_spawn_y + y_axis_translation, config.crouch_obs_width, config.obs_height, 1);
		else
			obs->set(Obstacle::CROUCH_OBS, generating_side, right_genx[generating_side] + 20,
				obs_spawn_y + y_axis_translation, config.crouch_obs_width, config.obs_height, 1);
		obs->update_graphic();

		int spikes_number = (after_obs_end_spikes - config.obs_after_spike_decrement);
		create_shift_spikes(spikes_number, generating_side, left_genx[generating_side], obs_spawn_y + y_axis_translation);
		create_shift_spikes(spikes_number, !generating_side, right_genx[!generating_side], obs_spawn_y + y_axis_translation, 1);

		crouch_obs = obs;
	}
	else if(next_obs_type==2)
	{
		left_generator_state.did_special_move = 1;
		left_generator_state.state = Obstacle_Generator_State::GENERATING;
		left_generator_state.saved_state = Obstacle_Generator_State::SHIFTING;
		left_generator_state.generating_side = !left_generator_state.generating_side;

		int spikes_number = config.obs_dist_in_batch_u;
		create_shift_spikes(spikes_number, !generating_side, left_genx[!generating_side], obs_spawn_y + y_axis_translation);
		shift_obs1 = create_shift_spikes(spikes_number, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation, 1);
	}
	else
	{
		left_generator_state.did_special_move = 1;
		left_generator_state.state = Obstacle_Generator_State::GENERATING;
		left_generator_state.saved_state = Obstacle_Generator_State::SHIFTING;
		left_generator_state.generating_side = !left_generator_state.generating_side;

		int spikes_number = config.obs_dist_in_batch_u;
		create_shift_spikes(spikes_number, !generating_side, left_genx[!generating_side], obs_spawn_y + y_axis_translation);
		shift_obs2 = create_shift_spikes(spikes_number, generating_side, right_genx[generating_side], obs_spawn_y + y_axis_translation, 1);
	}

	
}


void Tut_Obstacle_Generator::new_generate(float dt)
{
	if (next_obs_type >= 6)
		return;

	left_generator_state.last_spike_obs_y -= config.obs_vely*dt;

	if (first_generation ||
		((left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit - config.obs_vely*dt) < obs_spawn_y)
		)
	{
		left_generator_state.saved_state = left_generator_state.state;
		right_generator_state.saved_state = right_generator_state.state;

		if (left_generator_state.state == Obstacle_Generator_State::GENERATING
			&& right_generator_state.state == Obstacle_Generator_State::GENERATING)
		{
			generate_obs();
			next_obs_type++;
		}
		else
		{
			right_generator_state.state = Obstacle_Generator_State::GENERATING;
			right_generator_state.generating_side = !right_generator_state.generating_side;
			right_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);

			left_generator_state.state = Obstacle_Generator_State::GENERATING;
			left_generator_state.generating_side = !left_generator_state.generating_side;
			left_generator_state.num_remaining_obstacles = irand(config.num_obs_per_batch_min, config.num_obs_per_batch_max);
		}

		left_generator_state.last_state = left_generator_state.saved_state;
		right_generator_state.last_state = right_generator_state.saved_state;

		if (!first_generation)
			left_generator_state.last_spike_obs_y = left_generator_state.last_spike_obs_y + config.obs_dist_in_batch_u * config.unit;
		first_generation = 0;
		right_generator_state = left_generator_state;
	}
}

Tut_Obstacle_Generator* Tut_Obstacle_Generator::create(float ix, float iy)
{
	Tut_Obstacle_Generator* obsgen = new Tut_Obstacle_Generator(ix, iy);
	if (obsgen && obsgen->init())
	{
		obsgen->autorelease();
		return obsgen;
	}

	delete obsgen;
	return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:





void Obstacle::draw(float opacity)
{
#if !USE_SPRITES
	clear();
	float outline_size = 1;
	float spiky_point_w = 20;
	if (type == SHIFT_OBS)
	{
		drawSolidRect(Vec2(0, 0), Vec2(w, h), Color4F(1, 0, 0, opacity));
	}
	else if (type == JUMP_OBS || type ==CROUCH_OBS)
	{
		Vec2 verts[] = {
			Vec2(0,0),
			Vec2(w,0),
			Vec2(w - spiky_point_w, h / 2),
			Vec2(w, h),
			Vec2(0, h)};
		Color4F color(getColor());
		color.a = opacity;
		drawPolygon(verts, 5, color, outline_size, Color4F(0, 0, 0, opacity));
	}
#endif
}
Obstacle::Obstacle()
{
    type = JUMP_OBS;
    alive = false;
}

Obstacle* Obstacle::create()
{
    Obstacle* obs = new Obstacle();
    if (obs&& obs->init())
    {
        obs->autorelease();
        return obs;
    }

    delete obs;
    return nullptr;
}

bool Obstacle::init()
{
#if USE_SPRITES==1
    if (!Sprite::init())
        return false;
#else
	if (!DrawNode::init())
		return false;
#endif

    accum_timer = 0;
    paused = false;

	setAnchorPoint(Vec2(0, 0));

#if USE_SPRITES==1 && RENDER_OBSTACLE_GLOW
	//TODO: this is stupid, create different struct for different obs type.
	glow = Sprite::create();
	glow->setAnchorPoint(Vec2(0, 0));
	glow->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(glow);
#endif

#if OBSTACLE_DEBUG_DRAW
	debug_draw = DrawNode::create();
	game_scene_pointer->addChild(debug_draw,1000);
#endif

    scheduleUpdate();
    return true;
}

void Obstacle::update(float dt)
{
	dt *= time_scale;

    if (!paused)
    {
        if (alive)
        {
            y -= config.obs_vely * dt;

			if (y + h < -100)
				kill();
//BOX OBSTALES COLLISION
#if 1
			if ((x < 240 && box_intersect(left_player->x, left_player->y, left_player->w, left_player->h, x, y, w-1.f, h))
				|| (x >= 240 && box_intersect(right_player->x, right_player->y, right_player->w, right_player->h, x, y, w-1.f, h)))
			{
				player_dead = true;
				screen_shake_effect.reset_timer(0.5f, 10);
			}
#endif
//TRIANGLE OBSTACLES COLLISION
#if 0
            if (x < 240 && box_intersect(left_player->x, left_player->y, left_player->w, left_player->h, x, y, w, h))
            {
				if (type == SHIFT_OBS)
				{
					player_dead = true;
					screen_shake_effect.reset_timer(0.5f, 10);
				}
				else if (type == JUMP_OBS)
				{
					//triangle coords;
					float tx0, ty0, tx1, ty1, tx2, ty2;
					if (side == 0)//left
					{
						tx0 = x;
						ty0 = y;
						tx1 = x;
						ty1 = y + h;
						tx2 = x + w;
						ty2 = y + h / 2;
					}
					else
					{
						tx0 = x + w;
						ty0 = y;
						tx1 = x + w;
						ty1 = y + h;
						tx2 = x;
						ty2 = y + h / 2;
					}

					//this is just an approximative intersection test
					//NOTE: I think this is sufficient in our case. Could be wrong.
					if (point_in_rect(left_player->x, left_player->y, left_player->w, left_player->h, tx0, ty0)||
						point_in_rect(left_player->x, left_player->y, left_player->w, left_player->h, tx1, ty1)||
						point_in_rect(left_player->x, left_player->y, left_player->w, left_player->h, tx2, ty2)||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x, left_player->y)||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x+ left_player->w, left_player->y)||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x+ left_player->w, left_player->y+ left_player->h)||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x, left_player->y+ left_player->h)
						)
					{
						player_dead = true;
						screen_shake_effect.reset_timer(0.5f, 10);
					}
				}
				else if(type == CROUCH_OBS)
				{
					//collision with the rect body of the crouch obs
					float bx, by, bw, bh;
					if (side == 0)//left
					{
						//jump_obs_width is the width of the spike
						bx = x + config.jump_obs_width;
						by = y;
						bw = w - config.jump_obs_width;
						bh = h;
					}
					else
					{
						bx = x;
						by = y;
						bw = w - config.jump_obs_width;
						bh = h;
					}

					if (box_intersect(bx, by, bw, bh, left_player->x, left_player->y, left_player->w, left_player->h))
					{
						player_dead = true;
						screen_shake_effect.reset_timer(0.5f, 10);
					}
					else
					{
						//triangle coords;
						float tx0, ty0, tx1, ty1, tx2, ty2;
						if (side == 0)//left
						{
							tx0 = x;
							ty0 = y + h/2;
							tx1 = x + config.jump_obs_width;
							ty1 = y;
							tx2 = x + config.jump_obs_width;
							ty2 = y + h;
						}
						else
						{
							tx0 = x + w - config.jump_obs_width;
							ty0 = y;
							tx1 = x + w - config.jump_obs_width;
							ty1 = y + h;
							tx2 = x + w;
							ty2 = y + h / 2;
						}

						//this is just an approximative intersection test
						//NOTE: I think this is sufficient in our case. Could be wrong.
						if (point_in_rect(left_player->x, left_player->y, left_player->w, left_player->h, tx0, ty0) ||
							point_in_rect(left_player->x, left_player->y, left_player->w, left_player->h, tx1, ty1) ||
							point_in_rect(left_player->x, left_player->y, left_player->w, left_player->h, tx2, ty2) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x, left_player->y) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x + left_player->w, left_player->y) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x + left_player->w, left_player->y + left_player->h) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, left_player->x, left_player->y + left_player->h)
							)
						{
							player_dead = true;
							screen_shake_effect.reset_timer(0.5f, 10);
						}
					}
				}
            }
			else if (x >= 240 && box_intersect(right_player->x, right_player->y, right_player->w, right_player->h, x, y, w, h))
			{
				if (type == SHIFT_OBS)
				{
					player_dead = true;
					screen_shake_effect.reset_timer(0.5f, 10);
				}
				else if (type == JUMP_OBS)
				{
					//triangle coords;
					float tx0, ty0, tx1, ty1, tx2, ty2;
					if (side == 0)//left
					{
						tx0 = x;
						ty0 = y;
						tx1 = x;
						ty1 = y + h;
						tx2 = x + w;
						ty2 = y + h / 2;
					}
					else
					{
						tx0 = x + w;
						ty0 = y;
						tx1 = x + w;
						ty1 = y + h;
						tx2 = x;
						ty2 = y + h / 2;
					}

					//this is just an approximative intersection test
					//NOTE: I think this is sufficient in our case. Could be wrong.
					if (point_in_rect(right_player->x, right_player->y, right_player->w, right_player->h, tx0, ty0) ||
						point_in_rect(right_player->x, right_player->y, right_player->w, right_player->h, tx1, ty1) ||
						point_in_rect(right_player->x, right_player->y, right_player->w, right_player->h, tx2, ty2) ||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x, right_player->y) ||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x + right_player->w, right_player->y) ||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x + right_player->w, right_player->y + right_player->h) ||
						point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x, right_player->y + right_player->h)
						)
					{
						player_dead = true;
						screen_shake_effect.reset_timer(0.5f, 10);
					}
				}
				else if (type == CROUCH_OBS)
				{
					//collision with the rect body of the crouch obs
					float bx, by, bw, bh;
					if (side == 0)
					{
						//jump_obs_width is the width of the spike
						bx = x + config.jump_obs_width;
						by = y;
						bw = w - config.jump_obs_width;
						bh = h;
					}
					else
					{
						bx = x;
						by = y;
						bw = w - config.jump_obs_width;
						bh = h;
					}

					if (box_intersect(bx, by, bw, bh, right_player->x, right_player->y, right_player->w, right_player->h))
					{
						player_dead = true;
						screen_shake_effect.reset_timer(0.5f, 10);
					}
					else
					{
						//triangle coords;
						float tx0, ty0, tx1, ty1, tx2, ty2;
						if (side == 0)
						{
							tx0 = x;
							ty0 = y + h / 2;
							tx1 = x + config.jump_obs_width;
							ty1 = y;
							tx2 = x + config.jump_obs_width;
							ty2 = y + h;
						}
						else
						{
							tx0 = x + w - config.jump_obs_width;
							ty0 = y;
							tx1 = x + w - config.jump_obs_width;
							ty1 = y + h;
							tx2 = x + w;
							ty2 = y + h / 2;
						}

						//this is just an approximative intersection test
						//NOTE: I think this is sufficient in our case. Could be wrong.
						if (point_in_rect(right_player->x, right_player->y, right_player->w, right_player->h, tx0, ty0) ||
							point_in_rect(right_player->x, right_player->y, right_player->w, right_player->h, tx1, ty1) ||
							point_in_rect(right_player->x, right_player->y, right_player->w, right_player->h, tx2, ty2) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x, right_player->y) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x + right_player->w, right_player->y) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x + right_player->w, right_player->y + right_player->h) ||
							point_in_tri(tx0, ty0, tx1, ty1, tx2, ty2, right_player->x, right_player->y + right_player->h)
							)
						{
							player_dead = true;
							screen_shake_effect.reset_timer(0.5f, 10);
						}
					}
				}
			}
#endif

            accum_timer += dt;
            while (accum_timer > FIXED_TIMESTEP)
            {
                fixed_update(FIXED_TIMESTEP);
                accum_timer -= FIXED_TIMESTEP;
            }
        }
    }

#if OBSTACLE_DEBUG_DRAW
	debug_draw->setPosition(Vec2(x + screen_shake_effect.x, y + screen_shake_effect.y));
#endif

#if USE_SPRITES==1
	//x,y,w,h represent the collision box, if we are on the right side of a track, we're rotated 180 degrees..
	// adjust the position of the sprite so it is aligned with the collision geometry
	if (type ==SHIFT_OBS)
	{
		if (side == 1)//right
		{
			setPosition(Vec2(x + screen_shake_effect.x + w, y + screen_shake_effect.y + h + 5));// 5 = (shift_image_height - shift_obs_height)/2
		}
		else
			setPosition(Vec2(x + screen_shake_effect.x, y + screen_shake_effect.y - 5));
	}
	else if (type == JUMP_OBS)
	{
		
		if (side == 1)//right
		{
			setPosition(Vec2(x + screen_shake_effect.x + w, y + screen_shake_effect.y + h + 5));// 5 = (jump_image_height - jump_obs_height)/2
		}
		else
			setPosition(Vec2(x + screen_shake_effect.x, y + screen_shake_effect.y - 5));
	}
	else if (type == CROUCH_OBS)
	{
		if (side == 1)//right
		{
			setPosition(Vec2(x + screen_shake_effect.x, y + screen_shake_effect.y - 5));
			//setPosition(Vec2(100, 500));
		}
		else
		{
			setPosition(Vec2(x + screen_shake_effect.x + w, y + screen_shake_effect.y + h + 5));// 5 = (crouch_image_height - crouch_obs_height)/2
			//setPosition(Vec2(100, 20));
		}
	}
	//glow->setPosition(Vec2(x + screen_shake_effect.x, y + screen_shake_effect.y));
#else
	setPosition(Vec2(x, y));
#endif


	if (y > 568)
	{
		float t = (y - 568.f) / 200;
		if (t > 1)
			t = 1;

#if USE_SPRITES==1
		setOpacity((1 - t) * 255);
		setScale(1-t);
#else
		draw(1-t);
#endif
	}
	else
	{
		//TODO: remove this..(make transparancy one at the end of the alpha transition)
		//TODO: remove this fucking shiiiiit
#if USE_SPRITES==1
		setScale(1);
		setOpacity(255);
#else
		draw(1);
#endif
	}

	if (y+h < 120 && y+h>0)
	{
		float t = (y+h) / 120;
		//t = t*t;
#if USE_SPRITES==1
		setOpacity(t * 255);
		setScale(t);
#else
		draw(t);
#endif
	}

	generator->obstacles_glow_sprites[id]->setPosition(getPosition());
	generator->obstacles_glow_sprites[id]->setRotation(getRotation());
	generator->obstacles_glow_sprites[id]->setScale(getScale());
	generator->obstacles_glow_sprites[id]->setOpacity(getOpacity());
}

void Obstacle::fixed_update(float dt)
{
}

void Obstacle::update_graphic()
{
#if !USE_OBSTACLE_SPRITE
    clear();
    setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	if (type == CROUCH_OBS)
		drawSolidRect(Vec2(0, 0), Vec2(w, h), Color4F(1, 1, 0, 1));
	else
		drawSolidRect(Vec2(0, 0), Vec2(w, h), Color4F(1, 0, 0, 1));
#endif

#if OBSTACLE_DEBUG_DRAW
	debug_draw->clear();
	debug_draw->drawSolidRect(Vec2(0, 0), Vec2(w, h), Color4F(1, 1, 0, 0.5));
#endif
}

void Obstacle::reinit()
{
    y = obs_spawn_y;
    accum_timer = 0;
	setScale(1);
	setOpacity(255);
    paused = false;
}

void Obstacle::set(int t, int side, float x, float y, float w, float h, int generated_by_right)
{
	type = t;
	this->side = side;
	this->generated_by_right = generated_by_right;

#if USE_SPRITES==1
    
    if (t == JUMP_OBS)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
		if (side == 1)//right
		{
			setRotation(180);
			this->x -= w;
		}
		else
		{
			setRotation(0);
		}

		setTexture("spike.png");
		generator->obstacles_glow_sprites[id]->setTexture("spike_glow.png");
#if RENDER_OBSTACLE_GLOW
		glow->setTexture("spike_glow.png");
#endif
    }
    else if (t == CROUCH_OBS)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
		if (side == 0)//left
		{
			setRotation(180);
		}
		else
		{
			setRotation(0);
		}

		setTexture("long_spike.png");
		generator->obstacles_glow_sprites[id]->setTexture("long_spike_glow.png");
#if RENDER_OBSTACLE_GLOW
		glow->setTexture("long_spike_glow.png");
#endif
    }
    else if (t == SHIFT_OBS)
    {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		if (side == 1)//right
		{
			setRotation(180);
			this->x -= w;
		}
		else
		{
			setRotation(0);
		}

		setTexture("shift_spike.png");
		generator->obstacles_glow_sprites[id]->setTexture("shift_spike_glow.png");
#if RENDER_OBSTACLE_GLOW
		glow->setTexture("shift_spike_glow.png");
#endif
    }
#else
	type = t;

	if (t == JUMP_OBS)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		if (side == 1)//right
			this->x -= w;
	}
	else if (t == CROUCH_OBS)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		if (side == 1)//right
		{
			this->x -= w;
		}
	}
	else if (t == SHIFT_OBS)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		if (side == 1)//right
			this->x -= w;
	}
#endif
}


void Obstacle::kill()
{
	generator->obstacles_glow_sprites[id]->setVisible(false);
	paused = true;
	alive = false;
	setVisible(false);
	setPosition(Vec2(0, obs_spawn_y));
	generator->obstacles_glow_sprites[id]->setPosition(Vec2(0, obs_spawn_y));
}

void Obstacle::revive()
{
	paused = false;
	alive = true;
	setPosition(Vec2(0, obs_spawn_y));
	generator->obstacles_glow_sprites[id]->setPosition(Vec2(0, obs_spawn_y));
	setVisible(true);
	generator->obstacles_glow_sprites[id]->setVisible(true);
}