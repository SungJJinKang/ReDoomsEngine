#include "View.h"
#include "Utils/ConsoleVariable.h"

static TConsoleVariable<float> GNearPlane{ "r.NearPlane", 0.01f };
static TConsoleVariable<float> GFarPlane{ "r.FarPlane", 8000.0f };

FView::FView()
{
	Reset();
}

Matrix FView::Get3DViewMatrices()
{
	const Vector3& Eye = Transform.Position;
	const Vector3 Forward = Transform.Forward();
	const Vector3 Up = Transform.Up();
	return Matrix::CreateLookAt(Eye, Eye + Forward, Up);
}

Matrix FView::GetPerspectiveProjectionMatrix(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight)
{
	float AspectRatio = (float)InScreenWidth / (float)InScreenHeight;
	float FovAngleY = InFovInDegrees * XM_PI / 180.0f;

	if (AspectRatio < 1.0f)
	{
		FovAngleY /= AspectRatio;
	}

	return Matrix::CreatePerspectiveFieldOfView(FovAngleY, AspectRatio, GNearPlane, GFarPlane);
}

Matrix FView::GetViewPerspectiveProjectionMatrix(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight)
{
	return Get3DViewMatrices() * GetPerspectiveProjectionMatrix(InFovInDegrees, InScreenWidth, InScreenHeight);
}

Matrix FView::GetOrthoProjMatrices(const float InWidth, const float InHeight)
{
	return Matrix::CreateOrthographic(InWidth, InHeight, GNearPlane, GFarPlane);
}

void FView::Reset()
{
	Transform.Position = Vector3{ 0.0f, 0.0f, 10.0f };
	Transform.Scale = Vector3{ 1.0f, 1.0f, 1.0f };
	Transform.Rotation = Quaternion::LookRotation(Vector3::Forward, Vector3::Up);
}