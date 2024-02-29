#pragma once

#include "CommonInclude.h"

#include "Math/Transform.h"

class FView
{
public:

	FView();

	Matrix Get3DViewMatrices() const;
	Matrix GetPerspectiveProjectionMatrix() const;
	Matrix GetViewPerspectiveProjectionMatrix() const;
	Matrix GetOrthoProjMatrices() const;
	void Reset();

	FTransform Transform;

	float NearPlane = 0.01f;
	float FarPlane = 12000.0f;
	float FovInDegree = 90.0f;
	float ScreenWidth;
	float ScreenHeight;

private:

};

