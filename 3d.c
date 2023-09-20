#include <GLFW/glfw3.h>
//#include <GL/gl.h>
#include <stdio.h>
#include <math.h>

#include "color.h"

#define W_WIDTH 768
#define W_HEIGHT 432

#define V_NBRAY 768
#define V_FOV (M_PI/2.f)

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

void drawWall(float distance, int n);

void movement(GLFWwindow * window);

enum side get_side();

int width = 5;
int height = 7;
int map[] = {1,1,1,1,1,
             1,1,0,1,1,
             1,0,0,0,1,
             1,0,0,0,1,
             1,0,1,0,1,
             1,0,0,0,1,
             1,1,1,1,1};

float playerX = 2.5f;
float playerY = 2.5f;
float direction = M_PI*0.5f*-1.f;

float t_delta;

int tileLength = 10;
int viewRange = 4; //Nb of tile

float speedX;
float speedY;

float ray_x;
float ray_y;

enum side current_side;

void drawGround(){
    glBegin(GL_TRIANGLES);
        glColor3f(0, 0, 0);
        glVertex3f(-1, 0, 0);
        glColor3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-1, -1, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-1, -1, 0);
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(1, -1, 0);
    glEnd();
}

int main(void)
{
    GLFWwindow * window;

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


    glEnable(GL_MULTISAMPLE);

    double t = glfwGetTime();
    double time = glfwGetTime();

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

        /*drawQuadri(0, 0,
                   1, 0,
                   0.7, 0.7,
                   0, 1,
                   0,
                   0 ,1, 1);*/
        drawGround();

        float ray_direction;
        float ray_distance;
        for (int i=0; i<(V_NBRAY); i++){
            ray_direction = direction-(V_FOV/2.f) + ( V_FOV/((float)(V_NBRAY-1)) * ((float)i) );
            
            //if (time-t>=1.) printf("[%f]", distanceToWall(playerX, playerY, ray_direction));
            
            ray_distance = distanceToWall(playerX, playerY, ray_direction);
            current_side = get_side();

            drawWall(ray_distance, i);
        }


        if (time-t>=1.){
            //printf("%f | %f\n", direction, distanceToWall(playerX, playerY, direction));
            printf("%.2f FPS\n", 1.f/t_delta);
            t = time;
        }

        movement(window);
        
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

        if ( map[ ((int)ray_y)*width + (int)ray_x ]){
            float D = ((float)d)/((float) nbPerTile);
            //return ((float)d)/((float) nbPerTile);
            //return fabsf( sinf(direction+M_PI/2.f)*(x - playerX) - cosf(direction+M_PI/2.f)*(y - playerY)) * (float)tileLength;
            return D*cosf(direction-castDirection);
        }
    }

    return (float) (viewRange*tileLength);
}

void drawWall(float distance, int n){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/distance*4.f;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f;
    float yT = yB + h;

    C_color c = {127.f / 255.f, 98.f / 255.f, 127.f / 255.f};

    if (current_side == S_TOP)
        c = C_darken(c, 0.8);
    else if(current_side == S_LEFT)
        c = C_darken(c, 0.9);
    else if(current_side == S_RIGHT)
        c = C_darken(c, 0.9);


    //if ((int)distance<viewRange*tileLength)
    drawQuadri(xL, yB,
            xL, yT,
            xR, yT,
            xR, yB,
            0.f,
            C_darken(c, powf(1.f-(distance/(float)(viewRange*tileLength)), 0.8) ) );
}

void movement(GLFWwindow * window){
    int state;
    state = glfwGetKey(window, GLFW_KEY_LEFT);
    if (state == GLFW_PRESS)
        direction -= M_PI/100. * t_delta*75;

    state = glfwGetKey(window, GLFW_KEY_RIGHT);
    if (state == GLFW_PRESS)
        direction += M_PI/100. * t_delta*75;
        
    state = glfwGetKey(window, GLFW_KEY_UP);
    if (state == GLFW_PRESS){
        speedX = cosf(direction) * t_delta*2;
        speedY = sinf(direction) * t_delta*2;
    }    

    state = glfwGetKey(window, GLFW_KEY_DOWN);
    if (state == GLFW_PRESS){
        speedX = -cosf(direction) * t_delta*2;
        speedY = -sinf(direction) * t_delta*2;
    }

    if (map[ ((int)(playerY+speedY))*width + (int)(playerX+speedX) ] == 0){
        playerX += speedX;
        playerY += speedY;
    }

    speedX = 0.f;
    speedY = 0.f;
}

enum side get_side(){
    float x = ray_x - (int)ray_x;
    float y = ray_y - (int)ray_y;
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

