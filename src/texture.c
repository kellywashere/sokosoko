#include "texture.h"
#include <SDL2/SDL_image.h>

Texture* create_texture(SDL_Renderer* renderer, char* imgName) {
	int flags = IMG_INIT_PNG | IMG_INIT_JPG;
	if ( !(IMG_Init(flags) & flags ) ) {
		fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n",
				IMG_GetError() );
		return NULL;
	}

	// load png
	SDL_Surface* surf = IMG_Load(imgName);
	if (!surf) {
		fprintf(stderr, "Could not load bitmap image: %s\n", IMG_GetError());
		return NULL;
	}

	// convert to texture
	int w = surf->w;
	int h = surf->h;
	SDL_Texture* SDLtexture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
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
