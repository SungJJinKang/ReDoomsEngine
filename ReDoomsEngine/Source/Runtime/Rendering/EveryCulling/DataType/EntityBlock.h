#pragma once

#include "../EveryCullingCore.h"

#include "Math/Vector.h"
#include "Position_BoundingSphereRadius.h"
#include "VertexData.h"
#include "Math/Matrix.h"

#define EVERYCULLING_PAGE_SIZE 4096
#define EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK 16

namespace culling
{
	/// <summary>
	/// EntityBlock size should be less 4KB(Page size) for Block data being allocated in a page
	/// </summary>
	struct alignas(EVERYCULLING_CACHE_LINE_SIZE) EntityBlock
	{
		/// <summary>
		/// uninitiliazed constructor
		/// </summary>
		EntityBlock() {}

		/// <summary>
		/// You don't need to worry about false sharing.
		/// void* mRenderer and mCurrentEntityCount isn't read during CullJob
		/// </summary>
		uint8_t* mIsVisibleBitflag;

		// Written in PreCulling Stage ---------------------------------------------------------------------------------------------------

		// Below variables should be written(set) before start culling by user. -----------------------------------------------------------------------------

		culling::AlignedVec4* mAABBMinWorldPoint;
		culling::AlignedVec4* mAABBMaxWorldPoint;

		/// <summary>
		/// Whether renderer component is enabled.
		/// </summary>
		uint8_t* mIsObjectEnabled;
		culling::AlignedVec4* mScaleAndDrawDistance;

		/// <summary>
		/// x, y, z : components is position of entity
		/// w : component is radius of entity's sphere bound
		/// 
		/// Writeen in Pre Culling, Read in ViewFrustum Culling, Distance Culling
		/// </summary>
		culling::Position_BoundingSphereRadius* mWorldPositionAndWorldBoundingSphereRadius; // 4 * 16 byte

		uint32_t EntityCount;

		// ----------------------------------------------------------------------------------------------------------------------


		EASTL_FORCE_INLINE bool GetIsCulled(const size_t entityIndex, const size_t cameraIndex) const
		{
			return ( mIsVisibleBitflag[entityIndex] & (1 << cameraIndex) ) == 0;
		}

		/// <summary>
		/// Update IsCulled without branch
		///
		///	If visiblity flag is already 0, it preserve value.
		///	If visiblity flag is 1, visibility flag is updated based on isCulled parameter
		/// </summary>
		/// <param name="entityIndex"></param>
		/// <param name="cameraIndex"></param>
		/// <param name="isCullded"></param>
		/// <returns></returns>
		EASTL_FORCE_INLINE void UpdateIsCulled(const size_t entityIndex, const size_t cameraIndex, const bool isCullded)
		{
			// Setting value to invalid index is acceptable
			EA_ASSERT(entityIndex < EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);

			const char cullMask = ~((char)isCullded << cameraIndex);

			mIsVisibleBitflag[entityIndex] &= cullMask;
		}

		EASTL_FORCE_INLINE void SetCulled(const size_t entityIndex, const size_t cameraIndex)
		{
			mIsVisibleBitflag[entityIndex] &= ~(1 << cameraIndex);
		}

		EASTL_FORCE_INLINE void SetNotCulled(const size_t entityIndex, const size_t cameraIndex)
		{
			mIsVisibleBitflag[entityIndex] |= (1 << cameraIndex);
		}

		EASTL_FORCE_INLINE bool GetIsObjectEnabled(const size_t entityIndex) const
		{
			return mIsObjectEnabled[entityIndex / 8] & (1 << (entityIndex/8));
		}
		
		EASTL_FORCE_INLINE const culling::Position_BoundingSphereRadius& GetEntityWorldPositionAndBoudingSphereRadius(const size_t entityIndex) const
		{
			return mWorldPositionAndWorldBoundingSphereRadius[entityIndex];
		}

		EASTL_FORCE_INLINE void ResetEntityBlock(const unsigned long long currentTickCount)
		{
			EA::StdC::Memset8(mIsVisibleBitflag, 0xFF, sizeof(uint8_t) * EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK);
		}

		EASTL_FORCE_INLINE float GetDesiredMaxDrawDistance(const size_t entityIndex)
		{
			//EA_ASSERT(mScaleAndDrawDistance[entityIndex][3] >= 0.0f);

			return mScaleAndDrawDistance[entityIndex][3];
		}
		
		void ClearEntityBlock();
	};



	/// <summary>
	/// Size of Entity block should be less than 4kb(page size)
	/// </summary>
	static_assert(sizeof(EntityBlock) < EVERYCULLING_PAGE_SIZE);
	static_assert(sizeof(culling::Position_BoundingSphereRadius) == 16);
	/// <summary>
	/// EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK should be even number
	/// </summary>
	static_assert(EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK > 0 && EVERYCULLING_ENTITY_COUNT_IN_ENTITY_BLOCK % 2 == 0);
}
