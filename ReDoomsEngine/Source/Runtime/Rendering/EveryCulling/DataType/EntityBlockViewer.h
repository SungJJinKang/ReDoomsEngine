#pragma once

#include "EntityBlock.h"

namespace culling
{
	struct EntityBlock;

	/// <summary>
	/// Used for storing specific EntityBlock pointer
	/// </summary>
	class EntityBlockViewer
	{
		friend class EveryCulling;

	private:
		
		EntityBlock* mTargetEntityBlock;
		/// <summary>
		/// Entity Index in Entity Block
		/// </summary>
		uint64_t mEntityIndexInBlock;

		void DeInitializeEntityBlockViewer();
		void ResetEntityData();

	public:

		EntityBlockViewer();
		EntityBlockViewer(EntityBlock* const entityBlock, const size_t entityIndexInBlock);
		EntityBlockViewer(const EntityBlockViewer&) = delete;
		EntityBlockViewer& operator=(const EntityBlockViewer&) = delete;
		EntityBlockViewer(EntityBlockViewer&&) noexcept;
		EntityBlockViewer& operator=(EntityBlockViewer&&)noexcept ;

		bool IsValid() const;

		EntityBlock* GetTargetEntityBlock();

		const EntityBlock* GetTargetEntityBlock() const;

		size_t GetEntityIndexInBlock() const;
		
		/// <summary>
		/// Get if entity is visible from Camera of parameter cameraIndex
		/// </summary>
		/// <param name="cameraIndex">0 <= cameraIndex < EveryCulling::mCameraCount</param>
		/// <returns></returns>
		bool GetIsCulled(const uint32_t cameraIndex) const;

		void SetModelMatrix(const float* const modelMatrix);

		/**
		 * \brief Set entity's mesh vertex data.
		 * \param vertices 
		 * \param verticeCount 
		 * \param indices 
		 * \param indiceCount 
		 * \param verticeStride 
		 */
		void SetMeshVertexData
		(
			const culling::Vec3* const vertices,
			const uint64_t verticeCount,
			const uint32_t* const indices,
			const uint64_t indiceCount,
			const uint64_t verticeStride
		);
		
		const culling::VertexData& GetVertexData() const;

		void SetIsObjectEnabled(const bool isEnabled);

		void SetAABBWorldPosition(const float* const minWorldPos, const float* const maxWorldPos);

		void SetObjectWorldPosition(const float* const worldPos);

		void SetDesiredMaxDrawDistance(const float desiredMaxDrawDistance);

		/// <summary>
		/// Update EntityData
		///	Should be updated every frame before cull job
		/// </summary>
		/// <param name="entityWorldPositionVec3"></param>
		/// <param name="aabbMinWorldPositionVec3"></param>
		/// <param name="aabbMaxWorldPositionVec3"></param>
		/// <param name="entityModelMatrix4x4"></param>
		/// <returns></returns>
		void UpdateEntityData
		(
			const float* const entityWorldPositionVec3, 
			const float* const aabbMinWorldPositionVec3,
			const float* const aabbMaxWorldPositionVec3,
			const float* const entityModelMatrix4x4
		);

	};
}
