#pragma once

#include "../SWDepthBuffer.h"
#include "../../../DataType/Math/Vector.h"
#include "../../../DataType/Math/Triangle.h"

static_assert(EVERYCULLING_TILE_WIDTH % 16 == 0);
static_assert(EVERYCULLING_TILE_HEIGHT % 8 == 0);
static_assert(EVERYCULLING_TILE_WIDTH * EVERYCULLING_TILE_HEIGHT == 128 || EVERYCULLING_TILE_WIDTH * EVERYCULLING_TILE_HEIGHT == 256);

namespace culling
{
	namespace CoverageRasterizer
	{
		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		culling::EVERYCULLING_M256I FillBottomFlatTriangle
		(
			const Vec2& TileLeftBottomOriginPoint, 
			const Vec2& point1, 
			const Vec2& point2, 
			const Vec2& point3
		);
		
		/// <summary>
		/// Assume TwoDTriangle intersect with mask
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <param name="triangle"></param>
		culling::EVERYCULLING_M256I FillTopFlatTriangle
		(
			const Vec2& TileLeftBottomOriginPoint, 
			const Vec2& point1, 
			const Vec2& point2, 
			const Vec2& point3
		);

		

		/// <summary>
		/// https://dl.acm.org/doi/pdf/10.1145/800250.807490?casa_token=alcaBmG1OpoAAAAA:a7Wktjv1YCEp-IODF_dRPbZNDVkRQxPGz67vvDGBfseW6UayupLQM8JizEuMdHT22ymouD-ExDVNPmU
		/// 4page
		/// 
		/// 
		/// </summary>
		culling::EVERYCULLING_M256I FillTriangle
		(
			const Vec2& TileLeftBottomOriginPoint, 
			const culling::Vec2& triangleVertex1,
			const culling::Vec2& triangleVertex2,
			const culling::Vec2& triangleVertex3
		);
		culling::EVERYCULLING_M256I FillTriangle
		(
			const Vec2& LeftBottomPoint, 
			TwoDTriangle& triangle
		);
		culling::EVERYCULLING_M256I FillTriangle
		(
			const Vec2& LeftBottomPoint, 
			ThreeDTriangle& triangle
		);

		
		void FillFlatTriangleBatch
		(
			culling::EVERYCULLING_M256I& outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& TileLeftBottomOriginPoint,

			const culling::EVERYCULLING_M256I& leftFaceEvent,
			const culling::EVERYCULLING_M256I& rightFaceEvent,

			const float bottomEdgeY,
			const float topEdgeY
		);
		/*
		void FillTriangleBatch
		(
			const size_t triangleCount,
			culling::EVERYCULLING_M256I* const outCoverageMask, // 8 coverage mask. array size should be 8
			const Vec2& LeftBottomPoint,
			culling::EVERYCULLING_M256F& TriPointA_X,
			culling::EVERYCULLING_M256F& TriPointA_Y,

			culling::EVERYCULLING_M256F& TriPointB_X,
			culling::EVERYCULLING_M256F& TriPointB_Y,

			culling::EVERYCULLING_M256F& TriPointC_X,
			culling::EVERYCULLING_M256F& TriPointC_Y
		);
		*/

		
	};
}


