#include <sched.h>
#include <assert.h>

int main() {
	/* XXX: this test does not actually check whether sched_setscheduler
	 * returns the right value due to Linux not supporting that
	 */

	struct sched_param param = {
		.sched_priority = 0,
	};
	assert(sched_setscheduler(0, SCHED_OTHER, &param) >= 0);
	assert(sched_getscheduler(0) == SCHED_OTHER);
}
