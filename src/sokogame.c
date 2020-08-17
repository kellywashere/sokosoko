#include "sokogame.h"

void init_game_state(GameState* state, Sokolevel* lvl) {
	state->lvl = lvl;
	for (int ii = 0; ii < 4; ++ii) {
		state->dirkeys[ii] = 0;
	}
	state->is_animating = false;
	state->animate_boxes = true;
	state->blk_anim_time = 0;
	state->blk_anim_cyctime = 1000; // 1s
}

void game_update(GameState* state, int dt_ms) {
	state->blk_anim_time += dt_ms;
	if (state->blk_anim_time >= state->blk_anim_cyctime) {
		state->blk_anim_time -= state->blk_anim_cyctime;
	}
}



