#include "utils.h"

int randMinMax(int min, int max){
    if (min==max) return min;
    return (rand()%(max-min) + min);
}

float modPI(float nb){

    while(nb>M_PI){
        nb -= M_PI * 2.f;
    }
    while(nb<=-M_PI){
        nb += M_PI * 2.f;
    }

    return nb;
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

int isPillar(M_map map, float x, float y){
    if ( map.map[ ((int)y)*map.width + (int)x ] == 3 ) {
        return 1;
    }
    else if ( map.map[ ((int)y)*map.width + (int)x ] == 4 ) {
        return 1;
    }

    return 0;
}

float pillarRadius(M_map map, float x, float y){
    if ( map.map[ ((int)y)*map.width + (int)x ] == 3 ) {
        return PILLAR_3_RADIUS;
    }
    else if ( map.map[ ((int)y)*map.width + (int)x ] == 4 ) {
        return PILLAR_4_RADIUS;
    }
}

float distanceToCenter(float x, float y){
    return sqrtf(  powf( (x - (float)((int)x)-0.5f ), 2) 
                 + powf( (y - (float)((int)y)-0.5f ), 2) );
}

float distance(float x1, float y1, float x2, float y2){
    return sqrtf(  powf( (x1 - x2), 2) 
                 + powf( (y1 - y2), 2) );
}