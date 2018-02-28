#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "MenuScene.h"
#include "globals.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "LoadingScene.h"

#include <functional>

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
#include "SonarFrameworks.h"
#endif

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("MyCppGame" , Rect(0, 0, designResolutionSize.width*1.3, designResolutionSize.height*1.3));
        //glview = GLViewImpl::createWithRect("MyCppGame", Rect(0, 0, 400, 400));
#else
        glview = GLViewImpl::create("MyCppGame");
#endif
        director->setOpenGLView(glview);
    }


    //load config
#if 0&&(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

    FILE* file = fopen("config.txt", "r");
    char buff[1024];
    while (!feof(file))
    {
        char param_name[128];
        fgets(buff, 1024, file);

        sscanf(buff, "%s", param_name);
        if (!strcmp(param_name, "gravity_acceleration"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.gravity_acc);
        else if (!strcmp(param_name, "jump_speed"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.jump_impulsion);
        else if (!strcmp(param_name, "max_speed"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.max_velx);
        else if (!strcmp(param_name, "salabat_nabid_1"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.land_spring_tightness);
        else if (!strcmp(param_name, "mo3amil_i7tikak_nabid_1"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.land_spring_damp);
        else if (!strcmp(param_name, "salabat_nabid_2"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.crouch_spring_tightness);
        else if (!strcmp(param_name, "mo3amil_i7tikak_nabid_2"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.crouch_spring_damp);
        else if (!strcmp(param_name, "obs_speed"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.obs_vely);
        else if (!strcmp(param_name, "unite"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.unit);
        else if (!strcmp(param_name, "dist_mabin_obsat_u"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.obs_dist_in_batch_u);
        else if (!strcmp(param_name, "decalage_min_u"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.obs_lag_min_u);
        else if (!strcmp(param_name, "decalage_max_u"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.obs_lag_max_u);
        else if (!strcmp(param_name, "dist_chok_jay_min_u"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.next_obs_distance_min_u);
        else if (!strcmp(param_name, "dist_chok_jay_max_u"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.next_obs_distance_max_u);
        else if (!strcmp(param_name, "3adad_dlesobs_fchok_min"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.num_obs_per_batch_min);
        else if (!strcmp(param_name, "3adad_dlesobs_fchok_max"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.num_obs_per_batch_max);
        else if (!strcmp(param_name, "tol_obs_tn9az"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.jump_obs_width);
        else if (!strcmp(param_name, "tol_obs_7ni"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.crouch_obs_width);
        else if (!strcmp(param_name, "3rd_dles_obs"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.obs_height);
        else if (!strcmp(param_name, "t3baj_dsor3a"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.velocity_squash_amount);
        else if (!strcmp(param_name, "sor3a_dyal_doran"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.rot_vel);
        else if (!strcmp(param_name, "moda_dl7ni"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.crouch_duration);
        else if (!strcmp(param_name, "moda_prep_tn9az"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.jump_prep_duration);
        else if (!strcmp(param_name, "sor3a_minmum_fach_t9bl"))
            sscanf(buff+strlen(param_name)+1, "%f", &config.min_shift_velx);
        else if (!strcmp(param_name, "collision"))
            sscanf(buff+strlen(param_name)+1, "%d", &config.collision_enabled);
        else if (!strcmp(param_name, "decalage_3la_chok_min_u"))
            sscanf(buff + strlen(param_name) + 1, "%d", &config.first_obs_spike_lag_min_u);
        else if (!strcmp(param_name, "decalage_3la_chok_max_u"))
            sscanf(buff + strlen(param_name) + 1, "%d", &config.first_obs_spike_lag_max_u);
    }
#else


    //init all desync probabilities for all levels
    for(int i=0;i<NUM_LEVELS;i++)
        for(int j=0;j<NUM_DIFFICULTIES;j++)
            for(int k=0;k<6;k++)
                levels_data[i][j].obs_desync_probabilities[k] = 1.0f/6.0f;

    
    //easy : 160 - 210 - 260
    //normal : 300 - 350 -  400
    //hard : 440 - 490 - 540
    //super_hard : 580 - 630 - 680

    config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2290.f,//       land_spring_tightness;
        18.f,//         land_spring_damp;
        380.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        680.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        3,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1800.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        3,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };

    /*
    config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        1800.f,//       land_spring_tightness;
        13.f,//         land_spring_damp;
        200.f,//        crouch_spring_tightness;
        8.f,//          crouch_spring_damp;
        240.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        2,//            num_obs_per_batch_min;
        10,//           num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        205.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1000.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1 //          first_obs_spike_lag_max_u;
    };*/
#endif


	/////VERY EASY

	//++++ 160
	levels_data[0][0].config = { 1500.f,//      gravity_acc;
		550.f,//        jump_impulsion;
		1000.f,//       max_velx;
		1800.f,//       land_spring_tightness;
		13.f,//         land_spring_damp;
		150.f,//        crouch_spring_tightness;
		0.f,//          crouch_spring_damp;
		160.f,//        obs_vely;
		32,//           unit;
		13,//           obs_dist_in_batch_u;
		0,//            obs_lag_min_u;
		4,//            obs_lag_max_u;
		8,//            next_obs_distance_min_u;
		11,//           next_obs_distance_max_u;
		0,//            num_obs_per_batch_min;
		10,//           num_obs_per_batch_max;
		40.f,//         jump_obs_width;
		204.f,//         crouch_obs_width;
		20.f,//         obs_height;
		20.f,//         velocity_squash_amount;
		600.f,//        rot_vel;
		0.8f,//     crouch_duration;
		0.16f,//        jump_prep_duration;
		400.f,//        min_shift_velx;
		1,//            collision_enabled;
		-1,//           first_obs_spike_lag_min_u;
		1, //          first_obs_spike_lag_max_u;
		3,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
	};
	levels_data[0][0].duration = 20;
	levels_data[0][0].next_level_transition_time = 2;
	//levels_data[0][0].obs_desync_probabilities = {};
	for (int k = 0; k<6; k++)
		levels_data[0][0].obs_desync_probabilities[k] = 0.f;

	//++++ 210
	levels_data[0][1].config = { 1500.f,//      gravity_acc;
		550.f,//        jump_impulsion;
		1000.f,//       max_velx;
		1800.f,//       land_spring_tightness;
		13.f,//         land_spring_damp;
		180.f,//        crouch_spring_tightness;
		0.f,//          crouch_spring_damp;
		210.f,//        obs_vely;
		32,//           unit;
		12,//           obs_dist_in_batch_u;
		0,//            obs_lag_min_u;
		4,//            obs_lag_max_u;
		8,//            next_obs_distance_min_u;
		11,//           next_obs_distance_max_u;
		0,//            num_obs_per_batch_min;
		10,//           num_obs_per_batch_max;
		40.f,//         jump_obs_width;
		204.f,//         crouch_obs_width;
		20.f,//         obs_height;
		20.f,//         velocity_squash_amount;
		800.f,//        rot_vel;
		0.8f,//     crouch_duration;
		0.16f,//        jump_prep_duration;
		400.f,//        min_shift_velx;
		1,//            collision_enabled;
		-1,//           first_obs_spike_lag_min_u;
		1, //          first_obs_spike_lag_max_u;
		3,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
	};
	levels_data[0][1].duration = 20;
	levels_data[0][1].next_level_transition_time = 2;
	//    levels_data[0][1].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };

	//+++++++ 260
	levels_data[0][2].config = { 1500.f,//      gravity_acc;
		550.f,//        jump_impulsion;
		1000.f,//       max_velx;
		1860.f,//       land_spring_tightness;
		13.3f,//            land_spring_damp;
		203.f,//        crouch_spring_tightness;
		0.f,//          crouch_spring_damp;
		260.f,//        obs_vely;
		32,//           unit;
		12,//           obs_dist_in_batch_u;
		0,//            obs_lag_min_u;
		4,//            obs_lag_max_u;
		8,//            next_obs_distance_min_u;
		11,//           next_obs_distance_max_u;
		0,//            num_obs_per_batch_min;
		8,//            num_obs_per_batch_max;
		40.f,//         jump_obs_width;
		204.f,//         crouch_obs_width;
		20.f,//         obs_height;
		20.f,//         velocity_squash_amount;
		1000.f,//       rot_vel;
		0.8f,//     crouch_duration;
		0.16f,//        jump_prep_duration;
		400.f,//        min_shift_velx;
		1,//            collision_enabled;
		-1,//           first_obs_spike_lag_min_u;
		1, //          first_obs_spike_lag_max_u;
		2,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
	};
	levels_data[0][2].duration = 60;
	levels_data[0][2].next_level_transition_time = 1;
	//    levels_data[0][2].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };
	levels_data[0][2].obs_desync_probabilities[0] = 0.5;
	levels_data[0][2].obs_desync_probabilities[1] = 0.5;






    /////EASY
    
    //++++ 160
    levels_data[1][0].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        1800.f,//       land_spring_tightness;
        13.f,//         land_spring_damp;
        150.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        160.f,//        obs_vely;
        32,//           unit;
        13,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        10,//           num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        600.f,//        rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        3,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[1][0].duration = 23;
    levels_data[1][0].next_level_transition_time = 2;
    //levels_data[0][0].obs_desync_probabilities = {};

    //++++ 210
    levels_data[1][1].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        1800.f,//       land_spring_tightness;
        13.f,//         land_spring_damp;
        180.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        210.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        10,//           num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        800.f,//        rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        3,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[1][1].duration = 30;
    levels_data[1][1].next_level_transition_time = 2;
//    levels_data[0][1].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };

    //+++++++ 260
    levels_data[1][2].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        1860.f,//       land_spring_tightness;
        13.3f,//            land_spring_damp;
        203.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        260.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        8,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1000.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        2,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[1][2].duration = 60;
    levels_data[1][2].next_level_transition_time = 1;
//    levels_data[0][2].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };
    



    /////NORMAL

    //++++ 300
    levels_data[2][0].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        1920.f,//       land_spring_tightness;
        13.6f,//            land_spring_damp;
        206.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        300.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        7,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1500.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        2,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[2][0].duration = 18;
    levels_data[2][0].next_level_transition_time = 2;
//    levels_data[1][0].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };

    //++++ 350
    levels_data[2][1].config = config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2000.f,//       land_spring_tightness;
        14.f,//         land_spring_damp;
        210.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        350.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        7,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1500.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        1,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[2][1].duration = 30;
    levels_data[2][1].next_level_transition_time = 2;
//    levels_data[1][1].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };


    //levels_data[1].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f
    //                                            ,1.f/6.f ,1.f/6.f };

    //+++++++ 400
    levels_data[2][2].config = config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2120.f,//       land_spring_tightness;
        14.6f,//            land_spring_damp;
        226.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        400.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        11,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        6,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1500.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        1,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[2][2].duration = 60;
    levels_data[2][2].next_level_transition_time = 1;
//    levels_data[1][2].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };






    /////HARD

    //++++ 440
    levels_data[3][0].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2200.f,//       land_spring_tightness;
        15.f,//         land_spring_damp;
        330.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        440.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        12,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        4,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1700.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        0,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[3][0].duration = 18;
    levels_data[3][0].next_level_transition_time = 2;
//    levels_data[2][0].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };


    //++++ 490
    levels_data[3][1].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2230.f,//       land_spring_tightness;
        16.f,//         land_spring_damp;
        350.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        490.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        12,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        3,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1700.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        0,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[3][1].duration = 30;
    levels_data[3][1].next_level_transition_time = 2;
//    levels_data[2][1].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };


    //+++++++ 540
    levels_data[3][2].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2250.f,//       land_spring_tightness;
        16.5f,//            land_spring_damp;
        360.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        540.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        12,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        3,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1700.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        0,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[3][2].duration = 60;
    levels_data[3][2].next_level_transition_time = 1;
//    levels_data[2][2].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };





    /////SUPER_HARD

    //++++ 580
    levels_data[4][0].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2250.f,//       land_spring_tightness;
        16.5f,//            land_spring_damp;
        360.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        580.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        12,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        3,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1800.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        0,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[4][0].duration = 18;
    levels_data[4][0].next_level_transition_time = 2;
//    levels_data[3][0].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };


    //++++ 630
    levels_data[4][1].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2270.f,//       land_spring_tightness;
        17.f,//         land_spring_damp;
        370.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        630.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        12,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        3,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1800.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        0,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[4][1].duration = 30;
    levels_data[4][1].next_level_transition_time = 2;
//    levels_data[3][1].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };


    
    //+++++++ 680
    levels_data[4][2].config = { 1500.f,//      gravity_acc;
        550.f,//        jump_impulsion;
        1000.f,//       max_velx;
        2290.f,//       land_spring_tightness;
        18.f,//         land_spring_damp;
        380.f,//        crouch_spring_tightness;
        0.f,//          crouch_spring_damp;
        680.f,//        obs_vely;
        32,//           unit;
        12,//           obs_dist_in_batch_u;
        0,//            obs_lag_min_u;
        4,//            obs_lag_max_u;
        8,//            next_obs_distance_min_u;
        12,//           next_obs_distance_max_u;
        0,//            num_obs_per_batch_min;
        3,//            num_obs_per_batch_max;
        40.f,//         jump_obs_width;
        204.f,//         crouch_obs_width;
        20.f,//         obs_height;
        20.f,//         velocity_squash_amount;
        1800.f,//       rot_vel;
        0.8f,//     crouch_duration;
        0.16f,//        jump_prep_duration;
        400.f,//        min_shift_velx;
        1,//            collision_enabled;
        -1,//           first_obs_spike_lag_min_u;
        1, //          first_obs_spike_lag_max_u;
        0,//           obs_after_spike_decrement
		32.f,//     shift_obs_height
		20.f//		   shift_obs_width
    };
    levels_data[4][2].duration = 60;
    levels_data[4][2].next_level_transition_time = 1;
//    levels_data[3][2].obs_desync_probabilities = { 1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f,1.f/6.f ,1.f/6.f };

    
    for (int j = 0; j < NUM_DIFFICULTIES; j++)
    {
        levels_accum_duration[j][0] = levels_data[j][0].duration;
        for (int i = 1; i < NUM_LEVELS; i++)
        {
            levels_accum_duration[j][i] = levels_data[j][i].duration + levels_accum_duration[j][i - 1] + levels_data[j][i - 1].next_level_transition_time;
        }
    }


	level_themes[0] = {
		Color3B(255,236,53),//ground_obs
		Color3B(255,255,255),//ground_glow
		Color3B(255,24,119),//left_player
		Color3B(40,176,204) };

	level_themes[4] = {
		Color3B(0,161,81),//ground_obs
		Color3B(176, 204, 103),//ground_glow
		Color3B(255, 170, 2),//left_player
		Color3B(171, 1, 255) };

	level_themes[1] = {
		Color3B(240, 82, 0),//ground_obs
		Color3B(240, 240, 154),//ground_glow
		Color3B(67, 225, 10),//left_player
		Color3B(4, 4, 92) };

	level_themes[3] = {
		Color3B(146, 109, 39),//ground_obs
		Color3B(225,206, 154),//ground_glow
		Color3B(0, 255, 176),//left_player
		Color3B(216, 0, 255) };

	level_themes[2] = {
		Color3B(10, 88, 145),//ground_obs
		Color3B(187, 210, 255),//ground_glow
		Color3B(254, 255, 15),//left_player
		Color3B(255, 22, 17) };

#if USE_SIMPLE_AUDIO_ENGINE
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(.3f);
    //CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.1f);

    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MAIN_MUSIC);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(DEAD_MUSIC);

    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(JUMP_SFX);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SHIFT_SFX);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(LAND_SFX);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(DIE_SFX);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(CROUCH_SFX);

    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MAIN_MUSIC,true);
#else
    //experimental::AudioEngine::lazyInit();
	//TODO: make a bit field to mark loaded music (will be used in the loading screen)
	for (int i = 0; i < NUM_SOUNDS; i++)
		sound_loading_status[i] = 0;

    experimental::AudioEngine::preload(DIE_SFX, [&](bool isSuccess) {sound_loading_status[0] = 1; });
    experimental::AudioEngine::preload(MAIN_MUSIC, [&](bool isSuccess) {sound_loading_status[1] = 1; });
    experimental::AudioEngine::preload(JUMP_SFX, [&](bool isSuccess) {sound_loading_status[2] = 1; });
    experimental::AudioEngine::preload(SHIFT_SFX, [&](bool isSuccess) {sound_loading_status[3] = 1; });
    experimental::AudioEngine::preload(LAND_SFX, [&](bool isSuccess) {sound_loading_status[4] = 1; });
    experimental::AudioEngine::preload(CROUCH_SFX, [&](bool isSuccess) {sound_loading_status[5] = 1; });
    experimental::AudioEngine::preload(CROUCH_END_SFX, [&](bool isSuccess) {sound_loading_status[6] = 1; });
    experimental::AudioEngine::preload(MENU_MUSIC, [&](bool isSuccess) {sound_loading_status[7] = 1; });

	experimental::AudioEngine::preload(GAMEOVER_VOICE, [&](bool isSuccess) {sound_loading_status[8] = 1; });
	experimental::AudioEngine::preload(NEW_RECORD_SFX, [&](bool isSuccess) {sound_loading_status[9] = 1; });

#endif

	show_tutorial = UserDefault::getInstance()->getIntegerForKey("SHOW_TUTORAIL", 1);
    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60.0);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
    Size frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of large size.
    if (frameSize.width >= xLargeResolutionSize.width)
    {
		std::vector<std::string> search_paths;
		search_paths.push_back("imgs_xlarge_res");
		FileUtils::getInstance()->setSearchPaths(search_paths);
        director->setContentScaleFactor(MIN(xLargeResolutionSize.width /designResolutionSize.width, xLargeResolutionSize.height /designResolutionSize.height));
    }
	if (frameSize.width >= largeResolutionSize.width)
	{
		std::vector<std::string> search_paths;
		search_paths.push_back("imgs_large_res");
		FileUtils::getInstance()->setSearchPaths(search_paths);
		director->setContentScaleFactor(MIN(largeResolutionSize.width / designResolutionSize.width, largeResolutionSize.height / designResolutionSize.height));
	}
    else
    {
		std::vector<std::string> search_paths;
		search_paths.push_back("imgs_med_res");
		FileUtils::getInstance()->setSearchPaths(search_paths);
		director->setContentScaleFactor(MIN(mediumResolutionSize.width / designResolutionSize.width, mediumResolutionSize.height / designResolutionSize.height));
    }
    register_all_packages();


    //get high scores
	for (int i = 0; i < NUM_DIFFICULTIES; i++)
	{
		highscore[i] = UserDefault::getInstance()->getFloatForKey(highscores_store_key[i], -1);
		if (highscore[i] >= 0)
		{
			int hash_val = std::hash<int>{}((int)(highscore[i]*100));
			int stored_hash_val = UserDefault::getInstance()->getIntegerForKey(highscores_store_key_hash[i], -1);
			if (stored_hash_val != hash_val)
			{
				//Invalid highscore
				UserDefault::getInstance()->deleteValueForKey(highscores_store_key[i]);
				UserDefault::getInstance()->deleteValueForKey(highscores_store_key_hash[i]);
				highscore[i] = -1;
			}
		}
	}

    // create a scene. it's an autorelease object
    auto loading_scene = LoadingScene::createScene();
    // run
    director->runWithScene(loading_scene);

	/*auto menu_scene = MenuScene::createScene();
	director->runWithScene(menu_scene);*/

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

    experimental::AudioEngine::pauseAll();
    if (game_scene_pointer&&player_dead==false)
        game_scene_pointer->pause_game();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    if(!music_muted&&(!game_scene_pointer||player_dead==true))//the game should be paused so don't resume game_music
        experimental::AudioEngine::resumeAll();
}
