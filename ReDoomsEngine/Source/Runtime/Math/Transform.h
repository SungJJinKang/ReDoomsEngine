#pragma once
#include "CommonInclude.h"

enum ESpace
{
	World,
	Self
};

struct FTransform
{
	Vector3 Position;
	Vector3 Scale;
	Quaternion Rotation;

	inline Vector3 Up()
	{
		return Vector3::Transform(Vector3::Up, Rotation);
	}

	inline Vector3 Right()
	{
		return Vector3::Transform(Vector3::Right, Rotation);
	}

	inline Vector3 Forward()
	{
		return Vector3::Transform(Vector3::Forward, Rotation);
	}

	void LookAt(const FTransform& Target, const Vector3& Up);
	void LookAt(const Vector3& TargetPoint, const Vector3& Up);
	void Rotate(const Quaternion& Quat, const ESpace& RelativeTo);
	void Rotate(const Vector3 EulerAngle, const ESpace& RelativeTo);
	void RotateYaw(const float EulerAngle);
	void RotatePitch(const float EulerAngle);
	void RotateAround(const Vector3& CenterPoint, const Vector3& Axis, const float Angle);

	Vector3 TransformDirection(Vector3 Direction) const
	{
		Direction.Normalize();
		return Vector3::Transform(Direction, Rotation);
	}

	Vector3 TransformPoint(const Vector3& Point) const
	{
		return Vector3::Transform(Point, Rotation);
	}

	Vector3 TransformVector(const Vector3& Vector) const
	{
		return Vector3::Transform(Vector, Rotation);
	}

	void Translate(const Vector3& Translation, const ESpace& RelativeTo)
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

