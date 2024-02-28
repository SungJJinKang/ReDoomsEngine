#include "SIMD_Core.h"

namespace culling
{
	extern const EVERYCULLING_M128F M128F_Zero{ _mm_castsi128_ps(_mm_set1_epi16(0)) };
	extern const EVERYCULLING_M128F M128F_HALF_ONE{ _mm_set1_ps(0.5f) };
	extern const EVERYCULLING_M128F M128F_EVERY_BITS_ONE{ _mm_castsi128_ps(_mm_set1_epi16(-1)) };
}