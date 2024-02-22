#include "Transform.h"

void FTransform::LookAt(const FTransform& Target, const Vector3& Up)
{
	Vector3 NormalizedVector;
	(Target.Position - Position).Normalize(NormalizedVector);
	Rotation = Quaternion::LookRotation(NormalizedVector, Up);
}

void FTransform::LookAt(const Vector3& TargetPoint, const Vector3& Up)
{
	Vector3 NormalizedVector;
	(TargetPoint - Position).Normalize(NormalizedVector);
	Rotation = Quaternion::LookRotation(NormalizedVector, Up);
}

void FTransform::Rotate(const Quaternion& Quat, const ESpace& RelativeTo)
{
	if (RelativeTo == ESpace::Self)
	{
		Rotation = Rotation * Quat;
	}
	else if (RelativeTo == ESpace::World)
	{
		Rotation = Quat * Rotation;
	}
}

void FTransform::Rotate(const Vector3 EulerAngle, const ESpace& RelativeTo)
{
	Rotate(EulerAngleToQuaternion(EulerAngle), RelativeTo);
}

void FTransform::RotateYaw(const float EulerAngle)
{
	Rotation = Quaternion::CreateFromAxisAngle(Up(), EulerAngle);
}

void FTransform::RotatePitch(const float EulerAngle)
{
	Rotation = Quaternion::CreateFromAxisAngle(Right(), EulerAngle);
}

void FTransform::RotateAround(const Vector3& CenterPoint, const Vector3& Axis, const float Angle)
{
	Vector3 WorldPosition = Position;
	const Quaternion q = Quaternion::CreateFromAxisAngle(Axis, Angle);
	Vector3 Diff = WorldPosition - CenterPoint;
	Diff = q * Diff;
	WorldPosition = CenterPoint + Diff;
	Position = WorldPosition;

	Quaternion::CreateFromAxisAngle(Axis, Angle);
	Rotation = Quaternion{ Axis, Angle };
}
