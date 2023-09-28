#ifndef LIB_RENDER
    #define LIB_RENDER

    /********************/
    /* --- INCLUDES --- */
    /********************/

    #include <GLFW/glfw3.h>
    #include "../color/color.h"
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

#endif