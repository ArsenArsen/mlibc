#ifndef __BITS_GLIBC_GLIBC_STDIO_H
#define __BITS_GLIBC_GLIBC_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* tmpnam_r() is a nonstandard extension that is also available on a few other systems. */
char *tmpnam_r(char *buffer);

#ifdef __cplusplus
}
#endif

#endif /* __BITS_GLIBC_GLIBC_STDIO_H */
