#include "EveryCulling.h"

#include "DataType/EntityBlock.h"
#include "CullingModule/ViewFrustumCulling/ViewFrustumCulling.h"
#include "CullingModule/PreCulling/PreCulling.h"
#include "CullingModule/DistanceCulling/DistanceCulling.h"

void culling::EveryCulling::ResetCullingModules()
{
	for (auto cullingModule : mUpdatedCullingModules)
	{
		cullingModule->ResetCullingModule(mCurrentTickCount);
	}
}

void culling::EveryCulling::ResetEntityBlocks()
{
	//Maybe Compiler use SIMD or do faster than SIMD instruction
	for (EntityBlock& entityBlock : mEntityBlockList)
	{
		entityBlock.ResetEntityBlock(mCurrentTickCount);
	}
}

void culling::EveryCulling::ThreadCullJob(const size_t cameraIndex, const unsigned long long tickCount)
{

	const uint32_t entityBlockCount = static_cast<uint32_t>(GetActiveEntityBlockCount());
	const unsigned long long currentTickCount = mCurrentTickCount;

	if (entityBlockCount > 0 && currentTickCount == tickCount)
	{
		mRunningThreadCount++;

		for (size_t moduleIndex = 0; moduleIndex < mUpdatedCullingModules.size(); moduleIndex++)
		{
			culling::CullingModule* cullingModule = mUpdatedCullingModules[moduleIndex];
			EA_ASSERT(cullingModule != nullptr);

			if (cullingModule->IsEnabled == true)
			{
				OnStartCullingModule(cullingModule);

				cullingModule->ThreadCullJob(cameraIndex, currentTickCount);

				eastl::atomic_thread_fence(eastl::memory_order_seq_cst);

				while (cullingModule->GetFinishedThreadCount(cameraIndex) < mRunningThreadCount)
				{
					
				}

				OnEndCullingModule(cullingModule);
			}
		}
	}
}


void culling::EveryCulling::WaitToFinishCullJob(const uint32_t cameraIndex) const
{
	const CullingModule* lastEnabledCullingModule = GetLastEnabledCullingModule();
	if(lastEnabledCullingModule != nullptr)
	{
		while (lastEnabledCullingModule->GetFinishedThreadCount(cameraIndex) < mRunningThreadCount)
		{

		}
	}
}

void culling::EveryCulling::WaitToFinishCullJobOfAllCameras() const
{
	for (uint32_t cameraIndex = 0; cameraIndex < mCameraCount; cameraIndex++)
	{
		WaitToFinishCullJob(cameraIndex);
	}
}

void culling::EveryCulling::PreCullJob()
{
	mCurrentTickCount++;
	mRunningThreadCount = 0;

	ResetEntityBlocks();
	ResetCullingModules();

	//release!
	eastl::atomic_thread_fence(eastl::memory_order_seq_cst);
}

const culling::CullingModule* culling::EveryCulling::GetLastEnabledCullingModule() const
{
	culling::CullingModule* lastEnabledCullingModule = nullptr;
	for(int cullingModuleIndex = static_cast<int>(mUpdatedCullingModules.size()) - 1 ; cullingModuleIndex >= 0 ; cullingModuleIndex--)
	{
		if(mUpdatedCullingModules[cullingModuleIndex]->IsEnabled == true)
		{
			lastEnabledCullingModule = mUpdatedCullingModules[cullingModuleIndex];
			break;
		}
	}
	return lastEnabledCullingModule;
}

void culling::EveryCulling::SetEnabledCullingModule(const CullingModuleType cullingModuleType, const bool isEnabled)
{
	switch (cullingModuleType)
	{

	case CullingModuleType::PreCulling:
		mPreCulling->IsEnabled = isEnabled;
		break;

	case CullingModuleType::ViewFrustumCulling:
		mViewFrustumCulling->IsEnabled = isEnabled;
		break;

	case CullingModuleType::DistanceCulling:

		mDistanceCulling->IsEnabled = isEnabled;
		break;
		
	}
}

uint32_t culling::EveryCulling::GetRunningThreadCount() const
{
	return mRunningThreadCount;
}

culling::EveryCulling::EveryCulling(const uint32_t resolutionWidth, const uint32_t resolutionHeight)
	:
	mPreCulling{ eastl::make_unique<PreCulling>(this) },
	mDistanceCulling{ eastl::make_unique<DistanceCulling>(this) },
	mViewFrustumCulling{ eastl::make_unique<ViewFrustumCulling>(this) }
#ifdef ENABLE_SCREEN_SAPCE_AABB_CULLING
	, mScreenSpaceBoudingSphereCulling{ eastl::make_unique<ScreenSpaceBoundingSphereCulling>(this) }
#endif
	, mUpdatedCullingModules
		{
			mPreCulling.get(),
			mDistanceCulling.get(),
			mViewFrustumCulling.get()
		}
	, mCurrentTickCount()
	, bmIsEntityBlockPoolInitialized(false)
{
	//CacheCullBlockEntityJobs();
	bmIsEntityBlockPoolInitialized = true;
}

culling::EveryCulling::~EveryCulling() = default;

void culling::EveryCulling::SetCameraCount(const size_t cameraCount)
{
	mCameraCount = cameraCount;

	for (auto updatedCullingModule : mUpdatedCullingModules)
	{
		updatedCullingModule->OnSetCameraCount(cameraCount);
	}
}

unsigned long long culling::EveryCulling::GetTickCount() const
{
	return mCurrentTickCount;
}

void culling::EveryCulling::OnStartCullingModule(const culling::CullingModule* const cullingModule)
{
}

void culling::EveryCulling::OnEndCullingModule(const culling::CullingModule* const cullingModule)
{
}

void culling::EveryCulling::SetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& viewProjectionMatrix)
{
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);

	mCameraViewProjectionMatrixes[cameraIndex] = viewProjectionMatrix;
	
	if (cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetViewProjectionMatrix(cameraIndex, viewProjectionMatrix);
		}
	}
}

void culling::EveryCulling::SetFieldOfViewInDegree(const size_t cameraIndex, const float fov)
{
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
	EA_ASSERT(fov > 0.0f);

	mCameraFieldOfView[cameraIndex] = fov;

	if (cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraFieldOfView(cameraIndex, fov);
		}
	}
}

void culling::EveryCulling::SetCameraNearFarClipPlaneDistance
(
	const size_t cameraIndex,
	const float nearPlaneDistance, 
	const float farPlaneDistance
)
{
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
	EA_ASSERT(nearPlaneDistance > 0.0f);
	EA_ASSERT(farPlaneDistance > 0.0f);
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);


	mNearClipPlaneDistance[cameraIndex] = nearPlaneDistance;
	mFarClipPlaneDistance[cameraIndex] = farPlaneDistance;

	if (cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraNearClipPlaneDistance(cameraIndex, nearPlaneDistance);
			updatedCullingModule->OnSetCameraFarClipPlaneDistance(cameraIndex, farPlaneDistance);
		}
	}
}

void culling::EveryCulling::SetCameraWorldPosition(const size_t cameraIndex, const culling::Vec3& cameraWorldPos)
{
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);

	mCameraWorldPositions[cameraIndex] = cameraWorldPos;

	if (cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraWorldPosition(cameraIndex, cameraWorldPos);
		}
	}
}

void culling::EveryCulling::SetCameraRotation(const size_t cameraIndex, const culling::AlignedVec4& cameraRotation)
{
	EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);

	mCameraRotations[cameraIndex] = cameraRotation;

	if (cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT)
	{
		for (auto updatedCullingModule : mUpdatedCullingModules)
		{
			updatedCullingModule->OnSetCameraRotation(cameraIndex, cameraRotation);
		}
	}
}

void culling::EveryCulling::UpdateGlobalDataForCullJob(const size_t cameraIndex, const GlobalDataForCullJob& settingParameters)
{
	SetViewProjectionMatrix(cameraIndex, settingParameters.mViewProjectionMatrix);
	SetFieldOfViewInDegree(cameraIndex, settingParameters.mFieldOfViewInDegree);
	SetCameraNearFarClipPlaneDistance(cameraIndex, settingParameters.mCameraNearPlaneDistance, settingParameters.mCameraFarPlaneDistance);
	SetCameraWorldPosition(cameraIndex, settingParameters.mCameraWorldPosition);
	SetCameraRotation(cameraIndex, settingParameters.mCameraRotation);
}

size_t culling::EveryCulling::GetActiveEntityBlockCount() const
{
	return mEntityBlockList.size();
}
