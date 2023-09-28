#include "render.h"

void T_loadTexture(T_texture* texture, char* file_name){
    unsigned char *data ;
    data = stbi_load(file_name, &texture->width, &texture->height, &texture->nrChannels, STBI_rgb_alpha);

    glEnable (GL_TEXTURE_2D);
        glGenTextures(1, &texture->image);
        glBindTexture(GL_TEXTURE_2D, texture->image);
        //glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_MODULATE);
        //glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        //glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    glEnd();
    stbi_image_free(data);
}

void T_drawTriangle(float x1, float y1, 
                  float x2, float y2,
                  float x3, float y3,
                  float z,
                  C_color color){
    
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
        glColor3f(color.r, color.g, color.b);
        glVertex3f(x1, y1, z);
        glColor3f(color.r, color.g, color.b);
        glVertex3f(x2, y2, z);
        glColor3f(color.r, color.g, color.b);
        glVertex3f(x3, y3, z);
    glEnd();

}

void T_drawQuadri(float x1, float y1, 
                float x2, float y2,
                float x3, float y3,
                float x4, float y4,
                float z,
                C_color color){
    
    T_drawTriangle(
        x1, y1, 
        x2, y2,
        x3, y3,
        z,
        color
    );
    
    T_drawTriangle(
        x3, y3, 
        x4, y4,
        x1, y1,
        z,
        color
    );
}

void T_drawTexture(float xL, float yB, float xR, float yT,
                     float z,
                     float texture_start, float texture_end,
                     C_color color){
    
    glEnable (GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glColor3f(color.r, color.g, color.b);
        glTexCoord2f (texture_start,1.0f);
        glVertex3f(xL, yB, z); // bottom left
        glTexCoord2f (texture_end,1.0f);
        glVertex3f(xR, yB, z); // bottom right
        glTexCoord2f (texture_end,0.0f);
        glVertex3f(xR, yT, z);// top right
        glTexCoord2f (texture_start,0.0f);
        glVertex3f(xL, yT, z); // top left
    glEnd();

}