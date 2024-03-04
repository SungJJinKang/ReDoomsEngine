#include "Vector3.h"

#include "Vector2.h"
#include "Vector4.h"


const math::Vector3 math::Vector3::forward{ 0.0f, 0.0f, 1.0f};
const math::Vector3 math::Vector3::right{ 1.0f, 0.0f, 0.0f};
const math::Vector3 math::Vector3::up{ 0.0f, 1.0f, 0.0f};

math::Vector3::Vector3(const Vector2& vector) noexcept: x{ vector.x }, y{ vector.y }, z{ 0 }
{
}

math::Vector3::Vector3(const Vector4& vector) noexcept: x{ vector.x }, y{ vector.y }, z{ vector.z }
{
}

math::Vector3::type& math::Vector3::operator=(const Vector2& vector) noexcept
{
	x = vector.x;
	y = vector.y;
	z = 0;
	return *this;
}

math::Vector3::type& math::Vector3::operator=(const Vector4& vector) noexcept
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
	return *this;
}

JINMATH_NO_DISCARD math::Vector3 math::normalize(const Vector3& vector)
{
	return vector * inverseSqrt(dot(vector, vector));
}
