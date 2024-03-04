#pragma once
// references :
// https://stackoverflow.com/questions/66743623/what-is-difference-between-m128a-and-m128a?noredirect=1#comment117984269_66743623
// https://stackoverflow.com/questions/52112605/is-reinterpret-casting-between-hardware-simd-vector-pointer-and-the-correspond
// https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
// Unreal Engine Source Code  
//

#include "SIMD.h"

#include <immintrin.h>

#if defined(__GNUC__)  || defined( __clang__)

union JINMATH_UNION_M128F {
	__m128 raw;    // SSE 4 x float vector
	float m128_f32[4];  // scalar array of 4 floats

	JINMATH_FORCE_INLINE operator __m128() { return raw; }
	JINMATH_FORCE_INLINE operator __m128() const { return raw; }
	JINMATH_FORCE_INLINE operator __m128* () { return &raw; }
	JINMATH_FORCE_INLINE operator const __m128* () const { return &raw; }
	JINMATH_FORCE_INLINE __m128* operator& () { return &raw; }
	JINMATH_FORCE_INLINE const __m128* operator& () const { return &raw; }

	JINMATH_FORCE_INLINE JINMATH_UNION_M128F() {}
	JINMATH_FORCE_INLINE JINMATH_UNION_M128F(const __m128& _raw) : raw(_raw) {}
};

using JINMATH_M128F = JINMATH_UNION_M128F;

union JINMATH_UNION_M128D {
	__m128d raw;    // SSE 4 x float vector
	double m128_d32[2];  // scalar array of 4 floats

	JINMATH_FORCE_INLINE operator __m128d() { return raw; }
	JINMATH_FORCE_INLINE operator __m128d() const { return raw; }
	JINMATH_FORCE_INLINE operator __m128d* () { return &raw; }
	JINMATH_FORCE_INLINE operator const __m128d* () const { return &raw; }
	JINMATH_FORCE_INLINE __m128d* operator& () { return &raw; }
	JINMATH_FORCE_INLINE const __m128d* operator& () const { return &raw; }

	JINMATH_FORCE_INLINE JINMATH_UNION_M128D() {}
	JINMATH_FORCE_INLINE JINMATH_UNION_M128D(const __m128d& _raw) : raw(_raw) {}
};

using JINMATH_M128D = JINMATH_UNION_M128D;

union JINMATH_UNION_M128I {
	__m128i raw;    // SSE 4 x float vector
	int32_t m128_i32[4];  // scalar array of 4 floats

	JINMATH_FORCE_INLINE operator __m128i() { return raw; }
	JINMATH_FORCE_INLINE operator __m128i() const { return raw; }
	JINMATH_FORCE_INLINE operator __m128i* () { return &raw; }
	JINMATH_FORCE_INLINE operator const __m128i* () const { return &raw; }
	JINMATH_FORCE_INLINE __m128i* operator& () { return &raw; }
	JINMATH_FORCE_INLINE const __m128i* operator& () const { return &raw; }

	JINMATH_FORCE_INLINE JINMATH_UNION_M128I() {}
	JINMATH_FORCE_INLINE JINMATH_UNION_M128I(const __m128i& _raw) : raw(_raw) {}
};

using JINMATH_M128I = JINMATH_UNION_M128I;

union JINMATH_UNION_M256F {
	__m256 raw;    // SSE 4 x float vector
	float m128_f32[8];  // scalar array of 4 floats

	JINMATH_FORCE_INLINE operator __m256() { return raw; }
	JINMATH_FORCE_INLINE operator __m256() const { return raw; }
	JINMATH_FORCE_INLINE operator __m256* () { return &raw; }
	JINMATH_FORCE_INLINE operator const __m256* () const { return &raw; }
	JINMATH_FORCE_INLINE __m256* operator& () { return &raw; }
	JINMATH_FORCE_INLINE const __m256* operator& () const { return &raw; }

	JINMATH_FORCE_INLINE JINMATH_UNION_M256F() {}
	JINMATH_FORCE_INLINE JINMATH_UNION_M256F(const __m256& _raw) : raw(_raw) {}
};

using JINMATH_M256F = JINMATH_UNION_M256F;

union JINMATH_UNION_M256D {
	__m256d raw;    // SSE 4 x float vector
	double m128_d32[4];  // scalar array of 4 floats

	JINMATH_FORCE_INLINE operator __m256d() { return raw; }
	JINMATH_FORCE_INLINE operator __m256d() const { return raw; }
	JINMATH_FORCE_INLINE operator __m256d* () { return &raw; }
	JINMATH_FORCE_INLINE operator const __m256d* () const { return &raw; }
	JINMATH_FORCE_INLINE __m256d* operator& () { return &raw; }
	JINMATH_FORCE_INLINE const __m256d* operator& () const { return &raw; }

	JINMATH_FORCE_INLINE JINMATH_UNION_M256D() {}
	JINMATH_FORCE_INLINE JINMATH_UNION_M256D(const __m256d& _raw) : raw(_raw) {}
};

using JINMATH_M256D = JINMATH_UNION_M256D;

union JINMATH_UNION_M256I {
	__m256i raw;    // SSE 4 x float vector
	int32_t m128_i32[8];  // scalar array of 4 floats

	JINMATH_FORCE_INLINE operator __m256i() { return raw; }
	JINMATH_FORCE_INLINE operator __m256i() const { return raw; }
	JINMATH_FORCE_INLINE operator __m256i* () { return &raw; }
	JINMATH_FORCE_INLINE operator const __m256i* () const { return &raw; }
	JINMATH_FORCE_INLINE __m256i* operator& () { return &raw; }
	JINMATH_FORCE_INLINE const __m256i* operator& () const { return &raw; }

	JINMATH_FORCE_INLINE JINMATH_UNION_M256I() {}
	JINMATH_FORCE_INLINE JINMATH_UNION_M256I(const __m256i& _raw) : raw(_raw) {}
	};

using JINMATH_M256I = JINMATH_UNION_M256I;


#elif defined(_MSC_VER)

using JINMATH_M128F = __m128;
using JINMATH_M128D = __m128d;
using JINMATH_M128I = __m128i;



using JINMATH_M256F = __m256;
using JINMATH_M256D = __m256d;
using JINMATH_M256I = __m256i;

#endif



/*
#ifndef JINMATH_M128F
#define JINMATH_M128F(VECTOR4FLOAT) *reinterpret_cast<JINMATH_M128F*>(&VECTOR4FLOAT)
#endif

#ifndef JINMATH_M128D
#define JINMATH_M128D(VECTOR4DOUBLE) *reinterpret_cast<JINMATH_M128D*>(&VECTOR4DOUBLE)
#endif

#ifndef JINMATH_M128I
#define JINMATH_M128I(VECTOR4INT) *reinterpret_cast<JINMATH_M128I*>(&VECTOR4INT)
#endif
*/

//https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=69,124,3928,5197&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX&text=_mm_shuffle_ps

#ifndef JINMATH_SHUFFLEMASK
#define JINMATH_SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )
#endif

#ifndef JINMATH_M128F_REPLICATE
#define JINMATH_M128F_REPLICATE(_M128F, ElementIndex) _mm_permute_ps(_M128F, JINMATH_SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 
#endif

#ifndef JINMATH_M128F_SWIZZLE
#define JINMATH_M128F_SWIZZLE(_M128F, X, Y, Z, W) _mm_permute_ps(_M128F, JINMATH_SHUFFLEMASK(X, Y, Z, W)) 
#endif

#ifndef JINMATH_M256F_REPLICATE
#define JINMATH_M256F_REPLICATE(_M256F, ElementIndex) _mm256_permute_ps(_M256F, JINMATH_SHUFFLEMASK(ElementIndex, ElementIndex, ElementIndex, ElementIndex)) 
#endif

#ifndef JINMATH_M256F_SWIZZLE
#define JINMATH_M256F_SWIZZLE(_M256F, X, Y, Z, W) _mm256_permute_ps(_M256F, JINMATH_SHUFFLEMASK(X, Y, Z, W)) 
#endif

extern const JINMATH_M128F M128F_Zero;
extern const JINMATH_M128F M128F_HALF_ONE;
extern const JINMATH_M128F M128F_EVERY_BITS_ONE;

JINMATH_FORCE_INLINE JINMATH_M128F M128F_ADD(const JINMATH_M128F& M128_A, const JINMATH_M128F& M128_B)
{
	return _mm_add_ps(M128_A, M128_B);
}

JINMATH_FORCE_INLINE JINMATH_M256F M256F_ADD(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B)
{
	return _mm256_add_ps(M256_A, M256_B);
}

JINMATH_FORCE_INLINE JINMATH_M128F M128F_SUB(const JINMATH_M128F& M128_A, const JINMATH_M128F& M128_B)
{
	return _mm_sub_ps(M128_A, M128_B);
}

JINMATH_FORCE_INLINE JINMATH_M256F M256F_SUB(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B)
{
	return _mm256_sub_ps(M256_A, M256_B);
}

JINMATH_FORCE_INLINE JINMATH_M128F M128F_MUL(const JINMATH_M128F& M128_A, const JINMATH_M128F& M128_B)
{
	return _mm_mul_ps(M128_A, M128_B);
}

JINMATH_FORCE_INLINE JINMATH_M256F M256F_MUL(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B)
{
	return _mm256_mul_ps(M256_A, M256_B);
}

JINMATH_FORCE_INLINE JINMATH_M128F M128F_DIV(const JINMATH_M128F& M128_A, const JINMATH_M128F& M128_B)
{
	return _mm_div_ps(M128_A, M128_B);
}

JINMATH_FORCE_INLINE JINMATH_M256F M256F_DIV(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B)
{
	return _mm256_div_ps(M256_A, M256_B);
}

JINMATH_FORCE_INLINE JINMATH_M128F M128F_MUL_AND_ADD(const JINMATH_M128F& M128_A, const JINMATH_M128F& M128_B, const JINMATH_M128F& M128_C)
{
	return M128F_ADD(M128F_MUL(M128_A, M128_B), M128_C);
}

JINMATH_FORCE_INLINE JINMATH_M256F M256F_MUL_AND_ADD(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B, const JINMATH_M256F& M256_C)
{
	return _mm256_fmadd_ps(M256_A, M256_B, M256_C);
}

/*
JINMATH_FORCE_INLINE JINMATH_M128F M128F_CROSS(const JINMATH_M128F& M128_A, const JINMATH_M128F& M128_B)
{
	JINMATH_M128F A_YZXW = _mm_shuffle_ps(M128_A, M128_A, JINMATH_SHUFFLEMASK(1, 2, 0, 3));
	JINMATH_M128F B_ZXYW = _mm_shuffle_ps(M128_B, M128_B, JINMATH_SHUFFLEMASK(2, 0, 1, 3));
	JINMATH_M128F A_ZXYW = _mm_shuffle_ps(M128_A, M128_A, JINMATH_SHUFFLEMASK(2, 0, 1, 3));
	JINMATH_M128F B_YZXW = _mm_shuffle_ps(M128_B, M128_B, JINMATH_SHUFFLEMASK(1, 2, 0, 3));
	return M128F_SUB(M128F_MUL(A_YZXW, B_ZXYW), M128F_MUL(A_ZXYW, B_YZXW));
}

JINMATH_FORCE_INLINE JINMATH_M256F M256F_CROSS(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B)
{
	JINMATH_M256F A_YZXW = _mm256_shuffle_ps(M256_A, M256_A, JINMATH_SHUFFLEMASK(1, 2, 0, 3));
	JINMATH_M256F B_ZXYW = _mm256_shuffle_ps(M256_B, M256_B, JINMATH_SHUFFLEMASK(2, 0, 1, 3));
	JINMATH_M256F A_ZXYW = _mm256_shuffle_ps(M256_A, M256_A, JINMATH_SHUFFLEMASK(2, 0, 1, 3));
	JINMATH_M256F B_YZXW = _mm256_shuffle_ps(M256_B, M256_B, JINMATH_SHUFFLEMASK(1, 2, 0, 3));
	return M256F_SUB(M256F_MUL(A_YZXW, B_ZXYW), M256F_MUL(A_ZXYW, B_YZXW));
}
*/


JINMATH_FORCE_INLINE JINMATH_M256F M256F_SELECT(const JINMATH_M256F& M256_A, const JINMATH_M256F& M256_B, const JINMATH_M256F& MASK)
{
	return _mm256_blendv_ps(M256_A, M256_B, MASK);
}

JINMATH_FORCE_INLINE void M256F_SWAP(JINMATH_M256F& M256_A, JINMATH_M256F& M256_B, const JINMATH_M256F& MASK)
{
	const JINMATH_M256F TEMP_A = M256_A;
	const JINMATH_M256F TEMP_B = M256_B;
	M256_A = M256F_SELECT(TEMP_A, TEMP_B, MASK);
	M256_B = M256F_SELECT(TEMP_B, TEMP_A, MASK);
}
