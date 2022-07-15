#include <mlibc/random.hpp>
#include <mlibc/internal-sysdeps.hpp>
#include <bits/ensure.h>
#include <frg/random.hpp>

#include <string.h>

namespace {
uint64_t rng_seed() {
	static auto rng_seed = [] () {
		__ensure(mlibc::sys_getentropy && "RNG-reliant functions require getentropy");

		char v[sizeof(uint64_t)];
		__ensure(mlibc::sys_getentropy(&v, sizeof(v)) == 0 && "Failed to generate seed");

		/* TODO: fall back on RDRAND or the platform equivalent */

		uint64_t r;
		memcpy(&r, v, sizeof(r));
		return r;
	}();
	return rng_seed;
}

auto thread_counter() {
	static uint64_t counter = 0;
	return __atomic_fetch_add(&counter, 1, __ATOMIC_ACQ_REL);
}

auto &generator() {
	static thread_local frg::pcg_basic32 pcg {
		rng_seed(),
		thread_counter()
	};
	return pcg;
}
}

namespace mlibc {
uint32_t random() {
	return generator()();
}

uint32_t random(uint32_t bound) {
	return generator()(bound);
}
}
