#define __MLIBC_NEED_TMP_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/mktemp.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/random.hpp>
#include <mlibc/file-io.hpp>
#include <bits/ensure.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/access.h>
#include <mlibc/debug.hpp>

namespace {
constexpr auto zbase32_alphabet = "ybndrfg8ejkmcpqxot1uwisza345h769";
} /* namespace */

namespace mlibc {
void random_name(char *buf) {
	using mlibc::placeholder_len;
	uint32_t rand = mlibc::random(0xffffffff >> (32 - placeholder_len * 5));

	for (size_t i = 0; i < placeholder_len; i++) {
		buf[i] = zbase32_alphabet[rand & 0x1f];
		rand >>= 5;
	}
}

char *tmpnam_r_ish(char *s, char *placeholder_start) {
	if (!mlibc::sys_access) {
		mlibc::panicLogger()
			<< "mlibc::sys_access is required for bad temporary file functions\n"
			<< "preferably, fix your program"
			<< frg::endlog;
		__builtin_unreachable();
	}

	bool spin = false;
	if (!placeholder_start) {
		/* posix mandates that we're able to provide at least
		 * TMP_MAX files as result of this function, it also
		 * mandates that the return strings across runtime are
		 * entirely unique, but I have no interest in ensuring
		 * that; for this reason, just refuse to fail if we
		 * haven't generated TMP_MAX files yet
		 */
		static size_t tmp_cnt = 0;
		spin = __atomic_fetch_add(&tmp_cnt, 1, __ATOMIC_ACQ_REL) <= TMP_MAX;
		int len = snprintf(
			s, L_tmpnam + 1, "%s/tmpnam_%s", P_tmpdir, placeholder_val
		);
		__ensure(len <= L_tmpnam);
		placeholder_start = s + len - placeholder_len;
	}

	__ensure(strncmp(placeholder_start, placeholder_val, placeholder_len) == 0);
	for (int attempts = 0; spin || attempts < 100; spin || attempts++) {
		random_name(placeholder_start);

		if (mlibc::sys_access(s, F_OK) == 0) {
			continue;
		}

		return s;
	}
	return NULL;
}

int mkostemps(char *template_, int suffixlen, int flags) {
	using mlibc::placeholder_len;
	using mlibc::placeholder_val;
	using mlibc::sys_open;

	if (flags & ~(O_APPEND | O_CLOEXEC | O_SYNC)) {
		errno = EINVAL;
		return -1;
	}

	flags |= O_RDWR | O_CREAT | O_EXCL;

	size_t tllen = strlen(template_);
	if (tllen < placeholder_len + suffixlen) {
		errno = EINVAL;
		return -1;
	}

	char *placeholder = template_ + tllen - (placeholder_len + suffixlen);
	if (memcmp(placeholder, placeholder_val, placeholder_len) != 0) {
		errno = EINVAL;
		return -1;
	}

	int fd;
	for (int attempts = 0; attempts < 100; attempts++) {
		random_name(placeholder);

		errno = sys_open(template_, flags, S_IRUSR | S_IWUSR, &fd);
		if (!errno) {
			return fd;
		}

		if (errno != EEXIST) {
			break;
		}
	}

	return -1;
}
} /* namespace mlibc */

FILE *tmpfile() {
	char t[] = P_tmpdir "/tmp_XXXXXX";
	int fd = mlibc::mkostemps(t, 0, 0);
	if (fd < 0) {
		fprintf(stderr, "Failed to open tmpfile %s: %d\n", t, errno);
		return NULL;
	}
	return frg::construct<mlibc::fd_file>(getAllocator(), fd,
			[] (mlibc::abstract_file *abstract) { frg::destruct(getAllocator(), abstract); });
}

/* the only difference between tmpnam_r and tmpnam is that the latter can race
 * with other threads (except we use a thread local buffer)
 */
char *tmpnam(char *s) {
	static thread_local char buf[L_tmpnam + 1];
	if (!s) {
		s = buf;
	}
	return mlibc::tmpnam_r_ish(s, nullptr);
}
