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

void T_drawWeapon(T_texture weapon){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture ( GL_TEXTURE_2D, weapon.image);
    T_drawTexture(0.25f, -1.05f, 0.25f+0.4444f, 0.7901f-1.05f, 0, 0, 1, C_white);
}

void T_drawMap(M_map map, P_player player){
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

void T_drawBackgroundTexture(P_player player, T_texture text_princ, T_texture text_sec){
    int x = (int) player.X;
    int y = (int) player.Y;

    int R = VIEW_RANGE;

    for(int i_x=x-R; i_x<=x+R; i_x++){
    for(int i_y=y-R; i_y<=y+R; i_y++){
        
        float d_TR, d_TL, d_BR, d_BL;
        float a_TR, a_TL, a_BR, a_BL;

        d_TL = sqrtf( ((float)i_x -player.X)*((float)i_x -player.X)         + ((float)i_y -player.Y)*((float)i_y -player.Y) )           *TILE_LENGTH;
        d_TR = sqrtf( ((float)(i_x+1) -player.X)*((float)(i_x+1) -player.X) + ((float)i_y -player.Y)*((float)i_y -player.Y) )           *TILE_LENGTH;
        d_BL = sqrtf( ((float)i_x -player.X)*((float)i_x -player.X)         + ((float)(i_y+1) -player.Y)*((float)(i_y+1) -player.Y) )   *TILE_LENGTH;
        d_BR = sqrtf( ((float)(i_x+1) -player.X)*((float)(i_x+1) -player.X) + ((float)(i_y+1) -player.Y)*((float)(i_y+1) -player.Y) )   *TILE_LENGTH;
        
       
        a_TL = angleFromPosition(((float)i_x -player.X)*TILE_LENGTH    , ((float)i_y -player.Y)*TILE_LENGTH    , d_TL) - (player.direction);
        a_TR = angleFromPosition(((float)(i_x+1) -player.X)*TILE_LENGTH, ((float)i_y -player.Y)*TILE_LENGTH    , d_TR) - (player.direction); 
        a_BL = angleFromPosition(((float)i_x -player.X)*TILE_LENGTH    , ((float)(i_y+1) -player.Y)*TILE_LENGTH, d_BL) - (player.direction);
        a_BR = angleFromPosition(((float)(i_x+1) -player.X)*TILE_LENGTH, ((float)(i_y+1) -player.Y)*TILE_LENGTH, d_BR) - (player.direction);

        
        if (cosf(a_TL) > -0.78f && cosf(a_TR) > -0.78f && cosf(a_BL) > -0.78f && cosf(a_BR) > -0.78f){
            float x_screen_TL = modPI(a_TL)/(V_FOV/2.f);
            float x_screen_TR = modPI(a_TR)/(V_FOV/2.f);
            float x_screen_BL = modPI(a_BL)/(V_FOV/2.f);
            float x_screen_BR = modPI(a_BR)/(V_FOV/2.f);
            
            float d_TL_b = (d_TL*fabsf(cosf(a_TL)));
            float d_TR_b = (d_TR*fabsf(cosf(a_TR)));
            float d_BL_b = (d_BL*fabsf(cosf(a_BL)));
            float d_BR_b = (d_BR*fabsf(cosf(a_BR)));

            float y_screen_TL = (1.f/d_TL_b*WALL_HEIGHT) - player.Z / d_TL_b*WALL_HEIGHT;
            float y_screen_TR = (1.f/d_TR_b*WALL_HEIGHT) - player.Z / d_TR_b*WALL_HEIGHT;
            float y_screen_BL = (1.f/d_BL_b*WALL_HEIGHT) - player.Z / d_BL_b*WALL_HEIGHT;
            float y_screen_BR = (1.f/d_BR_b*WALL_HEIGHT) - player.Z / d_BR_b*WALL_HEIGHT;

            C_color c_TL = C_white;
            C_color c_TR = C_white;
            C_color c_BL = C_white;
            C_color c_BR = C_white;
            c_TL = C_darken(c_TL, powf(1.f-(d_TL_b/(((float)VIEW_RANGE-0.5f)*(float)TILE_LENGTH)), 0.8) ) ;
            c_TR = C_darken(c_TR, powf(1.f-(d_TR_b/(((float)VIEW_RANGE-0.5f)*(float)TILE_LENGTH)), 0.8) ) ;
            c_BL = C_darken(c_BL, powf(1.f-(d_BL_b/(((float)VIEW_RANGE-0.5f)*(float)TILE_LENGTH)), 0.8) ) ;
            c_BR = C_darken(c_BR, powf(1.f-(d_BR_b/(((float)VIEW_RANGE-0.5f)*(float)TILE_LENGTH)), 0.8) ) ;

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

            y_screen_TL = -(1.f/d_TL_b*WALL_HEIGHT) - player.Z / d_TL_b*WALL_HEIGHT;
            y_screen_TR = -(1.f/d_TR_b*WALL_HEIGHT) - player.Z / d_TR_b*WALL_HEIGHT;
            y_screen_BL = -(1.f/d_BL_b*WALL_HEIGHT) - player.Z / d_BL_b*WALL_HEIGHT;
            y_screen_BR = -(1.f/d_BR_b*WALL_HEIGHT) - player.Z / d_BR_b*WALL_HEIGHT;

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

float T_isSeeingObject(P_player player, float castDirection, O_objects_data* objects_data, int object_i){

    float d = sqrtf(  powf( (player.X - objects_data->objects[object_i].x ), 2) 
                    + powf( (player.Y - objects_data->objects[object_i].y ), 2) );
    float cos = (objects_data->objects[object_i].x - player.X)/d;
    float sin = (objects_data->objects[object_i].y - player.Y)/d;

    if( cos*cosf(castDirection )+ sin*sinf(castDirection) < 0 ){
        return 0.f;
    }
    
    float a = tanf(castDirection);
    float b = player.Y-a*player.X;

    float angle = angleFromPosition(objects_data->objects[object_i].x - player.X, objects_data->objects[object_i].y - player.Y, d);

    float ap = tanf(castDirection-(M_1_PI/2.f));
    float bp = objects_data->objects[object_i].y-ap*objects_data->objects[object_i].x;

    float inters_x = -(bp-b)/(ap-a);
    float inters_y = a*inters_x+b;

    if (sqrtf(  powf( (inters_x - objects_data->objects[object_i].x ), 2) 
                   + powf( (inters_y - objects_data->objects[object_i].y ), 2) ) > 3.f)
        return 0.f;

    objects_data->prop[object_i] = 1.f/2.f - (sqrtf(  powf( (inters_x - objects_data->objects[object_i].x ), 2) 
                              + powf( (inters_y - objects_data->objects[object_i].y ), 2) )) /6.f;

    d = sqrtf(  powf( (player.X - objects_data->objects[object_i].x ), 2) 
              + powf( (player.Y - objects_data->objects[object_i].y ), 2) );

    return d*cosf(player.direction-castDirection) * TILE_LENGTH;
}

void T_addObjectToDrawQueue(int queue[], int m, O_objects_data objects_data, int nbObjectVisible){
    int place = 0;
    while( objects_data.dist[ queue[place] ] > objects_data.dist[m] && place<nbObjectVisible){
        place ++;
    }

    for(int i = nbObjectVisible; i>place; i--){
        queue[i] = queue[i-1];
    }
    queue[place] = m;
}

Ray T_distanceToWall(P_player player, M_map map, float castDirection){
    int nbPerTile = 100;
    Ray ray;
    for(int d=0; d<VIEW_RANGE*TILE_LENGTH*nbPerTile; d++){
        ray.X = player.X + ((float)d)/((float) nbPerTile)/TILE_LENGTH * cosf(castDirection);
        ray.Y = player.Y + ((float)d)/((float) nbPerTile)/TILE_LENGTH * sinf(castDirection);

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
    ray.distance = (float) (VIEW_RANGE*TILE_LENGTH);
    return ray;
}

void T_drawObject(P_player player, float distance, int n, O_objects_data objects_data, int object_i){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/distance*WALL_HEIGHT;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f - player.Z / distance*WALL_HEIGHT ;
    float yT = yB + h;

    C_color c = C_white;

    float prop;

    prop = objects_data.prop[object_i];

    c = C_darken(c, powf(1.f-(distance/(((float)VIEW_RANGE-0.5f)*(float)TILE_LENGTH)), 0.8) ) ;

    glBindTexture ( GL_TEXTURE_2D, objects_data.objects[object_i].texture.image);
    T_drawTexture(xL, yB, xR, yT, 0.0f, prop, prop, c);
}

void T_drawWall(P_player player, M_map map, Ray ray, int n, enum side current_side){
    float w = 2.f/(float)V_NBRAY;
    float h = 2.f/ray.distance*WALL_HEIGHT;
    float xL = w*(float)n - 1.f;
    float xR = xL + w;
    float yB = -h/2.f - player.Z / ray.distance*WALL_HEIGHT;
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
    
    c = C_darken(c, powf(1.f-(ray.distance/(((float)VIEW_RANGE-0.5f)*(float)TILE_LENGTH)), 0.8) ) ;
    T_drawTexture(xL, yB, xR, yT, 0.0f, prop, prop, c);
}

void T_drawDamage(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
        glColor4f(C_red.r, C_red.g, C_red.b, 1);
        glVertex3f(-1, -1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(0, -1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(-1, 0, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor4f(C_red.r, C_red.g, C_red.b, 1);
        glVertex3f(1, 1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(0, 1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(1, 0, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor4f(C_red.r, C_red.g, C_red.b, 1);
        glVertex3f(1, -1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(0, -1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(1, 0, 0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor4f(C_red.r, C_red.g, C_red.b, 1);
        glVertex3f(-1, 1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(0, 1, 0);
        glColor4f(0, 0, 0, 0);
        glVertex3f(-1, 0, 0);
    glEnd();
}

void T_drawUI(M_map map, P_player player, T_texture weapon, T_texture cursor){
    //T_drawDamage();

    T_drawWeapon(weapon);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture ( GL_TEXTURE_2D, cursor.image);
    T_drawTexture(-0.027f, -0.049f, 0.027f, 0.049f, 0, 0, 1, C_white);


    T_drawMap(map, player);
}