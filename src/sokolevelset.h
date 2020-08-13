#ifndef __SOKOLEVELSET_H__
#define __SOKOLEVELSET_H__

/* Levelset is a double-linked list of sokolevels */

#include "sokolevel.h"
#include <stdbool.h>

#define MAX_DESCRIPTION_LEN 2048

typedef struct Sokolevelset_node {
	Sokolevel* level;  // the level itself
	int nrInSet;       // nr in order
	struct Sokolevelset_node* prev;
	struct Sokolevelset_node* next;
} Sokolevelset_node;

typedef struct Sokolevelset {
	char title[MAX_TITLE_LEN + 1];
	char author[MAX_AUTHOR_LEN + 1];
	char description[MAX_DESCRIPTION_LEN + 1];
	Sokolevelset_node* first;
	Sokolevelset_node* last;
	Sokolevelset_node* current;
} Sokolevelset;

Sokolevelset* create_levelset();
void          destroy_levelset(Sokolevelset* set); // also destroys all levels within!

void levelset_add_level(Sokolevelset* set, Sokolevel* lvl); // always adds to end of list

Sokolevel* levelset_cur(Sokolevelset* set);
Sokolevel* levelset_prev(Sokolevelset* set);
Sokolevel* levelset_next(Sokolevelset* set);

#endif
