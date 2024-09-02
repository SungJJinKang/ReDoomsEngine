#pragma once

#include "CommonInclude.h"

#include "Math/Transform.h"

class FView
{
public:

	FView();

	Matrix Get3DViewMatrices();
	Matrix GetPerspectiveProjectionMatrix(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight);
	Matrix GetViewPerspectiveProjectionMatrix(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight);
	Matrix GetOrthoProjMatrices(const float InWidth, const float InHeight);
	void Reset();

	FTransform Transform;

private:

	float NearPlane = 0.1f;
	float FarPlane = 1000.0f;

};

