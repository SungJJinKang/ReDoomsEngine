#pragma once

#include "MaskedSWOcclusionCullingStage.h"

#include "../SWDepthBuffer.h"

namespace culling
{
	class QueryOccludeeStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		float MinFloatFromM256F(const culling::EVERYCULLING_M256F& data);
		float MaxFloatFromM256F(const culling::EVERYCULLING_M256F& data);
		void ComputeBinBoundingBoxFromVertex
		(
			const culling::EVERYCULLING_M256F& screenPixelX,
			const culling::EVERYCULLING_M256F& screenPixelY,
			uint32_t& outBinBoundingBoxMinX,
			uint32_t& outBinBoundingBoxMinY,
			uint32_t& outBinBoundingBoxMaxX,
			uint32_t& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		);
		void ComputeBinBoundingBoxFromVertex
		(
			const float minScreenPixelX,
			const float minScreenPixelY,
			const float maxScreenPixelX,
			const float maxScreenPixelY,
			uint32_t& outBinBoundingBoxMinX,
			uint32_t& outBinBoundingBoxMinY,
			uint32_t& outBinBoundingBoxMaxX,
			uint32_t& outBinBoundingBoxMaxY,
			SWDepthBuffer& depthBuffer
		);
		void Clipping
		(
			const culling::EVERYCULLING_M256F& clipspaceVertexX,
			const culling::EVERYCULLING_M256F& clipspaceVertexY,
			const culling::EVERYCULLING_M256F& clipspaceVertexZ,
			const culling::EVERYCULLING_M256F& clipspaceVertexW,
			uint32_t& triangleCullMask
		);

		void QueryOccludee(const size_t cameraIndex, culling::EntityBlock* const entityBlock);

	public:

		QueryOccludeeStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount) override;
		const char* GetCullingModuleName() const override;
	};
}


