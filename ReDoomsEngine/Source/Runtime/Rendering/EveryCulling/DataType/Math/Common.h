#pragma once

#include "../../EveryCullingCore.h"
#include "EveryCullingSIMDCore.h"
#include "Vector.h"
#include "Matrix.h"

namespace culling
{
	extern const float PI;
	extern const float DEGREE_TO_RADIAN;
	extern const float RADIAN_TO_DEGREE;

	void NormalizePlane(AlignedVec4& plane) noexcept;

	/// <summary>
		/// Extract 6 Planes ( 8 Planes ) From View projection matrix
		/// SIMD Version
		///  sixPlanes[0] : x of Plane0, y of Plane0, z of Plane0, w of Plane0
		///  sixPlanes[1] : x of Plane1, y of Plane1, z of Plane1, w of Plane1
		///  sixPlanes[2] : x of Plane2, y of Plane2, z of Plane2, w of Plane2
		///  sixPlanes[3] : x of Plane3, y of Plane3, z of Plane3, w of Plane3
		///  sixPlanes[4] : x of Plane4, y of Plane4, z of Plane4, w of Plane4
		///  sixPlanes[5] : x of Plane5, y of Plane5, z of Plane5, w of Plane5
		/// references :
		/// https://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
		/// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
		/// https://macton.smugmug.com/Other/2008-07-15-by-Eye-Fi/n-xmKDH/i-bJq8JqZ/A
		/// </summary>
	void ExtractPlanesFromVIewProjectionMatrix(const Mat4x4& viewProjectionMatrix, AlignedVec4* sixPlanes, bool normalize) noexcept;

	void ExtractSIMDPlanesFromViewProjectionMatrix(const Mat4x4& viewProjectionMatrix, AlignedVec4* eightPlanes, bool normalize) noexcept;

	AlignedVec4 operator*(const culling::Mat4x4& mat4, const culling::Vec3& vec3) noexcept;

	AlignedVec4 operator*(const culling::Mat4x4& mat4, const culling::AlignedVec4& vec4) noexcept;

	culling::Mat4x4 operator*(const culling::Mat4x4& mat4_A, const culling::Mat4x4& mat4_B) noexcept;

	template <typename T>
	EASTL_FORCE_INLINE void SWAP(T& a, T& b) noexcept
	{
		const T original = a;
		a = b;
		b = original;
	}

	template <typename T>
	EASTL_FORCE_INLINE T CLAMP(const T& value, const T& min, const T& max) noexcept
	{
		EA_ASSERT(min <= max);
		return EVERYCULLING_MIN(EVERYCULLING_MAX(value, min), max);
	}
}