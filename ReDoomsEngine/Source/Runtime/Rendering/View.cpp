#include "View.h"

FView::FView()
{
	Reset();
}

Matrix FView::Get3DViewMatrices() const
{
	const Vector3& Eye = Transform.Position;
	const Vector3 Forward = Transform.Forward();
	const Vector3 Up = Transform.Up();
	return Matrix::CreateLookAt(Eye, Eye - Forward, Up);
}

Matrix FView::GetPerspectiveProjectionMatrix() const
{
	float AspectRatio = (float)ScreenWidth / (float)ScreenHeight;
	float FovAngleY = FovInDegree * XM_PI / 180.0f;

	if (AspectRatio < 1.0f)
	{
		FovAngleY /= AspectRatio;
	}

	return Matrix::CreatePerspectiveFieldOfView(FovAngleY, AspectRatio, NearPlane, FarPlane);
}

Matrix FView::GetViewPerspectiveProjectionMatrix() const
{
	return Get3DViewMatrices() * GetPerspectiveProjectionMatrix();
}

Matrix FView::GetOrthoProjMatrices() const
{
	return Matrix::CreateOrthographic(ScreenWidth, ScreenHeight, NearPlane, FarPlane);
}

void FView::Reset()
{
	Transform.Position = Vector3{ 0.0f, 50.0f, 50.0f };
	Transform.Scale = Vector3{ 1.0f, 1.0f, 1.0f };
	Transform.Rotation = Quaternion::LookRotation(Vector3::Forward, Vector3::Up);
	NearPlane = 0.01f;
	FarPlane = 12000.0f;
	FovInDegree = 90.0f;
}