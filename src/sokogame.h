#ifndef __SOKOGAME_H__
#define __SOKOGAME_H__

#include "sokolevel.h"

enum DirKey {     // for key press response
	NO_DIRKEY,
	DIRKEY_UP,
	DIRKEY_DOWN,
	DIRKEY_LEFT,
	DIRKEY_RIGHT
};
typedef enum DirKey DirKey;

/* For directional movement we use a weird approach:
 * last dir-key pressed is the one that is active, but older keys are remembered for
 * when we release the active one. So basically we have an ordered list of keys that are
 * currently down, where the top of that list is the one active.
 */

typedef struct GameState {
	Sokolevel* lvl;            // current level
	DirKey     dirkeys[4];     // for key press response, dirkey[0] is the active key

	// move/push anim stuff:
	bool       is_animating;   // true when in process of animating a move
	float      anim_progress;  // progess of animation (0.0 .. 1.0)
	GridPos    old_worker_pos; // from pos of worker (to pos is in lvl->workerPos)
	bool       push_animition; // true when animating box push
	// box anim
	bool       animate_boxes;    // a setting. when true, all boxes animate annoyingly ;)
	int        blk_anim_time;    // time in current cycle [ms]
	int        blk_anim_cyctime; // cycle time of blk anim [ms]
} GameState;

void init_game_state(GameState* state, Sokolevel* lvl);
void game_update(GameState* state, int dt_ms);

#endif
