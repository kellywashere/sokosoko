#ifndef __LEVELPARSER_H__
#define __LEVELPARSER_H__

#include "sokolevel.h"
#include "sokolevelset.h"
#include <libxml/parser.h>

Sokolevelset* parse_levelset_file(const char* fname);

#endif
