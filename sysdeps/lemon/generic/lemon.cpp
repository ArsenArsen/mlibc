#include <lemon/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <abi-bits/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>

namespace mlibc{
	int sys_futex_wait(int *pointer, int expected){
		return syscall(SYS_FUTEX_WAIT, pointer, expected, 0, 0, 0);
	}

	int sys_futex_wake(int *pointer) {
		return syscall(SYS_FUTEX_WAKE, pointer, 0, 0, 0, 0);
	}

	int sys_tcb_set(void* pointer){
		syscall(SYS_SET_FS_BASE, (uintptr_t)pointer, 0, 0, 0, 0);
		return 0;
	}

	int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
		__ensure(flags & MAP_ANONYMOUS);

		// Make sure to only map whole pages
		__ensure(!(size & 0xFFF));

		size_t sizePages = ((size + 0xFFF) & ~static_cast<size_t>(0xFFF)) >> 12;
		syscall(SYS_MMAP, (uintptr_t)window, sizePages, (uintptr_t)hint, 0, 0);

		if(!(*window))
			return -1;
		return 0;
	}

	int sys_vm_unmap(void* address, size_t size) {
		__ensure(!(size & 0xFFF));

		size_t sizePages = ((size + 0xFFF) & ~static_cast<size_t>(0xFFF)) >> 12;
		long ret = syscall(SYS_MUNMAP, (uintptr_t)address, sizePages, 0, 0, 0);

		return ret;
	}

	int sys_anon_allocate(size_t size, void **pointer) {
		// Make sure to only allocate whole pages
		__ensure(!(size & 0xFFF));
		syscall(SYS_ALLOC, ((size + 0xFFF) & ~static_cast<size_t>(0xFFF)) >> 12, (uintptr_t)pointer, 0, 0, 0);

		if (!(*pointer))
		    return -1;
		return 0;
	}

	int sys_anon_free(void *pointer, size_t size) {
		return 0; // Not implemented
	}

	void sys_libc_panic(){
		sys_libc_log("libc panic!");
		__builtin_trap();
		for(;;);
	}

	void sys_libc_log(const char* msg){
		syscall(0, (uintptr_t)msg, 0, 0, 0, 0);
	}

	#ifndef MLIBC_BUILDING_RTDL

	void sys_exit(int status){
		syscall(SYS_EXIT, status, 0, 0, 0, 0);
	}

	pid_t sys_getpid(){
		uint64_t _pid;
		syscall(SYS_GETPID, (uintptr_t)&_pid, 0, 0, 0, 0);

		pid_t pid = _pid;
		return pid;
	}
	
	int sys_clock_get(int clock, time_t *secs, long *nanos) {
		uint64_t _secs, _millis;
		syscall(SYS_UPTIME, (uintptr_t)&_secs, (uintptr_t)&_millis, 0, 0, 0);

		*secs = _secs;
		*nanos = _millis * 1000000;
		return 0;
	}

	int sys_getcwd(char *buffer, size_t size){
		return syscall(SYS_GET_CWD, buffer, size, 0, 0, 0);
	}

	int sys_chdir(const char *path){
		syscall(SYS_CHDIR, path, 0, 0, 0, 0);
		return 0;
	}

	int sys_sleep(time_t* sec, long* nanosec){
		syscall(SYS_NANO_SLEEP, (*sec) * 1000000000 + (*nanosec), 0, 0, 0, 0);
		return 0;
	}
	
	uid_t sys_getuid(){
		return syscall(SYS_GETUID, 0, 0, 0, 0, 0);
	}

	uid_t sys_geteuid(){
		return syscall(SYS_GETEUID, 0, 0, 0, 0, 0);
	}

	int sys_setuid(uid_t uid){
		return -syscall(SYS_SETUID, uid, 0, 0, 0, 0);
	}

	int sys_seteuid(uid_t euid){
		return -syscall(SYS_SETEUID, euid, 0, 0, 0, 0);
	}

	int sys_getgid(){
		mlibc::infoLogger() << "mlibc: sys_getgid is a stub" << frg::endlog;
		return 0;
	}

	int sys_getegid(){
		mlibc::infoLogger() << "mlibc: sys_getegid is a stub" << frg::endlog;
		return 0;
	}

	int sys_setgid(gid_t gid){
		mlibc::infoLogger() << "mlibc: sys_setgid is a stub" << frg::endlog;
		return 0;
	}

	int sys_setegid(gid_t egid){
		mlibc::infoLogger() << "mlibc: sys_setegid is a stub" << frg::endlog;
		return 0;
	}

	void sys_yield(){
		syscall(SYS_YIELD, 0, 0, 0, 0, 0);
	}

	int sys_clone(void *entry, void *user_arg, void *tcb, pid_t *tid_out){
		auto stack = prepare_stack(entry, user_arg, tcb);
		pid_t tid = syscall(SYS_SPAWN_THREAD, __mlibc_start_thread, stack, 0, 0, 0);

		if(tid_out){
			*tid_out = tid;
		}

		return 0;
	}

	void sys_thread_exit(){
		syscall(SYS_EXIT_THREAD, 0, 0, 0, 0, 0);
	}
	#endif
} 
