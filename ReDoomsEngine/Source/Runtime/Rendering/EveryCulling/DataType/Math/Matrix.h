#pragma once

#include "Vector.h"

namespace culling
{
	/// <summary>
	/// Column major Matrix 4X4
	/// </summary>
	struct alignas(32) Mat4x4
	{
		culling::AlignedVec4 mColumns[4];

		EASTL_FORCE_INLINE AlignedVec4& operator[](size_t index)
		{
			return mColumns[index];
		}

		EASTL_FORCE_INLINE const AlignedVec4& operator[](size_t index) const
		{
			return mColumns[index];
		}

		EASTL_FORCE_INLINE float* data()
		{
			return mColumns[0].values;
		}

		EASTL_FORCE_INLINE const float* data() const
		{
			return mColumns[0].values;
		}

		Mat4x4() {}
	};
}
