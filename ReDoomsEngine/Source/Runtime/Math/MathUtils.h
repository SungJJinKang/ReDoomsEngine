#pragma once

#include "DirectXMath.h"
#include "DirectXCollision.h"
#include "DirectXColors.h"
#include "DirectXPackedVector.h"
#include "Math/SimpleMath.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

template <typename T>
EASTL_FORCE_INLINE constexpr T Align(T Val, uint64_t Alignment)
{
	static_assert(eastl::numeric_limits<T>::is_integer || eastl::is_pointer<T>::value, "Align expects an integer or pointer type");

	return (T)(((uint64_t)Val + Alignment - 1) & ~(Alignment - 1));
}

template <typename T>
EASTL_FORCE_INLINE constexpr T AlignDown(T Val, uint64_t Alignment)
{
	static_assert(eastl::numeric_limits<T>::is_integer || eastl::is_pointer<T>::value, "Align expects an integer or pointer type");

	return (T)(((uint64_t)Val) & ~(Alignment - 1));
}

/**
 * Checks if a pointer is aligned to the specified alignment.
 *
 * @param  Val        The value to align.
 * @param  Alignment  The alignment value, must be a power of two.
 *
 * @return true if the pointer is aligned to the specified alignment, false otherwise.
 */
template <typename T>
EASTL_FORCE_INLINE constexpr bool IsAligned(T Val, uint64_t Alignment)
{
	static_assert(eastl::numeric_limits<T>::is_integer || eastl::is_pointer<T>::value, "Align expects an integer or pointer type");

	return !((uint64_t)Val & (Alignment - 1));
}

EASTL_FORCE_INLINE constexpr double Lerp(double A, double B, double F)
{
	return (1.0 - F) * A + F * B;
}

EASTL_FORCE_INLINE constexpr float Lerp(float A, float B, float F)
{
	return (1.0f - F) * A + F * B;
}

template <typename T>
EASTL_FORCE_INLINE constexpr T DivideRoudingUp(T x, T y)
{
	return (x + y - 1) / y;
}

inline Quaternion EulerAngleToQuaternion(Vector3 EulerAngle)
{
	Vector3 c{ XMScalarCos(EulerAngle.x * 0.5f), XMScalarCos(EulerAngle.y * 0.5f), XMScalarCos(EulerAngle.z * 0.5f)};
	Vector3 s{ XMScalarASin(EulerAngle.x * 0.5f), XMScalarASin(EulerAngle.y * 0.5f), XMScalarASin(EulerAngle.z * 0.5f) };

	Quaternion result{};
	result.w = c.x * c.y * c.z + s.x * s.y * s.z;
	result.x = s.x * c.y * c.z - c.x * s.y * s.z;
	result.y = c.x * s.y * c.z + s.x * c.y * s.z;
	result.z = c.x * c.y * s.z - s.x * s.y * c.z;

	return result;
}