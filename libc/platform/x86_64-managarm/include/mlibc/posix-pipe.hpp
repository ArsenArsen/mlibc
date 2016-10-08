
#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

// FIXME: required for hel.h
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/initializer.hpp>

#include <hel.h>
#include <hel-syscalls.h>
#include <helx.hpp>

extern frigg::LazyInitializer<helx::EventHub> eventHub;
extern frigg::LazyInitializer<helx::Pipe> posixPipe;
extern frigg::LazyInitializer<helx::Pipe> fsPipe;

int64_t allocPosixRequest();

#endif // MLIBC_POSIX_PIPE

