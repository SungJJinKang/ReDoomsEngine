#pragma once
#include "JINMATHCore.h"

#include "Vector3.h"
#include "Vector4.h"
#include "Utility.h"

namespace math
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Matrix2x2;
	struct Matrix3x3;
	struct Matrix4x4;


	struct Quaternion
	{
		using value_type = typename float;
		using type = typename Quaternion;

		/// <summary>
		/// this is radian
		/// </summary>
		struct
		{
			float w;

			float x;

			float y;

			float z;
		};

		JINMATH_FORCE_INLINE float* data() noexcept
		{
			return &w;
		}

		JINMATH_FORCE_INLINE const float* data() const noexcept
		{
			return &w;
		}

		JINMATH_FORCE_INLINE Quaternion() noexcept {}
		JINMATH_FORCE_INLINE explicit Quaternion(float value) noexcept
			: w{ value }, x{ value }, y{ value }, z{ value }
		{
		}

		JINMATH_FORCE_INLINE Quaternion(float wValue, float xValue, float yValue, float zValue) noexcept
			: w{ wValue }, x{ xValue }, y{ yValue }, z{ zValue }
		{
		}

		template <typename W, typename X, typename Y, typename Z>
		Quaternion(W wValue, X xValue, Y yValue, Z zValue) noexcept
			: w{ static_cast<float>(wValue) }, x{ static_cast<float>(xValue) }, y{ static_cast<float>(yValue) }, z{ static_cast<float>(zValue) }
		{
		}

		JINMATH_FORCE_INLINE Quaternion(const Quaternion& quat) noexcept
			: w{ quat.w }, x{ quat.x }, y{ quat.y }, z{ quat.z }
		{
		}

		
		Quaternion(float s, const Vector3& vector) noexcept;
		
		JINMATH_FORCE_INLINE explicit Quaternion(const Vector3& eulerAngle) noexcept
		{
			*this = type::EulerAngleToQuaternion(eulerAngle);
		}

		static type mat2Quaternion(const Matrix3x3& m);

		explicit Quaternion(const Matrix3x3& m) noexcept
		{
			*this = mat2Quaternion(m);
		}

		explicit Quaternion(const Matrix4x4& m) noexcept;

		JINMATH_FORCE_INLINE type& operator=(const type& vector) noexcept
		{
			w = vector.w;
			x = vector.x;
			y = vector.y;
			z = vector.z;
			return *this;
		}

	
		JINMATH_FORCE_INLINE type& operator=(const Matrix4x4& m) noexcept;

		JINMATH_FORCE_INLINE type& operator=(const Matrix3x3& m) noexcept;

		JINMATH_FORCE_INLINE type& operator=(const Vector3& eulerAngle) noexcept
		{
			*this = type::EulerAngleToQuaternion(eulerAngle);
			return *this;
		} 
		
		JINMATH_NO_DISCARD inline static size_t componentCount()  noexcept { return 4; }

		JINMATH_NO_DISCARD value_type& operator[](size_t i)
		{
			assert(i >= 0 || i < componentCount());
			switch (i)
			{
			case 0:
				return w;
			case 1:
				return x;
			case 2:
				return y;
			case 3:
				return z;
			default:
				JINMATH_ASSUME_ZERO;
			}
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE const value_type& operator[](size_t i) const
		{
			assert(i >= 0 || i < componentCount());
			switch (i)
			{
			case 0:
				return w;
			case 1:
				return x;
			case 2:
				return y;
			case 3:
				return z;
			default:
				JINMATH_ASSUME_ZERO;
			}
		}

		
		JINMATH_FORCE_INLINE type& operator+=(const Quaternion& rhs) noexcept
		{
			w += rhs.w;
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		
		JINMATH_FORCE_INLINE type& operator-=(const Quaternion& rhs) noexcept
		{
			w += rhs.w;
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}


		JINMATH_FORCE_INLINE type& operator*=(const Quaternion& rhs) noexcept
		{
			const Quaternion p(*this);
			const Quaternion q(rhs);

			this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
			this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
			this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
			this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
			return *this;
		}
		
		JINMATH_FORCE_INLINE type& operator/=(const Quaternion& rhs) noexcept
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}
		
		JINMATH_FORCE_INLINE type operator+(const Quaternion& rhs) const noexcept
		{
			Quaternion result{};

			result.w = w + rhs.w;
			result.x = x + rhs.x;
			result.y = y + rhs.y;
			result.z = z + rhs.z;

			return result;
		}


		JINMATH_FORCE_INLINE type operator-(const Quaternion& rhs) const noexcept
		{
			Quaternion result{};

			result.w = w - rhs.w;
			result.x = x - rhs.x;
			result.y = y - rhs.y;
			result.z = z - rhs.z;

			return result;
		}

		JINMATH_FORCE_INLINE type operator*(const Quaternion& rhs) const noexcept
		{
			Quaternion result{};

			result.w = this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z;
			result.x = this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y;
			result.y = this->w * rhs.y + this->y * rhs.w + this->z * rhs.x - this->x * rhs.z;
			result.z = this->w * rhs.z + this->z * rhs.w + this->x * rhs.y - this->y * rhs.x;

			return type{ result };
		}
		
		JINMATH_FORCE_INLINE type operator/(const Quaternion& rhs) const
		{
			Quaternion result{};

			result.w = w / rhs.w;
			result.x = x / rhs.x;
			result.y = y / rhs.y;
			result.z = z / rhs.z;

			return result;
		}

		/*
		static Matrix3x3 convert_quaternion_to_matrix(const Quaternion& quaternion)
		{

		}
		*/

		operator Matrix3x3() const noexcept;

		operator Matrix4x4() const noexcept;

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const type& rhs) const noexcept
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w;
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const type& rhs) const noexcept
		{
			return this->x != rhs.x || this->y != rhs.y || this->z != rhs.z || this->w != rhs.w;
		}

		template <typename X>
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const X& number) const noexcept
		{
			return this->x == number && this->y == number && this->z == number && this->w == number;
		}

		template <typename X>
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const X& number) const noexcept
		{
			return this->x != number || this->y != number || this->z != number || this->w != number;
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE type& operator++() noexcept
		{
			++x;
			++y;
			++z;
			++w;
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE type operator++(int32_t) noexcept
		{
			type Quaternion{ *this };
			++* this;
			return type{ Quaternion };
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE type& operator--() noexcept
		{
			--x;
			--y;
			--z;
			--w;
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE type operator--(int32_t) noexcept
		{
			type Quaternion{ *this };
			--* this;
			return type{ Quaternion };
		}

		math::Vector3 ToEulerAngle() const;
		/*
		
		static float angle(Quaternion const& x)
		{
			if (abs(x.w) > cos_one_over_two<float>())
			{
				float const a = asin(sqrt(x.x * x.x + x.y * x.y + x.z * x.z)) * static_cast<float>(2);
				if (x.w < static_cast<float>(0))
					return pi<float>() * static_cast<float>(2) - a;
				return a;
			}

			return acos(x.w) * static_cast<float>(2);
		}

		
		static Vector3 axis(qua<float, Q> const& x)
		{
			float const tmp1 = static_cast<float>(1) - x.w * x.w;
			if (tmp1 <= static_cast<float>(0))
				return vec<3, float, Q>(0, 0, 1);
			float const tmp2 = static_cast<float>(1) / sqrt(tmp1);
			return vec<3, float, Q>(x.x * tmp2, x.y * tmp2, x.z * tmp2);
		}
		*/

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE static float angle(const Quaternion& x)
		{
			if (math::Abs(x.w) > 0.877582561890372716130286068203503191)
			{
				float const a = math::asin(math::sqrt(x.x * x.x + x.y * x.y + x.z * x.z)) * static_cast<float>(2);
				if (x.w < static_cast<float>(0))
				{
					return static_cast<float>(math::PI) * static_cast<float>(2) - a;
				}
				return a;
			}

			return acos(x.w) * static_cast<float>(2);
		}
		
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 axis(const Quaternion& x)
		{
			float const tmp1 = static_cast<float>(1) - x.w * x.w;
			if (tmp1 <= static_cast<float>(0))
				return Vector3(0, 0, 1);
			float const tmp2 = static_cast<float>(1) / sqrt(tmp1);
			return Vector3(x.x * tmp2, x.y * tmp2, x.z * tmp2);
		}

		static Quaternion CreateFromAxisAngle(const float& angle, const Vector3& v);

		template<typename X, typename Y>
		static Quaternion CreateFromAxisAngle(const X& angle, const Vector3& v)
		{
			const float a{ angle };
			const float s{ std::sin(a * static_cast<float>(0.5)) };

			return Quaternion(std::cos(a * static_cast<float>(0.5)), v * s);
		}
		
		static Quaternion EulerAngleToQuaternion(const Vector3& eulerAngle) noexcept;
		static Quaternion EulerAngleToQuaternion(const float pitch, const float yaw, const float roll) noexcept;


		static float roll(const Quaternion& q);


		static float pitch(const Quaternion& q);


		static float yaw(const Quaternion& q)
		{
			return std::asin(::math::clamp(static_cast<float>(-2) * (q.x * q.z - q.w * q.y), static_cast<float>(-1), static_cast<float>(1)));
		}

		
		static Vector3 QuaternionToEulerAngle(const Quaternion& x);
		
		static Quaternion LookAt(const math::Vector3& direction, const math::Vector3& up);
		
	};

	
	JINMATH_NO_DISCARD inline float dot(const Quaternion& a, const Quaternion& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}
		
	JINMATH_NO_DISCARD inline Quaternion cross(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
			q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
	}
	
	JINMATH_NO_DISCARD inline Quaternion conjugate(const Quaternion& q)
	{
		return Quaternion(q.w, -q.x, -q.y, -q.z);
	}

	JINMATH_NO_DISCARD inline Quaternion operator/(const Quaternion& q, const float s)
	{
		return Quaternion(
			q.w / s, q.x / s, q.y / s, q.z / s);
	}

	JINMATH_NO_DISCARD inline Quaternion inverse(const Quaternion& q)
	{
		return operator/(conjugate(q), dot(q, q));
	}
		
	JINMATH_NO_DISCARD inline Quaternion operator+(const Quaternion& vector) noexcept
	{
		return Quaternion{ vector };
	}

	
	JINMATH_NO_DISCARD inline Quaternion operator-(const Quaternion& vector) noexcept
	{
		return Quaternion(
			-vector.x,
			-vector.y,
			-vector.z,
			-vector.w);
	}


	//
	
	JINMATH_NO_DISCARD Vector3 operator*(const Quaternion& q, const Vector3& v);
	JINMATH_NO_DISCARD Vector4 operator*(const Quaternion& q, const Vector4& v);

	JINMATH_NO_DISCARD inline Vector4 operator*(const Vector4& v, const Quaternion& q)
	{
		return inverse(q) * v;
	}
	
	JINMATH_NO_DISCARD inline Quaternion operator*(const Quaternion& q, const float s)
	{
		return Quaternion(
			q.w * s, q.x * s, q.y * s, q.z * s);
	}
	
	JINMATH_NO_DISCARD inline Quaternion operator*(const float s, const Quaternion& q)
	{
		return q * s;
	}
	

	template<typename T>
	JINMATH_NO_DISCARD inline Quaternion lerp(const Quaternion& x, const Quaternion& y, T a)
	{
		static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);

		// Lerp is only defined in [0, 1]
		assert(a >= static_cast<T>(0));
		assert(a <= static_cast<T>(1));

		return x * (static_cast<float>(1) - a) + (y * a);
	}

	template<typename T>
	JINMATH_NO_DISCARD inline Quaternion slerp(const Quaternion& x, const Quaternion& y, T a)
	{
		static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);

		Quaternion z = y;

		T cosTheta = dot(x, y);

		// If cosTheta < 0, the interpolation will take the long way around the sphere.
		// To fix this, one quat must be negated.
		if (cosTheta < static_cast<T>(0))
		{
			z = -y;
			cosTheta = -cosTheta;
		}

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if (cosTheta > static_cast<T>(1) - math::epsilon<T>()())
		{
			// Linear interpolation
			return Quaternion(
				math::lerp(x.w, z.w, a),
				math::lerp(x.x, z.x, a),
				math::lerp(x.y, z.y, a),
				math::lerp(x.z, z.z, a));
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = math::acos(cosTheta);
			return (math::sin((static_cast<T>(1) - a) * angle) * x + math::sin(a * angle) * z) / math::sin(angle);
		}
	}
	
}
