#ifndef __SOKOLEVEL_H__
#define __SOKOLEVEL_H__

#define MAX_TITLE_LEN 80
#define MAX_AUTHOR_LEN 80

#include <stdbool.h>

// #define GRIDVAL(LVL,R,C) (LVL)->grid[(R)*(LVL)->width + (C)]

enum gridVal {
	OUTSIDE          = 0,   // outside of level
	INSIDE           = 1,   // inside of level
	WALL             = 2,
	BOX              = 4,
	TARGET           = 8,
	BOX_ON_TARGET    = 12,  // BOX | TARGET
	WORKER           = 16,
	WORKER_ON_TARGET = 24   // WORKER | TARGET
};
typedef enum gridVal gridVal;

typedef struct Sokolevel {
	char     title[MAX_TITLE_LEN + 1];   // name of level
	char     author[MAX_AUTHOR_LEN + 1]; // author of level
	int      width;            // width of level
	int      height;           // height of level
	gridVal* grid;             // grid data,  row for row
	int      nrBoxes;          // total nr boxes in level
	int      nrBoxesOnTargets; // nr of boxes that are on a target
	int      workerCol;        // X-pos of worker
	int      workerRow;        // Y-pos of worker
	int*     helper;           // helper grid (e.g. for floodfill, path finding, etc.)
} Sokolevel;

Sokolevel* create_level(int w, int h);
void       destroy_level(Sokolevel* lvl);

void    set_grid(Sokolevel* lvl, int r, int c, gridVal v);
gridVal get_grid(Sokolevel* lvl, int r, int c);

bool check_level(Sokolevel* lvl);

void print_level(Sokolevel* lvl);
#endif
