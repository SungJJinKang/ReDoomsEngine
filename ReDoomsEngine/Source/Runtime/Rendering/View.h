#pragma once

#include "CommonInclude.h"

class FView
{
public:

	FView();

	XMFLOAT4X4 Get3DViewProjMatrices(const float InFovInDegrees, const float InScreenWidth, const float InScreenHeight);
	void Reset();
	void Set(const XMVECTOR& InEye, const XMVECTOR& InAt, const XMVECTOR& InUp);
	void RotateYaw(float InDegree);
	void RotatePitch(float InDegree);
	XMFLOAT4X4 GetOrthoProjMatrices(const float InWidth, const float InHeight);

private:

	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up;
};

