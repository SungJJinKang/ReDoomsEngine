#pragma once

#include "../../../EveryCullingCore.h"
#include "../../../DataType/Math/EveryCullingSIMDCore.h"

namespace culling
{
	namespace vertexTransformationHelper
	{
		void TransformThreeVerticesToClipSpace
		(
			culling::EVERYCULLING_M256F* outClipVertexX,
			culling::EVERYCULLING_M256F* outClipVertexY,
			culling::EVERYCULLING_M256F* outClipVertexZ,
			culling::EVERYCULLING_M256F* outClipVertexW,
			const float* const toClipspaceMatrix
		);

		void TransformThreeVerticesToClipSpace
		(
			culling::EVERYCULLING_M256F* outClipVertexX,
			culling::EVERYCULLING_M256F* outClipVertexY,
			culling::EVERYCULLING_M256F* outClipVertexZ,
			const float* const toClipspaceMatrix
		);

		void TransformVertexToClipSpace
		(
			culling::EVERYCULLING_M256F& outClipVertexX,
			culling::EVERYCULLING_M256F& outClipVertexY,
			culling::EVERYCULLING_M256F& outClipVertexZ,
			culling::EVERYCULLING_M256F& outClipVertexW,
			const float* const toClipspaceMatrix
		);

		void TransformVertexToClipSpace
		(
			culling::EVERYCULLING_M256F& outClipVertexX,
			culling::EVERYCULLING_M256F& outClipVertexY,
			culling::EVERYCULLING_M256F& outClipVertexZ,
			const float* const toClipspaceMatrix
		);

		void ConvertClipSpaceThreeVerticesToNDCSpace
		(
			culling::EVERYCULLING_M256F* outClipVertexX,
			culling::EVERYCULLING_M256F* outClipVertexY,
			culling::EVERYCULLING_M256F* outClipVertexZ,
			const culling::EVERYCULLING_M256F* oneDividedByW
		);

		void ConvertClipSpaceVertexToNDCSpace
		(
			culling::EVERYCULLING_M256F& outClipVertexX,
			culling::EVERYCULLING_M256F& outClipVertexY,
			culling::EVERYCULLING_M256F& outClipVertexZ,
			const culling::EVERYCULLING_M256F& oneDividedByW
		);

	}

}


