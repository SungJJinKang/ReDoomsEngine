#pragma once

#include "../EveryCullingCore.h"

namespace culling
{
	struct VertexData
	{
		eastl::atomic<uint64_t> mBinnedIndiceCount; //  8byte + ??

		culling::Vec3* mVertices; // 8byte or 4byte
		uint64_t mVerticeCount; // 8byte

		uint32_t* mIndices; // 8byte or 4byte
		uint64_t mIndiceCount; // 8byte

		/// <summary>
		/// Vertex Stride ( offset between vertices )
		///	ex) Vertex1.X Vertex1.Y Vertex1.Z Vertex1.UV_X(4byte) Vertex1.UV_Y(4byte) Vertex2.X Vertex2.Y Vertex2.Z
		///		-> Stride is 8byte!
		/// </summary>
		uint64_t mVertexStride; // 8byte

		VertexData() {}
		VertexData(const VertexData& InVertexData)
			: 
			mBinnedIndiceCount(0),
			mVertices(InVertexData.mVertices),
			mVerticeCount(InVertexData.mVerticeCount),
			mIndices(InVertexData.mIndices),
			mIndiceCount(InVertexData.mIndiceCount),
			mVertexStride(InVertexData.mVertexStride)
		{

		}
		
		EASTL_FORCE_INLINE void Reset(const unsigned long long currentTickCount)
		{
			if(EVERYCULLING_WHEN_TO_BIN_TRIANGLE(currentTickCount) == true)
			{
				// Clear binned triangle 
				mBinnedIndiceCount = 0;
			}
			
		}
	};

}