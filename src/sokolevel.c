#include "sokolevel.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Sokolevel* create_level(int w, int h) {
	Sokolevel* lvl = malloc(sizeof(Sokolevel));
	if (lvl) {
		lvl->width = w;
		lvl->height = h;
		lvl->grid = malloc(w * h * sizeof(gridVal));
		for (int r = 0; r < h; ++r) {
			for (int c = 0; c < w; ++c) {
				lvl->grid[lvl->width * r + c] = OUTSIDE;
			}
		}
		lvl->title[0] = '\0';
		lvl->author[0] = '\0';
	}
	return lvl;
}

void destroy_level(Sokolevel* lvl) {
	if (lvl) {
		free(lvl->grid);
	}
	free(lvl);
}

void level_set_grid(Sokolevel* lvl, int r, int c, gridVal v) {
	if (r >= 0 && r < lvl->height && c >= 0 && c < lvl->width) {
		lvl->grid[lvl->width * r + c] = v;
	}
}

gridVal level_get_grid(Sokolevel* lvl, int r, int c) {
	if (r >= 0 && r < lvl->height && c >= 0 && c < lvl->width) {
		return lvl->grid[lvl->width * r + c];
	}
	else {
		return INVALID;
	}
}

static bool inout_helperfn(Sokolevel* lvl, Stack* s, int r, int c) {
	// helper fn for in/outside floodfill
	if (r >= 0 && r < lvl->height && c >= 0 && c < lvl->width) {
		int idx = r * lvl->width + c;
		switch (lvl->grid[idx]) {
			case OUTSIDE:
				return true;
			case WALL:
				return true;
			case INSIDE:
				lvl->grid[idx] = OUTSIDE;
				push_on_int_stack(s, r);
				push_on_int_stack(s, c);
				return true;
			default:
				printf("Unexpected grid content: %d\n", lvl->grid[idx]);
				return false; // error
		}
	}
	return true;
}

static bool check_inside_outside(Sokolevel* lvl) {
	int r, c, idx;
	bool all_okay = true;

	// prepping for floodfill: set all floors to inside
	for (idx = 0; idx < lvl->width * lvl->height; ++idx) {
		if (lvl->grid[idx] == OUTSIDE) {
			lvl->grid[idx] = INSIDE;
		}
	}

	Stack* s = create_int_stack(2 * lvl->width * lvl->height);
	// go around border for floodfilling
	for (c = 0; c < lvl->width && all_okay; ++c) {
		all_okay = all_okay && inout_helperfn(lvl, s, 0, c);
		all_okay = all_okay && inout_helperfn(lvl, s, lvl->height - 1, c);
	}
	for (r = 1; r < lvl->height - 1 && all_okay; ++r) { // skip corners
		all_okay = all_okay && inout_helperfn(lvl, s, r, 0);
		all_okay = all_okay && inout_helperfn(lvl, s, r, lvl->width - 1);
	}
	// floodfill
	while (!is_stack_empty(s) && all_okay) {
		c = pop_from_int_stack(s);
		r = pop_from_int_stack(s);
		all_okay = all_okay && inout_helperfn(lvl, s, r - 1, c);
		all_okay = all_okay && inout_helperfn(lvl, s, r + 1, c);
		all_okay = all_okay && inout_helperfn(lvl, s, r, c - 1);
		all_okay = all_okay && inout_helperfn(lvl, s, r, c + 1);
	}
	destroy_stack(s);
	return all_okay;
}

bool check_level(Sokolevel* lvl) {
	// check level integrity
	// store findings in Sokolevel struct
	int nrBoxes = 0;
	int nrTargets = 0;
	int nrBoxesOnTargets = 0;
	int nrWorkers = 0;

	// first, some basic tallying
	for (int r = 0; r < lvl->height; ++r) {
		for (int c = 0; c < lvl->width; ++c) {
			gridVal v = lvl->grid[lvl->width * r + c];
			if (v & WORKER) {
				++nrWorkers;
				lvl->workerRow = r;
				lvl->workerCol = c;
			}
			if (v & BOX) {
				++nrBoxes;
			}
			if (v & TARGET) {
				++nrTargets;
			}
			if (v == BOX_ON_TARGET) {
				++nrBoxesOnTargets;
			}
		}
	}
	if (nrBoxes != nrTargets) {
		printf("Level %s: nr boxes is not equal to nr of targets\n", lvl->title);
		return false;
	}
	if (nrWorkers != 1) {
		printf("Level %s: nr workers does not equal 1\n", lvl->title);
		return false;
	}
	lvl->nrBoxes = nrBoxes;
	lvl->nrBoxesOnTargets = nrBoxesOnTargets;

	return check_inside_outside(lvl);
}

void print_level(Sokolevel* lvl) {
	char ch;
	int row_idx = 0;
	for (int r = 0; r < lvl->height; ++r) {
		// Find last wall to avoid trailing spaces (cosmetics...)
		int last_wall_col = 0;
		for (int c = lvl->width - 1; c >= 0; --c) {
			if (lvl->grid[row_idx + c] == WALL) {
				last_wall_col = c;
				break;
			}
		}
#ifdef DEBUG
		last_wall_col = lvl->width - 1; // print same length rows with trailing 'OUTSIDE'
#endif
		for (int c = 0; c <= last_wall_col; ++c) {
			gridVal v = lvl->grid[row_idx + c];
			switch (v) {
				case OUTSIDE:
#ifdef DEBUG
					ch = 'X';
					break;
#endif
				case INSIDE:
					ch = ' ';
					break;
				case WALL:
					ch = '#';
					break;
				case BOX:
					ch = '$';
					break;
				case TARGET:
					ch = '.';
					break;
				case BOX_ON_TARGET:
					ch = '*';
					break;
				case WORKER:
					ch = '@';
					break;
				case WORKER_ON_TARGET:
					ch = '+';
					break;
				default:
					ch = '?';
					break;
			}
			putchar(ch);
		}
		putchar('\n');
		row_idx += lvl->width;
	}
	if (strlen(lvl->title) > 0) {
		printf("; %s\n", lvl->title);
	}
	if (strlen(lvl->author) > 0) {
		printf("; Copyright: %s\n", lvl->author);
	}
	putchar('\n');
}

