#include "PreCulling.h"

#include "../MaskedSWOcclusionCulling/Utility/vertexTransformationHelper.h"
#define SCREEN_SPACE_MIN_VALUE (float)-50000.0f
#define SCREEN_SPACE_MAX_VALUE (float)50000.0f

void culling::PreCulling::ComputeScreenSpaceMinMaxAABBAndMinZ
(
	const size_t cameraIndex,
	culling::EntityBlock* const entityBlock, 
	const size_t entityIndex
)
{
	const culling::AlignedVec4& aabbMinWorldPoint = entityBlock->mAABBMinWorldPoint[entityIndex];
	const culling::AlignedVec4& aabbMaxWorldPoint = entityBlock->mAABBMaxWorldPoint[entityIndex];

	const culling::Mat4x4& worldToClipSpaceMatrix = mCullingSystem->GetCameraViewProjectionMatrix(cameraIndex);
	
	culling::EVERYCULLING_M256F aabbVertexX = _mm256_setr_ps(aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMinWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0], aabbMaxWorldPoint.values[0]);
	culling::EVERYCULLING_M256F aabbVertexY = _mm256_setr_ps(aabbMinWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMinWorldPoint.values[1], aabbMaxWorldPoint.values[1], aabbMaxWorldPoint.values[1]);
	culling::EVERYCULLING_M256F aabbVertexZ = _mm256_setr_ps(aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2], aabbMinWorldPoint.values[2], aabbMaxWorldPoint.values[2]);
	culling::EVERYCULLING_M256F aabbVertexW;
	
	// Convert world space to clip space
	culling::vertexTransformationHelper::TransformVertexToClipSpace
	(
		aabbVertexX,
		aabbVertexY,
		aabbVertexZ,
		aabbVertexW,
		worldToClipSpaceMatrix.data()
	);

	const culling::EVERYCULLING_M256F isHomogeneousWNegative = _mm256_cmp_ps(aabbVertexW, _mm256_set1_ps(eastl::numeric_limits<float>::epsilon()), _CMP_LT_OQ);

	const culling::EVERYCULLING_M256F oneDividedByW = culling::EVERYCULLING_M256F_DIV(_mm256_set1_ps(1.0f), aabbVertexW);

	const int isHomogeneousWNegativeMask = _mm256_movemask_ps(isHomogeneousWNegative);

	// If All vertex's w of clip space aabb is negative, it should be culled!
	entityBlock->UpdateIsCulled(entityIndex, cameraIndex, isHomogeneousWNegativeMask == 0x000000FF);
}

void culling::PreCulling::DoPreCull
(
	const size_t cameraIndex,
	culling::EntityBlock* const entityBlock
)
{
	for(size_t entityIndex = 0 ; entityIndex < entityBlock->EntityCount; entityIndex++)
	{
		if(entityBlock->GetIsObjectEnabled(entityIndex) == false)
		{
			entityBlock->SetCulled(entityIndex, cameraIndex);
		}	
		else
		{
			ComputeScreenSpaceMinMaxAABBAndMinZ(cameraIndex, entityBlock, entityIndex);
		}
	}
}

culling::PreCulling::PreCulling(EveryCulling* frotbiteCullingSystem)
	: CullingModule(frotbiteCullingSystem)
{
}

void culling::PreCulling::CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount)
{
	while (true)
	{
		culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);

		if (nextEntityBlock != nullptr)
		{
			DoPreCull(cameraIndex, nextEntityBlock);
		}
		else
		{
			break;
		}
	}

}

const char* culling::PreCulling::GetCullingModuleName() const
{
	return "PreCulling";
}
