#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/EveryCullingSIMDCore.h"
#include "depthUtility.h"

namespace culling
{
	namespace DepthValueComputer
	{

		/*
		enum class eDepthType
		{
			MinDepth,
			MaxDepth
		};
		*/
		

		void ComputeFlatTriangleMaxDepthValue
		(
			culling::EVERYCULLING_M256F& subTileMaxValues,
			const uint32_t tileOriginX, // 32x8 tile
			const uint32_t tileOriginY, // 32x8 tile

			const float vertexPoint1X, 
			const float vertexPoint1Y,
			const float vertexPoint1Z,

			const float vertexPoint2X,
			const float vertexPoint2Y,
			const float vertexPoint2Z,

			const float vertexPoint3X,
			const float vertexPoint3Y,
			const float vertexPoint3Z,

			const culling::EVERYCULLING_M256I& leftFaceEventOfTriangles, // eight _mm256i
			const culling::EVERYCULLING_M256I& rightFaceEventOfTriangles, // eight _mm256i

			const float minYOfTriangle,
			const float maxYOfTriangle
		);
		

	};
}


