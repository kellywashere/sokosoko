#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include "levelsetparser.h"
#include "texture.h"
#include "sokorender.h"
#include "sokogame.h"
#include "bitmap_font.h"

static int init_window(RenderData* renderData, bool fullscreen) {
	renderData->window   = NULL;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return 1;
	}

	renderData->window = SDL_CreateWindow("SDL Test", 1024, 768,
			(fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
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

	errcode = init_window(&renderData, true);
	if (errcode) {
		return errcode;
	}

	errcode = init_renderer(&renderData, vsync);
	if (errcode) {
		destroy_window(&renderData);
		return errcode;
	}

	// Texture* skin = create_texture(renderData.renderer, "assets/skins/BoxWorldIndigo.png");
	Texture* skin = create_texture(renderData.renderer, "assets/skins/HomeMade3.png");
	// Texture* skin = create_texture(renderData.renderer, "assets/skins/Pacman.png");
	char* glyphStr = " !\"  % '() +,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	BitmapFont* font = create_bitmap_font(renderData.renderer,
			"assets/fonts/kromagrad_16x16.png", 16, 16, glyphStr);

	Uint32 now_ticks = SDL_GetTicks();
	GameState state;
	init_game_state(&state, lvl);
	bool quit = false;
	while (!quit) {
		/* Event handling */
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_EVENT_KEY_DOWN) {
				switch (e.key.key) {
					case SDLK_UP:
					case SDLK_K:
						level_move_up(lvl);
						break;
					case SDLK_DOWN:
					case SDLK_J:
						level_move_down(lvl);
						break;
					case SDLK_LEFT:
					case SDLK_H:
						level_move_left(lvl);
						break;
					case SDLK_RIGHT:
					case SDLK_L:
						level_move_right(lvl);
						break;
					case SDLK_U:
						level_undo_last_push(lvl);
						break;
					case SDLK_N:
						levelset_next(levelset);
						lvl = levelset_cur(levelset);
						init_game_state(&state, lvl);
						break;
					case SDLK_P:
						levelset_prev(levelset);
						lvl = levelset_cur(levelset);
						init_game_state(&state, lvl);
						break;
					case SDLK_ESCAPE:
					case SDLK_Q:
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
			levelset_next(levelset);
			lvl = levelset_cur(levelset);
			init_game_state(&state, lvl);
		}

		/* Rendering */
		int win_w, win_h;
		SDL_GetWindowSize(renderData.window, &win_w, &win_h);

		SDL_RenderClear(renderData.renderer);

		SDL_FRect renderRect = {8,40, win_w - 16, win_h - 32 - 16};
		render_level(&renderData, &state, skin, &renderRect, false);

		char buf[80];
		sprintf(buf, "MOVES: %d", level_get_nrMoves(lvl));
		int w = render_text(renderData.renderer, font, 16, 8, buf);
		sprintf(buf, "PUSHES: %d", level_get_nrPushes(lvl));
		w += render_text(renderData.renderer, font, 64 + w, 8, buf);
		sprintf(buf, "LEVEL: %d", levelset_get_levelnr(levelset));
		render_text(renderData.renderer, font, win_w - strlen(buf) * font->charWidth - 8, 8, buf);

		SDL_RenderPresent(renderData.renderer);
	}

	destroy_bitmap_font(font);
	destroy_texture(skin);
	destroy_renderer(&renderData);
	destroy_window(&renderData);

	destroy_levelset(levelset);
	return 0;
}
