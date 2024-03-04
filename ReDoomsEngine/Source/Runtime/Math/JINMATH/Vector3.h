#pragma once
#include "JINMATHCore.h"

#include "Utility.h"

namespace math
{
	struct Vector2;
	struct Vector3;
	struct Vector4;

	// never put alignas
	struct Vector3
	{
		using value_type = float;
		using type = typename Vector3;

		union { float x, r; };
		union { float y, g; };
		union { float z, b; };

		JINMATH_FORCE_INLINE float* data() noexcept
		{
			return &x;
		}

		const JINMATH_FORCE_INLINE float* data() const noexcept
		{
			return &x;
		}

		static const Vector3 forward;
		static const Vector3 right;
		static const Vector3 up;

		JINMATH_FORCE_INLINE Vector3() {}
		JINMATH_FORCE_INLINE explicit Vector3(float xValue)  noexcept
			: x{ xValue }, y{ xValue }, z{ xValue }
		{
		}

		JINMATH_FORCE_INLINE Vector3(float xValue, float yValue, float zValue) noexcept
			: x{ xValue }, y{ yValue }, z{ zValue }
		{
		}
		
		
		explicit Vector3(const Vector2& vector) noexcept;

		JINMATH_FORCE_INLINE Vector3(const Vector3& vector) noexcept
			: x{ vector.x }, y{ vector.y }, z{ vector.z }
		{
		}
		
		explicit Vector3(const Vector4& vector) noexcept;

		JINMATH_FORCE_INLINE Vector3& operator=(value_type xValue) noexcept
		{
			x = xValue;
			y = xValue;
			z = xValue;
			return *this;
		}
		
		
		Vector3& operator=(const Vector2& vector) noexcept;

		JINMATH_FORCE_INLINE Vector3& operator=(const Vector3& vector) noexcept
		{
			x = vector.x;
			y = vector.y;
			z = vector.z;
			return *this;
		}
		
		Vector3& operator=(const Vector4& vector) noexcept;

		// 		Vector(const Vector3&) = default;
// 		Vector(Vector3&&) = default;
// 		Vector3& opreator=(const Vector3&) = default;
// 		Vector3& opreator=(Vector3&&) = default;


		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE static size_t componentCount() noexcept { return 3; }


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
			default:
				__assume(0);
			}
		}


		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float sqrMagnitude() const noexcept
		{
			return x * x + y * y + z * z;
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float magnitude() const noexcept
		{
			return static_cast<float>(std::sqrt(sqrMagnitude()));
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 normalized() const noexcept
		{
			float mag = magnitude();
			if (mag == 0.0f)
				return Vector3{0.0f, 0.0f, 0.0f};

			return Vector3{ static_cast<value_type>(x / mag), static_cast<value_type>(y / mag), static_cast<value_type>(z / mag) };
		}

		JINMATH_FORCE_INLINE void Normalize()
		{
			float mag = magnitude();
			if (mag > std::numeric_limits<float>::epsilon())
			{
				x = static_cast<value_type>(x / mag);
				y = static_cast<value_type>(y / mag);
				z = static_cast<value_type>(z / mag);
			}
		}

		
		JINMATH_FORCE_INLINE operator Vector3()
		{
			return Vector3{x, y, z};
		}
		
		JINMATH_FORCE_INLINE Vector3 operator+(const Vector3& rhs) const noexcept
		{
			return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		
		JINMATH_FORCE_INLINE Vector3 operator-(const Vector3& rhs) const noexcept
		{
			return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		
		JINMATH_FORCE_INLINE Vector3 operator*(const Vector3& rhs) const noexcept
		{
			return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
		}

		
		JINMATH_FORCE_INLINE Vector3 operator/(const Vector3& rhs) const noexcept
		{
			return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
		}

		
		JINMATH_FORCE_INLINE Vector3 operator%(const Vector3& rhs) const
		{
			return Vector3(JINMATH_MODULO(float, x, rhs.x), JINMATH_MODULO(float, y, rhs.y), JINMATH_MODULO(float, z, rhs.z));
		}

		
		JINMATH_FORCE_INLINE Vector3& operator+=(const Vector3& rhs) noexcept
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector3& operator-=(const Vector3& rhs) noexcept
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector3& operator*=(const Vector3& rhs) noexcept
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector3& operator/=(const Vector3& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector3& operator%=(const Vector3& rhs)
		{
			x = JINMATH_MODULO(float, x, rhs.x);
			y = JINMATH_MODULO(float, y, rhs.y);
			z = JINMATH_MODULO(float, z, rhs.z);
			return *this;
		}

		//

		JINMATH_FORCE_INLINE Vector3& operator+=(float scalar) noexcept
		{
			x += scalar;
			y += scalar;
			z += scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Vector3& operator-=(float scalar) noexcept
		{
			x -= scalar;
			y -= scalar;
			z -= scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Vector3& operator*=(float scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Vector3& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Vector3& operator%=(float scalar)
		{
			x = JINMATH_MODULO(float, x, scalar);
			y = JINMATH_MODULO(float, y, scalar);
			z = JINMATH_MODULO(float, z, scalar);
			return *this;
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector3& operator++() noexcept
		{
			++x;
			++y;
			++z;
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector3 operator++(int32_t) noexcept
		{
			Vector3 Vector{ *this };
			++* this;
			return Vector3{ Vector };
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector3& operator--() noexcept
		{
			--x;
			--y;
			--z;
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector3 operator--(int32_t) noexcept
		{
			Vector3 Vector{ *this };
			--* this;
			return Vector3{ Vector };
		}

	};

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator+(const Vector3& lhs, float scalar) noexcept
	{
		return Vector3{ lhs.x + scalar, lhs.y + scalar, lhs.z + scalar };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator-(const Vector3& lhs, float scalar) noexcept
	{
		return Vector3{ lhs.x - scalar, lhs.y - scalar, lhs.z - scalar };

	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator*(const Vector3& lhs, float scalar) noexcept
	{
		return Vector3{ lhs.x * scalar, lhs.y * scalar, lhs.z * scalar };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator/(const Vector3& lhs, float scalar)
	{
		return Vector3{ lhs.x / scalar, lhs.y / scalar, lhs.z / scalar };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator%(const Vector3& lhs, float scalar)
	{
		return Vector3{ JINMATH_MODULO(float, lhs.x, scalar), JINMATH_MODULO(float, lhs.y, scalar), JINMATH_MODULO(float, lhs.z, scalar) };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Vector3& lhs, const Vector3& rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Vector3& lhs, const Vector3& rhs) noexcept
	{
		return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Vector3& lhs, float scalar)
	{
		return lhs.x == scalar && lhs.y == scalar && lhs.z == scalar;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Vector3& lhs, float scalar)
	{
		return lhs.x != scalar || lhs.y != scalar || lhs.z != scalar;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator+(float scalar, const Vector3& rhs) noexcept
	{
		return Vector3{ scalar + rhs.x, scalar + rhs.y, scalar + rhs.z };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator-(float scalar, const Vector3& rhs) noexcept
	{
		return Vector3{ scalar - rhs.x, scalar - rhs.y, scalar - rhs.z };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator*(float scalar, const Vector3& rhs) noexcept
	{
		return Vector3{ scalar * rhs.x, scalar * rhs.y, scalar * rhs.z };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator/(float scalar, const Vector3& rhs)
	{
		return Vector3{ scalar / rhs.x, scalar / rhs.y, scalar / rhs.z };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator%(float scalar, const Vector3& rhs)
	{
		return Vector3{ JINMATH_MODULO(float, scalar, rhs.x), JINMATH_MODULO(float, scalar, rhs.y), JINMATH_MODULO(float, scalar, rhs.z) };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(float scalar, const Vector3& rhs)
	{
		return scalar == rhs.x && scalar == rhs.y && scalar == rhs.z;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(float scalar, const Vector3& rhs)
	{
		return scalar != rhs.x || scalar != rhs.y || scalar != rhs.z;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 lerpUnClamped(const Vector3& start, const Vector3& end, float t) noexcept
	{
		return start + (end - start) * t;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 lerp(const Vector3& start, const Vector3& end, float t) noexcept
	{
		return lerpUnClamped(start, end, clamp01<float>(t));
	}

	JINMATH_FORCE_INLINE Vector3 operator+(const Vector3& vector) noexcept
	{
		return Vector3{ vector };
	}

	
	JINMATH_FORCE_INLINE Vector3 operator-(const Vector3& vector) noexcept
	{
		return Vector3(
			-vector.x,
			-vector.y,
			-vector.z);
	}

	// //////////////////////////////////////////////////////

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float dot(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 cross(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(
			lhs.y * rhs.z - rhs.y * lhs.z,
			lhs.z * rhs.x - rhs.z * lhs.x,
			lhs.x * rhs.y - rhs.x * lhs.y);
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 sin(const Vector3& vector)
	{
		return Vector3{std::sin(vector.x), std::sin(vector.y), std::sin(vector.z)};
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 cos(const Vector3& vector)
	{
		return Vector3{std::cos(vector.x), std::cos(vector.y), std::cos(vector.z)};
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 tan(const Vector3& vector)
	{
		return Vector3{std::tan(vector.x), std::tan(vector.y), std::tan(vector.z)};
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 sqrt(const Vector3& vector)
	{
		return Vector3{std::sqrt(vector.x), std::sqrt(vector.y), std::sqrt(vector.z)};
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 inverseSqrt(const Vector3& vector)
	{
		return Vector3{ ::math::inverseSqrt(vector.x), ::math::inverseSqrt(vector.y), ::math::inverseSqrt(vector.z)};
	}

	
	JINMATH_NO_DISCARD Vector3 normalize(const Vector3& vector);


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 Max(const Vector3& vector1, const Vector3& vector2)
	{
		return Vector3(::math::Max(vector1.x, vector2.x), ::math::Max(vector1.y, vector2.y), ::math::Max(vector1.z, vector2.z));
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 Min(const Vector3& vector1, const Vector3& vector2)
	{
		return Vector3(::math::Min(vector1.x, vector2.x), ::math::Min(vector1.y, vector2.y), ::math::Min(vector1.z, vector2.z));
	}
	
	template<typename T>
	JINMATH_NO_DISCARD inline Vector3 slerp
	(
		const Vector3& x,
		const Vector3& y,
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

