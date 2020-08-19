#ifndef __SOKORENDER_H__
#define __SOKORENDER_H__

#include <SDL2/SDL.h>
#include "sokolevel.h"
#include "texture.h"
#include "sokogame.h"

typedef struct RenderData {
	SDL_Window*    window;
	SDL_Renderer*  renderer;
} RenderData;

int  init_renderer(RenderData* renderData, bool vsync);
void destroy_renderer(RenderData* renderData);

void render_level(RenderData* renderData, GameState* state, Texture* skin,
		SDL_Rect* boundRect, bool pow2scaling);

#endif
