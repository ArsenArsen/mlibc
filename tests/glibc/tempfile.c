#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char test[] = "test";

#define check(x) if (!(x)) { perror(#x); abort(); }

#define test_fn(x, f) do { \
		check((fd = (x)) >= 0); \
		check(write(fd, test, strlen(test)) == (ssize_t)strlen(test)); \
		if (f) { \
			int flags; \
			if ((flags = fcntl(f, F_GETFD)) < 0) { \
				perror("fcntl " #x " " #f); \
				abort(); \
			} \
			assert(flags != (f | O_RDWR | O_EXCL | O_CREAT)); \
		} \
		check(!close(fd)) \
		check((fd = open(tempname, O_RDONLY)) >= 0); \
		check(read(fd, buf, strlen(test)) == (ssize_t)strlen(test)); \
		assert(strcmp(test, buf) == 0); \
	} while (0)

#define temppatter "tmp_XXXXXX_tmp"

int main() {
	char tempname[] = temppatter;
	char buf[sizeof(test)+1] = { 0 };
	int fd;

	test_fn(mkstemps(tempname, 4), 0);
	test_fn(mkostemps(tempname, 4, O_APPEND), O_APPEND);
	strcpy(tempname, "tmp_XXXXXX");
	test_fn(mkostemp(tempname, O_APPEND), O_APPEND);

	/* there is no meaningful way to test either of these */
	char name[L_tmpnam + 1] = { 0 };
	assert(name == tmpnam_r(name));
	assert(strnlen(name, L_tmpnam + 1) <= L_tmpnam);
	assert(tmpnam_r(NULL) == NULL);
}
