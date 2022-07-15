#include <stdlib.h>

#include <stddef.h>
#include <mlibc/mktemp.hpp>

int mkstemps(char *template_, int suffixlen) {
	return mlibc::mkostemps(template_, suffixlen, 0);
}

int mkostemp(char *template_, int flags) {
	return mlibc::mkostemps(template_, 0, flags);
}

int mkostemps(char *template_, int suffixlen, int flags) {
	return mlibc::mkostemps(template_, suffixlen, flags);
}

char *tmpnam_r(char *s) {
	if (!s) {
		return NULL;
	}
	return mlibc::tmpnam_r_ish(s, nullptr);
}
