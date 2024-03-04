#pragma once
#include "JINMATHCore.h"

#include "Utility.h"
#include "SIMD_Core.h"

namespace math
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Quaternion;
}

namespace math
{
	/// <summary>
	/// This class is made for SIMD computation. So it's aligned to 16 byte
	/// </summary>
	struct alignas(16) Vector4
	{
		using value_type = typename float;
		using type = typename Vector4;

		union { float x, r; };
		union { float y, g; };
		union { float z, b; };
		union { float w, a; };

		JINMATH_FORCE_INLINE float* data() noexcept
		{
			return &x;
		}

		const JINMATH_FORCE_INLINE float* data() const noexcept
		{
			return &x;
		}

		static const Vector4 forward;
		static const Vector4 right;
		static const Vector4 up;
		static const Vector4 zero;
		static const Vector4 one;

		JINMATH_FORCE_INLINE Vector4() {}
		JINMATH_FORCE_INLINE explicit Vector4(float xValue)  noexcept
			: x{ xValue }, y{ xValue }, z{ xValue }, w{ xValue }
		{
		}

		// This is more slow than scalar version
// 		JINMATH_FORCE_INLINE Vector(float xValue)  noexcept
// 		{
// 			*reinterpret_cast<JINMATH_M128F*>(this) = _mm_set1_ps(xValue);
// 		}

		JINMATH_FORCE_INLINE Vector4(float xValue, float yValue, float zValue, float wValue) noexcept
			: x{ xValue }, y{ yValue }, z{ zValue }, w{ wValue }
		{
		}


		explicit Vector4(const Vector2& vector) noexcept;

		/// <summary>
		/// When passed parameter have same value Vector4
		/// </summary>
		/// <param name="vector"></param>
		/// <returns></returns>
		explicit Vector4(const Vector3& vector, const float _w = 0.0f) noexcept;

		JINMATH_FORCE_INLINE Vector4(const Vector4& vector) noexcept
			: x{ static_cast<float>(vector.x) }, y{ static_cast<float>(vector.y) }, z{ static_cast<float>(vector.z) }, w{ static_cast<float>(vector.w) }
		{
		}

		JINMATH_FORCE_INLINE Vector4(const __m128& m128f) noexcept
			: x{ m128f.m128_f32[0] }, y{ m128f.m128_f32[1] }, z{ m128f.m128_f32[2] }, w{ m128f.m128_f32[3] }
		{
		}

		Vector4(const Quaternion& rot) noexcept;
		
		JINMATH_FORCE_INLINE Vector4& operator=(value_type xValue) noexcept
		{
			x = xValue;
			y = xValue;
			z = xValue;
			w = xValue;
			return *this;
		}


		Vector4& operator=(const Vector2& vector) noexcept;

		Vector4& operator=(const Vector3& vector) noexcept;
		Vector4& operator=(const Quaternion& rot) noexcept;

		JINMATH_FORCE_INLINE Vector4& operator=(const Vector4& vector) noexcept
		{
			x = vector.x;
			y = vector.y;
			z = vector.z;
			w = vector.w;
			return *this;
		}


		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE static size_t componentCount() noexcept { return 4; }

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE value_type& operator[](size_t i)
		{
			assert(i >= 0 || i < componentCount());
			switch (i)
			{
			case 0:
				return x;
				break;
			case 1:
				return y;
				break;
			case 2:
				return z;
				break;
			case 3:
				return w;
				break;
			default:
				__assume(0);
			}
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE const value_type& operator[](size_t i) const
		{
			assert(i >= 0 || i < componentCount());
			switch (i)
			{
			case 0:
				return x;
				break;
			case 1:
				return y;
				break;
			case 2:
				return z;
				break;
			case 3:
				return w;
				break;
			default:
				__assume(0);
			}
		}


		
		
		JINMATH_FORCE_INLINE operator Vector4()
		{
			Vector4 result{};
			*reinterpret_cast<JINMATH_M128F*>(result.data()) = *reinterpret_cast<const JINMATH_M128F*>(data());

			return result;
		}


		/// <summary>
		/// scalar version is much faster than SIMD version
		/// </summary>
		/// <typeparam name="X"></typeparam>
		/// <typeparam name="enable_if_t"></typeparam>
		/// <param name="rhs"></param>
		/// <returns></returns>

		JINMATH_FORCE_INLINE Vector4 operator+(const Vector4& rhs) const noexcept
		{
			Vector4 Result{};
			*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_add_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return Result;
		}

		/*

		JINMATH_FORCE_INLINE Vector4 operator+(const Vector4& rhs) const noexcept
		{
			return Vector4(M128F_ADD(*reinterpret_cast<const JINMATH_M128F*>(this), *reinterpret_cast<const JINMATH_M128F*>(&rhs)));
		}
		*/


		JINMATH_FORCE_INLINE Vector4 operator-(const Vector4& rhs) const noexcept
		{
			Vector4 Result{};
			*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_sub_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return Result;
		}


		JINMATH_FORCE_INLINE Vector4 operator*(const Vector4& rhs) const noexcept
		{
			Vector4 Result{};
			*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_mul_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));
			 
			return Result;
		}


		JINMATH_FORCE_INLINE Vector4 operator/(const Vector4& rhs) const noexcept
		{
			Vector4 Result{};
			*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_div_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return Result;
		}


		JINMATH_FORCE_INLINE Vector4 operator%(const Vector4& rhs) const noexcept
		{
			return Vector4(JINMATH_MODULO(float, x, rhs.x), JINMATH_MODULO(float, y, rhs.y), JINMATH_MODULO(float, z, rhs.z), JINMATH_MODULO(float, w, rhs.w));
		}


		JINMATH_FORCE_INLINE Vector4& operator+=(const Vector4& rhs) noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_add_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return *this;
		}


		JINMATH_FORCE_INLINE Vector4& operator-=(const Vector4& rhs) noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_sub_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return *this;
		}


		JINMATH_FORCE_INLINE Vector4& operator*=(const Vector4& rhs) noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_mul_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return *this;
		}


		JINMATH_FORCE_INLINE Vector4& operator/=(const Vector4& rhs)
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_div_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

			return *this;
		}


		JINMATH_FORCE_INLINE Vector4& operator%=(const Vector4& rhs)
		{
			x = JINMATH_MODULO(float, x, rhs.x);
			y = JINMATH_MODULO(float, y, rhs.y);
			z = JINMATH_MODULO(float, z, rhs.z);
			w = JINMATH_MODULO(float, w, rhs.w);
			return *this;
		}

		//

		JINMATH_FORCE_INLINE Vector4& operator+=(float scalar) noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_add_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), _mm_set1_ps(scalar));

			return *this;
		}

		JINMATH_FORCE_INLINE Vector4& operator-=(float scalar) noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_sub_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), _mm_set1_ps(scalar));

			return *this;
		}

		JINMATH_FORCE_INLINE Vector4& operator*=(float scalar) noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_mul_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), _mm_set1_ps(scalar));

			return *this;
		}

		JINMATH_FORCE_INLINE Vector4& operator/=(float scalar)
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_div_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), _mm_set1_ps(scalar));

			return *this;
		}

		JINMATH_FORCE_INLINE Vector4& operator%=(float scalar)
		{
			x = JINMATH_MODULO(float, x, scalar);
			y = JINMATH_MODULO(float, y, scalar);
			z = JINMATH_MODULO(float, z, scalar);
			w = JINMATH_MODULO(float, w, scalar);
			return *this;
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector4& operator++() noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_add_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), _mm_set1_ps(1.0f));

			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector4 operator++(int32_t) noexcept
		{
			Vector4 Vector{ *this };
			++* this;
			return Vector4{ Vector };
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector4& operator--() noexcept
		{
			*reinterpret_cast<JINMATH_M128F*>(data()) = _mm_add_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), _mm_set1_ps(-1.0f));

			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector4 operator--(int32_t) noexcept
		{
			Vector4 Vector{ *this };
			--* this;
			return Vector4{ Vector };
		}

		// scalar version is more fast than SIMD
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float sqrMagnitude() const noexcept
		{
			const JINMATH_M128F mul0 = _mm_mul_ps(*reinterpret_cast<const JINMATH_M128F*>(data()), *reinterpret_cast<const JINMATH_M128F*>(data()));
			const JINMATH_M128F had0 = _mm_hadd_ps(mul0, mul0);
			const JINMATH_M128F had1 = _mm_hadd_ps(had0, had0);

			return _mm_cvtss_f32(had1);
		}

		// scalar version is more fast than SIMD
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float magnitude() const noexcept
		{
			return std::sqrt(sqrMagnitude());
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 normalized() const noexcept
		{
			float mag = magnitude();
			if (std::isnan(mag))
				return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };

			Vector4 Result{ *this };
			Result /= mag;

			return Result;
		}

		JINMATH_FORCE_INLINE void Normalize()
		{
			Vector4 result = *this;
			result *= math::inverseSqrt(result.magnitude());
		}
	};


	

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 Max(const Vector4& vector1, const Vector4& vector2)
	{
		const JINMATH_M128F* m128f_vec1 = reinterpret_cast<const JINMATH_M128F*>(&vector1);
		const JINMATH_M128F* m128f_vec2 = reinterpret_cast<const JINMATH_M128F*>(&vector2);

		return Vector4(_mm_max_ps(*m128f_vec1, *m128f_vec2));
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 Min(const Vector4& vector1, const Vector4& vector2)
	{
		const JINMATH_M128F* m128f_vec1 = reinterpret_cast<const JINMATH_M128F*>(&vector1);
		const JINMATH_M128F* m128f_vec2 = reinterpret_cast<const JINMATH_M128F*>(&vector2);

		return Vector4(_mm_min_ps(*m128f_vec1, *m128f_vec2));
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator+(const Vector4& lhs, float scalar) noexcept
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_add_ps(*reinterpret_cast<const JINMATH_M128F*>(lhs.data()), _mm_set1_ps(scalar));

		return Result;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator-(const Vector4& lhs, float scalar) noexcept
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_sub_ps(*reinterpret_cast<const JINMATH_M128F*>(lhs.data()), _mm_set1_ps(scalar));

		return Result;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator*(const Vector4& lhs, float scalar) noexcept
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_mul_ps(*reinterpret_cast<const JINMATH_M128F*>(lhs.data()), _mm_set1_ps(scalar));

		return Result;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator/(const Vector4& lhs, float scalar)
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_div_ps(*reinterpret_cast<const JINMATH_M128F*>(lhs.data()), _mm_set1_ps(scalar));

		return Result;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator%(const Vector4& lhs, float scalar)
	{
		return Vector4{ JINMATH_MODULO(float, lhs.x, scalar), JINMATH_MODULO(float, lhs.y, scalar), JINMATH_MODULO(float, lhs.z, scalar), JINMATH_MODULO(float, lhs.w, scalar) };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Vector4& lhs, const Vector4& rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Vector4& lhs, const Vector4& rhs) noexcept
	{
		return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Vector4& lhs, float scalar)
	{
		return lhs.x == scalar && lhs.y == scalar && lhs.z == scalar && lhs.w == scalar;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Vector4& lhs, float scalar)
	{
		return lhs.x != scalar || lhs.y != scalar || lhs.z != scalar || lhs.w != scalar;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator+(float scalar, const Vector4& rhs) noexcept
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_add_ps(_mm_set1_ps(scalar) , *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

		return Result;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator-(float scalar, const Vector4& rhs) noexcept
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_sub_ps(_mm_set1_ps(scalar), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

		return Result;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator*(float scalar, const Vector4& rhs) noexcept
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_mul_ps(_mm_set1_ps(scalar), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

		return Result;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator/(float scalar, const Vector4& rhs)
	{
		Vector4 Result{};
		*reinterpret_cast<JINMATH_M128F*>(&Result) = _mm_div_ps(_mm_set1_ps(scalar), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));

		return Result;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 operator%(float scalar, const Vector4& rhs)
	{
		return Vector4{ JINMATH_MODULO(float, scalar, rhs.x), JINMATH_MODULO(float, scalar, rhs.y), JINMATH_MODULO(float, scalar, rhs.z), JINMATH_MODULO(float, scalar, rhs.w) };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(float scalar, const Vector4& rhs)
	{
		return scalar == rhs.x && scalar == rhs.y && scalar == rhs.z && scalar == rhs.w;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(float scalar, const Vector4& rhs)
	{
		return scalar != rhs.x || scalar != rhs.y || scalar != rhs.z || scalar != rhs.w;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 lerpUnClamped(const Vector4& start, const Vector4& end, float t) noexcept
	{
		return start + (end - start) * t;
	}
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 lerp(const Vector4& start, const Vector4& end, float t) noexcept
	{
		return lerpUnClamped(start, end, clamp01<float>(t));
	}

	JINMATH_FORCE_INLINE Vector4 operator+(const Vector4& vector) noexcept
	{
		return Vector4{ vector };
	}


	JINMATH_FORCE_INLINE Vector4 operator-(const Vector4& vector) noexcept
	{
		return _mm_or_ps(_mm_set1_ps(-0.0f), *reinterpret_cast<const JINMATH_M128F*>(vector.data()));
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float dot(const Vector4& lhs, const Vector4& rhs)
	{
		const JINMATH_M128F mul0 = _mm_mul_ps(*reinterpret_cast<const JINMATH_M128F*>(lhs.data()), *reinterpret_cast<const JINMATH_M128F*>(rhs.data()));
		const JINMATH_M128F had0 = _mm_hadd_ps(mul0, mul0);
		const JINMATH_M128F had1 = _mm_hadd_ps(had0, had0);
		
		return _mm_cvtss_f32(had1);
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 sqrt(const Vector4& vector)
	{
		const JINMATH_M128F* m128f_vec = reinterpret_cast<const JINMATH_M128F*>(&vector);
		return Vector4{ _mm_sqrt_ps(*m128f_vec) };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 inverseSqrt(const Vector4& vector)
	{
		const JINMATH_M128F* m128f_vec = reinterpret_cast<const JINMATH_M128F*>(&vector);
		return Vector4{ _mm_rsqrt_ps(*m128f_vec) };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 normalize(const Vector4& vector)
	{
		return vector * math::inverseSqrt(math::dot(vector, vector));
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 cos(const Vector4& vector)
	{
		return _mm_cos_ps(*reinterpret_cast<const JINMATH_M128F*>(vector.data()));
	}

	// AX1 don't have sin instriction

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 sin(const Vector4& vector)
	{
		return _mm_sin_ps(*reinterpret_cast<const JINMATH_M128F*>(vector.data()));
	}

	// AX1 don't have tan instriction

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector4 tan(const Vector4& vector)
	{
		return _mm_tan_ps(*reinterpret_cast<const JINMATH_M128F*>(vector.data()));
	}
	
	template<typename T>
	JINMATH_NO_DISCARD inline Vector4 slerp
	(
		const Vector4& x,
		const Vector4& y,
		T const& a
	)
	{
		// get cosine of angle between vectors (-1 -> 1)
		T CosAlpha = math::dot(x, y);
		// get angle (0 -> pi)
		T Alpha = math::acos(CosAlpha);
		// get sine of angle between vectors (0 -> 1)
		T SinAlpha = math::sin(Alpha);
		// this breaks down when SinAlpha = 0, i.e. Alpha = 0 or pi
		T t1 = math::sin((static_cast<T>(1) - a) * Alpha) / SinAlpha;
		T t2 = math::sin(a * Alpha) / SinAlpha;

		// interpolate src vectors
		return x * t1 + y * t2;
	}

}




