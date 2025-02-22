#include <stdio.h>
#include <ctype.h>
#include "sokorender.h"

int init_renderer(RenderData* renderData, bool vsync) {
	renderData->renderer = NULL;
	renderData->renderer = SDL_CreateRenderer(renderData->window, NULL);
	if (renderData->renderer == NULL) {
		fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return 1;
	}
	if (vsync && !SDL_SetRenderVSync(renderData->renderer, 1)) {
		fprintf(stderr, "Could not set VSync on renderer! SDL Error: %s\n", SDL_GetError() );
		return 2;
	}
	return 0;
}

void destroy_renderer(RenderData* renderData) {
	SDL_DestroyRenderer(renderData->renderer);
}

int wallLUT[] = {5, 4, 5, 4, 1, 0, 1, 2}; // LUT for wall rendering
static void render_level_wall(RenderData* renderData, Sokolevel* lvl, GridPos pos,
		SDL_FRect dstRect, Texture* skin) {
	/* We have four corners in each wall section: TL, TR, BL, BR
	 * For each corner we look at 3 relevant neighboring cells: Horizontal, Diagonal, Vertical
	 * we encode like: H bit 2, D bit 1, V bit 0
	 * We use LUT to convert to single number, which relates to source cell to be used for rendering
	 */
	int skin_size = skin->width/4; // w,h of skin cells

	SDL_FRect srcRect = {0, 0, (float)skin_size/2, (float)skin_size/2}; // corner: w,h are size/2
	dstRect.w /= 2;  // corner: w,h are size/2
	dstRect.h /= 2;  // corner: w,h are size/2
	// Top-left
	int lutidx =
		(level_get_grid(lvl, pos.row    , pos.col - 1) == WALL ? 4 : 0) +
		(level_get_grid(lvl, pos.row - 1, pos.col - 1) == WALL ? 2 : 0) +
		(level_get_grid(lvl, pos.row - 1, pos.col    ) == WALL ? 1 : 0);
	srcRect.x = (wallLUT[lutidx] % 4) * skin_size;
	srcRect.y = ((wallLUT[lutidx] / 4) + 2) * skin_size;
	SDL_RenderTexture(renderData->renderer, skin->texture, &srcRect, &dstRect);
	// Top-right
	lutidx =
		(level_get_grid(lvl, pos.row    , pos.col + 1) == WALL ? 4 : 0) +
		(level_get_grid(lvl, pos.row - 1, pos.col + 1) == WALL ? 2 : 0) +
		(level_get_grid(lvl, pos.row - 1, pos.col    ) == WALL ? 1 : 0);
	srcRect.x = (wallLUT[lutidx] % 4) * skin_size + 0.5f*skin_size;
	srcRect.y = ((wallLUT[lutidx] / 4) + 2) * skin_size;
	dstRect.x += dstRect.w;
	SDL_RenderTexture(renderData->renderer, skin->texture, &srcRect, &dstRect);
	// Bottom-right
	lutidx =
		(level_get_grid(lvl, pos.row    , pos.col + 1) == WALL ? 4 : 0) +
		(level_get_grid(lvl, pos.row + 1, pos.col + 1) == WALL ? 2 : 0) +
		(level_get_grid(lvl, pos.row + 1, pos.col    ) == WALL ? 1 : 0);
	srcRect.x = (wallLUT[lutidx] % 4) * skin_size + 0.5f*skin_size;
	srcRect.y = ((wallLUT[lutidx] / 4) + 2) * skin_size + 0.5f*skin_size;
	dstRect.y += dstRect.h;
	SDL_RenderTexture(renderData->renderer, skin->texture, &srcRect, &dstRect);
	// Bottom-left
	lutidx =
		(level_get_grid(lvl, pos.row    , pos.col - 1) == WALL ? 4 : 0) +
		(level_get_grid(lvl, pos.row + 1, pos.col - 1) == WALL ? 2 : 0) +
		(level_get_grid(lvl, pos.row + 1, pos.col    ) == WALL ? 1 : 0);
	srcRect.x = (wallLUT[lutidx] % 4) * skin_size;
	srcRect.y = ((wallLUT[lutidx] / 4) + 2) * skin_size + 0.5f*skin_size;
	dstRect.x -= dstRect.w;
	SDL_RenderTexture(renderData->renderer, skin->texture, &srcRect, &dstRect);
}


void render_level(RenderData* renderData, GameState* state, Texture* skin,
		SDL_FRect* boundRect, bool pow2scaling) {
	Sokolevel* lvl = state->lvl;
	// figure out zoom-out factor
	int skin_size = skin->width/4; // w,h of skin cells
	int lvl_w_px;  // level width in px
	int lvl_h_px;  // level height in px
	int dest_size; // will contain cell size on destination
	if (pow2scaling) {
 		dest_size = skin_size; 
		lvl_w_px = lvl->width * skin_size;
		lvl_h_px = lvl->height * skin_size;
		while (lvl_w_px > boundRect->w || lvl_h_px > boundRect->h) {
			lvl_w_px /= 2;
			lvl_h_px /= 2;
			dest_size /= 2;
		}
	}
	else {
		int dest_size_w = ((boundRect->w / lvl->width) / 2) * 2; // shld be even
		int dest_size_h = ((boundRect->h / lvl->height) / 2) * 2; // shld be even
		dest_size = dest_size_w < dest_size_h ? dest_size_w : dest_size_h;
		lvl_w_px = lvl->width * dest_size;
		lvl_h_px = lvl->height * dest_size;
	}

	int offsx = boundRect->x + (boundRect->w - lvl_w_px)/2;
	int offsy = boundRect->y + (boundRect->h - lvl_h_px)/2;

	// render level
	SDL_FRect srcRect = {0, 0, skin_size, skin_size};
	SDL_FRect dstRect = {offsx, offsy, dest_size, dest_size};
	GridPos pos;
	for (pos.row = 0; pos.row < lvl->height; ++pos.row) {
		dstRect.x = offsx;
		for (pos.col = 0; pos.col < lvl->width; ++pos.col) {
			gridVal g = level_get_grid(lvl, pos.row, pos.col);

			srcRect.x = 0;
			srcRect.y = 0;
			if (g == WALL) { // TODO: use full wall texture info
				render_level_wall(renderData, lvl, pos, dstRect, skin);
			}
			else if ((g & WORKER) == WORKER) {
				char mc = tolower(level_last_move(lvl));
				switch (mc) {
					case 'u':
						srcRect.x = 0 * skin_size;
						srcRect.y = 4 * skin_size;
						break;
					case 'l':
						srcRect.x = 1 * skin_size;
						srcRect.y = 4 * skin_size;
						break;
					case 'd':
						srcRect.x = 2 * skin_size;
						srcRect.y = 4 * skin_size;
						break;
					case 'r':
						srcRect.x = 3 * skin_size;
						srcRect.y = 4 * skin_size;
						break;
					default:
						srcRect.x = 1 * skin_size;
						srcRect.y = 0 * skin_size;
						break;
				}
			}
			else if ((g & BOX) == BOX) {
				if (state->animate_boxes) {
					int phase = 4 * state->blk_anim_time / state->blk_anim_cyctime;
					srcRect.x = phase * skin_size;
					srcRect.y = 6 * skin_size;
				}
				else {
					srcRect.x = 2 * skin_size;
					srcRect.y = 0;
				}
			}
			if ((g & TARGET) == TARGET) {
				srcRect.y += skin_size;
			}
			if (g != OUTSIDE && g != WALL) {
				SDL_RenderTexture(renderData->renderer, skin->texture, &srcRect, &dstRect);
			}
			dstRect.x += dest_size;
		}
		dstRect.y += dest_size;
	}
}

