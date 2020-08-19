#include "sokolevelset.h"
#include <stdlib.h>
#include <stddef.h>

Sokolevelset* create_levelset() {
	Sokolevelset* set = malloc(sizeof(Sokolevelset));
	if (set) {
		set->first = NULL;
		set->last = NULL;
		set->current = NULL;
	}
	return set;
}

void destroy_levelset(Sokolevelset* set) {
	while (set->first) {
		Sokolevelset_node* n = set->first;
		set->first = n->next;
		destroy_level(n->level);
		free(n);
	}
}

void levelset_add_level(Sokolevelset* set, Sokolevel* lvl) {
	Sokolevelset_node* n = malloc(sizeof(Sokolevelset_node));
	n->level = lvl;
	n->next = NULL;
	n->prev = set->last;
	set->last = n;

	if (n->prev) {
		n->prev->next = n;
		n->nrInSet = n->prev->nrInSet + 1;
	}
	else {
		n->nrInSet = 1;
	}

	if (set->first == NULL) { // empty list
		set->first = n;
		set->current = n;
	}
}

Sokolevel* levelset_cur(Sokolevelset* set) {
	if (set->current) {
		return set->current->level;
	}
	else {
		return NULL;
	}
}

Sokolevel* levelset_prev(Sokolevelset* set) {
	Sokolevelset_node* n = set->current->prev;
	if (n) {
		set->current = n;
		return n->level;
	}
	return NULL;
}

Sokolevel* levelset_next(Sokolevelset* set) {
	Sokolevelset_node* n = set->current->next;
	if (n) {
		set->current = n;
		return n->level;
	}
	return NULL;
}

Sokolevel* levelset_first(Sokolevelset* set) {
	set->current = set->first;
	if (!set->current) {
		return NULL;
	}
	return set->current->level;
}

int levelset_get_levelnr(Sokolevelset* set) {
	if (!set->current) {
		return 0;
	}
	return set->current->nrInSet;
}

unsigned int levelset_size(Sokolevelset* set) {
	if (set->last) {
		return set->last->nrInSet;
	}
	else {
		return 0;
	}
}
