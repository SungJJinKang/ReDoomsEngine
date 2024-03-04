#include "Matrix_utility.h"

math::Matrix4x4 math::lookAtRH(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up)
{
	const math::Vector3 f((center - eye).normalized());
	const math::Vector3 s(normalize(cross(f, up)));
	const math::Vector3 u(cross(s, f));

	math::Matrix4x4  Result(1);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -dot(s, eye);
	Result[3][1] = -dot(u, eye);
	Result[3][2] = dot(f, eye);
	return Result;
}

math::Matrix4x4 math::lookAtLH(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up)
{
	const math::Vector3 f(normalize(center - eye));
	const math::Vector3 s(normalize(cross(up, f)));
	const math::Vector3 u(cross(f, s));

	math::Matrix4x4 Result(1);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = f.x;
	Result[1][2] = f.y;
	Result[2][2] = f.z;
	Result[3][0] = -dot(s, eye);
	Result[3][1] = -dot(u, eye);
	Result[3][2] = -dot(f, eye);
	return Result;
}

math::Matrix4x4 math::lookAt(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up)
{
#if (JINMATH_CURRENT_COORDINATE_SYSTEM == JINMATH_LEFT_HAND)
	return lookAtLH(eye, center, up);
#else
	return lookAtRH(eye, center, up);
#endif
}

math::Matrix4x4 math::orthoLH_ZO(float left, float right, float bottom, float top, float zNear, float zFar)
{
	math::Matrix4x4 Result(1);
	Result[0][0] = static_cast<float>(2) / (right - left);
	Result[1][1] = static_cast<float>(2) / (top - bottom);
	Result[2][2] = static_cast<float>(1) / (zFar - zNear);
	Result[3][0] = -(right + left) / (right - left);
	Result[3][1] = -(top + bottom) / (top - bottom);
	Result[3][2] = -zNear / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::orthoLH_NO(float left, float right, float bottom, float top, float zNear, float zFar)
{
	math::Matrix4x4 Result(1);
	Result[0][0] = static_cast<float>(2) / (right - left);
	Result[1][1] = static_cast<float>(2) / (top - bottom);
	Result[2][2] = static_cast<float>(2) / (zFar - zNear);
	Result[3][0] = -(right + left) / (right - left);
	Result[3][1] = -(top + bottom) / (top - bottom);
	Result[3][2] = -(zFar + zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::orthoRH_ZO(float left, float right, float bottom, float top, float zNear, float zFar)
{
	math::Matrix4x4 Result(1);
	Result[0][0] = static_cast<float>(2) / (right - left);
	Result[1][1] = static_cast<float>(2) / (top - bottom);
	Result[2][2] = -static_cast<float>(1) / (zFar - zNear);
	Result[3][0] = -(right + left) / (right - left);
	Result[3][1] = -(top + bottom) / (top - bottom);
	Result[3][2] = -zNear / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::orthoRH_NO(float left, float right, float bottom, float top, float zNear, float zFar)
{
	math::Matrix4x4 Result(1);
	Result[0][0] = static_cast<float>(2) / (right - left);
	Result[1][1] = static_cast<float>(2) / (top - bottom);
	Result[2][2] = -static_cast<float>(2) / (zFar - zNear);
	Result[3][0] = -(right + left) / (right - left);
	Result[3][1] = -(top + bottom) / (top - bottom);
	Result[3][2] = -(zFar + zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::ortho(const float& left, const float& right, const float& bottom, const float& top)
{
	math::Matrix4x4 Result(static_cast<float>(1));
	Result[0][0] = static_cast<float>(2) / (right - left);
	Result[1][1] = static_cast<float>(2) / (top - bottom);
	Result[2][2] = -static_cast<float>(1);
	Result[3][0] = -(right + left) / (right - left);
	Result[3][1] = -(top + bottom) / (top - bottom);
	return Result;
}

math::Matrix4x4 math::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
#if JINMATH_CURRENT_SETTING == JINMATH_LEFT_HAND_ZERO_TO_ONE
	return orthoLH_ZO(left, right, bottom, top, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_LEFT_HAND_NEGATIVE_ONE_TO_ONE
	return orthoLH_NO(left, right, bottom, top, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_RIGHT_HAND_ZERO_TO_ONE
	return orthoRH_ZO(left, right, bottom, top, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_RIGHT_HAND_NEGATIVE_ONE_TO_ONE
	return orthoRH_NO(left, right, bottom, top, zNear, zFar);
#endif
}

math::Matrix4x4 math::perspectiveRH_ZO(float fovy, float aspect, float zNear, float zFar)
{
	assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0));

	float const tanHalfFovy = std::tan(fovy / static_cast<float>(2));

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
	Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
	Result[2][2] = zFar / (zNear - zFar);
	Result[2][3] = -static_cast<float>(1);
	Result[3][2] = -(zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveRH_NO(float fovy, float aspect, float zNear, float zFar)
{
	assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0));

	float const tanHalfFovy = std::tan(fovy / static_cast<float>(2));

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
	Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
	Result[2][2] = -(zFar + zNear) / (zFar - zNear);
	Result[2][3] = -static_cast<float>(1);
	Result[3][2] = -(static_cast<float>(2) * zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveLH_ZO(float fovy, float aspect, float zNear, float zFar)
{
	assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0));

	float const tanHalfFovy = std::tan(fovy / static_cast<float>(2));

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
	Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
	Result[2][2] = zFar / (zFar - zNear);
	Result[2][3] = static_cast<float>(1);
	Result[3][2] = -(zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveLH_NO(float fovy, float aspect, float zNear, float zFar)
{
	assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0));

	float const tanHalfFovy = std::tan(fovy / static_cast<float>(2));

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
	Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
	Result[2][2] = (zFar + zNear) / (zFar - zNear);
	Result[2][3] = static_cast<float>(1);
	Result[3][2] = -(static_cast<float>(2) * zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspective(const float& fovy, const float& aspect, const float& zNear, const float& zFar)
{
#if JINMATH_CURRENT_SETTING == JINMATH_LEFT_HAND_ZERO_TO_ONE
	return perspectiveLH_ZO(fovy, aspect, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_LEFT_HAND_NEGATIVE_ONE_TO_ONE
	return perspectiveLH_NO(fovy, aspect, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_RIGHT_HAND_ZERO_TO_ONE
	return perspectiveRH_ZO(fovy, aspect, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_RIGHT_HAND_NEGATIVE_ONE_TO_ONE
	return perspectiveRH_NO(fovy, aspect, zNear, zFar);
#endif
}

math::Matrix4x4 math::perspectiveFovRH_ZO(float fov, float width, float height, float zNear, float zFar)
{
	assert(width > static_cast<float>(0));
	assert(height > static_cast<float>(0));
	assert(fov > static_cast<float>(0));

	float const rad = fov;
	float const h = std::cos(static_cast<float>(0.5) * rad) / std::sin(static_cast<float>(0.5) * rad);
	float const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = w;
	Result[1][1] = h;
	Result[2][2] = zFar / (zNear - zFar);
	Result[2][3] = -static_cast<float>(1);
	Result[3][2] = -(zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveFovRH_NO(float fov, float width, float height, float zNear, float zFar)
{
	assert(width > static_cast<float>(0));
	assert(height > static_cast<float>(0));
	assert(fov > static_cast<float>(0));

	float const rad = fov;
	float const h = std::cos(static_cast<float>(0.5) * rad) / std::sin(static_cast<float>(0.5) * rad);
	float const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = w;
	Result[1][1] = h;
	Result[2][2] = -(zFar + zNear) / (zFar - zNear);
	Result[2][3] = -static_cast<float>(1);
	Result[3][2] = -(static_cast<float>(2) * zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveFovLH_ZO(float fov, float width, float height, float zNear, float zFar)
{
	assert(width > static_cast<float>(0));
	assert(height > static_cast<float>(0));
	assert(fov > static_cast<float>(0));

	float const rad = fov;
	float const h = std::cos(static_cast<float>(0.5) * rad) / std::sin(static_cast<float>(0.5) * rad);
	float const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = w;
	Result[1][1] = h;
	Result[2][2] = zFar / (zFar - zNear);
	Result[2][3] = static_cast<float>(1);
	Result[3][2] = -(zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveFovLH_NO(float fov, float width, float height, float zNear, float zFar)
{
	assert(width > static_cast<float>(0));
	assert(height > static_cast<float>(0));
	assert(fov > static_cast<float>(0));

	float const rad = fov;
	float const h = std::cos(static_cast<float>(0.5) * rad) / std::sin(static_cast<float>(0.5) * rad);
	float const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

	math::Matrix4x4 Result(static_cast<float>(0));
	Result[0][0] = w;
	Result[1][1] = h;
	Result[2][2] = (zFar + zNear) / (zFar - zNear);
	Result[2][3] = static_cast<float>(1);
	Result[3][2] = -(static_cast<float>(2) * zFar * zNear) / (zFar - zNear);
	return Result;
}

math::Matrix4x4 math::perspectiveFov(float fov, float width, float height, float zNear, float zFar)
{
#if JINMATH_CURRENT_SETTING == JINMATH_LEFT_HAND_ZERO_TO_ONE
	return perspectiveFovLH_ZO(fov, width, height, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_LEFT_HAND_NEGATIVE_ONE_TO_ONE
	return perspectiveFovLH_NO(fov, width, height, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_RIGHT_HAND_ZERO_TO_ONE
	return perspectiveFovRH_ZO(fov, width, height, zNear, zFar);
#elif JINMATH_CURRENT_SETTING == JINMATH_RIGHT_HAND_NEGATIVE_ONE_TO_ONE
	return perspectiveFovRH_NO(fov, width, height, zNear, zFar);
#endif
}

math::Vector3 math::projectZeroToOne(const math::Vector3& obj, const math::Matrix4x4& model,
	const math::Matrix4x4& proj, const math::Vector4& viewport)
{
	math::Vector4 tmp = math::Vector4(obj, static_cast<float>(1));
	tmp = model * tmp;
	tmp = proj * tmp;

	tmp /= tmp.w;
	tmp.x = tmp.x * static_cast<float>(0.5) + static_cast<float>(0.5);
	tmp.y = tmp.y * static_cast<float>(0.5) + static_cast<float>(0.5);

	tmp[0] = tmp[0] * float(viewport[2]) + float(viewport[0]);
	tmp[1] = tmp[1] * float(viewport[3]) + float(viewport[1]);

	return math::Vector3(tmp);
}

math::Vector3 math::projectNOneToOne(const math::Vector3& obj, const math::Matrix4x4& model,
	const math::Matrix4x4& proj, const math::Vector4& viewport)
{
	math::Vector4 tmp = math::Vector4(obj, static_cast<float>(1));
	tmp = model * tmp;
	tmp = proj * tmp;

	tmp /= tmp.w;
	tmp = tmp * static_cast<float>(0.5) + static_cast<float>(0.5);
	tmp[0] = tmp[0] * float(viewport[2]) + float(viewport[0]);
	tmp[1] = tmp[1] * float(viewport[3]) + float(viewport[1]);

	return math::Vector3(tmp);
}

math::Vector3 math::project(const math::Vector3& obj, const math::Matrix4x4& model, const math::Matrix4x4& proj,
	const math::Vector4& viewport)
{
#if JINMATH_CURRENT_CLIP_RANGE == JINMATH_CLIP_RANGE_NEGATIVE_ONE_TO_ONE
	return projectZeroToOne(obj, model, proj, viewport);
#else
	return projectNOneToOne(obj, model, proj, viewport);
#endif
}

math::Matrix4x4 math::rotate(const math::Matrix4x4& m, const float& angle, const math::Vector3& v)
{
	const float a = angle;
	const float c = std::cos(a);
	const float s = std::sin(a);

	math::Vector3 axis(v.normalized());
	math::Vector3 temp((float(1) - c) * axis);

	math::Matrix4x4 Rotate{};
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	math::Matrix4x4 Result{};
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	Result[3] = m[3];
	return Result;
}

math::Matrix4x4 math::rotate(const float& angle, const math::Vector3& v)
{
	return math::rotate(Matrix4x4::identify, angle, v);
}

math::Matrix4x4 math::scale(const math::Matrix4x4& m, const float x, const float y, const float z)
{
	math::Matrix4x4 Result{};
	Result[0] = m[0] * x;
	Result[1] = m[1] * y;
	Result[2] = m[2] * z;
	Result[3] = m[3];
	return Result;
}

math::Matrix4x4 math::scale(const math::Matrix4x4& m, const math::Vector3& v)
{
	return scale(m, v.x, v.y, v.z);
}

math::Matrix4x4 math::scale(const math::Vector3& v)
{
	return math::scale(Matrix4x4::identify, v);
}

math::Matrix4x4 math::scale(const float x, const float y, const float z)
{
	return math::scale(Matrix4x4::identify, x, y, z);
}

math::Matrix4x4 math::translate(const Matrix4x4& m, const float x, const float y, const float z)
{
	math::Matrix4x4 Result(m);
	Result[3] = m[0] * x + m[1] *y+ m[2] * z + m[3];
	return Result;
}


math::Matrix4x4 math::translate(const math::Matrix4x4& m, const math::Vector3& v)
{
	return translate(m, v.x, v.y, v.z);
}

math::Matrix4x4 math::translate(const math::Vector3& v)
{
	return math::translate(Matrix4x4::identify, v);
}

math::Matrix4x4 math::translate(const float x, const float y, const float z)
{
	return math::translate(Matrix4x4::identify, x, y, z);
}