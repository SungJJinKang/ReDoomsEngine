#pragma once

#include "CommonInclude.h"

#include "Math/Transform.h"

class FView
{
public:

	FView();

	math::Matrix4x4 Get3DViewMatrices() const;
	math::Matrix4x4 GetPerspectiveProjectionMatrix() const;
	math::Matrix4x4 GetViewPerspectiveProjectionMatrix() const;
	math::Matrix4x4 GetOrthoProjMatrices() const;
	void Reset();

	FTransform Transform;

	float NearPlane = 0.01f;
	float FarPlane = 12000.0f;
	float FovInDegree = 90.0f;
	float ScreenWidth;
	float ScreenHeight;

private:

};

