#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M_H 25
#define M_W 25

#define M_ROOM_DIM_MAX 8
#define M_ROOM_DIM_MIN 3

#define M_NB_ROOM 8

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

int randMinMax(int min, int max){
    if (min==max) return min;
    return (rand()%(max-min) + min);
}

void printMaze(int* maze){
    for (int i_h=0; i_h<M_H; i_h++){
        for (int i_w=0; i_w<M_W; i_w++){
            if (maze[i_h*M_W + i_w])
                printf("██");
            else
                printf("  ");
        }
        printf("\n");
    }
}

int* initMaze(){
    int* maze = malloc(M_H*M_W * sizeof(int));
    for(int i=0; i<M_H*M_W; i++){
        maze[i] = 1;
    }

    return maze;
}

void makeRoom(int* maze, int room_x, int room_y, int room_h, int room_w){
     for(int i_h=0; i_h<room_h; i_h++){
        for(int i_w=0; i_w<room_w; i_w++){
            maze[(room_y+i_h)*M_W + (room_x+i_w)] = 0;
        }
     }
}

void makeVLine(int* maze, int x, int y1, int y2){
    int min_y = min(y1, y2);
    int max_y = max(y1, y2);
    for(int i=min_y; i<=max_y; i++){
        maze[i*M_W + x] = 0;
    }
}

void makeHLine(int* maze, int x1, int x2, int y){
    int min_x = min(x1, x2);
    int max_x = max(x1, x2);
    for(int i=min_x; i<=max_x; i++){
        maze[y*M_W + i] = 0;
    }
}

void makeTunnel(int* maze, int rooms_x[], int rooms_y[], int rooms_h[], int rooms_w[], int i_room_1, int i_room_2){
    int side_1 = randMinMax(0, 3);
    int side_2 = randMinMax(0, 3);

    int x1, y1, x2, y2;
    
    if(side_1 == 0){
        x1 = rooms_x[i_room_1]+randMinMax(1, rooms_w[i_room_1]-2);
        y1 = rooms_y[i_room_1];
    }
    else if(side_1 == 1){
        x1 = rooms_x[i_room_1]+rooms_w[i_room_1]-1;
        y1 = rooms_y[i_room_1]+randMinMax(1, rooms_h[i_room_1]-2);
    }
    else if(side_1 == 2){
        x1 = rooms_x[i_room_1]+randMinMax(1, rooms_w[i_room_1]-2);
        y1 = rooms_y[i_room_1]+rooms_h[i_room_1]-1;
    }
    else if(side_1 == 3){
        x1 = rooms_x[i_room_1];
        y1 = rooms_y[i_room_1]+randMinMax(1, rooms_h[i_room_1]-2);
    }

    if(side_2 == 0){
        x2 = rooms_x[i_room_2]+randMinMax(1, rooms_w[i_room_2]-2);
        y2 = rooms_y[i_room_2];
    }
    else if(side_2 == 1){
        x2 = rooms_x[i_room_2]+rooms_w[i_room_2]-1;
        y2 = rooms_y[i_room_2]+randMinMax(1, rooms_h[i_room_2]-2);
    }
    else if(side_2 == 2){
        x2 = rooms_x[i_room_2]+randMinMax(1, rooms_w[i_room_2]-2);
        y2 = rooms_y[i_room_2]+rooms_h[i_room_2]-1;
    }
    else if(side_2 == 3){
        x2 = rooms_x[i_room_2];
        y2 = rooms_y[i_room_2]+randMinMax(1, rooms_h[i_room_2]-2);
    }

    if(side_1%2 == side_2%2 && side_1%2==0){
        int mid_y = (y1+y2)/2;
        makeVLine(maze, x1, y1, mid_y);
        makeHLine(maze, x1, x2, mid_y);
        makeVLine(maze, x2, mid_y, y2);
    }else if(side_1%2 == side_2%2 && side_1%2==1){
        int mid_x = (x1+x2)/2;
        makeHLine(maze, mid_x, x1, y1);
        makeVLine(maze, mid_x, y1, y2);
        makeHLine(maze, mid_x, x2, y2);
    }
    else if(side_1%2==0){
        makeVLine(maze, x1, y1, y2);
        makeHLine(maze, x1, x2, y2);
    }
    else if(side_1%2==1){
        makeHLine(maze, x2, x1, y1);
        makeVLine(maze, x2, y2, y1);
    }
    

}

int isRoomOverlapping(int rooms_x[], int rooms_y[], int rooms_h[], int rooms_w[], int i_room){
    int angle_x, angle_y;
    for (int i=0; i<i_room; i++){
        angle_x = rooms_x[i_room];
        angle_y = rooms_y[i_room];
        if( angle_x>=rooms_x[i]-1 && angle_x<=rooms_x[i]+rooms_w[i]+1
         && angle_y>=rooms_y[i]-1 && angle_y<=rooms_y[i]+rooms_h[i]+1){
            return 1;
         }
        angle_x = rooms_x[i_room] + rooms_w[i_room] - 1;
        angle_y = rooms_y[i_room];
        if( angle_x>=rooms_x[i]-1 && angle_x<=rooms_x[i]+rooms_w[i]+1
         && angle_y>=rooms_y[i]-1 && angle_y<=rooms_y[i]+rooms_h[i]+1){
            return 1;
         }
        angle_x = rooms_x[i_room];
        angle_y = rooms_y[i_room] + rooms_h[i_room] - 1;
        if( angle_x>=rooms_x[i]-1 && angle_x<=rooms_x[i]+rooms_w[i]+1
         && angle_y>=rooms_y[i]-1 && angle_y<=rooms_y[i]+rooms_h[i]+1){
            return 1;
         }
        angle_x = rooms_x[i_room] + rooms_w[i_room] - 1;
        angle_y = rooms_y[i_room] + rooms_h[i_room] - 1;
        if( angle_x>=rooms_x[i]-1 && angle_x<=rooms_x[i]+rooms_w[i]+1
         && angle_y>=rooms_y[i]-1 && angle_y<=rooms_y[i]+rooms_h[i]+1){
            return 1;
         }
    }

    return 0;
}

int areRoomsConnected(int is_connected[]){
    for(int i=0; i<M_NB_ROOM; i++){
        if (!is_connected[i]) return 0;
    }
    return 1;
}

int* makeMaze(float* start_x, float* start_y){
    srand( time( NULL ) );
    int* maze = initMaze();

    int rooms_x[M_NB_ROOM];
    int rooms_y[M_NB_ROOM];
    int rooms_w[M_NB_ROOM];
    int rooms_h[M_NB_ROOM];

    for (int i=0; i<M_NB_ROOM; i++){
        int overlap = 1;
        while( overlap ){
            rooms_h[i] =  randMinMax(M_ROOM_DIM_MIN, M_ROOM_DIM_MAX);
            rooms_w[i] =  randMinMax(M_ROOM_DIM_MIN, M_ROOM_DIM_MAX);

            rooms_x[i] =  randMinMax(1, M_W - rooms_w[i] - 1);
            rooms_y[i] =  randMinMax(1, M_H - rooms_h[i] - 1);

            overlap = isRoomOverlapping(rooms_x, rooms_y, rooms_h, rooms_w, i);
        }

        makeRoom(maze, rooms_x[i], rooms_y[i], rooms_h[i], rooms_w[i]);
    }

    int is_connected[M_NB_ROOM];
    for(int i=0; i<M_NB_ROOM; i++){
        is_connected[i] = 0;
    }

    int connected = 0;
    int i1, i2;
    while(!connected){
        i1 = randMinMax(0, M_NB_ROOM);
        i2 = randMinMax(0, M_NB_ROOM);
        if(i1!=i2){
            makeTunnel(maze, rooms_x, rooms_y, rooms_h, rooms_w, i1, i2);
            is_connected[i1] = 1;
            is_connected[i2] = 1;
            connected = areRoomsConnected(is_connected);
        }
    }

    *start_x = (float) rooms_x[0] + ((float) rooms_w[0])/2.f;
    *start_y = (float) rooms_y[0] + ((float) rooms_h[0])/2.f;

    return maze;
};

