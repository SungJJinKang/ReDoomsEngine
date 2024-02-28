#pragma once

#include "EveryCullingCore.h"

#include "DataType/EntityGridCell.h"
#include "DataType/Math/Vector.h"
#include "DataType/Math/Matrix.h"

namespace culling
{
	class CullingModule;
	class ViewFrustumCulling;
	class ScreenSpaceBoundingSphereCulling;
	class MaskedSWOcclusionCulling;
	class QueryOcclusionCulling;
	class PreCulling;
	class DistanceCulling;
	struct EntityBlock;

	class EveryCulling
	{
	private:

		eastl::atomic<uint32_t> mRunningThreadCount;
		
		size_t mCameraCount;
		eastl::array<culling::Mat4x4, EVERYCULLING_MAX_CAMERA_COUNT> mCameraModelMatrixes;
		eastl::array<culling::Mat4x4, EVERYCULLING_MAX_CAMERA_COUNT> mCameraViewProjectionMatrixes;
		eastl::array<culling::Vec3, EVERYCULLING_MAX_CAMERA_COUNT> mCameraWorldPositions;
		eastl::array<culling::Vec4, EVERYCULLING_MAX_CAMERA_COUNT> mCameraRotations;
		eastl::array<float, EVERYCULLING_MAX_CAMERA_COUNT> mCameraFieldOfView;
		eastl::array<float, EVERYCULLING_MAX_CAMERA_COUNT> mFarClipPlaneDistance;
		eastl::array<float, EVERYCULLING_MAX_CAMERA_COUNT> mNearClipPlaneDistance;

		bool bmIsEntityBlockPoolInitialized;

		/// <summary>
		/// List of EntityBlock with no entity ( maybe entity was destroyed)
		/// </summary>
		eastl::vector<EntityBlock*> mFreeEntityBlockList;
		/// <summary>
		/// List of EntityBlock containing Entities
		/// </summary>
		eastl::vector<EntityBlock*> mActiveEntityBlockList;
		/// <summary>
		/// Allocated EntityBlock Arrays
		/// This objects will be released at destructor
		/// </summary>
		eastl::vector<EntityBlock*> mAllocatedEntityBlockChunkList;

		uint64_t mEntityBlockUniqueIDCounter;
		
		void AllocateEntityBlockPool();
		culling::EntityBlock* AllocateNewEntityBlockFromPool();
		/// <summary>
		/// Block Swap removedblock with last block, and return swapped lastblock to pool
		/// </summary>
		void FreeEntityBlock(EntityBlock* freedEntityBlock);
		EntityBlock* GetNewEntityBlockFromPool();

		void ResetCullingModules();
		/// <summary>
		/// Reset VisibleFlag
		/// </summary>
		void ResetEntityBlocks();

	public:

		eastl::unique_ptr<PreCulling> mPreCulling;
		eastl::unique_ptr<DistanceCulling> mDistanceCulling;
		eastl::unique_ptr<ViewFrustumCulling> mViewFrustumCulling;
		eastl::unique_ptr<MaskedSWOcclusionCulling> mMaskedSWOcclusionCulling;

	private:

		unsigned long long mCurrentTickCount;

		eastl::vector<culling::CullingModule*> mUpdatedCullingModules;

		// this function is called by multiple threads
		void OnStartCullingModule(const culling::CullingModule* const cullingModule);
		// this function is called by multiple threads
		void OnEndCullingModule(const culling::CullingModule* const cullingModule);
		
		void SetViewProjectionMatrix(const size_t cameraIndex, const culling::Mat4x4& viewProjectionMatrix);
		void SetFieldOfViewInDegree(const size_t cameraIndex, const float fov);
		void SetCameraNearFarClipPlaneDistance(const size_t cameraIndex, const float nearPlaneDistance, const float farPlaneDistance);;
		void SetCameraWorldPosition(const size_t cameraIndex, const culling::Vec3& cameraWorldPos);
		void SetCameraRotation(const size_t cameraIndex, const culling::Vec4& cameraRotation);

	public:

		enum class CullingModuleType
		{
			PreCulling,
			ViewFrustumCulling,
			MaskedSWOcclusionCulling,
			DistanceCulling
		};

		EveryCulling() = delete;
		EveryCulling(const uint32_t resolutionWidth, const uint32_t resolutionHeight);
		EveryCulling(const EveryCulling&) = delete;
		EveryCulling& operator=(const EveryCulling&) = delete;

		~EveryCulling();

		void SetCameraCount(const size_t cameraCount);

		unsigned long long GetTickCount() const;
		
		EASTL_FORCE_INLINE size_t GetCameraCount() const
		{
			return mCameraCount;
		}
		EASTL_FORCE_INLINE const culling::Vec3& GetCameraWorldPosition(const size_t cameraIndex) const
		{
			EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraWorldPositions[cameraIndex];
		}
		EASTL_FORCE_INLINE const culling::Mat4x4& GetCameraModelMatrix(const size_t cameraIndex) const
		{
			EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraModelMatrixes[cameraIndex];
		}
		EASTL_FORCE_INLINE const culling::Mat4x4& GetCameraViewProjectionMatrix(const size_t cameraIndex) const
		{
			EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraViewProjectionMatrixes[cameraIndex];
		}
		EASTL_FORCE_INLINE float GetCameraFieldOfView(const size_t cameraIndex) const
		{
			EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mCameraFieldOfView[cameraIndex];
		}
		EASTL_FORCE_INLINE float GetCameraFarClipPlaneDistance(const size_t cameraIndex) const
		{
			EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mFarClipPlaneDistance[cameraIndex];
		}
		EASTL_FORCE_INLINE float GetCameraNearClipPlaneDistance(const size_t cameraIndex) const
		{
			EA_ASSERT(cameraIndex >= 0 && cameraIndex < EVERYCULLING_MAX_CAMERA_COUNT);
			return mNearClipPlaneDistance[cameraIndex];
		}
		
		/// <summary>
		/// Get EntityBlock List with entities
		/// </summary>
		/// <returns></returns>
		const eastl::vector<EntityBlock*>& GetActiveEntityBlockList() const;
		size_t GetActiveEntityBlockCount() const;

		void ThreadCullJob(const size_t cameraIndex, const unsigned long long tickCount);
		//void ThreadCullJob(const uint32_t threadIndex, const uint32_t threadCount);

		/// <summary>
		/// Caller thread will stall until cull job of all entity block is finished
		/// </summary>
		void WaitToFinishCullJob(const uint32_t cameraIndex) const;
		void WaitToFinishCullJobOfAllCameras() const;


		/**
		 * \brief Reset cull job. Should be called every frame after finish cull job
		 */
		void PreCullJob();
		
		auto GetThreadCullJob(const size_t cameraIndex, const unsigned long long tickCount)
		{
			return [this, cameraIndex, tickCount]()
			{
				this->ThreadCullJob(cameraIndex, tickCount);
			};
		}

		const culling::CullingModule* GetLastEnabledCullingModule() const;
		void SetEnabledCullingModule(const CullingModuleType cullingModuleType, const bool isEnabled);
		uint32_t GetRunningThreadCount() const;

	};
}


