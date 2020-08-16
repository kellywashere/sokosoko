#ifndef __SOKOLEVEL_H__
#define __SOKOLEVEL_H__

#define MAX_TITLE_LEN 80
#define MAX_AUTHOR_LEN 80

#include <stdbool.h>

// #define GRIDVAL(LVL,R,C) (LVL)->grid[(R)*(LVL)->width + (C)]

enum gridVal { // values are important for boolean operations!
	INSIDE           = 0,   // inside of level
	OUTSIDE          = 1,   // outside of level
	WALL             = 2,
	BOX              = 4,
	TARGET           = 8,
	BOX_ON_TARGET    = 12,  // BOX | TARGET
	WORKER           = 16,
	WORKER_ON_TARGET = 24,  // WORKER | TARGET

	INVALID          = 256
};
typedef enum gridVal gridVal;

typedef struct GridPos {
	int row;
	int col;
} GridPos;

typedef struct MoveList {
	char*        moves;        // list of moves using std char notation
	unsigned int capacity;     // capacity of moves (reserved mem)
	unsigned int nrMoves;      // length of moves (nr chars)
	unsigned int nrPushes;     // nr of push moves in list
} MoveList;

typedef struct Sokolevel {
	char      title[MAX_TITLE_LEN + 1];   // name of level
	char      author[MAX_AUTHOR_LEN + 1]; // author of level
	int       width;            // width of level
	int       height;           // height of level
	gridVal*  grid;             // grid data,  row for row
	int       nrBoxes;          // total nr boxes in level
	int       nrBoxesOnTargets; // nr of boxes that are on a target
	GridPos   workerPos;        // position of worker
	MoveList* movelist;         // storing moves in level
} Sokolevel;

Sokolevel* create_level(int w, int h);
void       destroy_level(Sokolevel* lvl);

void    level_set_grid(Sokolevel* lvl, GridPos pos, gridVal v);
gridVal level_get_grid(Sokolevel* lvl, GridPos pos);

// next fns return a char (udlf for normal moves, UDLR for push moves, \0 for no move
char level_move_left(Sokolevel* lvl);
char level_move_right(Sokolevel* lvl);
char level_move_up(Sokolevel* lvl);
char level_move_down(Sokolevel* lvl);
char level_undo_move(Sokolevel* lvl);
char level_undo_last_push(Sokolevel* lvl); // undoes everything up to and including last push
char level_last_move(Sokolevel* lvl);

bool check_level(Sokolevel* lvl);

void print_level(Sokolevel* lvl);
#endif
