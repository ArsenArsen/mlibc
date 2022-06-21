#ifndef MLIBC_ABIS_LINUX_XATTR_H
#define MLIBC_ABIS_LINUX_XATTR_H

/* Linux headers collaborate with libc headers here not to redefine things,
 * though the definitions from the latter should be preferred. As such, we
 * defer to them if present, and provide a best effort (but near-impossible to
 * change) alternative otherwise.
 */

/* __USE_KERNEL_XATTR_DEFS is exported when XATTR_* are emitted, and
 * __UAPI_DEF_XATTR is used to determine the behaviour of that header, if it's
 * set to 1, the header exports these defines and __USE_KERNEL_XATTR_DEFS.
 * This applies for pretty much all other defines in libc-compat.h AFAICT.
 */
#if !defined(__USE_KERNEL_XATTR_DEFS) && __has_include(<linux/xattr.h>)
#	define __UAPI_DEF_XATTR 1
#	include <linux/xattr.h>
#else
/* We have this block to prevent depending on Linux headers at bootstrap time,
 * which could prove difficult, as installing those requires dealing with
 * kbuild. These values should not be subject to change any time soon
 */
enum {
	XATTR_CREATE = 1,
#define XATTR_CREATE XATTR_CREATE
	XATTR_REPLACE = 2,
#define XATTR_REPLACE XATTR_REPLACE
};
#	define __USE_KERNEL_XATTR_DEFS 0
#endif

#endif /* MLIBC_ABIS_LINUX_XATTR_H */
