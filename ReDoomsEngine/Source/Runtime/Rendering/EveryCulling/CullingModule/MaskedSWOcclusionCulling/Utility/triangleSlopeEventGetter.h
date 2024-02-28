#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/Vector.h"
namespace culling
{
	namespace triangleSlopeHelper
	{
		void GatherBottomFlatTriangleSlopeEvent
		(
			const culling::Vec2& TileLeftBottomOriginPoint,
			culling::EVERYCULLING_M256I& leftFaceEvent,
			culling::EVERYCULLING_M256I& rightFaceEvent,
			const float TriPointA_X,
			const float TriPointA_Y,

			const float TriPointB_X,
			const float TriPointB_Y,

			const float TriPointC_X,
			const float TriPointC_Y

		);

		void GatherTopFlatTriangleSlopeEvent
		(
			const size_t triangleCount,
			const culling::Vec2& TileLeftBottomOriginPoint,
			culling::EVERYCULLING_M256I& leftFaceEvent,
			culling::EVERYCULLING_M256I& rightFaceEvent,
			const float TriPointA_X,
			const float TriPointA_Y,

			const float TriPointB_X,
			const float TriPointB_Y,

			const float TriPointC_X,
			const float TriPointC_Y

		);
	};
}
