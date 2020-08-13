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

void set_grid(Sokolevel* lvl, int r, int c, gridVal v) {
	if (r >= 0 && r < lvl->height && c >= 0 && c < lvl->width) {
		lvl->grid[lvl->width * r + c] = v;
	}
}

gridVal get_grid(Sokolevel* lvl, int r, int c) {
	return lvl->grid[lvl->width * r + c];
}

bool check_level(Sokolevel* lvl) {
	// check level integrity
	// store findings in Sokolevel struct
	int nrBoxes = 0;
	int nrTargets = 0;
	int nrBoxesOnTargets = 0;
	int nrWorkers = 0;

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

	return true;
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
		for (int c = 0; c <= last_wall_col; ++c) {
			gridVal v = lvl->grid[row_idx + c];
			switch (v) {
				case OUTSIDE:
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

