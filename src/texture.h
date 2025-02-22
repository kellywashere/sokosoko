#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <SDL3/SDL.h>

typedef struct Texture {
	SDL_Texture* texture;
	int          width;
	int          height;
} Texture;

Texture* create_texture(SDL_Renderer* renderer, char* imgName);
void     destroy_texture(Texture* texture);

#endif
