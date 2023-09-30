#ifndef LIB_GAMEPLAY
    #define LIB_GAMEPLAY

    /********************/
    /* --- INCLUDES --- */
    /********************/

    #include <GLFW/glfw3.h>
    #include "../utils.h"

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    P_player G_movement(GLFWwindow * window, P_player player, M_map map,double t_delta);

#endif