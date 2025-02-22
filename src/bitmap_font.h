#ifndef __BITMAPFONT_H__
#define __BITMAPFONT_H__

#include "texture.h"
#include <SDL3/SDL.h>

#define TEXT_LEFT   0 /* x-coordinate is left */
#define TEXT_CENTER 1 /* x-coordinate is center */
#define TEXT_RIGHT  2 /* x-coordinate is right */
#define TEXT_TOP    0 /* y-coordinate is top */
#define TEXT_MID    4 /* y-coordinate is middle */
#define TEXT_BOT    8 /* y-coordinate is bottom */

typedef struct BitmapFont {
	Texture*     texture;
	int          charWidth;
	int          charHeight;
	int          sheetCols;    // width sheet in chars
	int          sheetRows;    // heigth sheet in chars
	int          map[256];     // mapping char to idx
} BitmapFont;

BitmapFont* create_bitmap_font(SDL_Renderer* renderer, char* imgName,
		int charW, int charH, char* glyphStr);
void destroy_bitmap_font(BitmapFont* bitmapFont);

int render_text(SDL_Renderer* renderer, BitmapFont* bmf,
		int x, int y, char* text); // returns text width in pixels
int render_text_flags(SDL_Renderer* renderer, BitmapFont* bmf,
		int x, int y, char* text, unsigned int flags); // ret text w

#endif
