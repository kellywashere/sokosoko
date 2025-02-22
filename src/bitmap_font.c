#include <SDL3/SDL.h>
#include <stdlib.h>

#include "texture.h"
#include "bitmap_font.h"

BitmapFont* create_bitmap_font(SDL_Renderer* renderer, char* imgName,
		int charW, int charH, char* glyphStr) {
	Texture* texture = create_texture(renderer, imgName);
	BitmapFont* bmf = NULL;
	if (texture) {
		bmf = malloc(sizeof(BitmapFont));
		if (bmf) {
			bmf->texture     = texture;
			bmf->charWidth   = charW;
			bmf->charHeight  = charH;
			bmf->sheetCols   = texture->width/charW;
			bmf->sheetRows   = texture->height/charH;

			// create char map
			for (int c = 0; c < 256; ++c) {
				bmf->map[c] = 0;
			}
			for (int ii = 0; glyphStr[ii] != 0; ++ii) {
				unsigned int mapIdx = (unsigned char)glyphStr[ii];
				if (bmf->map[mapIdx] == 0) {
					bmf->map[mapIdx] = ii;
				}
			}
		}
	}
	return bmf;
}

void destroy_bitmap_font(BitmapFont* bitmapFont){
	if (bitmapFont) {
		destroy_texture(bitmapFont->texture);
	}
	free(bitmapFont);
}

int render_text(SDL_Renderer* renderer, BitmapFont* bmf,
		int x, int y, char* text) {

	int text_width = 0;
	SDL_FRect dstRect = {x, y, bmf->charWidth, bmf->charHeight};
	SDL_FRect srcRect = {0, 0, bmf->charWidth, bmf->charHeight};

	for (int ii = 0; text[ii] != 0; ++ii) {
		char c = text[ii];
		int glyphIdx = bmf->map[ (unsigned char) c ];
		if (c == '\n') {
			dstRect.x = x;
			dstRect.y += bmf->charHeight;
		}
		else {
			if (c != ' ' && glyphIdx != 0) {
				int glyphRow = glyphIdx / bmf->sheetCols;
				int glyphCol = glyphIdx % bmf->sheetCols;
				if (glyphRow <= bmf->sheetRows) {
					srcRect.x = glyphCol * bmf->charWidth;
					srcRect.y = glyphRow * bmf->charHeight;
					SDL_RenderTexture(renderer, bmf->texture->texture, &srcRect, &dstRect);
				}
			}
			dstRect.x += bmf->charWidth;
			text_width += bmf->charWidth;
		}
	}
	return text_width;
}

int render_text_flags(SDL_Renderer* renderer, BitmapFont* bmf, int x, int y, char* text,
		unsigned int flags) {
	int text_width = 0;
	char line[200];
	int nrLines = 1;
	// some pre-work, to count nr of lines (needed for ypos)
	for (int ii = 0; text[ii] != 0; ++ii) {
		if (text[ii] == '\n') {
			++nrLines;
		}
	}
	if (flags & TEXT_BOT) {
		y -= nrLines * bmf->charHeight;

	}
	else if (flags & TEXT_MID) {
		y -= nrLines * bmf->charHeight / 2;
	}

	// start rendering
	int idxLine = 0;
	int idxTxt = 0;
	while (1) {
		char c = text[idxTxt++];
		if (c == '\n' || c == 0) {
			line[idxLine] = 0;
			int xpos = x;
			if (idxLine > 0) {
				int lineLen = idxLine - 1;
				if (flags & TEXT_RIGHT) {
					xpos = x - lineLen * bmf->charWidth;
				}
				else if (flags & TEXT_CENTER) {
					xpos = x - lineLen * bmf->charWidth / 2;
				}
				int tw = render_text(renderer, bmf, xpos, y, line);
				if (tw > text_width) {
					text_width = tw;
				}
			}
			if (c == 0) {
				break;
			}
			idxLine = 0;
			y += bmf->charHeight;
		}
		else {
			line[idxLine++] = c;
		}
	}
	return text_width;
}
