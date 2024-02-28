#include "Common.h"

#include "EAStdC/EAMathHelp.h"

const float culling::PI = 3.14159265358979323846f;
const float culling::DEGREE_TO_RADIAN = PI / 180.0f;
const float culling::RADIAN_TO_DEGREE = 180.0f / PI;

void culling::NormalizePlane(Vec4& plane) noexcept
{
	float mag = sqrtf(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
	EA_ASSERT(EA::StdC::IsNAN(mag) == false);

	plane[0] = plane[0] / mag;
	plane[1] = plane[1] / mag;
	plane[2] = plane[2] / mag;
	plane[3] = plane[3] / mag;
}

void culling::ExtractPlanesFromVIewProjectionMatrix(const Mat4x4& viewProjectionMatrix, Vec4* sixPlanes,
	bool normalize) noexcept
{
	sixPlanes[0][0] = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
	sixPlanes[0][1] = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
	sixPlanes[0][2] = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
	sixPlanes[0][3] = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];
	// Right clipping plane
	sixPlanes[1][0] = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
	sixPlanes[1][1] = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
	sixPlanes[1][2] = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
	sixPlanes[1][3] = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];
	// Top clipping plane
	sixPlanes[2][0] = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
	sixPlanes[2][1] = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
	sixPlanes[2][2] = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
	sixPlanes[2][3] = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];
	// Bottom clipping plane
	sixPlanes[3][0] = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
	sixPlanes[3][1] = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
	sixPlanes[3][2] = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
	sixPlanes[3][3] = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];
	// Near clipping plane
	sixPlanes[4][0] = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
	sixPlanes[4][1] = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
	sixPlanes[4][2] = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
	sixPlanes[4][3] = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];
	// Far clipping plane
	sixPlanes[5][0] = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
	sixPlanes[5][1] = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
	sixPlanes[5][2] = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
	sixPlanes[5][3] = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

	// Normalize the plane equations, if requested
	if (normalize == true)
	{
		for (size_t i = 0; i < 6; i++)
		{
			NormalizePlane(sixPlanes[i]);
		}
	}
}

void culling::ExtractSIMDPlanesFromViewProjectionMatrix(const Mat4x4& viewProjectionMatrix, Vec4* eightPlanes,
	bool normalize) noexcept
{

	Vec4 sixPlane[6];

	ExtractPlanesFromVIewProjectionMatrix(viewProjectionMatrix, sixPlane, normalize);

	eightPlanes[0][0] = sixPlane[0][0];
	eightPlanes[0][1] = sixPlane[1][0];
	eightPlanes[0][2] = sixPlane[2][0];
	eightPlanes[0][3] = sixPlane[3][0];

	eightPlanes[1][0] = sixPlane[0][1];
	eightPlanes[1][1] = sixPlane[1][1];
	eightPlanes[1][2] = sixPlane[2][1];
	eightPlanes[1][3] = sixPlane[3][1];

	eightPlanes[2][0] = sixPlane[0][2];
	eightPlanes[2][1] = sixPlane[1][2];
	eightPlanes[2][2] = sixPlane[2][2];
	eightPlanes[2][3] = sixPlane[3][2];

	eightPlanes[3][0] = sixPlane[0][3];
	eightPlanes[3][1] = sixPlane[1][3];
	eightPlanes[3][2] = sixPlane[2][3];
	eightPlanes[3][3] = sixPlane[3][3];

	eightPlanes[4][0] = sixPlane[4][0];
	eightPlanes[4][1] = sixPlane[5][0];
	eightPlanes[4][2] = sixPlane[4][0];
	eightPlanes[4][3] = sixPlane[5][0];

	eightPlanes[5][0] = sixPlane[4][1];
	eightPlanes[5][1] = sixPlane[5][1];
	eightPlanes[5][2] = sixPlane[4][1];
	eightPlanes[5][3] = sixPlane[5][1];

	eightPlanes[6][0] = sixPlane[4][2];
	eightPlanes[6][1] = sixPlane[5][2];
	eightPlanes[6][2] = sixPlane[4][2];
	eightPlanes[6][3] = sixPlane[5][2];

	eightPlanes[7][0] = sixPlane[4][3];
	eightPlanes[7][1] = sixPlane[5][3];
	eightPlanes[7][2] = sixPlane[4][3];
	eightPlanes[7][3] = sixPlane[5][3];

}

culling::Vec4 culling::operator*(const Mat4x4& mat4, const Vec3& vec3) noexcept
{
	return Vec4
	{
			mat4[0][0] * vec3.x + mat4[1][0] * vec3.y + mat4[2][0] * vec3.z + mat4[3][0],
			mat4[0][1] * vec3.x + mat4[1][1] * vec3.y + mat4[2][1] * vec3.z + mat4[3][1],
			mat4[0][2] * vec3.x + mat4[1][2] * vec3.y + mat4[2][2] * vec3.z + mat4[3][2],
			mat4[0][3] * vec3.x + mat4[1][3] * vec3.y + mat4[2][3] * vec3.z + mat4[3][3],
	};
}

culling::Mat4x4 culling::operator*(const Mat4x4& mat4_A, const Mat4x4& mat4_B) noexcept
{
	culling::EVERYCULLING_M256F _REULST_MAT4[2];
	culling::EVERYCULLING_M128F TEMP_M128F;

	const culling::EVERYCULLING_M128F* const A = reinterpret_cast<const culling::EVERYCULLING_M128F*>(mat4_A.data());
	//const EVERYCULLING_M128F* A = (const EVERYCULLING_M128F*)this->data(); // this is slower
	const culling::EVERYCULLING_M128F* const B = reinterpret_cast<const culling::EVERYCULLING_M128F*>(mat4_B.data());
	culling::EVERYCULLING_M128F* R = reinterpret_cast<culling::EVERYCULLING_M128F*>(&_REULST_MAT4);

	// First row of result (Matrix1[0] * Matrix2).
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL(EVERYCULLING_M128F_REPLICATE(B[0], 0), A[0]);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[0], 1), A[1], TEMP_M128F);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[0], 2), A[2], TEMP_M128F);
	R[0] = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[0], 3), A[3], TEMP_M128F);

	// Second row of result (Matrix1[1] * Matrix2).
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL(EVERYCULLING_M128F_REPLICATE(B[1], 0), A[0]);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[1], 1), A[1], TEMP_M128F);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[1], 2), A[2], TEMP_M128F);
	R[1] = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[1], 3), A[3], TEMP_M128F);

	// Third row of result (Matrix1[2] * Matrix2).
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL(EVERYCULLING_M128F_REPLICATE(B[2], 0), A[0]);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[2], 1), A[1], TEMP_M128F);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[2], 2), A[2], TEMP_M128F);
	R[2] = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[2], 3), A[3], TEMP_M128F);

	// Fourth row of result (Matrix1[3] * Matrix2).
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL(EVERYCULLING_M128F_REPLICATE(B[3], 0), A[0]);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[3], 1), A[1], TEMP_M128F);
	TEMP_M128F = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[3], 2), A[2], TEMP_M128F);
	R[3] = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(B[3], 3), A[3], TEMP_M128F);

	return culling::Mat4x4{ *reinterpret_cast<culling::Mat4x4*>(&_REULST_MAT4) };
}

culling::Vec4 culling::operator*(const Mat4x4& mat4, const Vec4& vec4) noexcept
{
	culling::EVERYCULLING_M128F tempVec4;

	const culling::EVERYCULLING_M128F* A = reinterpret_cast<const culling::EVERYCULLING_M128F*>(&mat4);
	const culling::EVERYCULLING_M128F* B = reinterpret_cast<const culling::EVERYCULLING_M128F*>(&vec4);
	culling::EVERYCULLING_M128F* R = reinterpret_cast<culling::EVERYCULLING_M128F*>(&tempVec4);

	// First row of result (Matrix1[0] * Matrix2).
	*R = culling::EVERYCULLING_M128F_MUL(EVERYCULLING_M128F_REPLICATE(*B, 0), A[0]);
	*R = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(*B, 1), A[1], *R);
	*R = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(*B, 2), A[2], *R);
	*R = culling::EVERYCULLING_M128F_MUL_AND_ADD(EVERYCULLING_M128F_REPLICATE(*B, 3), A[3], *R);

	return Vec4{ *(Vec4*)(&tempVec4) };
}
