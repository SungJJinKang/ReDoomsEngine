#include "EntityBlockViewer.h"

void culling::EntityBlockViewer::DeInitializeEntityBlockViewer()
{
	mTargetEntityBlock = nullptr;
	mEntityIndexInBlock = (uint64_t)-1;
}

void culling::EntityBlockViewer::ResetEntityData()
{
	SetIsObjectEnabled(true);
}

culling::EntityBlockViewer::EntityBlockViewer()
{
	DeInitializeEntityBlockViewer();
}

bool culling::EntityBlockViewer::IsValid() const
{
	return (mTargetEntityBlock != nullptr) && (mEntityIndexInBlock != (uint64_t)-1);
}

culling::EntityBlock* culling::EntityBlockViewer::GetTargetEntityBlock()
{
	EA_ASSERT(IsValid() == true);
	return mTargetEntityBlock;
}

size_t culling::EntityBlockViewer::GetEntityIndexInBlock() const
{
	EA_ASSERT(IsValid() == true);
	return mEntityIndexInBlock;
}

bool culling::EntityBlockViewer::GetIsCulled(const uint32_t cameraIndex) const
{
	EA_ASSERT(IsValid() == true);
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
	return mTargetEntityBlock->GetIsCulled(mEntityIndexInBlock, cameraIndex);
}

void culling::EntityBlockViewer::SetModelMatrix(const float* const modelMatrix)
{
	EA_ASSERT(IsValid() == true);
	if (IsValid() == true)
	{
		mTargetEntityBlock->SetModelMatrix(mEntityIndexInBlock, modelMatrix);
	}
}

const culling::EntityBlock* culling::EntityBlockViewer::GetTargetEntityBlock() const
{
	EA_ASSERT(IsValid() == true);
	return mTargetEntityBlock;
}

culling::EntityBlockViewer::EntityBlockViewer
(
	EntityBlock* const entityBlock, 
	const size_t entityIndexInBlock
)
	: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }
{
	EA_ASSERT(IsValid() == true);
	ResetEntityData();
}

culling::EntityBlockViewer::EntityBlockViewer(EntityBlockViewer&&) noexcept = default;
culling::EntityBlockViewer& culling::EntityBlockViewer::operator=(EntityBlockViewer&&) noexcept = default;

void culling::EntityBlockViewer::SetMeshVertexData
(
	const culling::Vec3* const vertices, 
	const uint64_t verticeCount,
	const uint32_t* const indices,
	const uint64_t indiceCount,
	const uint64_t verticeStride
)
{
	EA_ASSERT(IsValid() == true);
	if (IsValid() == true)
	{
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVertices = vertices;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVerticeCount = verticeCount;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mIndices = indices;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mIndiceCount = indiceCount;
		mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock].mVertexStride = verticeStride;
	}
}

const culling::VertexData& culling::EntityBlockViewer::GetVertexData() const
{
	EA_ASSERT(IsValid() == true);
	return mTargetEntityBlock->mVertexDatas[mEntityIndexInBlock];
}

void culling::EntityBlockViewer::SetIsObjectEnabled(const bool isEnabled)
{
	EA_ASSERT(IsValid() == true);
	if (IsValid() == true)
	{
		mTargetEntityBlock->SetIsObjectEnabled(mEntityIndexInBlock, isEnabled);
	}
}

void culling::EntityBlockViewer::SetAABBWorldPosition(const float* const minWorldPos, const float* const maxWorldPos)
{
	EA_ASSERT(IsValid() == true);
	if (IsValid() == true)
	{
		mTargetEntityBlock->SetAABBWorldPosition(mEntityIndexInBlock, minWorldPos, maxWorldPos);
	}
}

void culling::EntityBlockViewer::SetObjectWorldPosition(const float* const worldPos)
{
	EA_ASSERT(IsValid() == true);
	if (IsValid() == true)
	{
		mTargetEntityBlock->mWorldPositionAndWorldBoundingSphereRadius[mEntityIndexInBlock].SetPosition(worldPos);
	}
}

void culling::EntityBlockViewer::SetDesiredMaxDrawDistance(const float desiredMaxDrawDistance)
{
	EA_ASSERT(IsValid() == true);

	if (IsValid() == true)
	{
		mTargetEntityBlock->SetDesiredMaxDrawDistance(mEntityIndexInBlock, desiredMaxDrawDistance);
	}
}

void culling::EntityBlockViewer::UpdateEntityData(const float* const entityWorldPositionVec3, const float* const aabbMinWorldPositionVec3, const float* const aabbMaxWorldPositionVec3, const float* const entityModelMatrix4x4)
{
	EA_ASSERT(IsValid() == true);

	if (IsValid() == true)
	{
		SetObjectWorldPosition(entityWorldPositionVec3);
		SetAABBWorldPosition(aabbMinWorldPositionVec3, aabbMaxWorldPositionVec3);

		SetModelMatrix(entityModelMatrix4x4);
	}
}

