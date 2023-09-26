/********************/
/* --- INCLUDES --- */
/********************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*******************/
/* --- DEFINES --- */
/*******************/

#define M_H 25
#define M_W 25

#define M_ROOM_DIM_MAX 8
#define M_ROOM_DIM_MIN 3

#define M_NB_ROOM 8
#define M_NB_BOX 16

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

/*********************/
/* --- FUNCTIONS --- */
/*********************/

int randMinMax(int min, int max);
void M_printMaze(int* maze);
int* M_initMaze();
void M_makeRoom(int* maze, int room_x, int room_y, int room_h, int room_w);
void M_makeVLine(int* maze, int x, int y1, int y2);
void M_makeHLine(int* maze, int x1, int x2, int y);
void M_makeTunnel(int* maze, int rooms_x[], int rooms_y[], int rooms_h[], int rooms_w[], int i_room_1, int i_room_2);
int M_isRoomOverlapping(int rooms_x[], int rooms_y[], int rooms_h[], int rooms_w[], int i_room);
int M_areRoomsConnected(int is_connected[]);
int M_isCorner(int* maze, int x, int y);
void M_addDetails(int* maze, int rooms_x[], int rooms_y[], int rooms_h[], int rooms_w[]);
int* M_makeMaze(float* start_x, float* start_y);