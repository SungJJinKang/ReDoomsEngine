#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/SIMD_Core.h"
#include "../SWDepthBuffer.h"

#define FLOAT_OVERFLOW_CHECKER (float)2147483645.0f

namespace culling
{
	namespace depthBufferTileHelper
	{
		void ComputeBinBoundingBoxFromThreeVertices
		(
			const culling::EVERYCULLING_M256F& pointAScreenPixelX,
			const culling::EVERYCULLING_M256F& pointAScreenPixelY,

			const culling::EVERYCULLING_M256F& pointBScreenPixelX,
			const culling::EVERYCULLING_M256F& pointBScreenPixelY,

			const culling::EVERYCULLING_M256F& pointCScreenPixelX,
			const culling::EVERYCULLING_M256F& pointCScreenPixelY,

			culling::EVERYCULLING_M256I& outBinBoundingBoxMinX,
			culling::EVERYCULLING_M256I& outBinBoundingBoxMinY,
			culling::EVERYCULLING_M256I& outBinBoundingBoxMaxX,
			culling::EVERYCULLING_M256I& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		);

		

		
	};
}


