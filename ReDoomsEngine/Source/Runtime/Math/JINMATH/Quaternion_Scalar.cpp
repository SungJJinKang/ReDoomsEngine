#include "Quaternion_Scalar.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

math::Quaternion::Quaternion(float s, const Vector3& vector) noexcept : w{ s }, x { x }, y{ y }, z{ z }
{
}


math::Quaternion::type math::Quaternion::mat2Quaternion(const Matrix3x3& m)
{
	float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
	float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
	float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
	float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

	int32_t biggestIndex = 0;
	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = static_cast<float>(::std::sqrt(fourBiggestSquaredMinus1 + static_cast<float>(1)) * static_cast<float>(0.5));
	float mult = static_cast<float>(0.25) / biggestVal;

	switch (biggestIndex)
	{
	case 0:
		return Quaternion(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);
	case 1:
		return Quaternion((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);
	case 2:
		return Quaternion((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);
	case 3:
		return Quaternion((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);
	default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
		assert(false);
		return Quaternion(1, 0, 0, 0);
	}
}

math::Quaternion::Quaternion(const Matrix4x4& m) noexcept
{
	*this = mat2Quaternion(static_cast<Matrix3x3>( m));
}

math::Quaternion::type& math::Quaternion::operator=(const Matrix4x4& m) noexcept
{
	*this = mat2Quaternion(static_cast<Matrix3x3>(m));
	return *this;
}

math::Quaternion::type& math::Quaternion::operator=(const Matrix3x3& m) noexcept
{
	*this = mat2Quaternion(m);
	return *this;
}

math::Quaternion::operator math::Matrix3x3() const noexcept
{
	Matrix3x3 Result{ float(1) };

	float qxx(this->x * this->x);
	float qyy(this->y * this->y);
	float qzz(this->z * this->z);
	float qxz(this->x * this->z);
	float qxy(this->x * this->y);
	float qyz(this->y * this->z);
	float qwx(this->w * this->x);
	float qwy(this->w * this->y);
	float qwz(this->w * this->z);

	Result[0][0] = float(1) - float(2) * (qyy + qzz);
	Result[0][1] = float(2) * (qxy + qwz);
	Result[0][2] = float(2) * (qxz - qwy);

	Result[1][0] = float(2) * (qxy - qwz);
	Result[1][1] = float(1) - float(2) * (qxx + qzz);
	Result[1][2] = float(2) * (qyz + qwx);

	Result[2][0] = float(2) * (qxz + qwy);
	Result[2][1] = float(2) * (qyz - qwx);
	Result[2][2] = float(1) - float(2) * (qxx + qyy);

	return Result;
}

math::Quaternion::operator math::Matrix4x4() const noexcept
{
	return Matrix4x4(this->operator math::Matrix3x3());
}

math::Vector3 math::Quaternion::ToEulerAngle() const
{
	return QuaternionToEulerAngle(*this);
}

math::Quaternion math::Quaternion::CreateFromAxisAngle(const float& angle, const Vector3& v)
{
	const float a{ angle };
	const float s{ std::sin(a * static_cast<float>(0.5)) };

	return Quaternion(std::cos(a * static_cast<float>(0.5)), v * s);
}

math::Quaternion math::Quaternion::EulerAngleToQuaternion(const Vector3& eulerAngle) noexcept
{
	Vector3 c = math::cos(eulerAngle * float(0.5f));
	Vector3 s = math::sin(eulerAngle * float(0.5f));

	Quaternion result{};
	result.w = c.x * c.y * c.z + s.x * s.y * s.z;
	result.x = s.x * c.y * c.z - c.x * s.y * s.z;
	result.y = c.x * s.y * c.z + s.x * c.y * s.z;
	result.z = c.x * c.y * s.z - s.x * s.y * c.z;

	return result;
}


math::Quaternion math::Quaternion::EulerAngleToQuaternion(const float pitch, const float yaw, const float roll) noexcept
{
	return EulerAngleToQuaternion(Vector3{ pitch, yaw, roll });
}

float math::Quaternion::roll(const Quaternion& q)
{
	const float y = static_cast<float>(2) * (q.x * q.y + q.w * q.z);
	const float x = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;

	const float epsilon = std::numeric_limits<float>::epsilon();;
	if (std::abs(x) < epsilon && std::abs(y) < epsilon)  //avoid atan2(0,0) - handle singularity - Matiis
	{
		return static_cast<float>(0);
	}
	return static_cast<float>(std::atan2(y, x));
}

float math::Quaternion::pitch(const Quaternion& q)
{
	//return float(atan(float(2) * (q.y * q.z + q.w * q.x).w * q.w - q.x * q.x - q.y * q.y + q.z * q.z));
	const float y = static_cast<float>(2) * (q.y * q.z + q.w * q.x);
	const float x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

	const float epsilon = std::numeric_limits<float>::epsilon();;
	if (std::abs(x) < epsilon && std::abs(y) < epsilon) //avoid atan2(0,0) - handle singularity - Matiis
	{
		return static_cast<float>(static_cast<float>(2) * math::atan2(q.x, q.w));
	}

	return static_cast<float>(math::atan2(y, x));
}

math::Vector3 math::Quaternion::QuaternionToEulerAngle(const Quaternion& x)
{
	return Vector3(Quaternion::pitch(x), Quaternion::yaw(x), Quaternion::roll(x));
}

math::Vector3 math::operator*(const Quaternion& q, const Vector3& v)
{
	const Vector3 QuatVector(q.x, q.y, q.z);
	const Vector3 uv(cross(QuatVector, v));
	const Vector3 uuv(cross(QuatVector, uv));

	return v + ((uv * q.w) + uuv) * static_cast<float>(2);
}

math::Vector4 math::operator*(const Quaternion& q, const Vector4& v)
{
	return Vector4(q * Vector3(v), v.w);
}

namespace math
{
	namespace details
	{
		math::Quaternion quatLookAtRH(const math::Vector3& direction, const math::Vector3& up)
		{
			math::Matrix3x3 Result{};

			Result[2] = -direction;
			const math::Vector3 Right = cross(up, Result[2]);
			Result[0] = Right * math::inverseSqrt(math::Max(static_cast<float>(0.00001f), dot(Right, Right)));
			Result[1] = cross(Result[2], Result[0]);

			return math::Quaternion(Result);
		}

		math::Quaternion quatLookAtLH(const math::Vector3& direction, const math::Vector3& up)
		{
			math::Matrix3x3 Result{};

			Result[2] = direction;
			const math::Vector3 Right = cross(up, Result[2]);
			Result[0] = Right * math::inverseSqrt(math::Max(static_cast<float>(0.00001f), dot(Right, Right)));
			Result[1] = cross(Result[2], Result[0]);

			return math::Quaternion(Result);
		}

	}
}

math::Quaternion math::Quaternion::LookAt(const math::Vector3& direction, const math::Vector3& up)
{

#if JINMATH_CURRENT_COORDINATE_SYSTEM == JINMATH_LEFT_HAND

	return details::quatLookAtLH(direction, up);

#else

	return details::quatLookAtRH(direction, up);

# 		endif

}