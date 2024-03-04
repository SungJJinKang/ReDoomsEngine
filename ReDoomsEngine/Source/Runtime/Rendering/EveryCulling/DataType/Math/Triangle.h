#pragma once

#include "Common.h"
#include "Vector.h"

#include "EveryCullingSIMDCore.h"

namespace culling
{
	struct TwoDTriangle
	{
		Vec2 Points[3];
	};

	struct ThreeDTriangle
	{
		Vec3 Points[3];
	};


	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vec2
	/// </summary>
	/// <param name="triangle"></param>

	/// <summary>
	/// Sort TwoDTriangle Points y ascending.
	/// Point1 is TopMost Vec2
	/// </summary>
	/// <param name="triangle"></param>
	void SortTriangle(TwoDTriangle& triangle);

	void SortTriangle(ThreeDTriangle& triangle);

	bool IsFrontFaceOfProjectSpaceTriangle(const TwoDTriangle& triangle);


	/// <summary>
	/// 
	/// verticesX[3]
	/// verticesX[0] : Triangle1_Vertex1_X Triangle2_Vertex1_X Triangle3_Vertex1_X Triangle4_Vertex1_X
	/// verticesX[1] : Triangle1_Vertex2_X Triangle2_Vertex2_X Triangle3_Vertex2_X Triangle4_Vertex2_X
	/// verticesX[2] : Triangle1_Vertex3_X Triangle2_Vertex3_X Triangle3_Vertex3_X Triangle4_Vertex3_X
	///
	/// verticesY[3]
	/// verticesY[0] : Triangle1_Vertex1_Y Triangle2_Vertex1_Y Triangle3_Vertex1_Y Triangle4_Vertex1_Y
	/// verticesY[1] : Triangle1_Vertex2_Y Triangle2_Vertex2_Y Triangle3_Vertex2_Y Triangle4_Vertex2_Y
	/// verticesY[2] : Triangle1_Vertex3_Y Triangle2_Vertex3_Y Triangle3_Vertex3_Y Triangle4_Vertex3_Y
	/// 
	/// </summary>
	/// <param name="vertexX"></param>
	/// <param name="vertexY"></param>
	/// <returns></returns>
	uint32_t TestTrianglesIsFrontFaceUsingSIMD
	(
		const culling::EVERYCULLING_M256F* const verticesX,
		const culling::EVERYCULLING_M256F* const verticesY
	);

	

	/// <summary>
	/// 
	/// Sort TwoDTriangle Points y ascending.
	/// TriPoint[0] is TopMost
	/// 
	/// TriPoint[0] : First Points Of Triangle
	/// TriPoint[1] : Second Points Of Triangle
	/// TriPoint[2] : Third Points Of Triangle
	/// 
	/// Passed culling::EVERYCULLING_M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX"></param>
	/// <param name="TriPointY"></param>
	void Sort_8_2DTriangles(culling::EVERYCULLING_M256F* TriPointX, culling::EVERYCULLING_M256F* TriPointY);

	/// <summary>
	/// 
	/// Sort TwoDTriangle Points y ascending.
	/// TriPoint[0] is TopMost
	/// 
	/// TriPoint[0] : First Points Of Triangle
	/// TriPoint[1] : Second Points Of Triangle
	/// TriPoint[2] : Third Points Of Triangle
	/// 
	/// Passed culling::EVERYCULLING_M256F* is pointer of 3 size array
	/// 
	/// </summary>
	/// <param name="TriPointX">3 size array culling::EVERYCULLING_M256F</param>
	/// <param name="TriPointY">3 size array culling::EVERYCULLING_M256F</param>
	/// <param name="TriPointZ">3 size array culling::EVERYCULLING_M256F</param>
	void Sort_8_3DTriangles(culling::EVERYCULLING_M256F* TriPointX, culling::EVERYCULLING_M256F* TriPointY, culling::EVERYCULLING_M256F* TriPointZ);

	void Sort_8_2DTriangles
	(
		culling::EVERYCULLING_M256F& TriPointA_X,
		culling::EVERYCULLING_M256F& TriPointA_Y,

		culling::EVERYCULLING_M256F& TriPointB_X,
		culling::EVERYCULLING_M256F& TriPointB_Y,

		culling::EVERYCULLING_M256F& TriPointC_X,
		culling::EVERYCULLING_M256F& TriPointC_Y
	);

	void Sort_8_3DTriangles
	(
		culling::EVERYCULLING_M256F& TriPointA_X,
		culling::EVERYCULLING_M256F& TriPointA_Y,
		culling::EVERYCULLING_M256F& TriPointA_Z,

		culling::EVERYCULLING_M256F& TriPointB_X,
		culling::EVERYCULLING_M256F& TriPointB_Y,
		culling::EVERYCULLING_M256F& TriPointB_Z,

		culling::EVERYCULLING_M256F& TriPointC_X,
		culling::EVERYCULLING_M256F& TriPointC_Y,
		culling::EVERYCULLING_M256F& TriPointC_Z
	);

	void Sort_8_3DTriangles
	(
		culling::EVERYCULLING_M256F& TriPointA_X,
		culling::EVERYCULLING_M256F& TriPointA_Y,
		culling::EVERYCULLING_M256F& TriPointA_Z,
		
		culling::EVERYCULLING_M256F& TriPointB_X,
		culling::EVERYCULLING_M256F& TriPointB_Y,
		culling::EVERYCULLING_M256F& TriPointB_Z,
		
		culling::EVERYCULLING_M256F& TriPointC_X,
		culling::EVERYCULLING_M256F& TriPointC_Y,
		culling::EVERYCULLING_M256F& TriPointC_Z,

		culling::EVERYCULLING_M256I& mask
	);
}