#include "View.h"

FView::FView()
{
	Reset();
}

XMFLOAT4X4 FView::Get3DViewMatrices()
{
	XMFLOAT4X4 Result;
	XMStoreFloat4x4(&Result, XMMatrixLookAtRH(Eye, LookAt, Up));
	return Result;
}

XMFLOAT4X4 FView::Get3DProjMatrices(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight)
{
	float AspectRatio = (float)InScreenWidth / (float)InScreenHeight;
	float FovAngleY = InFovInDegrees * XM_PI / 180.0f;

	if (AspectRatio < 1.0f)
	{
		FovAngleY /= AspectRatio;
	}

	XMFLOAT4X4 Result;
	XMStoreFloat4x4(&Result, XMMatrixPerspectiveFovRH(FovAngleY, AspectRatio, 0.01f, 125.0f));
	return Result;
}

XMFLOAT4X4 FView::Get3DViewProjMatrices(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight)
{
	float AspectRatio = (float)InScreenWidth / (float)InScreenHeight;
	float FovAngleY = InFovInDegrees * XM_PI / 180.0f;

	if (AspectRatio < 1.0f)
	{
		FovAngleY /= AspectRatio;
	}

	XMFLOAT4X4 Result;
	XMStoreFloat4x4(&Result, XMMatrixMultiply(XMMatrixLookAtLH(Eye, Eye + LookAt, Up), XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, 0.01f, 125.0f)));
	return Result;
}

XMFLOAT4X4 FView::GetOrthoProjMatrices(const float InWidth, const float InHeight)
{
	XMFLOAT4X4 Result;
	XMStoreFloat4x4(&Result, XMMatrixMultiply(XMMatrixLookAtLH(Eye, Eye + LookAt, Up), XMMatrixOrthographicLH(InWidth, InHeight, 0.01f, 125.0f)));
	return Result;
}
void FView::RotateYaw(float InDegree)
{
	XMMATRIX Rotation = XMMatrixRotationAxis(Up, InDegree);

	Eye = XMVector3TransformCoord(Eye, Rotation);
}

void FView::RotatePitch(float InDegree)
{
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(Eye, Up));
	XMMATRIX Rotation = XMMatrixRotationAxis(Right, InDegree);

	Eye = XMVector3TransformCoord(Eye, Rotation);
}

void FView::Translate(XMVECTOR Value)
{
	Eye += Value;
}

void FView::Reset()
{
	Eye = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	LookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

void FView::Set(const XMVECTOR& InEye, const XMVECTOR& InAt, const XMVECTOR& InUp)
{
	Eye = InEye;
	LookAt = InAt;
	Up = InUp;
}