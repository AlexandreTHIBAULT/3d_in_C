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

    

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    int O_isValidPosition(M_map map, float x, float y);
    int O_addObjects(O_object objects[], M_map map, T_texture text_princ, T_texture text_sec);
    O_objects_data O_spawnMonsters(M_map map, T_texture text_monster);
    void O_killMonster(O_objects_data* monsters_data, int monster_id);
    void O_shoot(P_player player, O_objects_data* missile_data, Ray missile_dir[], T_texture text);
    void O_updateShootMoove(O_objects_data* missile_data, Ray missile_dir[], double t_delta);
    void O_despawnShoot(O_objects_data* missile_data, Ray missile_dir[], int missile_id);
    void O_updateShoot(M_map map, O_objects_data* missile_data, Ray missile_dir[]);
    void O_checkEnemiesTouch(O_objects_data* monsters_data, O_objects_data* missile_data, Ray missile_dir[]);
    O_objects_data O_makeCompleteData(O_objects_data objects_data, O_objects_data monsters_data, O_objects_data missile_data);
    void O_monsterMoveOne(M_map map, O_objects_data* monsters_data, int monster_id, P_player player, float dist, double t_delta);
    void O_monsterMoveAll(M_map map, O_objects_data* monsters_data, P_player player, double t_delta);

#endif