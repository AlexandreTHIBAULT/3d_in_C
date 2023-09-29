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

#define SHOW_FPS
#define RANDOM_MAP

#define W_WIDTH 1152
#define W_HEIGHT 648

#define V_NBRAY 300
#define V_FOV (M_PI/2.f)

#define K_UP            GLFW_KEY_W
#define K_LEFT          GLFW_KEY_A
#define K_DOWN          GLFW_KEY_S
#define K_RIGHT         GLFW_KEY_D
#define K_TURN_LEFT     GLFW_KEY_Q
#define K_TURN_RIGHT    GLFW_KEY_E
#define K_JUMP          GLFW_KEY_SPACE

/*********************/
/* --- FUNCTIONS --- */
/*********************/

void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
float isSeeingObject(P_player player, float castDirection, int object_i);
void addObjectToDrawQueue(int queue[], int m, int nbMonster);
Ray distanceToWall(P_player player, M_map map, float castDirection);
void drawObject(P_player player, float distance, int n, int object_i);
void drawWall(P_player player, M_map map, Ray ray, int n, enum side current_side);
P_player movement(GLFWwindow * window, P_player player, M_map map,double t_delta);
void drawBackgroundTexture(P_player player, T_texture text_princ, T_texture text_sec);
void drawMap(M_map map, P_player player);

/*********************/
/* --- VARIABLES --- */
/*********************/

/*
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
#endif */

const float z_max = 2.f; // height of jumps
const float t2 = 1.f; // jump duration
const float jump_a = 4.f*z_max/3.f/(t2*t2);
const float jump_b = jump_a * t2 * -1.f;

const int tileLength = 10;
const int viewRange = 7; //Nb of tile

const float wall_height = 11.f;

O_objects objects[O_NB_MAX_OBJECTS];
float objects_prop[O_NB_MAX_OBJECTS];
float objects_dist[O_NB_MAX_OBJECTS];
int nb_oject;

/****************/
/* --- MAIN --- */
/****************/

int main(void)
{
    /* Variables */
    GLFWwindow * window;
    double xpos, ypos, xpos_prev, ypos_prev; // for mouse movements
    P_player player = {X:2.5f, Y:2.5f, Z:0.f, speedX:0.f, speedY:0.f, direction:0.f, isJumping:0, t_jumpStart:0.};
    M_map map;
    enum side current_side;
    double t_delta;
    Ray ray;
    // textures
    T_texture t_Brick;
    T_texture t_Metal;
    T_texture t_Error;
    T_texture t_Soil;
    T_texture t_Soil2;
    T_texture t_Tank;
    T_texture t_Monster;
    T_texture t_Chain;
    T_texture t_Chain2;
    T_texture t_Slingshot;

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
    T_loadTexture(&t_Brick      , "textures/brick.jpg"    );
    T_loadTexture(&t_Metal      , "textures/metal.jpg"    );
    T_loadTexture(&t_Error      , "textures/error.jpg"    );
    T_loadTexture(&t_Soil       , "textures/soil.jpg"     );
    T_loadTexture(&t_Soil2      , "textures/soil2.jpg"    );
    T_loadTexture(&t_Tank       , "textures/tank.jpg"     );
    T_loadTexture(&t_Monster    , "textures/monster.png"  );
    T_loadTexture(&t_Chain      , "textures/chain.png"    );
    T_loadTexture(&t_Chain2     , "textures/chain2.png"   );
    T_loadTexture(&t_Slingshot  , "textures/slingshot.png");

    /* Create map */
    #ifdef RANDOM_MAP
        map = M_makeMaze(&player.X, &player.Y);
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
        drawBackgroundTexture(player, t_Soil, t_Soil2);

        float ray_direction;
        for (int i=0; i<(V_NBRAY); i++){
            ray_direction = player.direction-(V_FOV/2.f) + ( V_FOV/((float)(V_NBRAY-1)) * ((float)i) );            
            ray = distanceToWall(player, map, ray_direction);
            current_side = get_side(ray.X, ray.Y);

            for(int i=0; i<O_NB_MAX_OBJECTS; i++){
                queue_objects[i] = 0;
            }

            int nb_objects_visible = 0;
            for(int k = 0; k<nb_oject; k++){
                objects_dist[k] = isSeeingObject(player, ray_direction, k);
                if(nb_objects_visible!=0 && objects_dist[k]){
                    addObjectToDrawQueue(queue_objects, k, nb_objects_visible);
                    nb_objects_visible ++;
                }
                else if(objects_dist[k]){
                    queue_objects[0] = k;
                    nb_objects_visible ++;
                }
            }

            if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 1){
                glBindTexture ( GL_TEXTURE_2D, t_Brick.image);
            }
            else if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 2){
                glBindTexture ( GL_TEXTURE_2D, t_Metal.image);
            }
            else if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 3){
                glBindTexture ( GL_TEXTURE_2D, t_Tank.image);
            }
            else {
                glBindTexture ( GL_TEXTURE_2D, t_Error.image);
            }

            if (ray.distance < (float) (viewRange*tileLength))
                drawWall(player, map, ray, i, current_side);
            
            glEnable(GL_BLEND); //Enable blending.
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

            for(int k=0; k<nb_objects_visible; k++){
                int m = queue_objects[k];
                if (objects_dist[m] && objects_dist[m]<ray.distance)
                    drawObject(player, objects_dist[m], i, m);
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
        drawMap(map, player);
        T_drawWeapon(t_Slingshot);

        /* Inputs */
        player = movement(window, player, map, t_delta);
        
        glfwGetCursorPos(window, &xpos, &ypos);
        if(xpos!=xpos_prev){
            player.direction += (float)(xpos-xpos_prev) * M_PI/100. * t_delta*25;
            xpos_prev = xpos;
        }
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapInterval(1);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    #ifdef RANDOM_MAP
        free(map.map);
    #endif
    return 0;
}

void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

float isSeeingObject(P_player player, float castDirection, int object_i){
    float d = sqrtf(  powf( (player.X - objects[object_i].x ), 2) 
                    + powf( (player.Y - objects[object_i].y ), 2) );
    float cos = (objects[object_i].x - player.X)/d;
    float sin = (objects[object_i].y - player.Y)/d;

    if( cos*cosf(castDirection )+ sin*sinf(castDirection) < 0 ){
        return 0.f;
    }
    
    float a = tanf(castDirection);
    float b = player.Y-a*player.X;

    float angle = angleFromPosition(objects[object_i].x - player.X, objects[object_i].y - player.Y, d);

    float ap = tanf(castDirection-(M_1_PI/2.f));
    float bp = objects[object_i].y-ap*objects[object_i].x;

    float inters_x = -(bp-b)/(ap-a);
    float inters_y = a*inters_x+b;

    if (sqrtf(  powf( (inters_x - objects[object_i].x ), 2) 
                   + powf( (inters_y - objects[object_i].y ), 2) ) > 3.f)
        return 0.f;

    objects_prop[object_i] = 1.f/2.f - (sqrtf(  powf( (inters_x - objects[object_i].x ), 2) 
                              + powf( (inters_y - objects[object_i].y ), 2) )) /6.f;

    d = sqrtf(  powf( (player.X - objects[object_i].x ), 2) 
              + powf( (player.Y - objects[object_i].y ), 2) );

    return d*cosf(player.direction-castDirection) * tileLength;
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

Ray distanceToWall(P_player player, M_map map, float castDirection){
    int nbPerTile = 100;
    Ray ray;
    for(int d=0; d<viewRange*tileLength*nbPerTile; d++){
        ray.X = player.X + ((float)d)/((float) nbPerTile)/tileLength * cosf(castDirection);
        ray.Y = player.Y + ((float)d)/((float) nbPerTile)/tileLength * sinf(castDirection);

        if (isPillar(map, ray.X, ray.Y)){
            if(distanceToCenter(ray.X, ray.Y)<pillarRadius(map, ray.X, ray.Y)){
                float D = ((float)d)/((float) nbPerTile);
                ray.distance = D*cosf(player.direction-castDirection);
                return ray;
            }
        }
        else if ( map.map[ ((int)ray.Y)*map.width + (int)ray.X ]){
            float D = ((float)d)/((float) nbPerTile);
            //return ((float)d)/((float) nbPerTile);
            //return fabsf( sinf(direction+M_PI/2.f)*(x - playerX) - cosf(direction+M_PI/2.f)*(y - playerY)) * (float)tileLength;
            ray.distance = D*cosf(player.direction-castDirection);
            return ray;
        }
    }
    ray.distance = (float) (viewRange*tileLength);
    return ray;
}

void drawObject(P_player player, float distance, int n, int object_i){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/distance*wall_height;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f - player.Z / distance*wall_height ;
    float yT = yB + h;

    C_color c = C_white;

    float prop;

    prop = objects_prop[object_i];
    //prop_b = prop;

    c = C_darken(c, powf(1.f-(distance/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;

    glBindTexture ( GL_TEXTURE_2D, objects[object_i].texture.image);
    T_drawTexture(xL, yB, xR, yT, 0.0f, prop, prop, c);
}

void drawWall(P_player player, M_map map, Ray ray, int n, enum side current_side){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/ray.distance*wall_height;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f - player.Z / ray.distance*wall_height ;
    float yT = yB + h;

    C_color c = C_white;

    float prop;

    if (current_side == S_TOP){
        if(!isPillar(map, ray.X, ray.Y)) c = C_darken(c, 0.8);
        prop = 1.f - (ray.X - (int)ray.X);
    }
    else if(current_side == S_LEFT){
        if(!isPillar(map, ray.X, ray.Y)) c = C_darken(c, 0.9);
        prop = ray.Y - (int)ray.Y;
    }
    else if(current_side == S_RIGHT){
        if(!isPillar(map, ray.X, ray.Y)) c = C_darken(c, 0.9);
        prop = 1.f - (ray.Y - (int)ray.Y);
    }
    else {
        prop = ray.X - (int)ray.X;
    }
    
    c = C_darken(c, powf(1.f-(ray.distance/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
    T_drawTexture(xL, yB, xR, yT, 0.0f, prop, prop, c);
}

P_player movement(GLFWwindow * window, P_player player, M_map map, double t_delta){
    int state;
    state = glfwGetKey(window, K_TURN_LEFT);
    if (state == GLFW_PRESS)
        player.direction -= M_PI/100. * t_delta*75;

    state = glfwGetKey(window, K_TURN_RIGHT);
    if (state == GLFW_PRESS)
        player.direction += M_PI/100. * t_delta*75;
        
    // Movement
    int mv = 0;
    if (glfwGetKey(window, K_UP) == GLFW_PRESS){
        player.speedX += cosf(player.direction);
        player.speedY += sinf(player.direction);
        mv = 1;
    }
    if(glfwGetKey(window, K_DOWN) == GLFW_PRESS){
        player.speedX += -cosf(player.direction);
        player.speedY += -sinf(player.direction);
        mv = 1;
    }
    if(glfwGetKey(window, K_LEFT) == GLFW_PRESS){
        player.speedX += sinf(player.direction);
        player.speedY += -cosf(player.direction);
        mv = 1;
    }
    if(glfwGetKey(window, K_RIGHT) == GLFW_PRESS){
        player.speedX += -sinf(player.direction);
        player.speedY += cosf(player.direction);
        mv = 1;
    }

    /* Normalize */
    float norm = sqrtf(player.speedX*player.speedX + player.speedY*player.speedY);
    if(norm != 0){
        player.speedX /= norm;
        player.speedY /= norm;
    }

    player.speedX *= t_delta*2.f;
    player.speedY *= t_delta*2.f;

    if (map.map[ ((int)(player.Y+player.speedY))*map.width + (int)(player.X+player.speedX) ] == 0  
    || (isPillar(map, player.X+player.speedX, player.Y+player.speedY) && distanceToCenter(player.X+player.speedX, player.Y+player.speedY)>=pillarRadius(map, player.X+player.speedX, player.Y+player.speedY)) ){
        
        player.X += player.speedX;
        player.Y += player.speedY;
    }
    else {
        float destination_X = player.X+player.speedX/100.f;
        float destination_Y = player.Y+player.speedY/100.f;

        enum side dest_side = get_side(destination_X, destination_Y);

        if ( (dest_side == S_BOTTOM || dest_side == S_TOP) && ( map.map[((int)(player.Y))*map.width + (int)(player.X+player.speedX) ] == 0 )){
            player.X += player.speedX;
        }
        else if( (dest_side == S_LEFT || dest_side == S_RIGHT) && ( map.map[ (int)(player.Y+player.speedY)*map.width + (int)(player.X) ] == 0 )){
            player.Y += player.speedY;
        }
    }

    player.speedX = 0.f;
    player.speedY = 0.f;

    // Jump
    float t_jump;
    if( !player.isJumping && glfwGetKey(window, K_JUMP) == GLFW_PRESS ){
        player.isJumping = 1;
        player.t_jumpStart = glfwGetTime();
    }
    else if (player.Z <= 0.0f && (glfwGetTime() - player.t_jumpStart)>=t2){
        player.isJumping = 0;
        player.Z = 0.0f;
    }
    
    if(player.isJumping){
        t_jump = glfwGetTime() - player.t_jumpStart;
        player.Z = (jump_a*t_jump*t_jump + jump_b*t_jump) * -1.f;
    }

    return player;
}

void drawBackgroundTexture(P_player player, T_texture text_princ, T_texture text_sec){ // int viewRange, int tileLength, , float wall_height
    int x = (int) player.X;
    int y = (int) player.Y;

    int R = viewRange;

    for(int i_x=x-R; i_x<=x+R; i_x++){
    for(int i_y=y-R; i_y<=y+R; i_y++){
        
        float d_TR, d_TL, d_BR, d_BL;
        float a_TR, a_TL, a_BR, a_BL;

        d_TL = sqrtf( ((float)i_x -player.X)*((float)i_x -player.X)         + ((float)i_y -player.Y)*((float)i_y -player.Y) )           *tileLength;
        d_TR = sqrtf( ((float)(i_x+1) -player.X)*((float)(i_x+1) -player.X) + ((float)i_y -player.Y)*((float)i_y -player.Y) )           *tileLength;
        d_BL = sqrtf( ((float)i_x -player.X)*((float)i_x -player.X)         + ((float)(i_y+1) -player.Y)*((float)(i_y+1) -player.Y) )   *tileLength;
        d_BR = sqrtf( ((float)(i_x+1) -player.X)*((float)(i_x+1) -player.X) + ((float)(i_y+1) -player.Y)*((float)(i_y+1) -player.Y) )   *tileLength;
        
       
        a_TL = angleFromPosition(((float)i_x -player.X)*tileLength    , ((float)i_y -player.Y)*tileLength    , d_TL) - (player.direction);
        a_TR = angleFromPosition(((float)(i_x+1) -player.X)*tileLength, ((float)i_y -player.Y)*tileLength    , d_TR) - (player.direction); 
        a_BL = angleFromPosition(((float)i_x -player.X)*tileLength    , ((float)(i_y+1) -player.Y)*tileLength, d_BL) - (player.direction);
        a_BR = angleFromPosition(((float)(i_x+1) -player.X)*tileLength, ((float)(i_y+1) -player.Y)*tileLength, d_BR) - (player.direction);

        
        if (cosf(a_TL) > -0.78f && cosf(a_TR) > -0.78f && cosf(a_BL) > -0.78f && cosf(a_BR) > -0.78f){
            float x_screen_TL = modPI(a_TL)/(V_FOV/2.f);
            float x_screen_TR = modPI(a_TR)/(V_FOV/2.f);
            float x_screen_BL = modPI(a_BL)/(V_FOV/2.f);
            float x_screen_BR = modPI(a_BR)/(V_FOV/2.f);
            
            float d_TL_b = (d_TL*fabsf(cosf(a_TL)));
            float d_TR_b = (d_TR*fabsf(cosf(a_TR)));
            float d_BL_b = (d_BL*fabsf(cosf(a_BL)));
            float d_BR_b = (d_BR*fabsf(cosf(a_BR)));

            float y_screen_TL = (1.f/d_TL_b*wall_height) - player.Z / d_TL_b*wall_height;
            float y_screen_TR = (1.f/d_TR_b*wall_height) - player.Z / d_TR_b*wall_height;
            float y_screen_BL = (1.f/d_BL_b*wall_height) - player.Z / d_BL_b*wall_height;
            float y_screen_BR = (1.f/d_BR_b*wall_height) - player.Z / d_BR_b*wall_height;

            C_color c_TL = C_white;
            C_color c_TR = C_white;
            C_color c_BL = C_white;
            C_color c_BR = C_white;
            c_TL = C_darken(c_TL, powf(1.f-(d_TL_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
            c_TR = C_darken(c_TR, powf(1.f-(d_TR_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
            c_BL = C_darken(c_BL, powf(1.f-(d_BL_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;
            c_BR = C_darken(c_BR, powf(1.f-(d_BR_b/(((float)viewRange-0.5f)*(float)tileLength)), 0.8) ) ;

            // CEILING
            glBindTexture ( GL_TEXTURE_2D, text_princ.image);
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

            y_screen_TL = -(1.f/d_TL_b*wall_height) - player.Z / d_TL_b*wall_height;
            y_screen_TR = -(1.f/d_TR_b*wall_height) - player.Z / d_TR_b*wall_height;
            y_screen_BL = -(1.f/d_BL_b*wall_height) - player.Z / d_BL_b*wall_height;
            y_screen_BR = -(1.f/d_BR_b*wall_height) - player.Z / d_BR_b*wall_height;

            // FLOOR
            glBindTexture ( GL_TEXTURE_2D, (i_x*i_y+452+i_y)%16?text_princ.image:text_sec.image);
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

void drawMap(M_map map, P_player player){
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
        glVertex3f(map_start_x                 +square_size_x*player.X -square_size_x/4, map_start_y                 -square_size_y*player.Y +square_size_y/4, 0); // bottom left
        glTexCoord2f (1.0f,0.0f);
        glVertex3f(map_start_x+square_size_x/2 +square_size_x*player.X -square_size_x/4, map_start_y                 -square_size_y*player.Y +square_size_y/4, 0); // bottom right
        glTexCoord2f (1.0f,1.0f);
        glVertex3f(map_start_x+square_size_x/2 +square_size_x*player.X -square_size_x/4, map_start_y-square_size_y/2 -square_size_y*player.Y +square_size_y/4, 0);// top right
        glTexCoord2f (0.0f,1.0f);
        glVertex3f(map_start_x                 +square_size_x*player.X -square_size_x/4, map_start_y-square_size_y/2 -square_size_y*player.Y +square_size_y/4, 0); // top left
    glEnd();

    
}
