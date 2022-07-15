#ifndef __INTERNAL_POSIX_INCLUDE_MLIBC_MKTEMP_HPP
#define __INTERNAL_POSIX_INCLUDE_MLIBC_MKTEMP_HPP

#include <bits/size_t.h>

namespace [[gnu::visibility("hidden")]] mlibc {
constexpr char placeholder_val[] = "XXXXXX";
constexpr size_t placeholder_len = sizeof(placeholder_val) - 1;
/**
 * generates a random placeholder_len-character string and places it into *buf.
 * does absolutely no verification
 */
void random_name(char *buf);

/**
 * common logic between tmpnam{,_r} and tempnam, extracted
 *
 * @param s The full filename buffer
 * @param placeholder_start Where the XXXXXX sequence starts, if set, will
 *                          prevent tmpnam_r_ish from writing over *s and
 *                          searching for the placeholder
 */
char *tmpnam_r_ish(char *s, char *placeholder_start);
int mkostemps(char *template_, int suffixlen, int flags);
} /* namespace mlibc */

#endif /* __INTERNAL_POSIX_INCLUDE_MLIBC_MKTEMP_HPP */
