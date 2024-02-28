#include "SolveMeshRoleStage.h"

#include "../MaskedSWOcclusionCulling.h"
#include "../Utility/vertexTransformationHelper.h"


/*
bool culling::SolveMeshRoleStage::CheckIsOccluderFromBoundingSphere
(
	const size_t cameraIndex,
	const culling::Vec3& spherePointInWorldSpace,
	const float sphereRadiusInWorldSpace
) const
{
	//check https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space
	//		https://stackoverflow.com/questions/3717226/radius-of-projected-sphere
	//		https://en.wikipedia.org/wiki/Spherical_cap

	// TODO : Consider camera rotation.

	const culling::Vec3 vecFromCameraToBoundingSphere = mCullingSystem->GetCameraWorldPosition(cameraIndex) - spherePointInWorldSpace;
	const float distanceFromCameraToBoundingSphere = culling::Dot(vecFromCameraToBoundingSphere, vecFromCameraToBoundingSphere);

	const float disFromCameraToSphereHorizon = sqrtf(fabs(distanceFromCameraToBoundingSphere - sphereRadiusInWorldSpace * sphereRadiusInWorldSpace));
	const float halfOfFieldOfView = (mCullingSystem->GetCameraFieldOfView(cameraIndex) * culling::DEGREE_TO_RADIAN) * 0.5f;
	
	const float radiusOfViewSpaceSphere = (eastl::cos(halfOfFieldOfView) / eastl::sin(halfOfFieldOfView)) * sphereRadiusInWorldSpace / disFromCameraToSphereHorizon;
	
	return (radiusOfViewSpaceSphere > mOccluderViewSpaceBoundingSphereRadius);
}
*/

bool culling::SolveMeshRoleStage::CheckIsOccluder
(
	EntityBlock* const currentEntityBlock,
	const size_t entityIndex,
	const culling::Vec3& cameraWorldPos
) const
{
	bool bIsOccluder = false;

	const culling::Position_BoundingSphereRadius& entityPositionAndBoundingSphereRadius = currentEntityBlock->GetEntityWorldPositionAndBoudingSphereRadius(entityIndex);
	
	if (((entityPositionAndBoundingSphereRadius.GetPosition() - cameraWorldPos).magnitude() - entityPositionAndBoundingSphereRadius.GetBoundingSphereRadius()) <= mOccluderLimitOfDistanceToCamera)
	{
		const float clampedAABBMinScreenSpacePointX = CLAMP(currentEntityBlock->mAABBMinScreenSpacePointX[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mWidth);
		const float clampedAABBMinScreenSpacePointY = CLAMP(currentEntityBlock->mAABBMinScreenSpacePointY[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mHeight);
		const float clampedAABBMaxScreenSpacePointX = CLAMP(currentEntityBlock->mAABBMaxScreenSpacePointX[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mWidth);
		const float clampedAABBMaxScreenSpacePointY = CLAMP(currentEntityBlock->mAABBMaxScreenSpacePointY[entityIndex], 0.0f, (float)mMaskedOcclusionCulling->mDepthBuffer.mResolution.mHeight);

		EA_ASSERT(clampedAABBMinScreenSpacePointX >= 0.0f);
		EA_ASSERT(clampedAABBMinScreenSpacePointY >= 0.0f);
		EA_ASSERT(clampedAABBMaxScreenSpacePointX >= 0.0f);
		EA_ASSERT(clampedAABBMaxScreenSpacePointY >= 0.0f);
		EA_ASSERT(clampedAABBMaxScreenSpacePointX >= clampedAABBMinScreenSpacePointX);
		EA_ASSERT(clampedAABBMaxScreenSpacePointY >= clampedAABBMinScreenSpacePointY);

		const float screenSpaceAABBArea
			= (clampedAABBMaxScreenSpacePointX - clampedAABBMinScreenSpacePointX) *
			(clampedAABBMaxScreenSpacePointY - clampedAABBMinScreenSpacePointY);

		EA_ASSERT(screenSpaceAABBArea >= 0.0f);
		//EA_ASSERT(screenSpaceAABBArea > eastl::numeric_limits<float>::epsilon());

		if (screenSpaceAABBArea >= mOccluderAABBScreenSpaceMinArea)
		{
			bIsOccluder = true;
		}
	}

	return bIsOccluder;
}

culling::SolveMeshRoleStage::SolveMeshRoleStage(MaskedSWOcclusionCulling* occlusionCulling)
	: MaskedSWOcclusionCullingStage(occlusionCulling)
{

}

void culling::SolveMeshRoleStage::SolveMeshRole
(
	const size_t cameraIndex,
	EntityBlock* const currentEntityBlock,
	bool& isOccluderExist
)
{
	for(size_t entityIndex = 0 ; entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK; entityIndex++)
	{
		if(currentEntityBlock->GetIsCulled(entityIndex, cameraIndex) == false)
		{ // All vertices's w of clip space aabb is negative, it can't be occluder. it's already culled in PreCulling Stage
			
			const bool isOccluder = CheckIsOccluder(currentEntityBlock, entityIndex, mCullingSystem->GetCameraWorldPosition(cameraIndex));
			
			isOccluderExist |= isOccluder;
			if(isOccluder == true)
			{
				mMaskedOcclusionCulling->mOccluderListManager.AddOccluder(currentEntityBlock, entityIndex);
			}
		}
	}	
}

void culling::SolveMeshRoleStage::CullBlockEntityJob(const size_t cameraIndex, const unsigned long long currentTickCount)
{
	if (EVERYCULLING_WHEN_TO_BIN_TRIANGLE(currentTickCount))
	{
		bool isOccluderExist = false;
		while (true)
		{
			culling::EntityBlock* const nextEntityBlock = GetNextEntityBlock(cameraIndex);

			if (nextEntityBlock != nullptr)
			{
				SolveMeshRole(cameraIndex, nextEntityBlock, isOccluderExist);
			}
			else
			{
				break;
			}
		}

		if (isOccluderExist == true)
		{
			mMaskedOcclusionCulling->SetIsOccluderExistTrue();
		}
	}
}

const char* culling::SolveMeshRoleStage::GetCullingModuleName() const
{
	return "SolveMeshRoleStage";
}

void culling::SolveMeshRoleStage::ResetCullingModule(const unsigned long long currentTickCount)
{
	MaskedSWOcclusionCullingStage::ResetCullingModule(currentTickCount);
	
}

void culling::SolveMeshRoleStage::SetOccluderAABBScreenSpaceMinArea(const float occluderAABBScreenSpaceMinArea)
{
	EA_ASSERT(occluderAABBScreenSpaceMinArea >= 0.0f);
	mOccluderAABBScreenSpaceMinArea = occluderAABBScreenSpaceMinArea;
}

void culling::SolveMeshRoleStage::SetOccluderLimitOfDistanceToCamera(const float OccluderLimitOfDistanceToCamera)
{
	EA_ASSERT(OccluderLimitOfDistanceToCamera >= 0.0f);
	mOccluderLimitOfDistanceToCamera = OccluderLimitOfDistanceToCamera;
}
