#ifndef __BITS_GLIBC_GLIBC_STDLIB_H
#define __BITS_GLIBC_GLIBC_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* mkstemps(): unstandardized, but appears on several other systems. */
int mkstemps(char *__template, int suffixlen);

/* mkostemp() and mkostemps(): are glibc extensions. */
int mkostemp(char *__template, int flags);
int mkostemps(char *__template, int suffixlen, int flags);

#ifdef __cplusplus
}
#endif

#endif /* __BITS_GLIBC_GLIBC_STDLIB_H */
