/********************/
/* --- INCLUDES --- */
/********************/

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "libs/color/color.h"
#include "libs/maze/maze.h"
#include "libs/utils.h"
#include "libs/render/render.h"
#include "libs/object/object.h"

/*******************/
/* --- DEFINES --- */
/*******************/

#define W_WIDTH 1152
#define W_HEIGHT 648

#define V_NBRAY 300
#define V_FOV (M_PI/2.f)

#define K_UP GLFW_KEY_W
#define K_DOWN GLFW_KEY_S
#define K_LEFT GLFW_KEY_A
#define K_RIGHT GLFW_KEY_D
#define K_TURN_LEFT GLFW_KEY_Q
#define K_TURN_RIGHT GLFW_KEY_E
#define K_JUMP GLFW_KEY_SPACE

#define SHOW_FPS
#define RANDOM_MAP

/*********************/
/* --- FUNCTIONS --- */
/*********************/

void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
float isSeeingObject(float X, float Y, float castDirection, int monster);
void addObjectToDrawQueue(int queue[], int m, int nbMonster);
float distanceToWall(float X, float Y, float castDirection);
void drawObject(float distance, int n, int object);
void drawWall(float distance, int n);
void movement(GLFWwindow * window);
float distanceToCenter(float x, float y);
void drawBackgroundTexture();
void drawMap(M_map map);

/*********************/
/* --- VARIABLES --- */
/*********************/

#ifndef RANDOM_MAP
    int width_map = 6;
    int height_map = 11;
    int map[] = {1,1,2,1,2,1,
                1,1,0,1,0,1,
                1,0,0,0,0,1,
                1,0,0,0,0,1,
                1,0,2,0,0,1,
                1,0,0,0,2,1,
                1,0,3,0,0,1,
                1,0,3,1,0,1,
                1,0,0,2,2,1,
                1,0,0,0,0,1,
                1,1,1,1,1,1};
#endif
#ifdef RANDOM_MAP
    M_map map;
#endif

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

T_texture t_Brick;
T_texture t_Metal;
T_texture t_Error;
T_texture t_Soil;
T_texture t_Soil2;
T_texture t_Tank;
T_texture t_Monster;
T_texture t_Chain;
T_texture t_Chain2;

O_objects objects[O_NB_MAX_OBJECTS];
float objects_prop[O_NB_MAX_OBJECTS];
float objects_dist[O_NB_MAX_OBJECTS];
int nb_oject;

/****************/
/* --- MAIN --- */
/****************/

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

    /* LOAD TEXTURE */

    T_loadTexture(&t_Brick   , "textures/brick.jpg"   );
    T_loadTexture(&t_Metal   , "textures/metal.jpg"   );
    T_loadTexture(&t_Error   , "textures/error.jpg"   );
    T_loadTexture(&t_Soil    , "textures/soil.jpg"    );
    T_loadTexture(&t_Soil2   , "textures/soil2.jpg"   );
    T_loadTexture(&t_Tank    , "textures/tank.jpg"    );
    T_loadTexture(&t_Monster , "textures/monster.png" );
    T_loadTexture(&t_Chain   , "textures/chain.png"   );
    T_loadTexture(&t_Chain2  , "textures/chain2.png"  );

    /* Create map */
    #ifdef RANDOM_MAP
        map = M_makeMaze(&playerX, &playerY);
        M_printMaze(map.map);
        nb_oject = O_addObjects(objects, map, t_Chain, t_Chain2);
    #endif

    /* SETUP MONSTERS */
    


    /* SETUP OBJECTS */
    
    int queue_objects[O_NB_MAX_OBJECTS];
    for(int i=0; i<O_NB_MAX_OBJECTS; i++){
        queue_objects[i] = 0;
    }

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
        drawBackgroundTexture();

        float ray_direction;
        float ray_distance;
        for (int i=0; i<(V_NBRAY); i++){
            ray_direction = direction-(V_FOV/2.f) + ( V_FOV/((float)(V_NBRAY-1)) * ((float)i) );            
            ray_distance = distanceToWall(playerX, playerY, ray_direction);
            current_side = get_side(ray_x, ray_y);

            for(int i=0; i<O_NB_MAX_OBJECTS; i++){
                queue_objects[i] = 0;
            }

            int nb_objects_visible = 0;
            for(int k = 0; k<nb_oject; k++){
                objects_dist[k] = isSeeingObject(playerX, playerY, ray_direction, k);
                if(nb_objects_visible!=0 && objects_dist[k]){
                    addObjectToDrawQueue(queue_objects, k, nb_objects_visible);
                    nb_objects_visible ++;
                }
                else if(objects_dist[k]){
                    queue_objects[0] = k;
                    nb_objects_visible ++;
                }
            }

            if( map.map[ ((int)ray_y)*map.width + (int)ray_x ] == 1){
                glBindTexture ( GL_TEXTURE_2D, t_Brick.image);
            }
            else if( map.map[ ((int)ray_y)*map.width + (int)ray_x ] == 2){
                glBindTexture ( GL_TEXTURE_2D, t_Metal.image);
            }
            else if( map.map[ ((int)ray_y)*map.width + (int)ray_x ] == 3){
                glBindTexture ( GL_TEXTURE_2D, t_Tank.image);
            }
            else {
                glBindTexture ( GL_TEXTURE_2D, t_Error.image);
            }

            if (ray_distance < (float) (viewRange*tileLength))
                drawWall(ray_distance, i);
            
            glEnable(GL_BLEND); //Enable blending.
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

            for(int k=0; k<nb_objects_visible; k++){
                int m = queue_objects[k];
                if (objects_dist[m] && objects_dist[m]<ray_distance)
                    drawObject(objects_dist[m], i, m);
            }
            
        }

        /* SHOW FPS */
        if (time-t>=1.){
            //printf("%f | %f\n", direction, distanceToWall(playerX, playerY, direction));
            #ifdef SHOW_FPS
                printf("%.2f FPS\n", 1.f/t_delta);
            #endif
            t = time;
        }
        
        /* Draw map in the corner of the screen */
        drawMap(map);

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

void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        #ifdef RANDOM_MAP
            free(map.map);
        #endif
    }
}

float isSeeingObject(float X, float Y, float castDirection, int object_i){
    float d = sqrtf(  powf( (X - objects[object_i].x ), 2) 
                    + powf( (Y - objects[object_i].y ), 2) );
    float cos = (objects[object_i].x - X)/d;
    float sin = (objects[object_i].y - Y)/d;

    if( cos*cosf(castDirection )+ sin*sinf(castDirection) < 0 ){
        return 0.f;
    }
    
    float a = tanf(castDirection);
    float b = Y-a*X;

    float angle = angleFromPosition(objects[object_i].x - X, objects[object_i].y - Y, d);

    float ap = tanf(castDirection-(M_1_PI/2.f));
    float bp = objects[object_i].y-ap*objects[object_i].x;

    float inters_x = -(bp-b)/(ap-a);
    float inters_y = a*inters_x+b;

    if (sqrtf(  powf( (inters_x - objects[object_i].x ), 2) 
                   + powf( (inters_y - objects[object_i].y ), 2) ) > 3.f)
        return 0.f;

    objects_prop[object_i] = 1.f/2.f - (sqrtf(  powf( (inters_x - objects[object_i].x ), 2) 
                              + powf( (inters_y - objects[object_i].y ), 2) )) /6.f;

    d = sqrtf(  powf( (X - objects[object_i].x ), 2) 
              + powf( (Y - objects[object_i].y ), 2) );

    //float D = ((float)d);
    return d*cosf(direction-castDirection) * tileLength;
    
}

void addObjectToDrawQueue(int queue[], int m, int nbObject){
    int place = 0;
    while( objects_dist[ queue[place] ] > objects_dist[m] && place<nbObject){
        place ++;
    }

    for(int i = nbObject; i>place; i--){
        queue[i] = queue[i-1];
    }
    queue[place] = m;
}

float distanceToWall(float X, float Y, float castDirection){
    int nbPerTile = 100;
    for(int d=0; d<viewRange*tileLength*nbPerTile; d++){
        ray_x = X + ((float)d)/((float) nbPerTile)/tileLength * cosf(castDirection);
        ray_y = Y + ((float)d)/((float) nbPerTile)/tileLength * sinf(castDirection);

        if (isPillar(map, ray_x, ray_y)){
            if(distanceToCenter(ray_x, ray_y)<pillarRadius(map, ray_x, ray_y)){
                float D = ((float)d)/((float) nbPerTile);
                return D*cosf(direction-castDirection);
            }
        }
        else if ( map.map[ ((int)ray_y)*map.width + (int)ray_x ]){
            float D = ((float)d)/((float) nbPerTile);
            //return ((float)d)/((float) nbPerTile);
            //return fabsf( sinf(direction+M_PI/2.f)*(x - playerX) - cosf(direction+M_PI/2.f)*(y - playerY)) * (float)tileLength;
            return D*cosf(direction-castDirection);
        }
    }

    return (float) (viewRange*tileLength);
}

void drawObject(float distance, int n, int object){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/distance*wall_height;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f - playerZ / distance*wall_height ;
    float yT = yB + h;

    C_color c = C_white;

    float prop, prop_b;

   
    prop = objects_prop[object];
    prop_b = prop;
    

    c = C_darken(c, powf(1.f-(distance/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;

    glBindTexture ( GL_TEXTURE_2D, objects[object].texture.image);
    T_drawTexture(xL, yB, xR, yT, 0.0f, prop, prop_b, c);

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
        if(!isPillar(map, ray_x, ray_y)) c = C_darken(c, 0.8);
        prop = 1.f - (ray_x - (int)ray_x);
        prop_b = prop+(1.f/256.f);
    }
    else if(current_side == S_LEFT){
        if(!isPillar(map, ray_x, ray_y)) c = C_darken(c, 0.9);
        prop = ray_y - (int)ray_y;
        prop_b = prop+(1.f/256.f);
    }
    else if(current_side == S_RIGHT){
        if(!isPillar(map, ray_x, ray_y)) c = C_darken(c, 0.9);
        prop = 1.f - (ray_y - (int)ray_y);
        prop_b = prop+(1.f/256.f);
    }
    else {
        prop = ray_x - (int)ray_x;
        prop_b = prop+(1.f/256.f);
        //c = (C_color){prop_x, 1.0f, 0.0f};
    }
    
    prop_b = prop;
    c = C_darken(c, powf(1.f-(distance/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
    T_drawTexture(xL, yB, xR, yT, 0.0f, prop, prop_b, c);
    //if ((int)distance<viewRange*tileLength)
    /*T_drawQuadri(xL, yB,
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

    if (map.map[ ((int)(playerY+speedY))*map.width + (int)(playerX+speedX) ] == 0  
    || (isPillar(map, playerX+speedX, playerY+speedY) && distanceToCenter(playerX+speedX, playerY+speedY)>=pillarRadius(map, playerX+speedX, playerY+speedY)) ){
        
        playerX += speedX;
        playerY += speedY;
    }
    else {
        float destination_X = playerX+speedX/100.f;
        float destination_Y = playerY+speedY/100.f;

        enum side dest_side = get_side(destination_X, destination_Y);

        if ( (dest_side == S_BOTTOM || dest_side == S_TOP) && ( map.map[((int)(playerY))*map.width + (int)(playerX+speedX) ] == 0 )){
            playerX += speedX;
        }
        else if( (dest_side == S_LEFT || dest_side == S_RIGHT) && ( map.map[ (int)(playerY+speedY)*map.width + (int)(playerX) ] == 0 )){
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

float distanceToCenter(float x, float y){
    return sqrtf(  powf( (x - (float)((int)x)-0.5f ), 2) 
                     + powf( (y - (float)((int)y)-0.5f ), 2) );
}

void drawBackgroundTexture(){
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
        
       
        a_TL = angleFromPosition(((float)i_x -playerX)*tileLength    , ((float)i_y -playerY)*tileLength    , d_TL) - (direction);
        a_TR = angleFromPosition(((float)(i_x+1) -playerX)*tileLength, ((float)i_y -playerY)*tileLength    , d_TR) - (direction); 
        a_BL = angleFromPosition(((float)i_x -playerX)*tileLength    , ((float)(i_y+1) -playerY)*tileLength, d_BL) - (direction);
        a_BR = angleFromPosition(((float)(i_x+1) -playerX)*tileLength, ((float)(i_y+1) -playerY)*tileLength, d_BR) - (direction);

        
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
            glBindTexture ( GL_TEXTURE_2D, t_Soil.image);
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
            glBindTexture ( GL_TEXTURE_2D, (i_x*i_y+452+i_y)%16?t_Soil.image:t_Soil2.image);
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

void drawMap(M_map map){
    float square_size_x = 0.015f;
    float square_size_y = square_size_x / W_HEIGHT * W_WIDTH;

    float map_start_x = square_size_x*3 - 1.f;
    float map_start_y = 1.f - square_size_y;

    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

    for(int i_x=0; i_x<map.width ; i_x++){
    for(int i_y=0; i_y<map.height; i_y++){

        if (map.map[ i_y*map.width + i_x ]!=1){
            glDisable (GL_TEXTURE_2D);
            glBegin(GL_QUADS);
                //glColor3f(1, 1, 1);
                glColor4f(1, 1, 1 ,0.2);
                glTexCoord2f (0.0f,0.0f);
                glVertex3f(map_start_x+square_size_x*i_x, map_start_y -square_size_y*i_y, 0); // bottom left
                glTexCoord2f (1.0f,0.0f);
                glVertex3f(map_start_x+square_size_x +square_size_x*i_x, map_start_y -square_size_y*i_y, 0); // bottom right
                glTexCoord2f (1.0f,1.0f);
                glVertex3f(map_start_x+square_size_x +square_size_x*i_x, map_start_y-square_size_y -square_size_y*i_y, 0);// top right
                glTexCoord2f (0.0f,1.0f);
                glVertex3f(map_start_x +square_size_x*i_x, map_start_y-square_size_y -square_size_y*i_y, 0); // top left
            glEnd();
        }

    }
    }
    glDisable(GL_BLEND);

    glDisable (GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glTexCoord2f (0.0f,0.0f);
        glVertex3f(map_start_x                 +square_size_x*playerX -square_size_x/4, map_start_y                 -square_size_y*playerY +square_size_y/4, 0); // bottom left
        glTexCoord2f (1.0f,0.0f);
        glVertex3f(map_start_x+square_size_x/2 +square_size_x*playerX -square_size_x/4, map_start_y                 -square_size_y*playerY +square_size_y/4, 0); // bottom right
        glTexCoord2f (1.0f,1.0f);
        glVertex3f(map_start_x+square_size_x/2 +square_size_x*playerX -square_size_x/4, map_start_y-square_size_y/2 -square_size_y*playerY +square_size_y/4, 0);// top right
        glTexCoord2f (0.0f,1.0f);
        glVertex3f(map_start_x                 +square_size_x*playerX -square_size_x/4, map_start_y-square_size_y/2 -square_size_y*playerY +square_size_y/4, 0); // top left
    glEnd();

    
}
