#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>

#define W_WIDTH 640
#define W_HEIGHT 480

void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void drawTriangle(float x1, float y1, 
                  float x2, float y2,
                  float x3, float y3,
                  float z,
                  float r, float g, float b);

void drawQuadri(float x1, float y1, 
                float x2, float y2,
                float x3, float y3,
                float x4, float y4,
                float z,
                float r, float g, float b);



int width = 5;
int height = 5;
int map[] = {1,1,1,1,1,
             1,0,1,0,1,
             1,0,0,0,1,
             1,0,0,0,1,
             1,1,1,1,1};

float playerX = 2.5f;
float playerY = 2.5f;
float direction = M_PI*0.5f*-1.f;

float t_delta;

int tileLength = 10;
int viewRange = 5; //Nb of tile

float distanceToWall(float X, float Y){
    int x, y;
    int nbPerTile = 10;
    for(int d=0; d<viewRange*tileLength*nbPerTile; d++){
        x = X + ((float)d)/((float) nbPerTile)/tileLength * cosf(direction);
        y = Y + ((float)d)/((float) nbPerTile)/tileLength * sinf(direction);

        if ( map[ ((int)y)*width + (int)x ])
            return ((float)d)/((float) nbPerTile);
    }

    return (float) (viewRange*tileLength);
}

int main(void)
{
    GLFWwindow * window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Setup error callback */
    glfwSetErrorCallback(error_callback);

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

        drawQuadri(0, 0,
                   1, 0,
                   0.7, 0.7,
                   0, 1,
                   0,
                   0 ,1, 1);


        if (time-t>=1.){
            printf("%f | %f\n", direction, distanceToWall(playerX, playerY));
            t = time;
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
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        direction -= M_PI/100. * t_delta*50;
    else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        direction += M_PI/100. * t_delta*50;

}

void drawTriangle(float x1, float y1, 
                  float x2, float y2,
                  float x3, float y3,
                  float z,
                  float r, float g, float b){

    glBegin(GL_TRIANGLES);
        glColor3f(r, g, b);
        glVertex3f(x1, y1, z);
        glColor3f(r, g, b);
        glVertex3f(x2, y2, z);
        glColor3f(r, g, b);
        glVertex3f(x3, y3, z);
    glEnd();

}

void drawQuadri(float x1, float y1, 
                float x2, float y2,
                float x3, float y3,
                float x4, float y4,
                float z,
                float r, float g, float b){
    
    drawTriangle(
        x1, y1, 
        x2, y2,
        x3, y3,
        z,
        r, g, b
    );
    
    drawTriangle(
        x3, y3, 
        x4, y4,
        x1, y1,
        z,
        r, g, b
    );
}

