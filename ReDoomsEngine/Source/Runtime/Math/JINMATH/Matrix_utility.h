#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Utility.h"

namespace math
{
	
	math::Matrix4x4 frustum(const float& left, const float& right, const float& bottom, const float& top, const float& nearVal, const float& farVal);
	math::Matrix4x4 infinitePerspective(float fovy, float aspect, float zNear);
	math::Matrix4x4 lookAtRH(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up);
	math::Matrix4x4 lookAtLH(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up);
	math::Matrix4x4 lookAt(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up);
	math::Matrix4x4 orthoLH_ZO(float left, float right, float bottom, float top, float zNear, float zFar);
	math::Matrix4x4 orthoLH_NO(float left, float right, float bottom, float top, float zNear, float zFar);
	math::Matrix4x4 orthoRH_ZO(float left, float right, float bottom, float top, float zNear, float zFar);
	math::Matrix4x4 orthoRH_NO(float left, float right, float bottom, float top, float zNear, float zFar);
	math::Matrix4x4 ortho(const float& left, const float& right, const float& bottom, const float& top);
	math::Matrix4x4 ortho(float left, float right, float bottom, float top, float zNear, float zFar);
	math::Matrix4x4 perspectiveRH_ZO(float fovy, float aspect, float zNear, float zFar);
	math::Matrix4x4 perspectiveRH_NO(float fovy, float aspect, float zNear, float zFar);
	math::Matrix4x4 perspectiveLH_ZO(float fovy, float aspect, float zNear, float zFar);
	math::Matrix4x4 perspectiveLH_NO(float fovy, float aspect, float zNear, float zFar);
	math::Matrix4x4 perspective(const float& fovy, const float& aspect, const float& zNear, const float& zFar);
	math::Matrix4x4 perspectiveFovRH_ZO(float fov, float width, float height, float zNear, float zFar);
	math::Matrix4x4 perspectiveFovRH_NO(float fov, float width, float height, float zNear, float zFar);
	math::Matrix4x4 perspectiveFovLH_ZO(float fov, float width, float height, float zNear, float zFar);
	math::Matrix4x4 perspectiveFovLH_NO(float fov, float width, float height, float zNear, float zFar);
	math::Matrix4x4 perspectiveFov(float fov, float width, float height, float zNear, float zFar);

	/*
	template<typename float, typename U>
	math::Matrix4x4 pickMatrix(const math::Vector2& center, const math::Vector2& delta, const math::Vector4<U>& viewport);
	*/

	
	math::Vector3 projectZeroToOne(const math::Vector3& obj, const math::Matrix4x4& model, const math::Matrix4x4& proj, const math::Vector4& viewport);
	math::Vector3 projectNOneToOne(const math::Vector3& obj, const math::Matrix4x4& model, const math::Matrix4x4& proj, const math::Vector4& viewport);
	math::Vector3 project(const math::Vector3& obj, const math::Matrix4x4& model, const math::Matrix4x4& proj, const math::Vector4& viewport);
	math::Matrix4x4 rotate(const math::Matrix4x4& m, const float& angle, const math::Vector3& v);
	math::Matrix4x4 rotate(const float& angle, const math::Vector3& v);
	math::Matrix4x4 scale(const math::Matrix4x4& m, const float x, const float y, const float z);
	math::Matrix4x4 scale(const math::Matrix4x4& m, const math::Vector3& v);
	math::Matrix4x4 scale(const math::Vector3& v);
	math::Matrix4x4 scale(const float x, const float y, const float z);
	math::Matrix4x4 translate(const math::Matrix4x4& m, const float x, const float y, const float z);
	math::Matrix4x4 translate(const math::Matrix4x4& m, const math::Vector3& v);
	math::Matrix4x4 translate(const math::Vector3& v);
	math::Matrix4x4 translate(const float x, const float y, const float z);

	/*
	
	math::Matrix4x4 tweakedInfinitePerspective(float fovy, float aspect, float zNear);

	template<typename float, typename U>
	math::Vector3 unProject(const math::Vector3& win, const math::Matrix4x4& model, const math::Matrix4x4& proj, const math::Vector4<U>& viewport);
	*/
}