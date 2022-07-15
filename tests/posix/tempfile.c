#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char test[] = "test";

#define check(x) if (!(x)) { perror(#x); abort(); }

int main() {
	char tempname[] = "tempXXXXXX";
	char buf[sizeof(test)+1] = { 0 };

	int fd;
	check((fd = mkstemp(tempname)) >= 0);
	check(write(fd, test, strlen(test)) == (ssize_t)strlen(test));
	check(!close(fd))
	check((fd = open(tempname, O_RDONLY)) >= 0);
	check(read(fd, buf, strlen(test)) == (ssize_t)strlen(test));
	assert(strcmp(test, buf) == 0);

	/* superficial tests */
	unsetenv("TMPDIR");
	char *fname = tempnam(".", "abc");
	assert(strncmp(fname, "./abc", 5) == 0);
	free(fname);

	putenv("TMPDIR=./.");
	fname = tempnam(".", "abc");
	assert(strncmp(fname, "././abc", 7) == 0);
	free(fname);

	/* there is no meaningful way to test either of these */
	char name[L_tmpnam + 1] = { 0 };
	assert(name == tmpnam(name));
	assert(strlen(name) <= L_tmpnam);
	assert(strlen(tmpnam(NULL)) <= L_tmpnam);
}
