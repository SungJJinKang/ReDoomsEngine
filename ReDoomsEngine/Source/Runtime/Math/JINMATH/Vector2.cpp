#include "Vector2.h"

#include "Vector3.h"
#include "Vector4.h"


const math::Vector2 math::Vector2::right{ 1.0f, 0.0f };
const math::Vector2 math::Vector2::up{ 0.0f, 1.0f };

math::Vector2::Vector2(const Vector3& vector) noexcept: x{ vector.x }, y{ vector.y}
{
}

math::Vector2::Vector2(const Vector4& vector) noexcept: x{ vector.x }, y{ vector.y}
{
}

math::Vector2::type& math::Vector2::operator=(const Vector3& vector) noexcept
{
	x = vector.x;
	y = vector.y;
	return *this;
}

math::Vector2::type& math::Vector2::operator=(const Vector4& vector) noexcept
{
	x = vector.x;
	y = vector.y;
	return *this;
}

JINMATH_NO_DISCARD math::Vector2 math::normalize(const Vector2& vector)
{
	return vector * inverseSqrt(dot(vector, vector));
}
