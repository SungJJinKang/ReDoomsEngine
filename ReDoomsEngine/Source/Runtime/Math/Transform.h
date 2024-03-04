#pragma once
#include "CommonInclude.h"

enum ESpace
{
	World,
	Self
};

struct FTransform
{
	math::Vector3 Position;
	math::Vector3 Scale;
	math::Quaternion Rotation;

	inline math::Vector3 Up() const
	{
		return Rotation * math::Vector3::up;
	}

	inline math::Vector3 Right() const
	{
		return Rotation * math::Vector3::right;
	}

	inline math::Vector3 Forward() const
	{
		return Rotation * math::Vector3::forward;
	}

	void LookAt(const FTransform& Target, const math::Vector3& Up);
	void LookAt(const math::Vector3& TargetPoint, const math::Vector3& Up);
	void Rotate(const math::Quaternion& Quat, const ESpace RelativeTo);
	void Rotate(const math::Vector3 EulerAngle, const ESpace RelativeTo);
	void RotateYaw(const float EulerAngle, const ESpace RelativeTo);
	void RotatePitch(const float EulerAngle, const ESpace RelativeTo);
	void RotateAround(const math::Vector3& CenterPoint, const math::Vector3& Axis, const float Angle);

	math::Vector3 TransformDirection(math::Vector3 Direction) const
	{
		Direction.Normalize();
		return Rotation * Direction;
	}

	math::Vector3 TransformPoint(const math::Vector3& Point) const
	{
		return Rotation * Point;
	}

	math::Vector3 TransformVector(const math::Vector3& Vector) const
	{
		return Rotation * Vector;
	}

	void Translate(const math::Vector3& Translation, const ESpace& RelativeTo)
	{
		if (RelativeTo == ESpace::World)
		{
			Position = Position + Translation;
		}
		else if (RelativeTo == ESpace::Self)
		{
			Position = Position + TransformVector(Translation);
		}
	}

};

