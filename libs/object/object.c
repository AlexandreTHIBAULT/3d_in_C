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
            x = (float)randMinMax(1, (map.width-1) *5) / 5.f;
            y = (float)randMinMax(1, (map.height-1)*5) / 5.f;
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

void O_shoot(P_player player, O_objects_data* missile_data, Ray missile_dir[], T_texture text){
    float dir_X, dir_Y;
    dir_X = cosf(player.direction);
    dir_Y = sinf(player.direction);

    Ray missile_d = {dir_X, dir_Y, 0};
    O_object missile_obj = {player.X, player.Y, text};

    missile_data->objects[missile_data->nb] = missile_obj;
    missile_dir[missile_data->nb] = missile_d;
    missile_data->nb ++;

}

void O_updateShootMoove(O_objects_data* missile_data, Ray missile_dir[], double t_delta){
    for(int i=0; i<missile_data->nb; i++){
        missile_data->objects[i].x += missile_dir[i].X * t_delta * 4.f;
        missile_data->objects[i].y += missile_dir[i].Y * t_delta * 4.f;
    }
}

void O_despawnShoot(O_objects_data* missile_data, Ray missile_dir[], int missile_id){
    missile_data->objects[missile_id] =  missile_data->objects[(missile_data->nb)-1];
    missile_dir[missile_id] =  missile_dir[(missile_data->nb)-1];
    missile_data->nb -= 1;
}

void O_updateShoot(M_map map, O_objects_data* missile_data, Ray missile_dir[]){
    for(int i=0; i<missile_data->nb; i++){
        if(!isPositionFree(map, missile_data->objects[i].x, missile_data->objects[i].y)){
            O_despawnShoot(missile_data, missile_dir, i);
            return;
        }
    }
}

void O_checkEnemiesTouch(O_objects_data* monsters_data, O_objects_data* missile_data, Ray missile_dir[]){
    for(int i=0; i<missile_data->nb; i++){
        for(int m=0; m<monsters_data->nb; m++){
            if(distance(missile_data->objects[i].x, missile_data->objects[i].y, monsters_data->objects[m].x, monsters_data->objects[m].y) < 0.3){
                O_killMonster(monsters_data, m);
                O_despawnShoot(missile_data, missile_dir, i);
                return;
            }
        }
    }
}

O_objects_data O_makeCompleteData(O_objects_data objects_data, O_objects_data monsters_data, O_objects_data missile_data){
    O_objects_data complete_data;
    for(int i=0; i<objects_data.nb; i++){
        complete_data.objects[i] = objects_data.objects[i];
    }
    for(int i=0; i<monsters_data.nb; i++){
        complete_data.objects[objects_data.nb+i] = monsters_data.objects[i];
    }
    for(int i=0; i<missile_data.nb; i++){
        complete_data.objects[monsters_data.nb+objects_data.nb+i] = missile_data.objects[i];
    }
    complete_data.nb = objects_data.nb + monsters_data.nb + missile_data.nb;

    return complete_data;
}

void O_monsterMoveOne(M_map map, O_objects_data* monsters_data, int monster_id, P_player player, float dist, double t_delta){
    float dest_X, dest_Y;
    float dir_X, dir_Y;
    dir_X = (player.X - monsters_data->objects[monster_id].x) / dist;
    dir_Y = (player.Y - monsters_data->objects[monster_id].y) / dist;

    dest_X = monsters_data->objects[monster_id].x + dir_X * t_delta * 0.5f;
    dest_Y = monsters_data->objects[monster_id].y + dir_Y * t_delta * 0.5f;

    if(isPositionFree(map, dest_X, dest_Y)){
        monsters_data->objects[monster_id].x = dest_X;
        monsters_data->objects[monster_id].y = dest_Y;
    }
}

void O_monsterMoveAll(M_map map, O_objects_data* monsters_data, P_player player, double t_delta){
    for(int i=0; i<monsters_data->nb; i++){
        float dist = distance(monsters_data->objects[i].x, monsters_data->objects[i].y, player.X, player.Y);
        if(dist<VIEW_RANGE)
            O_monsterMoveOne(map, monsters_data, i, player, dist, t_delta);
    }
}