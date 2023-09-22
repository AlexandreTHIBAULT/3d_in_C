#include <GLFW/glfw3.h>
//#include <GL/gl.h>
#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "color.h"

#define W_WIDTH 768
#define W_HEIGHT 432

#define V_NBRAY 200
#define V_FOV (M_PI/2.f)

#define K_UP GLFW_KEY_W
#define K_DOWN GLFW_KEY_S
#define K_LEFT GLFW_KEY_A
#define K_RIGHT GLFW_KEY_D
#define K_TURN_LEFT GLFW_KEY_Q
#define K_TURN_RIGHT GLFW_KEY_E
#define K_JUMP GLFW_KEY_SPACE

enum side {
    S_RIGHT,
    S_TOP,
    S_LEFT,
    S_BOTTOM
};

void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void drawTriangle(float x1, float y1, 
                  float x2, float y2,
                  float x3, float y3,
                  float z,
                  C_color color);

void drawQuadri(float x1, float y1, 
                float x2, float y2,
                float x3, float y3,
                float x4, float y4,
                float z,
                C_color color);

float distanceToWall(float X, float Y, float castDirection);

void drawWallTexture(float xL, float yB, float xR, float yT,
                     float z,
                     float texture_start, float texture_end,
                     C_color color);

void drawWall(float distance, int n);

void movement(GLFWwindow * window);

enum side get_side(float X, float Y);

void drawGround(C_color c);
void drawCeiling(C_color c);

int width_map = 6;
int height_map = 11;
int map[] = {1,1,2,1,2,1,
             1,1,0,1,0,1,
             1,0,0,0,0,1,
             1,0,0,0,0,1,
             1,0,2,0,0,1,
             1,0,0,0,2,1,
             1,0,0,0,0,1,
             1,0,0,1,0,1,
             1,0,0,2,2,1,
             1,0,0,0,0,1,
             1,1,1,1,1,1};

float playerX = 2.5f;
float playerY = 2.5f;
float playerZ = 0.f;
float direction = M_PI*0.5f;

int jump = 0;
float t_jump_start = 0.f;

const float z_max = 2.f;
const float t2 = 1.f;
const float jump_a = 4.f*z_max/3.f/(t2*t2);
const float jump_b = jump_a * t2 * -1.f;

float t_delta;

int tileLength = 10;
int viewRange = 7; //Nb of tile

float speedX;
float speedY;

float ray_x;
float ray_y;

float wall_height = 11.f;

enum side current_side;

GLuint t_Brick;
int width_Brick, height_Brick, nrChannels_Brick;
GLuint t_Metal;
int width_Metal, height_Metal, nrChannels_Metal;
GLuint t_Error;
int width_Error, height_Error, nrChannels_Error;
GLuint t_Soil;
int width_Soil, height_Soil, nrChannels_Soil;

void load_texture(GLuint* texture, char* file_name, int* w, int* h, int* n){
    unsigned char *data ;
    data = stbi_load(file_name, w, h, n, 0);

    glEnable (GL_TEXTURE_2D);
        glGenTextures(1, texture);
        glBindTexture ( GL_TEXTURE_2D, *texture);
        glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_MODULATE);
        glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *w, *h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    glEnd();
    stbi_image_free(data);
}

float angleFromPosition(float x, float y, float d){
    float cos = x/d;
    float sin = y/d;

    if (sin > 0){
        return acosf(cos);
    }else{
        return -acosf(cos);
    }
}

float modPI(float nb){

    while(nb>M_PI){
        nb -= M_PI * 2.f;
    }
    while(nb<-M_PI){
        nb += M_PI * 2.f;
    }

    return nb;
}

void drawCeilingTexture(){
    int x = (int) playerX;
    int y = (int) playerY;

    int R = viewRange;

    for(int i_x=x-R; i_x<=x+R; i_x++){
    for(int i_y=y-R; i_y<=y+R; i_y++){
        
        float d_TR, d_TL, d_BR, d_BL;
        float a_TR, a_TL, a_BR, a_BL;

        d_TL = sqrtf( ((float)i_x -playerX)*((float)i_x -playerX)         + ((float)i_y -playerY)*((float)i_y -playerY) )           *tileLength;
        d_TR = sqrtf( ((float)(i_x+1) -playerX)*((float)(i_x+1) -playerX) + ((float)i_y -playerY)*((float)i_y -playerY) )           *tileLength;
        d_BL = sqrtf( ((float)i_x -playerX)*((float)i_x -playerX)         + ((float)(i_y+1) -playerY)*((float)(i_y+1) -playerY) )   *tileLength;
        d_BR = sqrtf( ((float)(i_x+1) -playerX)*((float)(i_x+1) -playerX) + ((float)(i_y+1) -playerY)*((float)(i_y+1) -playerY) )   *tileLength;
        
       
        a_TL = angleFromPosition(((float)i_x -playerX)*tileLength    , ((float)i_y -playerY)*tileLength    , d_TL) - modPI(direction);
        a_TR = angleFromPosition(((float)(i_x+1) -playerX)*tileLength, ((float)i_y -playerY)*tileLength    , d_TR) - modPI(direction); 
        a_BL = angleFromPosition(((float)i_x -playerX)*tileLength    , ((float)(i_y+1) -playerY)*tileLength, d_BL) - modPI(direction);
        a_BR = angleFromPosition(((float)(i_x+1) -playerX)*tileLength, ((float)(i_y+1) -playerY)*tileLength, d_BR) - modPI(direction);

        
        if (cosf(a_TL) > -0.78f && cosf(a_TR) > -0.78f && cosf(a_BL) > -0.78f && cosf(a_BR) > -0.78f){
            float x_screen_TL = modPI(a_TL)/(V_FOV/2.f);
            float x_screen_TR = modPI(a_TR)/(V_FOV/2.f);
            float x_screen_BL = modPI(a_BL)/(V_FOV/2.f);
            float x_screen_BR = modPI(a_BR)/(V_FOV/2.f);
            
            float d_TL_b = (d_TL*fabsf(cosf(a_TL)));
            float d_TR_b = (d_TR*fabsf(cosf(a_TR)));
            float d_BL_b = (d_BL*fabsf(cosf(a_BL)));
            float d_BR_b = (d_BR*fabsf(cosf(a_BR)));

            float y_screen_TL = (1.f/d_TL_b*wall_height) - playerZ / d_TL_b*wall_height;
            float y_screen_TR = (1.f/d_TR_b*wall_height) - playerZ / d_TR_b*wall_height;
            float y_screen_BL = (1.f/d_BL_b*wall_height) - playerZ / d_BL_b*wall_height;
            float y_screen_BR = (1.f/d_BR_b*wall_height) - playerZ / d_BR_b*wall_height;

            C_color c_TL = C_white;
            C_color c_TR = C_white;
            C_color c_BL = C_white;
            C_color c_BR = C_white;
            c_TL = C_darken(c_TL, powf(1.f-(d_TL_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
            c_TR = C_darken(c_TR, powf(1.f-(d_TR_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
            c_BL = C_darken(c_BL, powf(1.f-(d_BL_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
            c_BR = C_darken(c_BR, powf(1.f-(d_BR_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;

            // CEILING
            glBindTexture ( GL_TEXTURE_2D, t_Soil);
            glEnable (GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                glColor3f(c_BL.r, c_BL.g, c_BL.b);
                glTexCoord2f (0.0f,0.0f);
                glVertex3f(x_screen_BL, y_screen_BL, 0); // bottom left
                glColor3f(c_BR.r, c_BR.g, c_BR.b);
                glTexCoord2f (1.0f,0.0f);
                glVertex3f(x_screen_BR, y_screen_BR, 0); // bottom right
                glColor3f(c_TR.r, c_TR.g, c_TR.b);
                glTexCoord2f (1.0f,1.0f);
                glVertex3f(x_screen_TR, y_screen_TR, 0);// top right
                glColor3f(c_TL.r, c_TL.g, c_TL.b);
                glTexCoord2f (0.0f,1.0f);
                glVertex3f(x_screen_TL, y_screen_TL, 0); // top left
            glEnd();

            y_screen_TL = -(1.f/d_TL_b*wall_height) - playerZ / d_TL_b*wall_height;
            y_screen_TR = -(1.f/d_TR_b*wall_height) - playerZ / d_TR_b*wall_height;
            y_screen_BL = -(1.f/d_BL_b*wall_height) - playerZ / d_BL_b*wall_height;
            y_screen_BR = -(1.f/d_BR_b*wall_height) - playerZ / d_BR_b*wall_height;

            // FLOOR
            glBindTexture ( GL_TEXTURE_2D, t_Soil);
            glEnable (GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                glColor3f(c_BL.r, c_BL.g, c_BL.b);
                glTexCoord2f (0.0f,0.0f);
                glVertex3f(x_screen_BL, y_screen_BL, 0); // bottom left
                glColor3f(c_BR.r, c_BR.g, c_BR.b);
                glTexCoord2f (1.0f,0.0f);
                glVertex3f(x_screen_BR, y_screen_BR, 0); // bottom right
                glColor3f(c_TR.r, c_TR.g, c_TR.b);
                glTexCoord2f (1.0f,1.0f);
                glVertex3f(x_screen_TR, y_screen_TR, 0);// top right
                glColor3f(c_TL.r, c_TL.g, c_TL.b);
                glTexCoord2f (0.0f,1.0f);
                glVertex3f(x_screen_TL, y_screen_TL, 0); // top left
            glEnd();

        }

    }
    }
}

int main(void)
{
    GLFWwindow * window;
    double xpos, ypos, xpos_prev, ypos_prev;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Setup error callback */
    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Setup key callback */
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glEnable(GL_MULTISAMPLE);

    double t = glfwGetTime();
    double time = glfwGetTime();
    

    /* LOAD TEXTURE*/
    load_texture(&t_Brick, "brick.jpg", &width_Brick, &height_Brick, &nrChannels_Brick);
    load_texture(&t_Metal, "metal.jpg", &width_Metal, &height_Metal, &nrChannels_Metal);
    load_texture(&t_Error, "error.jpg", &width_Error, &height_Error, &nrChannels_Error);
    load_texture(&t_Soil, "soil.jpg", &width_Soil, &height_Soil, &nrChannels_Soil);
    
    /*-------------------------------*/

    glfwGetCursorPos(window, &xpos, &ypos);
    xpos_prev = xpos;
    ypos_prev = ypos;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        
        double time_prev = time;
        time = glfwGetTime();
        t_delta = time - time_prev;


        /* Render here */
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        C_color ground_c = {0.2, 0.2, 0.2};
        C_color ceiling_c = {0.2, 0.2, 0.2};
        drawGround(ground_c);
        //drawCeiling(ceiling_c);
        drawCeilingTexture();

        float ray_direction;
        float ray_distance;
        for (int i=0; i<(V_NBRAY); i++){
            ray_direction = direction-(V_FOV/2.f) + ( V_FOV/((float)(V_NBRAY-1)) * ((float)i) );
            
            //if (time-t>=1.) printf("[%f]", distanceToWall(playerX, playerY, ray_direction));
            
            ray_distance = distanceToWall(playerX, playerY, ray_direction);
            current_side = get_side(ray_x, ray_y);

            if( map[ ((int)ray_y)*width_map + (int)ray_x ] == 1){
                glBindTexture ( GL_TEXTURE_2D, t_Brick);
            }
            else if( map[ ((int)ray_y)*width_map + (int)ray_x ] == 2){
                glBindTexture ( GL_TEXTURE_2D, t_Metal);
            }
            else {
                glBindTexture ( GL_TEXTURE_2D, t_Error);
            }

            if (ray_distance < (float) (viewRange*tileLength))
                drawWall(ray_distance, i);
        }


        if (time-t>=1.){
            //printf("%f | %f\n", direction, distanceToWall(playerX, playerY, direction));
            printf("%.2f FPS\n", 1.f/t_delta);
            t = time;
        }

        /* Inputs */
        movement(window);

        glfwGetCursorPos(window, &xpos, &ypos);
        if(xpos!=xpos_prev){
            direction += (float)(xpos-xpos_prev) * M_PI/100. * t_delta*25;
            xpos_prev = xpos;
        }
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapInterval(1);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    /*else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        direction += M_PI/100. * t_delta*50;
    else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        direction -= M_PI/100. * t_delta*50;*/

}

void drawTriangle(float x1, float y1, 
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

void drawQuadri(float x1, float y1, 
                float x2, float y2,
                float x3, float y3,
                float x4, float y4,
                float z,
                C_color color){
    
    drawTriangle(
        x1, y1, 
        x2, y2,
        x3, y3,
        z,
        color
    );
    
    drawTriangle(
        x3, y3, 
        x4, y4,
        x1, y1,
        z,
        color
    );
}

float distanceToWall(float X, float Y, float castDirection){
    //float x, y;
    int nbPerTile = 100;
    for(int d=0; d<viewRange*tileLength*nbPerTile; d++){
        ray_x = X + ((float)d)/((float) nbPerTile)/tileLength * cosf(castDirection);
        ray_y = Y + ((float)d)/((float) nbPerTile)/tileLength * sinf(castDirection);

        if ( map[ ((int)ray_y)*width_map + (int)ray_x ]){
            float D = ((float)d)/((float) nbPerTile);
            //return ((float)d)/((float) nbPerTile);
            //return fabsf( sinf(direction+M_PI/2.f)*(x - playerX) - cosf(direction+M_PI/2.f)*(y - playerY)) * (float)tileLength;
            return D*cosf(direction-castDirection);
        }
    }

    return (float) (viewRange*tileLength);
}

void drawWallTexture(float xL, float yB, float xR, float yT,
                     float z,
                     float texture_start, float texture_end,
                     C_color color){
    
    glEnable (GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glColor3f(color.r, color.g, color.b);
        glTexCoord2f (texture_start,0.0f);
        glVertex3f(xL, yB, z); // bottom left
        glTexCoord2f (texture_end,0.0f);
        glVertex3f(xR, yB, z); // bottom right
        glTexCoord2f (texture_end,1.0f);
        glVertex3f(xR, yT, z);// top right
        glTexCoord2f (texture_start,1.0f);
        glVertex3f(xL, yT, z); // top left
    glEnd();

}

void drawWall(float distance, int n){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/distance*wall_height;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f - playerZ / distance*wall_height ;
    float yT = yB + h;

    C_color c = C_white;

    float prop, prop_b;

    if (current_side == S_TOP){
        c = C_darken(c, 0.8);
        prop = 1.f - (ray_x - (int)ray_x);
        prop_b = prop+(1.f/256.f);
    }
    else if(current_side == S_LEFT){
        c = C_darken(c, 0.9);
        prop = ray_y - (int)ray_y;
        prop_b = prop+(1.f/256.f);
    }
    else if(current_side == S_RIGHT){
        c = C_darken(c, 0.9);
        prop = 1.f - (ray_y - (int)ray_y);
        prop_b = prop+(1.f/256.f);
    }
    else {
        prop = ray_x - (int)ray_x;
        prop_b = prop+(1.f/256.f);
        //c = (C_color){prop_x, 1.0f, 0.0f};
    }

    c = C_darken(c, powf(1.f-(distance/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
    drawWallTexture(xL, yB, xR, yT, 0.0f, prop, prop_b, c);
    //if ((int)distance<viewRange*tileLength)
    /*drawQuadri(xL, yB,
            xL, yT,
            xR, yT,
            xR, yB,
            0.f,
            C_darken(c, powf(1.f-(distance/(float)(viewRange*tileLength)), 0.8) ) );*/
}

void movement(GLFWwindow * window){
    int state;
    state = glfwGetKey(window, K_TURN_LEFT);
    if (state == GLFW_PRESS)
        direction -= M_PI/100. * t_delta*75;

    state = glfwGetKey(window, K_TURN_RIGHT);
    if (state == GLFW_PRESS)
        direction += M_PI/100. * t_delta*75;
        
    // Movement
    int mv = 0;

    if (glfwGetKey(window, K_UP) == GLFW_PRESS){
        speedX += cosf(direction);
        speedY += sinf(direction);
        mv = 1;
    }
    if(glfwGetKey(window, K_DOWN) == GLFW_PRESS){
        speedX += -cosf(direction);
        speedY += -sinf(direction);
        mv = 1;
    }
    if(glfwGetKey(window, K_LEFT) == GLFW_PRESS){
        speedX += sinf(direction);
        speedY += -cosf(direction);
        mv = 1;
    }
    if(glfwGetKey(window, K_RIGHT) == GLFW_PRESS){
        speedX += -sinf(direction);
        speedY += cosf(direction);
        mv = 1;
    }

    /* Normalize */
    float norm = sqrtf(speedX*speedX + speedY*speedY);
    if(norm != 0){
        speedX /= norm;
        speedY /= norm;
    }

    speedX *= t_delta*2.f;
    speedY *= t_delta*2.f;

    if (map[ ((int)(playerY+speedY))*width_map + (int)(playerX+speedX) ] == 0){
        playerX += speedX;
        playerY += speedY;
    }
    else {
        float destination_X = playerX+speedX/100.f;
        float destination_Y = playerY+speedY/100.f;

        enum side dest_side = get_side(destination_X, destination_Y);

        if ( (dest_side == S_BOTTOM || dest_side == S_TOP) && ( map[((int)(playerY))*width_map + (int)(playerX+speedX) ] == 0 )){
            playerX += speedX;
        }
        else if( (dest_side == S_LEFT || dest_side == S_RIGHT) && ( map[ (int)(playerY+speedY)*width_map + (int)(playerX) ] == 0 )){
            playerY += speedY;
        }
    }

    //if ( fabsf(speedX)<=0.01f && fabsf(speedY)<=0.01f ){
        speedX = 0.f;
        speedY = 0.f;
    //}

    // Jump
    float t_jump;
    if( !jump && glfwGetKey(window, K_JUMP) == GLFW_PRESS ){
        //printf("jump\n");
        jump = 1;
        t_jump_start = glfwGetTime();
    }
    else if (playerZ <= 0.0f && (glfwGetTime() - t_jump_start)>=t2){
        jump = 0;
        playerZ = 0.0f;
    }
    
    if(jump){
        t_jump = glfwGetTime() - t_jump_start;
        //printf("%f | %f\n", t_jump, playerZ);
        playerZ = (jump_a*t_jump*t_jump + jump_b*t_jump) * -1.f;
    }
    
}

enum side get_side(float X, float Y){
    float x = X - (int)X;
    float y = Y - (int)Y;
    float a1 = x*x + y*y;
    float a2 = (1-x)*(1-x) + y*y;
    float a3 = (1-x)*(1-x) + (1-y)*(1-y);
    float a4 = x*x + (1-y)*(1-y);
    int d1 = (a1>a3);
    int d2 = (a2>a4);

    if(d1 && d2)
        return S_BOTTOM;
    else if(d1 && !d2)
        return S_RIGHT;
    else if(!d1 && d2)
        return S_LEFT;
    else
        return S_TOP;
}

void drawGround(C_color c){ 
    float offset = playerZ;

    drawQuadri(-1, -1, 
                1, -1,
                1, -1.f-playerZ,
                -1, -1.f-playerZ, 
                0,
                c);

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
        glColor3f(0, 0, 0);
        glVertex3f(-1, 0.f, 0);
        glColor3f(0, 0, 0);
        glVertex3f(1, 0.f, 0);
        glColor3f(c.r, c.g, c.b);
        glVertex3f(-1, -1.f-offset, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0, 0, 0);
        glVertex3f(1, 0.f, 0);
        glColor3f(c.r, c.g, c.b);
        glVertex3f(-1, -1.f-offset, 0);
        glColor3f(c.r, c.g, c.b);
        glVertex3f(1, -1.f-offset, 0);
    glEnd();
}

void drawCeiling(C_color c){
    float offset = playerZ;

    drawQuadri(-1, 1, 
                1, 1,
                1, 1.f-offset,
                -1, 1.f-offset, 
                0,
                c);

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
        glColor3f(0, 0, 0);
        glVertex3f(-1, 0.f, 0);
        glColor3f(0, 0, 0);
        glVertex3f(1, 0.f, 0);
        glColor3f(c.r, c.g, c.b);
        glVertex3f(-1, 1.f-offset, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0, 0, 0);
        glVertex3f(1, 0.f, 0);
        glColor3f(c.r, c.g, c.b);
        glVertex3f(-1, 1.f-offset, 0);
        glColor3f(c.r, c.g, c.b);
        glVertex3f(1, 1.f-offset, 0);
    glEnd();
}