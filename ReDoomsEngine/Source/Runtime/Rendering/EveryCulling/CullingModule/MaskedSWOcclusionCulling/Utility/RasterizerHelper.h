#pragma once

#include "../../../EveryCullingCore.h"

#include "../../../DataType/Math/SIMD_Core.h"

namespace culling
{
	namespace rasterizerHelper
	{
		void GetMiddlePointOfTriangle
		(
			const culling::EVERYCULLING_M256F& TriPointA_X,
			const culling::EVERYCULLING_M256F& TriPointA_Y,
			const culling::EVERYCULLING_M256F& TriPointA_Z,

			const culling::EVERYCULLING_M256F& TriPointB_X,
			const culling::EVERYCULLING_M256F& TriPointB_Y,
			const culling::EVERYCULLING_M256F& TriPointB_Z,

			const culling::EVERYCULLING_M256F& TriPointC_X,
			const culling::EVERYCULLING_M256F& TriPointC_Y,
			const culling::EVERYCULLING_M256F& TriPointC_Z,

			culling::EVERYCULLING_M256F& outLeftMiddlePointC_X,
			culling::EVERYCULLING_M256F& outLeftMiddlePointC_Y,
			culling::EVERYCULLING_M256F& outLeftMiddlePointC_Z,

			culling::EVERYCULLING_M256F& outRightMiddlePointC_X,
			culling::EVERYCULLING_M256F& outRightMiddlePointC_Y,
			culling::EVERYCULLING_M256F& outRightMiddlePointC_Z
		);
	};
}
