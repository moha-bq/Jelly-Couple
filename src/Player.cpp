#include "Player.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "HelloWorldScene.h"
#include "Obstacle_Generator.h"

using namespace cocos2d;

Player::Player(int type)
{
	this->type = type;
}

Player* Player::create(int type)
{
    Player* player = new Player(type);
    if (player && player->init())
    {
        player->autorelease();
        return player;
    }

    delete player; 
    player = nullptr;
    return nullptr;
}

bool Player::init()
{
#if PLAYER_SPRITE
    if (!Sprite::init())
        return false;
#else
    if (!DrawNode::init())
        return false;
#endif

    auto kb_listener = EventListenerKeyboard::create();
    kb_listener->onKeyPressed = CC_CALLBACK_2(Player::onKeyPressed, this);
    kb_listener->onKeyReleased = CC_CALLBACK_2(Player::onKeyReleased, this);

    auto touch_listener = EventListenerTouchAllAtOnce::create();
    touch_listener->onTouchesBegan = CC_CALLBACK_2(Player::onTouchesBegan, this);
    touch_listener->onTouchesMoved= CC_CALLBACK_2(Player::onTouchesMoved, this);
    touch_listener->onTouchesEnded = CC_CALLBACK_2(Player::onTouchesEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(kb_listener, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

    scheduleUpdate();

#if PLAYER_SPRITE
	this->setTexture("Px.png");
	size = 32.f;
#else
	size = 32;
#endif


	shifting_circle = DrawNode::create();
	shifting_circle->setVisible(false);
	game_scene_pointer->addChild(shifting_circle);

	num_taps_while_on_air = 0;

	init_distance_over_ground = 80;

	w = h = size;
	inity = y = 180;
/*
	if (type == LEFT_PLAYER)
	{
		x = 240 - splitter_rect_width / 2 - w;
	}
	else
	{
		x = 240 + splitter_rect_width / 2;
	}

	state = STANDING;
	last_state = STANDING;*/

	if (type == LEFT_PLAYER)
	{
		x = 240 - splitter_rect_width / 2 - w - init_distance_over_ground;
	}
	else
	{
		x = 240 + splitter_rect_width / 2 + init_distance_over_ground;
	}

	state = JUMPING;
	last_state = JUMPING;

	jumpkey = crouchkey = last_jumpkey = last_crouchkey = 0;
	velx = 0;
	gravity_shifted = 0;
	last_gravity_shifted = 0;

	max_double_press_interval = 0.5f;//secs

	land_spring_vel = 0;
	crouch_spring_vel = 0;

	angle = 0;
	rotation_done = 0;

	crouch_width = 0.5f*size;

	crouch_timer = config.crouch_duration;

	color = init_color;


	white_flash_duration = 0.3f;
	white_flash_timer = 0;

	num_swipes = 0;
	num_next_swipes = 0;

	accum_timer = 0;

	paused = false;
	nullify_input = false;

    return true;
}

void Player::reinit()
{
	num_taps_while_on_air = 0;
    w = h = size;
    y = inity;

    /*if (type == LEFT_PLAYER)
    {
        x = 240 - splitter_rect_width / 2 - w;
    }
    else
    {
        x = 240 + splitter_rect_width / 2;
    }

    state = STANDING;
    last_state = STANDING;*/

	if (type == LEFT_PLAYER)
	{
		x = 240 - splitter_rect_width / 2 - w - init_distance_over_ground;
	}
	else
	{
		x = 240 + splitter_rect_width / 2 + init_distance_over_ground;
	}

	state = JUMPING;
	last_state = JUMPING;



    jumpkey = crouchkey = last_jumpkey = last_crouchkey = 0;
    velx = 0;
    gravity_shifted = 0;
	last_gravity_shifted = 0;

    jump_prep_timer = config.jump_prep_duration;

    land_spring_vel = 0;

    crouch_spring_vel = 0;

    angle = 0;
    rotation_done = 0;

    crouch_timer = config.crouch_duration;

    white_flash_timer = 0;

    num_swipes = 0;
    num_next_swipes = 0;

    accum_timer = 0;

	paused = false;
	nullify_input = false;
}

void Player::update(float dt)
{
	dt *= time_scale;

	fixed_update(dt);
    /*accum_timer += dt;
	//XXX : this way of updating the player state is wrong. things that represent what happened in the last frame, now hold erroneous data!.. 
    while (accum_timer > FIXED_TIMESTEP)
    {
		//subdivide timestep so springs won't blow
        fixed_update(FIXED_TIMESTEP);
        accum_timer -= FIXED_TIMESTEP;
    }
	fixed_update(accum_timer);
	accum_timer = 0;*/
#if 0
	volatile int XXX = 0;
	for (int i = 0; i < 8000000; i++)
		XXX++;
#endif 
}

void Player::fixed_update(float dt)
{
	if (!paused)
	{
		missed_shift = false;
		/*for (int swipe_index = 0; swipe_index < num_swipes; swipe_index++)
		{
			Swipe* swipe = &swipes[swipe_index];

			if (type == RIGHT_PLAYER && swipe->start_pos.x > 240)
			{
				if (swipe->dir == Swipe::RIGHT)
				{
					jumpkey = 1;
					last_jumpkey = 0;
				}
				else if (swipe->dir == Swipe::LEFT)
				{
					crouchkey = 1;
					last_crouchkey = 0;
				}
			}
			if (type == LEFT_PLAYER && swipe->start_pos.x < 240)
			{
				if (swipe->dir == Swipe::LEFT)
				{
					jumpkey = 1;
					last_jumpkey = 0;
				}
				else if (swipe->dir == Swipe::RIGHT)
				{
					crouchkey = 1;
					last_crouchkey = 0;
				}
			}
		}*/
		elapsed_since_jumpkey_pressed += dt;
		//color = Color4F(this->getColor());
		last_gravity_shifted = gravity_shifted;
		last_state = state;

		if (gravity_shifted)
		{
			//std::swap(jumpkey, crouchkey);
			int temp = jumpkey;
			jumpkey = crouchkey;
			crouchkey = temp;

			//std::swap(last_jumpkey, last_crouchkey);
			temp = last_jumpkey;
			last_jumpkey = last_crouchkey;
			last_crouchkey = temp;
		}

		if (nullify_input)
		{
			jumpkey = 0;
			last_jumpkey = 0;
			crouchkey = 0;
			last_crouchkey = 0;
		}

		int jump_dir = 1;
		if (type == LEFT_PLAYER)
			jump_dir = -1;
		if (gravity_shifted)
			jump_dir = -jump_dir;

		if (state == STANDING)
		{
			if (jumpkey&&!last_jumpkey)
			{
				if (game_scene_pointer->tut_text_transition_state == HelloWorld::NO_TRANSITION)
					tut_player_jumped[type] = (type == 0) ? gravity_shifted : !gravity_shifted;
				state = JUMP_PREP;
				jump_prep_timer = config.jump_prep_duration;
#if USE_SIMPLE_AUDIO_ENGINE
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(JUMP_SFX);
#else
				if (!sfx_muted)
				{
					experimental::AudioEngine::play2d(JUMP_SFX, false, SFX_VOLUME);
				}
#endif
			}
			if (crouchkey&&!last_crouchkey)
			{
				if (game_scene_pointer->tut_text_transition_state == HelloWorld::NO_TRANSITION)
					tut_player_crouched[type] = (type == 0) ? gravity_shifted : !gravity_shifted;

				state = CROUCHING;
				crouch_timer = config.crouch_duration;
#if USE_SIMPLE_AUDIO_ENGINE
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(CROUCH_SFX);
#else
				if (!sfx_muted)
				{
					experimental::AudioEngine::play2d(CROUCH_SFX, false, SFX_VOLUME);
				}
#endif
			}
			if (w != size || fabs(land_spring_vel) > EPS)
			{
				float fixed_dt = FIXED_TIMESTEP;
				float acc = dt;
				while (acc > fixed_dt)
				{
					float sa = config.land_spring_tightness*(size - w) - config.land_spring_damp*land_spring_vel;
					land_spring_vel += sa*fixed_dt;
					w += land_spring_vel*fixed_dt;
					h -= land_spring_vel*fixed_dt;
					acc -= fixed_dt;
				}
				{
					float sa = config.land_spring_tightness*(size - w) - config.land_spring_damp*land_spring_vel;
					land_spring_vel += sa*acc;
					w += land_spring_vel*acc;
					h -= land_spring_vel*acc;
				}
				
			}
		}
		else if (state == JUMP_PREP)
		{
			if (jump_prep_timer > config.jump_prep_duration / 2)
			{
				float t = 1 - (jump_prep_timer - config.jump_prep_duration / 2) / (config.jump_prep_duration / 2);
				float squash_amount = coserp(0, t, -24);
				w = size + squash_amount;
				h = size - squash_amount;
			}
			else if (jump_prep_timer > 0)
			{
				float t = 1 - jump_prep_timer / (config.jump_prep_duration / 2);
				float squash_amount = coserp(-24, t, 0);
				w = size + squash_amount;
				h = size - squash_amount;
			}
			else
			{
				state = JUMPING;
				elapsed_since_jumpkey_pressed = 0;
				velx = jump_dir * config.jump_impulsion;
			}
			jump_prep_timer -= dt;
		}
		else if (state == JUMPING || state == SHIFTING)
		{
			if (jumpkey && !last_jumpkey && state == JUMPING)
			{
				if (elapsed_since_jumpkey_pressed < max_double_press_interval)
				{
					if (game_scene_pointer->tut_text_transition_state == HelloWorld::NO_TRANSITION)
						tut_player_shifted[type] = 1;

					//we have a double press/swipe/..
					gravity_shifted = !gravity_shifted;
					state = SHIFTING;
					velx = jump_dir * fmin(config.min_shift_velx, fabs(velx));
					rotation_done = 0;

					white_flash_timer = white_flash_duration;
					shifting_circle->setPosition(x + w/2,y + h/2);
					shifting_circle->setVisible(true);
#if USE_SIMPLE_AUDIO_ENGINE
					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SHIFT_SFX);
#else
					if (!sfx_muted)
					{
						experimental::AudioEngine::play2d(SHIFT_SFX, false, SFX_VOLUME);
					}
#endif
				}
				else
				{
					missed_shift = true;
				}
			}

			if (state == SHIFTING&&!rotation_done)
			{
				if (gravity_shifted)
				{
					angle += dt * config.rot_vel;
					if (angle > 180)
					{
						angle = 180;
						rotation_done = 1;
					}
				}
				else
				{
					angle -= dt * config.rot_vel;
					if (angle < 0)
					{
						angle = 0;
						rotation_done = 1;
					}
				}
			}

			{//TODO: move this out to a function with the spirng code too
				float accum_timer = dt;
				while (accum_timer > FIXED_TIMESTEP)
				{
					velx += -jump_dir * config.gravity_acc * FIXED_TIMESTEP;
					x += velx*FIXED_TIMESTEP;
					accum_timer -= FIXED_TIMESTEP;
				}
				velx += -jump_dir * config.gravity_acc * accum_timer;
				x += velx*accum_timer;
			}

			float velocity_squash_amount = (abs(velx) / config.max_velx) * config.velocity_squash_amount;
			w = size + (velocity_squash_amount);
			h = size - (velocity_squash_amount);

			handle_landing();
		}
		else if (state == CROUCHING)
		{
			if (w != crouch_width || fabs(land_spring_vel) > EPS)
			{
				float fixed_dt = FIXED_TIMESTEP;
				float acc = dt;
				while (acc > dt)
				{
					float sa = config.crouch_spring_tightness*(crouch_width - w) - config.crouch_spring_damp*crouch_spring_vel;
					crouch_spring_vel += sa*fixed_dt;
					w += crouch_spring_vel*fixed_dt;
					h -= crouch_spring_vel*fixed_dt;
					acc -= fixed_dt;
				}
				{
					float sa = config.crouch_spring_tightness*(crouch_width - w) - config.crouch_spring_damp*crouch_spring_vel;
					crouch_spring_vel += sa*acc;
					w += crouch_spring_vel*acc;
					h -= crouch_spring_vel*acc;
				}
			}
			/*if (type == LEFT_PLAYER)
			{
				static float ti = 0;
				if (w > crouch_width)
				{
					float ww = lerp(w, ti*4, crouch_width);
					h += (w - ww);
					w = ww;
					ti += dt;
					if (w <= crouch_width)
						ti = 0;
				}
			}
			else
			{
				static float ti = 0;
				if (w > crouch_width)
				{
					float ww = lerp(w, ti, crouch_width);
					h += (w - ww);
					w = ww;
					ti += dt;
					if (w <= crouch_width)
						ti = 0;
				}
			}*/

			crouch_timer -= dt;
			if (crouch_timer < 0)
			{
				state = STANDING;
#if USE_SIMPLE_AUDIO_ENGINE
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(CROUCH_END_SFX);
#else
				if (!sfx_muted)
				{
					experimental::AudioEngine::play2d(CROUCH_END_SFX, false, SFX_VOLUME);
				}
#endif
			}
		}

		//check if player landed after new position.. this is a special jump case!(for the passing through the ground artifact)
		if (state == JUMPING || state == SHIFTING)
			handle_landing();

		//align player with track
		if (state == STANDING || state == JUMP_PREP || state == CROUCHING)
			adjust_position_x();
		adjust_position_y();


		if (last_gravity_shifted)//reswap the keys
		{
			std::swap(jumpkey, crouchkey);
			std::swap(last_jumpkey, last_crouchkey);
		}


		last_jumpkey = jumpkey;
		last_crouchkey = crouchkey;

		//reset keys..used for taps
		jumpkey = crouchkey = false;
#if 0 // handle queued events while jumping or shifting
		if(state == STANDING && (last_state == JUMPING || last_state == SHIFTING))
		{
			bool processed_left = false;
			bool processed_right = false;
			for (int i = 0; i < num_taps_while_on_air; i++)
			{
				cocos2d::Vec2 tap_pos = taps_while_on_air[i];
				if (!processed_left && type == LEFT_PLAYER && tap_pos.x < 240)
				{
					if (tap_pos.x < 480 / 4)
						jumpkey = true;
					else
						crouchkey = true;
					processed_left = true;
				}
				else if (!processed_right && type == RIGHT_PLAYER && tap_pos.x>240)
				{
					if (tap_pos.x < (480 / 4) * 3)
						crouchkey = true;
					else
						jumpkey = true;
					processed_right = true;
				}
			}
			num_taps_while_on_air = 0;
		}
#endif
	}


	setPosition(Vec2(x + w / 2 + screen_shake_effect.x, y + h / 2 + screen_shake_effect.y));
	//setPosition(Vec2(x + w / 2, y + h / 2));
	if(type==LEFT_PLAYER)
		setRotation(-angle);
	else
		setRotation(angle);
	//setPosition(Vec2(0,0));

#if 0
	if (white_flash_timer > white_flash_duration / 2)
	{
		float t = (white_flash_timer - white_flash_duration / 2) / (white_flash_duration / 2);
		color.r = lerp(init_color.r, t, 1);
		color.g = lerp(init_color.g, t, 1);
		color.b = lerp(init_color.b, t, 1);
		white_flash_timer -= dt;
	}
	else if (white_flash_timer > 0)
	{
		float t = white_flash_timer / (white_flash_duration / 2);
		color.r = lerp(1, t, init_color.r);
		color.g = lerp(1, t, init_color.g);
		color.b = lerp(1, t, init_color.b);
		white_flash_timer -= dt;
	}
	else
	{
		color = init_color;
	}
#endif
	if (white_flash_timer > 0)
	{
		float t = white_flash_timer / white_flash_duration;
		shifting_circle->clear();
		shifting_circle->drawCircle(Vec2(0, 0), 32 * (1 - t), 0, 100, false, Color4F(color.r, color.g, color.b, t));
		/*shifting_circle->drawCircle(Vec2(0, 0), 20 * (1-t), 0, 100, false, Color4F(1, 1, 1, t*0.7));
		shifting_circle->drawCircle(Vec2(0, 0), 8 * (1 - t), 0, 100, false, Color4F(1, 1, 1, t*0.7));*/

		shifting_circle->setPosition(shifting_circle->getPosition() + Vec2(0, -config.obs_vely* 0.2 * dt));
		white_flash_timer -= dt;
	}
	else
	{
		shifting_circle->setVisible(false);
	}


#if PLAYER_SPRITE
	this->setScaleX((float)w / size);
	this->setScaleY((float)h / size);
	if (type == LEFT_PLAYER)
	{
		if (state == STANDING)
		{
			if (!gravity_shifted)
				setRotation(180);
			else
				setRotation(0);
		}
	}
	
#else
	clear();
	drawSolidRect(Vec2(-w / 2, -h / 2), Vec2(w / 2, h / 2), color);
#if 0
	float stroke_margin = 2;
	float stroke_size = 2;
	Color4F stroke_color = Color4F::WHITE;// Color4F((type == LEFT_PLAYER) ? level_themes[current_difficulty].right_player : level_themes[current_difficulty].left_player);

	if (type == LEFT_PLAYER)
	{
		drawSolidRect(Vec2(w / 2, h / 2 - stroke_margin), Vec2(-w / 2 + stroke_margin, h / 2 - stroke_margin - stroke_size), stroke_color);
		drawSolidRect(Vec2(-w / 2 + stroke_margin, h / 2 - stroke_margin), Vec2(-w / 2 + stroke_margin + stroke_size, -h / 2 + stroke_margin), stroke_color);
		drawSolidRect(Vec2(-w / 2 + stroke_margin, -h / 2 + stroke_margin), Vec2(w / 2, -h / 2 + stroke_margin + stroke_size), stroke_color);
	}
	else
	{
		drawSolidRect(Vec2(-w / 2, h / 2 - stroke_margin), Vec2(w / 2 - stroke_margin, h / 2 - stroke_margin - stroke_size), stroke_color);
		drawSolidRect(Vec2(w / 2 - stroke_margin, h / 2 - stroke_margin), Vec2(w / 2 - stroke_margin - stroke_size, -h / 2 + stroke_margin), stroke_color);
		drawSolidRect(Vec2(w / 2 - stroke_margin, -h / 2 + stroke_margin), Vec2(-w / 2, -h / 2 + stroke_margin + stroke_size), stroke_color);
	}
#endif
#endif

#if 0
	//draw coordinate system axes
	drawDot(Vec2(0, 0), 2, Color4F(1, 1, 1, 1));
	drawLine(Vec2(0, 0), Vec2(0, 50), Color4F(1, 1, 1, 1));
	drawLine(Vec2(0, 0), Vec2(50, 0), Color4F(1, 1, 1, 1));
#endif

}


void Player::onKeyPressed(EventKeyboard::KeyCode key, Event* ev)
{
	/*float max_dist = 200;
	float debug_var_l = left_player->get_distance_to_ground();
	float debug_var_r = right_player->get_distance_to_ground();*/

    if (type == RIGHT_PLAYER)
    {
        if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
            jumpkey = true;
        if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
            crouchkey = true;

		/*if ((state == Player::JUMPING || state == Player::SHIFTING) && get_distance_to_ground() < max_dist)
		{
			if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
				push_tap(Vec2(460, 0));
			if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
				push_tap(Vec2(300, 0));

		}*/
    }
	else
	{
		if (key == EventKeyboard::KeyCode::KEY_Q)
			jumpkey = true;
		if (key == EventKeyboard::KeyCode::KEY_D)
			crouchkey = true;

		/*if ((state == Player::JUMPING || state == Player::SHIFTING) && get_distance_to_ground() < max_dist)
		{
			if (key == EventKeyboard::KeyCode::KEY_Q)
				push_tap(Vec2(50, 0));
			if (key == EventKeyboard::KeyCode::KEY_D)
				push_tap(Vec2(220, 0));
		}*/
	}
}

void Player::onKeyReleased(EventKeyboard::KeyCode key, Event* ev)
{
    if (type == RIGHT_PLAYER)
    {
        if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
            jumpkey = false;
        if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
            crouchkey = false;
    }
    else
    {
        if (key == EventKeyboard::KeyCode::KEY_Q)
            jumpkey = false;
        if (key == EventKeyboard::KeyCode::KEY_D)
            crouchkey = false;
    }
}


void Player::handle_landing()
{
    if (gravity_shifted)
    {
        if (type == LEFT_PLAYER)
        {
            if (velx<0 && x - splitter_rect_width / 2 < 0)
            {
                state = STANDING;
                velx = 0;
            }
        }
        else
        {
            if (velx>0 && x + splitter_rect_width / 2 + w > 480)
            {
                state = STANDING;
                velx = 0;
            }
        }
    }
    else
    {
        if (type == LEFT_PLAYER)
        {
            if (velx>0 && x + w + splitter_rect_width / 2 > 240)
            {
                state = STANDING;
                velx = 0;
            }
        }
        else
        {
            if (velx<0 && x - splitter_rect_width / 2 < 240)
            {
                state = STANDING;
                velx = 0;
            }
        }
    }

	if (state == STANDING) // landed
	{
		screen_shake_effect.reset_timer(0.2f, 4);
#if USE_SIMPLE_AUDIO_ENGINE
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(LAND_SFX);
#else
		if (!sfx_muted)
		{
			experimental::AudioEngine::play2d(LAND_SFX, false, SFX_VOLUME);
		}
#endif
	}
}

void Player::adjust_position_x()
{
    if (gravity_shifted)
    {
        if (type == LEFT_PLAYER)
            x = floor_rect_width;
        else
            x = 480 - floor_rect_width - w;
    }
    else
    {
        if (type == LEFT_PLAYER)
            x = 240 - splitter_rect_width / 2 - w;
        else
            x = 240 + splitter_rect_width / 2;
    }

}

void Player::adjust_position_y()
{
    y = inity - (h - size) / 2;
}



void Player::onTouchesBegan(std::vector<Touch*> touches, Event* ev)
{
	/*for (auto touch : touches)
	{
		float max_dist = 40;
		float debug_var = left_player->get_distance_to_ground();
		debug_var = right_player->get_distance_to_ground();
		if (left_player->get_distance_to_ground() < max_dist)
		{
			push_tap(touch->getStartLocation());
		}
		if (right_player->get_distance_to_ground() < max_dist)
		{
			push_tap(touch->getStartLocation());
		}
	}*/
}
void Player::onTouchesMoved(std::vector<Touch*> touches, Event* ev)
{

}
void Player::onTouchesEnded(std::vector<Touch*> touches, Event* ev)
{
    //detect swipes
    for (auto touch : touches)
    {
        Vec2 current_pos = touch->getLocation();
        Vec2 start_pos = touch->getStartLocation();
        Vec2 delta = current_pos - start_pos;

        if (delta.lengthSquared() > 50 * 50)
        {
            Swipe swipe;
            swipe.start_pos = start_pos;
            if (fabs(delta.x) < fabs(delta.y))
            {
                if (delta.y > 0)//swipe up
                    swipe.dir = Swipe::UP;
                else
                    swipe.dir = Swipe::DOWN;
            }
            else
            {
                if (delta.x > 0)
                    swipe.dir = Swipe::RIGHT;
                else
                    swipe.dir = Swipe::LEFT;
            }

            push_swipe(swipe);
            
            // the sane thing to do is to add next_swipes only if the current swipe dir is different that the last one.. 
            /*
            if (state == STANDING)
                push_swipe(swipe);
            else
            {
                push_next_swipe(swipe);
            }*/
        }
    }

}

void Player::push_swipe(Swipe swipe)
{
    if (num_swipes == MAX_SWIPES)
        assert(!"CANT ADD MORE SWIPES");
    swipes[num_swipes++] = swipe;
}

void Player::push_next_swipe(Swipe swipe)
{
    if (num_next_swipes == MAX_SWIPES)
        assert(!"CANT ADD MORE NEXT_SWIPES");
    next_swipes[num_next_swipes++] = swipe;
}


float Player::get_distance_to_ground()
{
	if (gravity_shifted)
	{
		if (type == LEFT_PLAYER)
			return x - floor_rect_width;
		else
			return (480 - floor_rect_width - w) - x;
	}
	else
	{
		if (type == LEFT_PLAYER)
			return (240 - splitter_rect_width / 2 - w) - x;
		else
			return x - (240 + splitter_rect_width / 2);
	}
}

void Player::push_tap(Vec2 pos)
{
	if (num_taps_while_on_air == MAX_TAPS)
		return;
	taps_while_on_air[num_taps_while_on_air++] = pos;
}