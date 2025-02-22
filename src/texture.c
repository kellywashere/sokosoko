#include <stdio.h>
#include <stdlib.h>
#include <SDL3_image/SDL_image.h>
#include "texture.h"

Texture* create_texture(SDL_Renderer* renderer, char* imgName) {
	// load png
	SDL_Surface* surf = IMG_Load(imgName);
	if (!surf) {
		fprintf(stderr, "Could not load bitmap image: %s\n", SDL_GetError());
		return NULL;
	}

	// convert to texture
	int w = surf->w;
	int h = surf->h;
	SDL_Texture* SDLtexture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_DestroySurface(surf);
	if( !SDLtexture ) {
		fprintf(stderr, "Unable to create bitmap texture! SDL Error: %s\n", SDL_GetError());
		return NULL;
	}

	Texture* texture = malloc(sizeof(Texture));

	if (texture) {
		texture->texture = SDLtexture;
		texture->width   = w;
		texture->height  = h;
	}
	return texture;
}

void destroy_texture(Texture* texture) {
	if (texture) {
		SDL_DestroyTexture(texture->texture);
	}
	free(texture);
}
