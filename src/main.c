#include <stdio.h>
#include "levelsetparser.h"

#include "util.h"

int main(int argc, char* argv[]) {
//	parse_levelset_file("sokolevels/Thinking_Rabbit_Arranged.slc");
	char* fname = "test.slc";
	if (argc > 1) {
		fname = argv[1];
	}
	
	Sokolevelset* levelset = parse_levelset_file(fname);

	printf("Title: %s\n", levelset->title);
	printf("Author: %s\n", levelset->author);
	printf("Description: %s\n", levelset->description);
	printf("nr Levels: %d\n", levelset_size(levelset));

	Sokolevel* lvl = levelset_first(levelset);
	while (lvl) {
		print_level(lvl);
		lvl = levelset_next(levelset);
	}

	destroy_levelset(levelset);

    return 0;
}
