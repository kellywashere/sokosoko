#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "util.h"
#include "levelsetparser.h"
#include "sokolevelset.h"

#define MAX_STR_LEN 2048

// See also: http://www.jamesh.id.au/articles/libxml-sax/libxml-sax.html

// TODO: remove debug test
// TODO: UNKNOWN state

enum Sax_state {
	INIT,
	ROOT,
	SET_TITLE,
	SET_DESCRIPTION,
	COLLECTION,
	LEVEL,
	LEVEL_ROW,
	UNKNOWN
};
typedef enum Sax_state Sax_state;

typedef struct Sax_FSM_data {
	Sax_state     state;                             // state
	char*         str;                               // string collected by characters fn
	unsigned int  str_len;                           // lenght of current character str
	Sokolevelset* levelset;                          // level set
	Sokolevel*    level;                             // level being processed
	int           level_row;                         // row of level to be read from xml
} Sax_FSM_data;

static Sax_FSM_data* create_fsm_data() {
	Sax_FSM_data* data = malloc(sizeof(Sax_FSM_data));
	if (data) {
		data->state = INIT;
		data->str = malloc(MAX_STR_LEN);
		data->str_len = 0;
		data->levelset = create_levelset();
		data->level = NULL;
	}
	return data;
}

static Sokolevelset* destroy_fsm_data(Sax_FSM_data* data) {
	// NOTE: does not destroy levelset (of course) but rather returns it
	Sokolevelset* levelset = NULL;
	if (data) {
		free(data->str);
		levelset = data->levelset;
	}
	free(data);
	return levelset;
}

static void sax_endDocument(void* user_data) {
	printf("end doc\n");
}

// helper fn to find attribute with certain name
static const xmlChar* saxhelper_getAttr(const xmlChar** attrs, const char* name) {
	if (attrs) {
		while ((*attrs) && (*(attrs + 1))) {
			if (strcmpi((char*)*attrs, name) == 0) {
				return *(attrs + 1);
			}
			attrs += 2;
		}
	}
	return NULL;
}

static void sax_startElement(void* user_data, const xmlChar* name, const xmlChar** attrs) {
	Sax_FSM_data* data = user_data;
	data->str_len = 0;
	Sax_state current_state = data->state;

	switch (data->state) {
		case INIT:
			data->state = ROOT;
			break;
		case ROOT:
			if (strcmpi((char*)name, "Title") == 0) {
				data->state = SET_TITLE;
			}
			else if (strcmpi((char*)name, "Description") == 0) {
				data->state = SET_DESCRIPTION;
			}
			else if (strcmpi((char*)name, "LevelCollection") == 0) {
				data->state = COLLECTION;
				const xmlChar* author = saxhelper_getAttr(attrs, "Copyright");
				if (author) {
					strncpy(data->levelset->author, (char*)author, MAX_AUTHOR_LEN - 1);
					data->levelset->author[MAX_AUTHOR_LEN] = '\0';
				}
			}
			break;
		case COLLECTION:
			if (strcmpi((char*)name, "Level") == 0) {
				data->state = LEVEL;
				const xmlChar* title = saxhelper_getAttr(attrs, "Id");
				const xmlChar* author = saxhelper_getAttr(attrs, "Copyright");
				const xmlChar* width_str = saxhelper_getAttr(attrs, "Width");
				const xmlChar* height_str = saxhelper_getAttr(attrs, "Height");
				if (width_str && height_str) {
					int width = atoi((char*)width_str);
					int height = atoi((char*)height_str);
					data->level = create_level(width, height);
					data->level_row = 0;
					if (title) {
						strncpy(data->level->title, (char*)title, MAX_TITLE_LEN);
					}
					if (author) {
						strncpy(data->level->author, (char*)author, MAX_AUTHOR_LEN);
					}
				}
			}
		case LEVEL:
			if (strcmpi((char*)name, "L") == 0) {
				data->state = LEVEL_ROW;
			}
		default:
			break;
	}
	if (current_state == data->state) {
		printf("Unexpected element start: %s\n", (char*)name);
	}
}

void sax_characters(void* user_data, const xmlChar* ch, int len) {
	Sax_FSM_data* data = user_data;
	int l = MIN(MAX_STR_LEN - data->str_len, len);
	strncpy(data->str + data->str_len, (char*)ch, l);
	data->str_len += l;
}

static void sax_endElement(void* user_data, const xmlChar* name) {
	Sax_FSM_data* data = user_data;
	Sax_state current_state = data->state;

	int l;
	switch (data->state) {
		case SET_TITLE:
			l = MIN(data->str_len, MAX_TITLE_LEN);
			strncpy(data->levelset->title, data->str, l);
			data->levelset->title[l] = '\0';
			data->state = ROOT;
			break;
		case SET_DESCRIPTION:
			l = MIN(data->str_len, MAX_DESCRIPTION_LEN);
			strncpy(data->levelset->description, data->str, l);
			data->levelset->description[l] = '\0';
			data->state = ROOT;
			break;
		case COLLECTION:
			data->state = ROOT;
			break;
		case LEVEL:
			data->state = COLLECTION;
			if (check_level(data->level)) {
				levelset_add_level(data->levelset, data->level);
				printf("adding level\n");
			}
			break;
		case LEVEL_ROW:
			data->state = LEVEL;
			for (int c = 0; c < data->str_len; ++c) {
				gridVal v = OUTSIDE;
				switch (data->str[c]) {
					case '#':
						v = WALL;
						break;
					case '@':
						v = WORKER;
						break;
					case '+':
						v = WORKER_ON_TARGET;
						break;
					case '$':
						v = BOX;
						break;
					case '*':
						v = BOX_ON_TARGET;
						break;
					case '.':
						v = TARGET;
						break;
					default:
						v = OUTSIDE;
						break;
				}
				level_set_grid(data->level, data->level_row, c, v);
			}
			++data->level_row;
			break;
		default:
			break;
	}
}

static xmlEntityPtr sax_getEntity(void *user_data, const xmlChar *name) {
	Sax_FSM_data* data = user_data;
	printf("get entity: %s\n", name);
	return xmlGetPredefinedEntity(name);
}

Sokolevelset* parse_levelset_file(const char* fname) {
	xmlSAXHandler handler;
	Sax_FSM_data* fsm_data;

	fsm_data = create_fsm_data();

	memset(&handler, 0, sizeof(handler));
	handler.endDocument = sax_endDocument;
	handler.startElement = sax_startElement;
	handler.endElement = sax_endElement;
	handler.getEntity = sax_getEntity;
	handler.characters = sax_characters;

	int errcode = xmlSAXUserParseFile(&handler, fsm_data, fname);
	Sokolevelset* levelset = fsm_data->levelset;

	if (errcode) {
		fprintf(stderr, "Error parsing %s (errcode %d)\n", fname, errcode);
		destroy_levelset(destroy_fsm_data(fsm_data));
		return NULL;
	}

	destroy_fsm_data(fsm_data);
	return fsm_data->levelset;
}

