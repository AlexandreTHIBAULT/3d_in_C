#include <GLFW/glfw3.h>
#include "../color.h"

typedef struct texture
{
    GLuint image;
    int width;
    int height;
    int nrChannels;
} T_texture;

void drawTriangle(float x1, float y1, 
                  float x2, float y2,
                  float x3, float y3,
                  float z,
                  C_color color);

                  