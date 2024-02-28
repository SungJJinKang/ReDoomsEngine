#pragma once

#include "../../../EveryCullingCore.h"
#include "../SWDepthBuffer.h"

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


		void ConvertNDCSpaceThreeVerticesToScreenPixelSpace
		(
			const culling::EVERYCULLING_M256F* ndcSpaceVertexX,
			const culling::EVERYCULLING_M256F* ndcSpaceVertexY,
			culling::EVERYCULLING_M256F* outScreenPixelSpaceX,
			culling::EVERYCULLING_M256F* outScreenPixelSpaceY,
			culling::SWDepthBuffer& depthBuffer
		);

		void ConvertClipSpaceThreeVerticesToScreenPixelSpace
		(
			const culling::EVERYCULLING_M256F* clipSpaceVertexX,
			const culling::EVERYCULLING_M256F* clipSpaceVertexY,
			const culling::EVERYCULLING_M256F* clipSpaceVertexReverseW,
			culling::EVERYCULLING_M256F* outScreenPixelSpaceX,
			culling::EVERYCULLING_M256F* outScreenPixelSpaceY,
			culling::SWDepthBuffer& depthBuffer
		);

		void ConvertNDCSpaceVertexToScreenPixelSpace
		(
			const culling::EVERYCULLING_M256F& ndcSpaceVertexX,
			const culling::EVERYCULLING_M256F& ndcSpaceVertexY,
			culling::EVERYCULLING_M256F& outScreenPixelSpaceX,
			culling::EVERYCULLING_M256F& outScreenPixelSpaceY,
			culling::SWDepthBuffer& depthBuffer
		);
	}

}


