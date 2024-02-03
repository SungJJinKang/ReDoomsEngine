#pragma once

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