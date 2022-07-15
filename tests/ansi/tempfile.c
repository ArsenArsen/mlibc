#include <assert.h>
#include <stdio.h>
#include <string.h>

const char test[] = "Test!";

/* a bit of a superficial test, but it tests linkage and fd_file, so it's
 * alright
 */
int main() {
	FILE *f = tmpfile();
	char buf[sizeof(test) + 1] = { 0 };
	assert(f && "Failed to open file");

	assert(fwrite(test, 1, strlen(test), f) == strlen(test));
	assert(fflush(f) == 0);
	assert(fseek(f, 0, SEEK_SET) == 0);
	assert(fread(&buf, 1, strlen(test), f) == strlen(test));
	assert(strcmp(buf, test) == 0);

	/* there is no meaningful way to test either of these */
	char name[L_tmpnam + 1] = { 0 };
	assert(name == tmpnam(name));
	assert(strlen(name) <= L_tmpnam);
	assert(strlen(tmpnam(NULL)) <= L_tmpnam);
}
