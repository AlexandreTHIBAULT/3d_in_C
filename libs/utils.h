#ifndef LIB_UTILS
    #define LIB_UTILS

    /********************/
    /* --- INCLUDES --- */
    /********************/

    #include <stdlib.h>
    #include <math.h>
    #include "maze/maze.h"

    /*******************/
    /* --- DEFINES --- */
    /*******************/

    #define W_WIDTH 1152
    #define W_HEIGHT 648

    #define V_NBRAY 300
    #define V_FOV (M_PI/2.f)

    #define K_UP         GLFW_KEY_W
    #define K_DOWN       GLFW_KEY_S
    #define K_LEFT       GLFW_KEY_A
    #define K_RIGHT      GLFW_KEY_D
    #define K_TURN_LEFT  GLFW_KEY_Q
    #define K_TURN_RIGHT GLFW_KEY_E
    #define K_JUMP       GLFW_KEY_SPACE
    #define K_SHOOT      GLFW_KEY_LEFT_SHIFT

    #define O_NB_MAX_MONSTERS 10
    #define O_NB_MAX_OBJECTS 256

    #define PILLAR_3_RADIUS 0.33f
    #define PILLAR_4_RADIUS 0.1f
    #define WALL_HEIGHT 11.f
    #define TILE_LENGTH 10
    #define VIEW_RANGE 7
    #define NB_MONSTER 20

    #define SHOW_FPS

    /*****************/
    /* --- TYPES --- */
    /*****************/

    enum side {
        S_RIGHT,
        S_TOP,
        S_LEFT,
        S_BOTTOM
    };

    typedef struct player
    {
        float X;
        float Y;
        float Z;
        float speedX;
        float speedY;
        float direction;
        int isJumping;
        double t_jumpStart;
        double t_lastShoot;
    } P_player;

    typedef struct ray
    {
        float X;
        float Y;
        float distance;
    } Ray;

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    int randMinMax(int min, int max);
    float modPI(float nb);
    float angleFromPosition(float x, float y, float d);
    enum side get_side(float X, float Y);
    int isPillar(M_map map, float x, float y);
    float pillarRadius(M_map map, float x, float y);
    float distanceToCenter(float x, float y);
    float distance(float x1, float y1, float x2, float y2);
    int isPositionFree(M_map map, float x, float y);

#endif