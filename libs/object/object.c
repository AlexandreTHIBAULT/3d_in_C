#include "object.h"

int O_isValidPosition(M_map map, float x, float y){
    return map.map[ (int)(y)*map.width + (int)(x) ]==0;
}

int O_addObjects(O_object objects[], M_map map, T_texture text_princ, T_texture text_sec){
    int nbChain = 20;
    
    for(int i=0; i<nbChain; i++){
        float x=0;
        float y=0;
        while(!O_isValidPosition(map, x, y)){
            x = (float)randMinMax(1, (map.width-1) *5) / 5.f;
            y = (float)randMinMax(1, (map.height-1)*5) / 5.f;
        }
        O_object chain = {x, y, randMinMax(0, 2)==1?text_princ:text_sec};

        objects[i] = chain;
    }
    
    return nbChain;
}

O_objects_data O_spawnMonsters(M_map map, T_texture text_monster){
    O_objects_data monsters_data;

    for(int i=0; i<NB_MONSTER; i++){
        float x=0;
        float y=0;
        while(!O_isValidPosition(map, x, y)){
            printf("---");
            x = (float) randMinMax(1, (map.width-1 ));
            y = (float) randMinMax(1, (map.height-1));
        }
        O_object monster = {x, y, text_monster};
        monsters_data.objects[i] = monster;
        monsters_data.prop[i] = 0;
        monsters_data.dist[i] = 0;
    }
    monsters_data.nb = NB_MONSTER;

    return monsters_data;
}

void O_killMonster(O_objects_data* monsters_data, int monster_id){
    monsters_data->objects[monster_id] =  monsters_data->objects[(monsters_data->nb)-1];
    monsters_data->nb -= 1;
}

O_objects_data O_makeCompleteData(O_objects_data objects_data, O_objects_data monsters_data){
    O_objects_data complete_data;
    for(int i=0; i<objects_data.nb; i++){
        complete_data.objects[i] = objects_data.objects[i];
    }
    for(int i=0; i<monsters_data.nb; i++){
        complete_data.objects[objects_data.nb+i] = monsters_data.objects[i];
    }
    complete_data.nb = objects_data.nb + monsters_data.nb;

    return complete_data;
}