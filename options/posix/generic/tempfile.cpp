#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>

#include <mlibc/mktemp.hpp>
#include <bits/ensure.h>

namespace {
bool is_dir(const char *path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISDIR(st.st_mode);
}
}

int mkstemp(char *template_) {
	return mlibc::mkostemps(template_, 0, 0);
}

char *tempnam(const char *dir, const char *pfx) {
	using mlibc::placeholder_len;
	using mlibc::placeholder_val;
	using mlibc::tmpnam_r_ish;
	const char *apt_dir = secure_getenv("TMPDIR");

	if (pfx == NULL) {
		pfx = "temp";
	}

	size_t pfxlen = strlen(pfx);
	if (pfxlen > 5) {
		/* for some reason, this variable is limited to five
		 * characters:
		 *
		 * This argument may be a null pointer or point to a string of
		 * up to five bytes to be used as the beginning of the
		 * filename.
		 *   - POSIX.1‐2017 tempnam(3p)
		 */
		pfxlen = 5;
	}

	/* directory order:
	 * 1. try TMPDIR, if it doesn't exist/isn't a dir, then
	 * 2. try dir, with the same check, then
	 * 3. fall back to P_tmpdir (/tmp) without any ensures
	 */

	if (apt_dir != NULL && !is_dir(apt_dir)) {
		apt_dir = NULL;
	}
	if (apt_dir == NULL && is_dir(dir)) {
		apt_dir = dir;
	}
	if (apt_dir == NULL) {
		apt_dir = P_tmpdir;
	}

	size_t bufsz = pfxlen + strlen(apt_dir) + placeholder_len + 2;
	auto path_buffer = static_cast<char *>(calloc(bufsz + 1, 1));
	if (!path_buffer) {
		errno = ENOMEM;
		return NULL;
	}

	auto rs = snprintf(path_buffer, bufsz,
		"%s/%.5s%s", apt_dir, pfx, placeholder_val
	);

	/* should not be possible */
	__ensure(rs > 0 && static_cast<size_t>(rs) <= bufsz);

	auto placeholder_start = path_buffer + rs - 6;
	auto ret = tmpnam_r_ish(path_buffer, placeholder_start);

	if (ret) {
		__ensure(ret == path_buffer);
		return ret;
	}

	free(path_buffer);
	return NULL;
}

char *mkdtemp(char *pattern) {
	using mlibc::placeholder_len;
	using mlibc::placeholder_val;

	size_t tllen = strlen(pattern);
	if (tllen < placeholder_len) {
		errno = EINVAL;
		return NULL;
	}

	char *placeholder = pattern + tllen - (placeholder_len);
	if (memcmp(placeholder, placeholder_val, placeholder_len) != 0) {
		errno = EINVAL;
		return NULL;
	}

	for (int attempts = 0; attempts < 100; attempts++) {
		mlibc::random_name(placeholder);

		if (mkdir(pattern, S_IRWXU) == 0) {
			return pattern;
		}

		if (errno && errno != EEXIST) {
			break;
		}
	}
	
	return NULL;
}
