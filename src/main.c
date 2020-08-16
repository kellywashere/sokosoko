#include <SDL2/SDL.h>
#include <stdio.h>
#include <ctype.h>
#include "levelsetparser.h"
#include "util.h"
#include "texture.h"

typedef struct RenderData {
	SDL_Window*    window;
	SDL_Renderer*  renderer;
} RenderData;

static int init_window(RenderData* renderData) {
	renderData->window   = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return 1;
	}

	renderData->window = SDL_CreateWindow("SDL Test",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			SDL_WINDOW_SHOWN);
	if (renderData->window == NULL) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return 2;
	}
	return 0;
}

static void destroy_window(RenderData* renderData) {
	SDL_DestroyWindow(renderData->window);
	SDL_Quit();
}
static int init_renderer(RenderData* renderData, bool vsync) { renderData->renderer = NULL;
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	if (vsync)
		flags |= SDL_RENDERER_PRESENTVSYNC;
	renderData->renderer = SDL_CreateRenderer(renderData->window, -1, flags);
	if (renderData->renderer == NULL) {
		fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
		return 1;
	}
	return 0;
}

static void destroy_renderer(RenderData* renderData) {
	SDL_DestroyRenderer(renderData->renderer);
}

static void render_level(RenderData* renderData, Sokolevel* lvl, Texture* skin) {
	// figure out zoom-out factor
	int skin_size = skin->width/4; // w,h of skin cells
	int lvl_w_px = lvl->width * skin_size;
	int lvl_h_px = lvl->height * skin_size;
	int win_w, win_h;
	SDL_GetWindowSize(renderData->window, &win_w, &win_h);
	int dest_size = skin_size; // will contain cell size on destination
	while (lvl_w_px > win_w || lvl_h_px > win_h) {
		lvl_w_px /= 2;
		lvl_h_px /= 2;
		dest_size /= 2;
	}
	int offsx = (win_w - lvl_w_px)/2;
	int offsy = (win_h - lvl_h_px)/2;

	// render level
	SDL_Rect srcRect = {0, 0, skin_size, skin_size};
	SDL_Rect dstRect = {offsx, offsy, dest_size, dest_size};
	GridPos pos;
	for (pos.row = 0; pos.row < lvl->height; ++pos.row) {
		dstRect.x = offsx;
		for (pos.col = 0; pos.col < lvl->width; ++pos.col) {
			gridVal g = level_get_grid(lvl, pos);

			srcRect.x = 0;
			srcRect.y = 0;
			if (g == WALL) { // TODO: use full wall texture info
				srcRect.x = skin_size;
				srcRect.y = 3 * skin_size;
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
				srcRect.x = 2 * skin_size;
			}
			if ((g & TARGET) == TARGET) {
				srcRect.y += skin_size;
			}
			if (g != OUTSIDE) {
				SDL_RenderCopy(renderData->renderer, skin->texture, &srcRect, &dstRect);
			}
			dstRect.x += dest_size;
		}
		dstRect.y += dest_size;
	}
}

int main(int argc, char* argv[]) {
	// parse_levelset_file("sokolevels/Thinking_Rabbit_Arranged.slc");
	char* fname = "test.slc";
	if (argc > 1) {
		fname = argv[1];
	}

	Sokolevelset* levelset = parse_levelset_file(fname);

	// Print info
	printf("Title: %s\n", levelset->title);
	printf("Author: %s\n", levelset->author);
	printf("Description: %s\n", levelset->description);
	printf("nr Levels: %d\n", levelset_size(levelset));

	// Print levels
	Sokolevel* lvl = levelset_first(levelset);
	while (lvl) {
		print_level(lvl);
		lvl = levelset_next(levelset);
	}

	lvl = levelset_first(levelset);

	RenderData renderData;
	unsigned int errcode;
	bool vsync = true;

	errcode = init_window(&renderData);
	if (errcode) {
		return errcode;
	}

	errcode = init_renderer(&renderData, vsync);
	if (errcode) {
		destroy_window(&renderData);
		return errcode;
	}

	Texture* skin = create_texture(renderData.renderer, "assets/skins/BoxWorldIndigo.png");
	bool quit = false;
	while (!quit) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_UP:
					case SDLK_k:
						level_move_up(lvl);
						break;
					case SDLK_DOWN:
					case SDLK_j:
						level_move_down(lvl);
						break;
					case SDLK_LEFT:
					case SDLK_h:
						level_move_left(lvl);
						break;
					case SDLK_RIGHT:
					case SDLK_l:
						level_move_right(lvl);
						break;
					case SDLK_u:
						level_undo_last_push(lvl);
						break;
					case SDLK_ESCAPE:
					case SDLK_q:
						quit = true;
						break;
					default:
						break;
				}
			}
		}

		SDL_RenderClear(renderData.renderer);
		render_level(&renderData, lvl, skin);
		SDL_RenderPresent(renderData.renderer);
	}

	destroy_texture(skin);
	destroy_renderer(&renderData);
	destroy_window(&renderData);

	destroy_levelset(levelset);
	return 0;
}
