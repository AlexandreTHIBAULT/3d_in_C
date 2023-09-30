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

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    int O_isValidPosition(M_map map, float x, float y);
    int O_addObjects(O_object objects[], M_map map, T_texture text_princ, T_texture text_sec);
    O_objects_data O_spawnMonsters(M_map map, T_texture text_monster);
    void O_killMonster(O_objects_data* monsters_data, int monster_id);
    O_objects_data O_makeCompleteData(O_objects_data objects_data, O_objects_data monsters_data);

#endif