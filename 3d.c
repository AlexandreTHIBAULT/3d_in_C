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
#include "libs/gameplay/gameplay.h"

/*********************/
/* --- FUNCTIONS --- */
/*********************/

void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/****************/
/* --- MAIN --- */
/****************/

int main(void)
{
    /* Variables */
    GLFWwindow * window;
    double xpos, ypos, xpos_prev, ypos_prev; // for mouse movements
    P_player player = {X:2.5f, Y:2.5f, Z:0.f, speedX:0.f, speedY:0.f, direction:0.f, isJumping:0, t_jumpStart:0., t_lastShoot:0.};
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
    T_texture t_End;
    T_texture t_Cursor;
    T_texture t_Missile;

    O_objects_data objects_data;
    O_objects_data monsters_data;
    O_objects_data missile_data;
    O_objects_data complete_data;

    missile_data.nb = 0;
    Ray missile_dir[O_NB_MAX_OBJECTS];

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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    T_loadTexture(&t_End        , "textures/end.png"      );
    T_loadTexture(&t_Cursor     , "textures/cursor.png"   );
    T_loadTexture(&t_Missile    , "textures/rock.png"     );

    /* Create map */
    map = M_makeMaze(&player.X, &player.Y);
    M_printMaze(map.map);
    objects_data.nb = O_addObjects(objects_data.objects, map, t_Chain, t_Chain2);

    /* SETUP MONSTERS */
    monsters_data = O_spawnMonsters(map, t_Monster);

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
        T_drawBackgroundTexture(player, t_Soil, t_Soil2);

        complete_data = O_makeCompleteData(objects_data, monsters_data, missile_data);

        float ray_direction;
        for (int i=0; i<(V_NBRAY); i++){
            ray_direction = player.direction-(V_FOV/2.f) + ( V_FOV/((float)(V_NBRAY-1)) * ((float)i) );            
            ray = T_distanceToWall(player, map, ray_direction);
            current_side = get_side(ray.X, ray.Y);

            for(int i=0; i<O_NB_MAX_OBJECTS; i++){
                queue_objects[i] = 0;
            }

            int nb_objects_visible = 0;
            for(int k = 0; k<complete_data.nb; k++){
                complete_data.dist[k] = T_isSeeingObject(player, ray_direction, &complete_data, k);
                if(nb_objects_visible!=0 && complete_data.dist[k]){
                    T_addObjectToDrawQueue(queue_objects, k, complete_data, nb_objects_visible);
                    nb_objects_visible ++;
                }
                else if(complete_data.dist[k]){
                    queue_objects[0] = k;
                    nb_objects_visible ++;
                }
            }

            // 1=Brick
            if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 1){
                glBindTexture ( GL_TEXTURE_2D, t_Brick.image);
            }
            // 2=Metal
            else if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 2){
                glBindTexture ( GL_TEXTURE_2D, t_Metal.image);
            }
            // 3=Tank
            else if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 3){
                glBindTexture ( GL_TEXTURE_2D, t_Tank.image);
            }
            // 4=End
            else if( map.map[ ((int)ray.Y)*map.width + (int)ray.X ] == 4){
                glBindTexture ( GL_TEXTURE_2D, t_End.image);
            }
            // else=Error
            else {
                glBindTexture ( GL_TEXTURE_2D, t_Error.image);
            }

            if (ray.distance < (float) (VIEW_RANGE*TILE_LENGTH))
                T_drawWall(player, map, ray, i, current_side);
            
            glEnable(GL_BLEND); //Enable blending.
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

            for(int k=0; k<nb_objects_visible; k++){
                int m = queue_objects[k];
                if (complete_data.dist[m] && complete_data.dist[m]<ray.distance)
                    T_drawObject(player, complete_data.dist[m], i, complete_data, m);
            }
            
        }

        O_monsterMoveAll(map, &monsters_data, player, t_delta);
        O_updateShootMoove(&missile_data, missile_dir, t_delta);
        O_updateShoot(map, &missile_data, missile_dir);
        O_checkEnemiesTouch(&monsters_data, &missile_data, missile_dir);

        /* SHOW FPS */
        if (time-t>=1.){
            //printf("%f | %f\n", direction, T_distanceToWall(playerX, playerY, direction));
            #ifdef SHOW_FPS
                printf("%.2f FPS\n", 1.f/t_delta);
            #endif
            t = time;
        }
        
        /* Draw UI: map, weapon... */
        T_drawUI(map, player, t_Slingshot, t_Cursor);

        /* Inputs */
        player = G_movement(window, player, map, t_delta);
        player = G_action(window, player, &missile_data, missile_dir, t_Missile, t_delta);

        glfwGetCursorPos(window, &xpos, &ypos);
        if(xpos!=xpos_prev){
            player.direction += (float)(xpos-xpos_prev) * M_PI/100. ;
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


