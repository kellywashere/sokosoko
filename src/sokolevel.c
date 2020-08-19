#include "sokolevel.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
		lvl->movelist = NULL;
	}
	return lvl;
}

void destroy_level(Sokolevel* lvl) {
	if (lvl) {
		free(lvl->grid);
		free(lvl->movelist);
	}
	free(lvl);
}

void level_set_grid(Sokolevel* lvl, int row, int col, gridVal v) {
	if (row >= 0 && row < lvl->height && col >= 0 && col < lvl->width) {
		lvl->grid[lvl->width * row + col] = v;
	}
}

gridVal level_get_grid(Sokolevel* lvl, int row, int col) {
	if (row >= 0 && row < lvl->height && col >= 0 && col < lvl->width) {
		return lvl->grid[lvl->width * row + col];
	}
	else {
		return OUTSIDE; //INVALID;
	}
}

static void level_print_status(Sokolevel* lvl) {
	printf("moves: %4d    pushes: %4d    target: %3d/%d\n",
			level_get_nrMoves(lvl), level_get_nrPushes(lvl), lvl->nrBoxesOnTargets, lvl->nrBoxes);
}

static char move_to_char(int dr, int dc, bool is_push) { // valid move assumed
	char mc;
	if (dr < 0) {
		mc = 'u';
	}
	else if (dr > 0) {
		mc = 'd';
	}
	else if (dc < 0) {
		mc = 'l';
	}
	else {
		mc = 'r';
	}
	if (is_push) {
		mc = toupper(mc);
	}
	return mc;
}

static void char_to_move(char mc, int* dr, int* dc, bool* is_push) {
	*is_push = isupper(mc);
	mc = tolower(mc);
	*dr = 0;
	*dc = 0;
	switch (mc) {
		case 'u':
			*dr = -1;
			break;
		case 'd':
			*dr = 1;
			break;
		case 'l':
			*dc = -1;
			break;
		case 'r':
			*dc = 1;
			break;
		default:
			break;
	}
}

static void level_add_move(Sokolevel* lvl, char mc) {
	// TODO: error checking in mem alloc
	if (mc) {
		if (!lvl->movelist) {
			lvl->movelist = malloc(sizeof(MoveList));
			lvl->movelist->capacity = 256;
			lvl->movelist->moves = malloc(sizeof(char) * lvl->movelist->capacity);
			lvl->movelist->nrMoves = 0;
			lvl->movelist->nrPushes = 0;
		}
		else if (lvl->movelist->nrMoves + 1 >= lvl->movelist->capacity) {
			lvl->movelist->capacity += 256;
			lvl->movelist->moves = realloc(lvl->movelist->moves,
					sizeof(char) * lvl->movelist->capacity);
		}
		lvl->movelist->moves[lvl->movelist->nrMoves++] = mc;
		if (isupper(mc)) {
			++lvl->movelist->nrPushes;
		}
	}
}

static char level_move(Sokolevel* lvl, int dr, int dc) {
	bool is_push = false;
	char mc = '\0';
	int r = lvl->workerPos.row + dr; // worker target
	int c = lvl->workerPos.col + dc; // worker target
	if (r < 0 || r >= lvl->height || c < 0 || c >= lvl->width) { // can't actually happen
		return '\0';
	}
	gridVal g = lvl->grid[r * lvl->width + c];
	if (g == WALL) {
		return '\0';
	}
	if ((g & BOX) == BOX) { // attempting push move
		int r2 = r + dr; // box target
		int c2 = c + dc; // box target
		if (r2 < 0 || r2 >= lvl->height || c2 < 0 || c2 >= lvl->width) { // can't actually happen
			return '\0';
		}
		gridVal g2 = lvl->grid[r2 * lvl->width + c2];
		if (g2 == WALL || (g2 & BOX) == BOX) {
			return '\0';
		}
		// execute box move
		lvl->grid[r * lvl->width + c] &= ~BOX;
		lvl->grid[r2 * lvl->width + c2] |= BOX;
		if ((g & TARGET) == TARGET) {
			--lvl->nrBoxesOnTargets;
		}
		if ((g2 & TARGET) == TARGET) {
			++lvl->nrBoxesOnTargets;
		}
		is_push = true;
	}
	// execute worker move
	lvl->grid[lvl->width * lvl->workerPos.row + lvl->workerPos.col] &= ~WORKER;
	lvl->grid[r * lvl->width + c] |= WORKER;
	lvl->workerPos.row = r;
	lvl->workerPos.col = c;
	mc = move_to_char(dr, dc, is_push);
	level_add_move(lvl, mc);
	// level_print_status(lvl);
	return mc;
}

// next fns return a char (udlf for normal moves, UDLR for push moves, \0 for no move
char level_move_left(Sokolevel* lvl) {
	return level_move(lvl, 0, -1);
}

char level_move_right(Sokolevel* lvl) {
	return level_move(lvl, 0, 1);
}

char level_move_up(Sokolevel* lvl) {
	return level_move(lvl, -1, 0);
}

char level_move_down(Sokolevel* lvl) {
	return level_move(lvl, 1, 0);
}

char level_undo_move(Sokolevel* lvl) {
	if (!lvl->movelist || lvl->movelist->nrMoves == 0) {
		return '\0';
	}
	char mc = lvl->movelist->moves[--lvl->movelist->nrMoves];
	int dr, dc;
	bool is_push;
	char_to_move(mc, &dr, &dc, &is_push);
	if (is_push) {
		--lvl->movelist->nrPushes;
	}
	// undo worker move
	int r = lvl->workerPos.row;
	int c = lvl->workerPos.col;
	lvl->grid[lvl->width * r + c] &= ~WORKER;
	lvl->workerPos.row = (r - dr);
	lvl->workerPos.col = (c - dc);
	lvl->grid[lvl->width * lvl->workerPos.row + lvl->workerPos.col] |= WORKER;
	if (is_push) { // undo box move
		lvl->grid[lvl->width * (r + dr) + (c + dc)] &= ~BOX;
		lvl->grid[lvl->width * r + c] |= BOX;
		if ((lvl->grid[lvl->width * (r + dr) + (c + dc)] & TARGET) == TARGET) {
			--lvl->nrBoxesOnTargets;
		}
		if ((lvl->grid[lvl->width * r + c ] & TARGET) == TARGET) {
			++lvl->nrBoxesOnTargets;
		}
	}
	level_print_status(lvl);
	return mc;
}

char level_undo_last_push(Sokolevel* lvl) { // undoes everything up to and including last push
	char mc;
	do {
		mc = level_undo_move(lvl);
	} while (mc != '\0' && islower(mc));
	return mc;
}

char level_last_move(Sokolevel* lvl) {
	if (!lvl->movelist || lvl->movelist->nrMoves == 0) {
		return '\0';
	}
	return lvl->movelist->moves[lvl->movelist->nrMoves - 1];
}

bool is_level_won(Sokolevel* lvl) {
	return lvl->nrBoxesOnTargets == lvl->nrBoxes;
}

static bool inout_helperfn(Sokolevel* lvl, Stack* s, int r, int c) {
	// helper fn for in/outside floodfill
	GridPos pos;
	if (r >= 0 && r < lvl->height && c >= 0 && c < lvl->width) {
		int idx = r * lvl->width + c;
		switch (lvl->grid[idx]) {
			case OUTSIDE:
				return true;
			case WALL:
				return true;
			case INSIDE:
				lvl->grid[idx] = OUTSIDE;
				pos.row = r;
				pos.col = c;
				push_on_stack(s, &pos);
				return true;
			default:
				printf("Unexpected grid content: %d\n", lvl->grid[idx]);
				return false; // error
		}
	}
	return true;
}

static bool check_inside_outside(Sokolevel* lvl) {
	GridPos pos;
	bool all_okay = true;

	// prepping for floodfill: set all floors to inside
	for (int idx = 0; idx < lvl->width * lvl->height; ++idx) {
		if (lvl->grid[idx] == OUTSIDE) {
			lvl->grid[idx] = INSIDE;
		}
	}

	Stack* s = create_stack(sizeof(GridPos), lvl->width * lvl->height);
	// go around border for floodfilling
	for (int c = 0; c < lvl->width && all_okay; ++c) {
		all_okay = all_okay && inout_helperfn(lvl, s, 0, c);
		all_okay = all_okay && inout_helperfn(lvl, s, lvl->height - 1, c);
	}
	for (int r = 1; r < lvl->height - 1 && all_okay; ++r) { // skip corners
		all_okay = all_okay && inout_helperfn(lvl, s, r, 0);
		all_okay = all_okay && inout_helperfn(lvl, s, r, lvl->width - 1);
	}
	// floodfill
	while (!is_stack_empty(s) && all_okay) {
		pop_from_stack(s, &pos);
		all_okay = all_okay && inout_helperfn(lvl, s, pos.row - 1, pos.col    );
		all_okay = all_okay && inout_helperfn(lvl, s, pos.row + 1, pos.col    );
		all_okay = all_okay && inout_helperfn(lvl, s, pos.row    , pos.col - 1);
		all_okay = all_okay && inout_helperfn(lvl, s, pos.row    , pos.col + 1);
	}
	destroy_stack(s);
	return all_okay;
}

unsigned int level_get_nrMoves(Sokolevel* lvl) {
	if (!lvl->movelist) {
		return 0;
	}
	return lvl->movelist->nrMoves;
}

unsigned int level_get_nrPushes(Sokolevel* lvl) {
	if (!lvl->movelist) {
		return 0;
	}
	return lvl->movelist->nrPushes;
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
				lvl->workerPos.row = r;
				lvl->workerPos.col = c;
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

