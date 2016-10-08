
// for _Exit()
#include <stdlib.h>

#include <string.h>
#include <errno.h>

// for fork() and execve()
#include <unistd.h>
// for sched_yield()
#include <sched.h>
// for waitpid()
#include <sys/wait.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <frigg/debug.hpp>
#include <frigg/vector.hpp>
#include <frigg/string.hpp>
#include <frigg/protobuf.hpp>

#include <posix.frigg_pb.hpp>

// defined in entry.cpp
void __mlibc_reinitPosixPipe();

// defined in enter-fork.S
extern "C" pid_t __mlibc_enterFork();

extern "C" pid_t __mlibc_doFork(uintptr_t child_ip, uintptr_t child_sp) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::FORK);
	request.set_child_ip(child_ip);
	request.set_child_sp(child_sp);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	HelError error;
	posixPipe->sendStringReqSync(serialized.data(), serialized.size(),
			*eventHub, request_num, 0, error);
	HEL_CHECK(error);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	assert(response.error() == managarm::posix::Errors::SUCCESS);
	
	return response.pid();
}

extern "C" void __mlibc_fixForkedChild() {
	__mlibc_reinitPosixPipe();
}

uid_t getuid(void) {
	return 0;
}
gid_t getgid(void) {
	return 0;
}

uid_t geteuid(void) {
	return 0;
}
gid_t getegid(void) {
	return 0;
}

pid_t getpid(void) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::GET_PID);

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	HelError error;
	posixPipe->sendStringReqSync(serialized.data(), serialized.size(),
			*eventHub, request_num, 0, error);
	HEL_CHECK(error);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	assert(response.error() == managarm::posix::Errors::SUCCESS);
	return response.pid();
}
pid_t getppid(void) {
	frigg::infoLogger() << "mlibc: Broken getppid() called" << frigg::endLog;
	return 1;
}

pid_t fork(void) {
	return __mlibc_enterFork();
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	managarm::posix::ClientRequest<MemoryAllocator> request(*memoryAllocator);
	request.set_request_type(managarm::posix::ClientRequestType::EXEC);
	request.set_path(frigg::String<MemoryAllocator>(*memoryAllocator, path));

	int64_t request_num = allocPosixRequest();
	frigg::String<MemoryAllocator> serialized(*memoryAllocator);
	request.SerializeToString(&serialized);
	HelError error;
	posixPipe->sendStringReqSync(serialized.data(), serialized.size(),
			*eventHub, request_num, 0, error);
	HEL_CHECK(error);

	int8_t buffer[128];
	size_t length;
	HelError response_error;
	posixPipe->recvStringRespSync(buffer, 128, *eventHub, request_num, 0, response_error, length);
	HEL_CHECK(response_error);

	managarm::posix::ServerResponse<MemoryAllocator> response(*memoryAllocator);
	response.ParseFromArray(buffer, length);
	if(response.error() == managarm::posix::Errors::SUCCESS) {
		HEL_CHECK(helExitThisThread());
		__builtin_unreachable();
	}else{
		__ensure(!"Unexpected error in execve()!");
		__builtin_unreachable();
	}
}

pid_t waitpid(pid_t pid, int *status, int flags) {
	frigg::infoLogger() << "mlibc: Broken waitpid("
			<< pid << ", " << flags << ") called!" << frigg::endLog;
	__ensure(flags & WNOHANG);
	errno = ECHILD;
	return -1;
}

void _Exit(int status) {
	HEL_CHECK(helExitThisThread());
	__builtin_unreachable();
}

int sched_yield(void) {
	HEL_CHECK(helYield());
	return 0;
}

