#pragma once
#include "Utility.h"
namespace math
{

	JINMATH_FORCE_INLINE float Pythagorean(float a, float b)
	{
		return std::sqrt(a * a + b * b);
	}
	
	JINMATH_FORCE_INLINE float Pythagorean(float a, float b, float c)
	{
		return std::sqrt(a * a + b * b + c * c);
	}
}