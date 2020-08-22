#include "util.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int strcmpi(const char* str1, const char* str2) {
	for (int ii = 0; ; ++ii) {
		int d = tolower(str1[ii]) - tolower(str2[ii]);
		if (d != 0) {
			return d;
		}
		// str1[ii] == str2[ii]
		if (str1[ii] == 0) {
			return 0;
		}
	}
}



