
#include <bits/ensure.h>
#include <sched.h>
#include <errno.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int sched_yield(void) {
	if(mlibc::sys_yield) {
		mlibc::sys_yield();
	}else{
		// Missing sched_yield() is not an error.
		MLIBC_MISSING_SYSDEP();
	}
	return 0;
}

int sched_getaffinity(pid_t, size_t, cpu_set_t *) {
	mlibc::infoLogger() << "\e[31mmlibc: sched_getaffinity() always fails\e[39m" << frg::endlog;
	return -1;
}

int sched_get_priority_max(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_get_priority_min(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int __mlibc_cpu_isset(int, cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int __mlibc_cpu_count(cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unshare(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clone(int (*)(void *), void *, int, void *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_getscheduler(pid_t pid) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sched_getscheduler, -1);
	int ret;
	if (int e = sysdep(pid, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sched_setscheduler, -1);
	int ret;
	if (int e = sysdep(pid, policy, param, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}
