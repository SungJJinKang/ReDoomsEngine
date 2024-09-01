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

void FTransform::Rotate(const Quaternion& Quat)
{
	Rotation = Rotation * Quat;
}

void FTransform::Rotate(const Vector3 EulerAngle)
{
	Rotate(Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(EulerAngle.y), XMConvertToRadians(EulerAngle.x), XMConvertToRadians(EulerAngle.z)));
}

void FTransform::RotateYaw(const float EulerAngle, const ESpace& RelativeTo)
{
	Rotate( Quaternion::CreateFromAxisAngle((RelativeTo == ESpace::World) ? Vector3::Up : Up(), EulerAngle));
}

void FTransform::RotatePitch(const float EulerAngle, const ESpace& RelativeTo)
{
	Rotate(Quaternion::CreateFromAxisAngle((RelativeTo == ESpace::World) ? Vector3::Right : Right(), EulerAngle));
}

void FTransform::RotateRoll(const float EulerAngle, const ESpace& RelativeTo)
{
	Rotate(Quaternion::CreateFromAxisAngle((RelativeTo == ESpace::World) ? Vector3::Forward : Forward(), EulerAngle));
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
