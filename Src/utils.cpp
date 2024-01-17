#include "utils.hpp"


// Pseudo Random Number Generator
pcg_random_t PCG_STRUCT;

uint32_t pcg32_random_r() {
    uint64_t oldstate = PCG_STRUCT.state;
    PCG_STRUCT.state = oldstate * 6364136223846793005ULL + (PCG_STRUCT.inc|1);
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


Vec3 randVec3onSphere(Vec3 normal) {
	Vec3 v = randBiVec3();
	if ( glm::dot(v, normal)<0 ) {
		return -v;
	}
	return v;
}
