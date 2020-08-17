#include <SDL2/SDL.h>
#include <stdio.h>
#include <ctype.h>
#include "levelsetparser.h"
#include "util.h"
#include "texture.h"
#include "sokorender.h"
#include "sokogame.h"

static int init_window(RenderData* renderData) {
	renderData->window   = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return 1;
	}

	renderData->window = SDL_CreateWindow("SDL Test",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			1024, 768,
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

int main(int argc, char* argv[]) {
	// parse_levelset_file("sokolevels/Thinking_Rabbit_Arranged.slc");
	char* fname = "assets/levels/Original.slc";
	if (argc > 1) {
		fname = argv[1];
	}

	Sokolevelset* levelset = parse_levelset_file(fname);

	// Print info
	printf("Title: %s\n", levelset->title);
	printf("Author: %s\n", levelset->author);
	printf("Description: %s\n", levelset->description);
	printf("nr Levels: %d\n", levelset_size(levelset));

	Sokolevel* lvl = levelset_first(levelset);

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

	// Texture* skin = create_texture(renderData.renderer, "assets/skins/BoxWorldIndigo.png");
	Texture* skin = create_texture(renderData.renderer, "assets/skins/Nightshift2.png");
	// Texture* skin = create_texture(renderData.renderer, "assets/skins/Pacman.png");

	Uint32 now_ticks = SDL_GetTicks();
	GameState state;
	init_game_state(&state, lvl);
	bool quit = false;
	while (!quit) {
		/* Event handling */
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

		/* Time passing */
		Uint32 t = SDL_GetTicks();
		Uint32 time_passed = t - now_ticks;
		now_ticks = t;
		// it would seem rounding errors in time_passed stack up,
		// but that is not true (do the math ;) )

		game_update(&state, time_passed);

		if (is_level_won(lvl)) {
			lvl = levelset_next(levelset);
		}

		/* Rendering */
		SDL_RenderClear(renderData.renderer);
		render_level(&renderData, &state, skin);
		SDL_RenderPresent(renderData.renderer);
	}

	destroy_texture(skin);
	destroy_renderer(&renderData);
	destroy_window(&renderData);

	destroy_levelset(levelset);
	return 0;
}
