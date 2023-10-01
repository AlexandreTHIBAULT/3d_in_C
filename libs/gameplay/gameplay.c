#include "gameplay.h"

P_player G_action(GLFWwindow * window, P_player player, O_objects_data* missile_data, Ray missile_dir[], T_texture text, double t_delta){
    int state;
    state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    double t = glfwGetTime();

    if (state == GLFW_PRESS && t-player.t_lastShoot>0.5){
        player.t_lastShoot = t;
        O_shoot(player, missile_data, missile_dir, text);
    }

    return player;
}

P_player G_movement(GLFWwindow * window, P_player player, M_map map, double t_delta){
    float z_max = 2.f; // height of jumps
    float t2 = 1.f; // jump duration
    float jump_a = 4.f*z_max/3.f/(t2*t2);
    float jump_b = jump_a * t2 * -1.f;
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

    //if (map.map[ ((int)(player.Y+player.speedY))*map.width + (int)(player.X+player.speedX) ] == 0  
    //|| (isPillar(map, player.X+player.speedX, player.Y+player.speedY) && distanceToCenter(player.X+player.speedX, player.Y+player.speedY)>=pillarRadius(map, player.X+player.speedX, player.Y+player.speedY)) ){
    if(isPositionFree(map, player.X+player.speedX, player.Y+player.speedY)){
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
