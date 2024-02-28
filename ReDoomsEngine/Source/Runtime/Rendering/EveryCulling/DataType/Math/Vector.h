#pragma once	

#include "../../EveryCullingCore.h"

#include "SIMD_Core.h"

namespace culling
{
	struct Vec2
	{
		float x, y;

		EASTL_FORCE_INLINE Vec2() {}
		EASTL_FORCE_INLINE Vec2(const float _x, const float _y)
			:x(_x), y(_y)
		{

		}

		EASTL_FORCE_INLINE float* data()
		{
			return &x;
		}

		EASTL_FORCE_INLINE const float* data() const
		{
			return &x;
		}
	};

	struct Vec3
	{
		float x, y, z;

		EASTL_FORCE_INLINE Vec3() {}
		EASTL_FORCE_INLINE Vec3(const float _x, const float _y, const float _z)
			:x(_x), y(_y), z(_z)
		{

		}

		EASTL_FORCE_INLINE float* data()
		{
			return &x;
		}

		EASTL_FORCE_INLINE const float* data() const
		{
			return &x;
		}

		EASTL_FORCE_INLINE float sqrMagnitude() const
		{
			return x * x + y * y + z * z;
		}

		EASTL_FORCE_INLINE float magnitude() const
		{
			return sqrtf(sqrMagnitude());
		}
	};

	


	/// <summary>
	/// Why alignas(16)? : FOR SIMD!!
	/// </summary>
	struct alignas(16) Vec4
	{
		float values[4];

		EASTL_FORCE_INLINE float& operator[](size_t index)
		{
			return values[index];
		}

		EASTL_FORCE_INLINE float operator[](size_t index) const
		{
			return values[index];
		}

		EASTL_FORCE_INLINE float* data()
		{
			return values;
		}

		EASTL_FORCE_INLINE const float* data() const
		{
			return values;
		}

		EASTL_FORCE_INLINE float sqrMagnitude() const
		{
			const culling::EVERYCULLING_M128F mul0 = _mm_mul_ps(*reinterpret_cast<const culling::EVERYCULLING_M128F*>(data()), *reinterpret_cast<const culling::EVERYCULLING_M128F*>(data()));
			const culling::EVERYCULLING_M128F had0 = _mm_hadd_ps(mul0, mul0);
			const culling::EVERYCULLING_M128F had1 = _mm_hadd_ps(had0, had0);
			return _mm_cvtss_f32(had1);
		}

		EASTL_FORCE_INLINE float magnitude() const
		{
			return sqrtf(sqrMagnitude());
		}
	};

	struct alignas(16) Quat
	{
		float values[4];

		EASTL_FORCE_INLINE float& operator[](size_t index)
		{
			return values[index];
		}

		EASTL_FORCE_INLINE float operator[](size_t index) const
		{
			return values[index];
		}

		EASTL_FORCE_INLINE float* data()
		{
			return values;
		}

		EASTL_FORCE_INLINE const float* data() const
		{
			return values;
		}

		EASTL_FORCE_INLINE float sqrMagnitude() const
		{
			const culling::EVERYCULLING_M128F mul0 = _mm_mul_ps(*reinterpret_cast<const culling::EVERYCULLING_M128F*>(data()), *reinterpret_cast<const culling::EVERYCULLING_M128F*>(data()));
			const culling::EVERYCULLING_M128F had0 = _mm_hadd_ps(mul0, mul0);
			const culling::EVERYCULLING_M128F had1 = _mm_hadd_ps(had0, had0);
			return _mm_cvtss_f32(had1);
		}

		EASTL_FORCE_INLINE float magnitude() const
		{
			return sqrtf(sqrMagnitude());
		}
	};

	EASTL_FORCE_INLINE culling::Vec2 operator+(const culling::Vec2& a, const culling::Vec2& b)
	{
		return culling::Vec2(a.x + b.x, a.y + b.y);
	}

	EASTL_FORCE_INLINE culling::Vec2 operator-(const culling::Vec2& a, const culling::Vec2& b)
	{
		return culling::Vec2(a.x - b.x, a.y - b.y);

	}

	EASTL_FORCE_INLINE culling::Vec3 operator+(const culling::Vec3& a, const culling::Vec3& b)
	{
		return culling::Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	EASTL_FORCE_INLINE culling::Vec3 operator-(const culling::Vec3& a, const culling::Vec3& b)
	{
		return culling::Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
	}


	EASTL_FORCE_INLINE culling::Vec4 operator+(const culling::Vec4& a, const culling::Vec4& b)
	{
		culling::Vec4 result;

		*reinterpret_cast<culling::EVERYCULLING_M128F*>(result.data()) = _mm_add_ps(*reinterpret_cast<const culling::EVERYCULLING_M128F*>(a.data()), *reinterpret_cast<const culling::EVERYCULLING_M128F*>(b.data()));

		return result;
	}

	EASTL_FORCE_INLINE culling::Vec4 operator-(const culling::Vec4& a, const culling::Vec4& b)
	{
		culling::Vec4 result;

		*reinterpret_cast<culling::EVERYCULLING_M128F*>(result.data()) = _mm_sub_ps(*reinterpret_cast<const culling::EVERYCULLING_M128F*>(a.data()), *reinterpret_cast<const culling::EVERYCULLING_M128F*>(b.data()));

		return result;
	}

	EASTL_FORCE_INLINE float Dot(const culling::Vec2& a, const culling::Vec2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	EASTL_FORCE_INLINE float Dot(const culling::Vec3& a, const culling::Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	EASTL_FORCE_INLINE float PerpDot(const culling::Vec2& lhs, const culling::Vec2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	EASTL_FORCE_INLINE culling::Vec3 Cross(const culling::Vec3& lhs, const culling::Vec3& rhs)
	{
		return culling::Vec3(
			lhs.y * rhs.z - rhs.y * lhs.z,
			lhs.z * rhs.x - rhs.z * lhs.x,
			lhs.x * rhs.y - rhs.x * lhs.y);
	}
}