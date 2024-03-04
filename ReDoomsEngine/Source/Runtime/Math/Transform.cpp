#include "Transform.h"

void FTransform::LookAt(const FTransform& Target, const math::Vector3& Up)
{
	math::Vector3 NormalizedVector = (Target.Position - Position).normalized();
	Rotation = math::Quaternion::LookAt(NormalizedVector, Up);
}

void FTransform::LookAt(const math::Vector3& TargetPoint, const math::Vector3& Up)
{
	math::Vector3 NormalizedVector = (TargetPoint - Position).normalized();
	Rotation = math::Quaternion::LookAt(NormalizedVector, Up);
}

void FTransform::Rotate(const math::Quaternion& Quat, const ESpace RelativeTo)
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

void FTransform::Rotate(const math::Vector3 EulerAngle, const ESpace RelativeTo)
{
	Rotate(math::Quaternion::EulerAngleToQuaternion(EulerAngle.x, EulerAngle.y, EulerAngle.z), RelativeTo);
}

void FTransform::RotateYaw(const float EulerAngle, const ESpace RelativeTo)
{
	Rotate(math::Quaternion::EulerAngleToQuaternion(0.0f, EulerAngle, 0.0f), RelativeTo);
}

void FTransform::RotatePitch(const float EulerAngle, const ESpace RelativeTo)
{
	Rotate(math::Quaternion::EulerAngleToQuaternion(EulerAngle, 0.0f, 0.0f), RelativeTo);
}

void FTransform::RotateAround(const math::Vector3& CenterPoint, const math::Vector3& Axis, const float Angle)
{
	math::Vector3 WorldPosition = Position;
	const math::Quaternion q = math::Quaternion::CreateFromAxisAngle(Angle, Axis);
	math::Vector3 Diff = WorldPosition - CenterPoint;
	Diff = q * Diff;
	WorldPosition = CenterPoint + Diff;
	Position = WorldPosition;
	Rotation = math::Quaternion::CreateFromAxisAngle(Angle, Axis);;
}
