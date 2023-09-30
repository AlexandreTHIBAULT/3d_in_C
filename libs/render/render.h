#ifndef LIB_RENDER
    #define LIB_RENDER

    /********************/
    /* --- INCLUDES --- */
    /********************/

    #include <GLFW/glfw3.h>
    #include "../color/color.h"
    #include "../utils.h"
    #include <stb_image.h>

    /*****************/
    /* --- TYPES --- */
    /*****************/

    typedef struct texture
    {
        GLuint image;
        int width;
        int height;
        int nrChannels;
    } T_texture;
    
    typedef struct object
    {
        float x;
        float y;
        T_texture texture;
    } O_object;

    typedef struct objects_data
    {
        O_object objects[O_NB_MAX_OBJECTS];
        float prop[O_NB_MAX_OBJECTS];
        float dist[O_NB_MAX_OBJECTS];
        int nb;
    } O_objects_data;

    #include "../object/object.h"

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    void T_loadTexture(T_texture* texture, char* file_name);
    void T_drawTriangle(float x1, float y1, 
                        float x2, float y2,
                        float x3, float y3,
                        float z,
                        C_color color);
    void T_drawQuadri(float x1, float y1, 
                        float x2, float y2,
                        float x3, float y3,
                        float x4, float y4,
                        float z,
                        C_color color);
    void T_drawTexture(float xL, float yB, float xR, float yT,
                        float z,
                        float texture_start, float texture_end,
                        C_color color); 
    void T_drawWeapon(T_texture weapon);
    void T_drawMap(M_map map, P_player player); 
    void T_drawBackgroundTexture(P_player player, T_texture text_princ, T_texture text_sec);
    float T_isSeeingObject(P_player player, float castDirection, O_objects_data* objects_data, int object_i); 
    void T_addObjectToDrawQueue(int queue[], int m, O_objects_data objects_data, int nbObjectVisible); 
    Ray T_distanceToWall(P_player player, M_map map, float castDirection); 
    void T_drawObject(P_player player, float distance, int n, O_objects_data objects_data, int object_i); 
    void T_drawWall(P_player player, M_map map, Ray ray, int n, enum side current_side);
    void T_drawUI(M_map map, P_player player, T_texture weapon, T_texture cursor); 

#endif