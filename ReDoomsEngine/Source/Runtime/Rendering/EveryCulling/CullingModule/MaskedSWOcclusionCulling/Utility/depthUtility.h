#pragma once

#include "../../../EveryCullingCore.h"

namespace culling
{
	namespace depthUtility
	{
		void ComputeDepthPlane
		(
			const float vertexPoint1X,
			const float vertexPoint1Y,
			const float vertexPoint1Z,
			const float vertexPoint2X,
			const float vertexPoint2Y,
			const float vertexPoint2Z,
			const float vertexPoint3X,
			const float vertexPoint3Y,
			const float vertexPoint3Z,
			float& outZPixelDx,
			float& outZPixelDy
		);
	}
}
;

