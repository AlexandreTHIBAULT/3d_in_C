#include "object.h"

int O_isValidPosition(M_map map, float x, float y){
    return map.map[ (int)(y)*map.width + (int)(x) ]==0;
}

int O_addObjects(O_objects objects[], M_map map, T_texture text_princ, T_texture text_sec){
    int nbChain = 20;
    
    for(int i=0; i<nbChain; i++){
        float x=0;
        float y=0;
        while(!O_isValidPosition(map, x, y)){
            x = (float)randMinMax(1, (map.width-1) *5) / 5.f;
            y = (float)randMinMax(1, (map.height-1)*5) / 5.f;
        }
        O_objects chain = {x, y, randMinMax(0, 2)==1?text_princ:text_sec};

        objects[i] = chain;
    }
    
    return nbChain;
}