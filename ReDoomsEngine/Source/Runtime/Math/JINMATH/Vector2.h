#pragma once
#include "JINMATHCore.h"

#include "Utility.h"

namespace math
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	
	/// Column vector
	struct Vector2
	{
		using value_type = float;
		using type = typename Vector2;

		union { float x, r; };
		union { float y, g; };

		JINMATH_FORCE_INLINE float* data() noexcept
		{
			return &x;
		}

		const JINMATH_FORCE_INLINE float* data() const noexcept
		{
			return &x;
		}


		static const Vector2 right;
		static const Vector2 up;


		JINMATH_FORCE_INLINE Vector2() {}
		JINMATH_FORCE_INLINE explicit Vector2(float xValue)  noexcept
			: x{ xValue }, y { xValue }
		{
		}


		JINMATH_FORCE_INLINE Vector2(float xValue, float yValue) noexcept
			: x{ xValue }, y{ yValue }
		{
		}
		
		JINMATH_FORCE_INLINE Vector2(const Vector2& vector) noexcept
			: x{ vector.x }, y{ vector.y}
		{
		}

		
		explicit Vector2(const Vector3& vector) noexcept;
		explicit Vector2(const Vector4& vector) noexcept;

		JINMATH_FORCE_INLINE Vector2& operator=(value_type xValue) noexcept
		{
			x = xValue;
			y = xValue;
			return *this;
		}
				
		JINMATH_FORCE_INLINE Vector2& operator=(const Vector2& vector) noexcept
		{
			x = vector.x;
			y = vector.y;
			return *this;
		}

		
		Vector2& operator=(const Vector3& vector) noexcept;
		Vector2& operator=(const Vector4& vector) noexcept;

		// 		Vector(const Vector2&) = default;
// 		Vector(Vector2&&) = default;
// 		Vector2& opreator=(const Vector2&) = default;
// 		Vector2& opreator=(Vector2&&) = default;

	

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE static size_t componentCount() noexcept { return 2; }

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
			default:
				__assume(0);
			}
		}
		

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float sqrMagnitude() const noexcept
		{
			return x* x + y * y;
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float magnitude() const noexcept
		{
			return std::sqrt(sqrMagnitude());
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 normalized() const noexcept
		{
			float mag = magnitude();
			if (mag == 0)
				return Vector2{0.0f, 0.0f};

			return Vector2{ static_cast<value_type>(x / mag), static_cast<value_type>(y / mag) };
		}

		JINMATH_FORCE_INLINE void Normalize()
		{
			float mag = magnitude();
			if (mag > std::numeric_limits<float>::epsilon())
			{
				x = static_cast<value_type>(x / mag);
				y = static_cast<value_type>(y / mag);
			}
		}

		
		JINMATH_FORCE_INLINE operator Vector2()
		{
			return Vector2{x, y};
		}

		JINMATH_FORCE_INLINE Vector2 operator+(const Vector2& rhs) const noexcept
		{
			return Vector2(x + rhs.x, y + rhs.y);
		}

		
		JINMATH_FORCE_INLINE Vector2 operator-(const Vector2& rhs) const noexcept
		{
			return Vector2(x - rhs.x, y - rhs.y);
		}

		
		JINMATH_FORCE_INLINE Vector2 operator*(const Vector2& rhs) const noexcept
		{
			return Vector2(x * rhs.x, y * rhs.y);
		}

		
		JINMATH_FORCE_INLINE Vector2 operator/(const Vector2& rhs) const noexcept
		{
			return Vector2(x / rhs.x, y / rhs.y);
		}

		
		JINMATH_FORCE_INLINE Vector2 operator%(const Vector2& rhs) const
		{
			return Vector2(JINMATH_MODULO(float, x, rhs.x), JINMATH_MODULO(float, y, rhs.y));
		}

		
		JINMATH_FORCE_INLINE Vector2& operator+=(const Vector2& rhs) noexcept
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator-=(const Vector2& rhs) noexcept
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator*=(const Vector2& rhs) noexcept
		{
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator/=(const Vector2& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator%=(const Vector2& rhs)
		{
			x = JINMATH_MODULO(float, x, rhs.x);
			y = JINMATH_MODULO(float, y, rhs.y);
			return *this;
		}

		
		//

		

		//

		

		//

		
		JINMATH_FORCE_INLINE Vector2& operator+=(float scalar) noexcept
		{
			x += scalar;
			y += scalar;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator-=(float scalar) noexcept
		{
			x -= scalar;
			y -= scalar;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator*=(float scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Vector2& operator%=(float scalar)
		{
			x = JINMATH_MODULO(float, x, scalar);
			y = JINMATH_MODULO(float, y, scalar);
			return *this;
		}

		



		//

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector2& operator++() noexcept
		{
			++x;
			++y;
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector2 operator++(int32_t) noexcept
		{
			Vector2 Vector{ *this };
			++* this;
			return Vector2{ Vector };
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector2& operator--() noexcept
		{
			--x;
			--y;
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Vector2 operator--(int32_t) noexcept
		{
			Vector2 Vector{ *this };
			--* this;
			return Vector2{ Vector };
		}
		
	};

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator+(const Vector2& lhs, float scalar) noexcept
	{
		return Vector2{ lhs.x + scalar, lhs.y + scalar };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator-(const Vector2& lhs, float scalar) noexcept
	{
		return Vector2{ lhs.x - scalar, lhs.y - scalar };

	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator*(const Vector2& lhs, float scalar) noexcept
	{
		return Vector2{ lhs.x * scalar, lhs.y * scalar };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator/(const Vector2& lhs, float scalar)
	{
		return Vector2{ lhs.x / scalar, lhs.y / scalar };
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator%(const Vector2& lhs, float scalar)
	{
		return Vector2{ JINMATH_MODULO(float, lhs.x, scalar), JINMATH_MODULO(float, lhs.y, scalar) };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Vector2& lhs, const Vector2& rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Vector2& lhs, const Vector2& rhs) noexcept
	{
		return lhs.x != rhs.x || lhs.y != rhs.y;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Vector2& lhs, float scalar)
	{
		return lhs.x == scalar && lhs.y == scalar;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Vector2& lhs, float scalar)
	{
		return lhs.x != scalar || lhs.y != scalar;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator+(float scalar, const Vector2& rhs) noexcept
	{
		return Vector2{ scalar + rhs.x, scalar + rhs.y };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator-(float scalar, const Vector2& rhs) noexcept
	{
		return Vector2{ scalar - rhs.x, scalar - rhs.y };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator*(float scalar, const Vector2& rhs) noexcept
	{
		return Vector2{ scalar * rhs.x, scalar * rhs.y };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator/(float scalar, const Vector2& rhs)
	{
		return Vector2{ scalar / rhs.x, scalar / rhs.y };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 operator%(float scalar, const Vector2& rhs)
	{
		return Vector2{ JINMATH_MODULO(float, scalar, rhs.x), JINMATH_MODULO(float, scalar, rhs.y) };
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(float scalar, const Vector2& rhs)
	{
		return scalar == rhs.x && scalar == rhs.y;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(float scalar, const Vector2& rhs)
	{
		return scalar != rhs.x || scalar != rhs.y;
	}

	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 lerpUnClamped(const Vector2& start, const Vector2& end, float t) noexcept
	{
		return start + (end - start) * t;
	}


	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 lerp(const Vector2& start, const Vector2& end, float t) noexcept
	{
		return lerpUnClamped(start, end, clamp01(t));
	}
	
	JINMATH_FORCE_INLINE Vector2 operator+(const Vector2& vector) noexcept
	{
		return Vector2{ vector };
	}

	
	JINMATH_FORCE_INLINE Vector2 operator-(const Vector2& vector) noexcept
	{
		return Vector2(
			-vector.x,
			-vector.y);
	}

	// ///////////////////////////////////////////////

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float dot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	/// <summary>
	/// http://www.sunshine2k.de/articles/Notes_PerpDotProduct_R2.pdf
	/// 
	/// perpDot return area of the parallelogram from two vectors
	/// </summary>
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE float perpDot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 cos(const Vector2& vector)
	{
		return Vector2{std::sin(vector.x), std::sin(vector.y)};
	}

	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 sin(const Vector2& vector)
	{
		return Vector2{ std::cos(vector.x), std::cos(vector.y)};
	}
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 tan(const Vector2& vector)
	{
		return Vector2{ std::tan(vector.x), std::tan(vector.y)};
	}
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 sqrt(const Vector2& vector)
	{
		return Vector2{std::sqrt(vector.x), std::sqrt(vector.y)};
	}
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 inverseSqrt(const Vector2& vector)
	{
		return Vector2{ ::math::inverseSqrt(vector.x), ::math::inverseSqrt(vector.y)};
	}
	
	JINMATH_NO_DISCARD Vector2 normalize(const Vector2& vector);
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 Max(const Vector2& vector1, const Vector2& vector2)
	{
		return Vector2(::math::Max(vector1.x, vector2.x), ::math::Max(vector1.y, vector2.y));
	}
	
	JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector2 Min(const Vector2& vector1, const Vector2& vector2)
	{
		return Vector2(::math::Min(vector1.x, vector2.x), ::math::Min(vector1.y, vector2.y));
	}

	template<typename T>
	JINMATH_NO_DISCARD inline Vector2 slerp
	(
		const Vector2& x,
		const Vector2& y,
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

