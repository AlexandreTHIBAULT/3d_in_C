#ifndef LIB_OBJECT
    #define LIB_OBJECT

    /********************/
    /* --- INCLUDES --- */
    /********************/

    #include "../render/render.h"
    #include "../maze/maze.h"

    /*******************/
    /* --- DEFINES --- */
    /*******************/

    #define O_NB_MAX_MONSTERS 10
    #define O_NB_MAX_OBJECTS 255

    /*****************/
    /* --- TYPES --- */
    /*****************/

    typedef struct object
    {
        float x;
        float y;
        T_texture texture;
    } O_objects;

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    int O_isValidPosition(M_map map, float x, float y);
    int O_addObjects(O_objects objects[], M_map map, T_texture text_princ, T_texture text_sec);


#endif