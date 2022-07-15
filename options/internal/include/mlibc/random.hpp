#ifndef __MLIBC_OPTIONS_INTERNAL_INCLUDE_MLIBC_BEST_EFFORT_RANDOM_HPP
#define __MLIBC_OPTIONS_INTERNAL_INCLUDE_MLIBC_BEST_EFFORT_RANDOM_HPP

#include <stdint.h>

namespace [[gnu::visibility("hidden")]] mlibc {
/* thread-local, non-CS PRNG */
uint32_t random();
uint32_t random(uint32_t bound);
}

#endif /* __MLIBC_OPTIONS_INTERNAL_INCLUDE_MLIBC_BEST_EFFORT_RANDOM_HPP */
