#ifndef LIB_GAMEPLAY
    #define LIB_GAMEPLAY

    /********************/
    /* --- INCLUDES --- */
    /********************/

    #include <GLFW/glfw3.h>
    #include "../utils.h"
    #include "../object/object.h"

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    P_player G_action(GLFWwindow * window, P_player player, O_objects_data* missile_data, Ray missile_dir[], T_texture text, double t_delta);
    P_player G_movement(GLFWwindow * window, P_player player, M_map map,double t_delta);

#endif