// getch()
// found on http://stackoverflow.com/a/7469410/1579481
#include <termios.h>
#include <stdio.h>

static struct termios old, newp;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  newp = old; /* make new settings same as old settings */
  newp.c_lflag &= ~ICANON; /* disable buffered i/o */
  newp.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &newp); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void)
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}

#include <stdio.h>
#include "levelsetparser.h"

#include "util.h"

int main(int argc, char* argv[]) {
	// parse_levelset_file("sokolevels/Thinking_Rabbit_Arranged.slc");
	char* fname = "test.slc";
	if (argc > 1) {
		fname = argv[1];
	}

	Sokolevelset* levelset = parse_levelset_file(fname);

	// Print info
	printf("Title: %s\n", levelset->title);
	printf("Author: %s\n", levelset->author);
	printf("Description: %s\n", levelset->description);
	printf("nr Levels: %d\n", levelset_size(levelset));

	// Print levels
	Sokolevel* lvl = levelset_first(levelset);
	while (lvl) {
		print_level(lvl);
		lvl = levelset_next(levelset);
	}

	lvl = levelset_first(levelset);
	bool done = false;
	do {
		print_level(lvl);
		char ch = getch();
		switch (ch) {
			case 'h':
				level_move_left(lvl);
				break;
			case 'j':
				level_move_down(lvl);
				break;
			case 'k':
				level_move_up(lvl);
				break;
			case 'l':
				level_move_right(lvl);
				break;
			case 'u':
				level_undo_move(lvl);
				break;
			case 'q':
				done = true;
				break;
			default:
				break;
		}
	} while (!done);

	destroy_levelset(levelset);

	return 0;
}
