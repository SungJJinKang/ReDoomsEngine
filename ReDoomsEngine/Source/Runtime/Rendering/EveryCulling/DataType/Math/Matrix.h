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
		Mat4x4(const Matrix& Mat)
		{
			// Covert row-major matrix to column-major
			mColumns[0][0] = Mat(0, 0);
			mColumns[0][1] = Mat(1, 0);
			mColumns[0][2] = Mat(2, 0);
			mColumns[0][3] = Mat(3, 0);

			mColumns[1][0] = Mat(0, 1);
			mColumns[1][1] = Mat(1, 1);
			mColumns[1][2] = Mat(2, 1);
			mColumns[1][3] = Mat(3, 1);

			mColumns[2][0] = Mat(0, 2);
			mColumns[2][1] = Mat(1, 2);
			mColumns[2][2] = Mat(2, 2);
			mColumns[2][3] = Mat(3, 2);

			mColumns[3][0] = Mat(0, 3);
			mColumns[3][1] = Mat(1, 3);
			mColumns[3][2] = Mat(2, 3);
			mColumns[3][3] = Mat(3, 3);
		}
		Mat4x4& operator=(const Matrix& Mat)
		{
			// Covert row-major matrix to column-major
			mColumns[0][0] = Mat(0, 0);
			mColumns[0][1] = Mat(1, 0);
			mColumns[0][2] = Mat(2, 0);
			mColumns[0][3] = Mat(3, 0);

			mColumns[1][0] = Mat(0, 1);
			mColumns[1][1] = Mat(1, 1);
			mColumns[1][2] = Mat(2, 1);
			mColumns[1][3] = Mat(3, 1);

			mColumns[2][0] = Mat(0, 2);
			mColumns[2][1] = Mat(1, 2);
			mColumns[2][2] = Mat(2, 2);
			mColumns[2][3] = Mat(3, 2);

			mColumns[3][0] = Mat(0, 3);
			mColumns[3][1] = Mat(1, 3);
			mColumns[3][2] = Mat(2, 3);
			mColumns[3][3] = Mat(3, 3);
			return *this;
		}

		Mat4x4(const AlignedMatrix& Mat)
		{
			// Covert row-major matrix to column-major
			mColumns[0][0] = Mat(0, 0);
			mColumns[0][1] = Mat(1, 0);
			mColumns[0][2] = Mat(2, 0);
			mColumns[0][3] = Mat(3, 0);

			mColumns[1][0] = Mat(0, 1);
			mColumns[1][1] = Mat(1, 1);
			mColumns[1][2] = Mat(2, 1);
			mColumns[1][3] = Mat(3, 1);

			mColumns[2][0] = Mat(0, 2);
			mColumns[2][1] = Mat(1, 2);
			mColumns[2][2] = Mat(2, 2);
			mColumns[2][3] = Mat(3, 2);

			mColumns[3][0] = Mat(0, 3);
			mColumns[3][1] = Mat(1, 3);
			mColumns[3][2] = Mat(2, 3);
			mColumns[3][3] = Mat(3, 3);
		}
		Mat4x4& operator=(const AlignedMatrix& Mat)
		{
			// Covert row-major matrix to column-major
			mColumns[0][0] = Mat(0, 0);
			mColumns[0][1] = Mat(1, 0);
			mColumns[0][2] = Mat(2, 0);
			mColumns[0][3] = Mat(3, 0);

			mColumns[1][0] = Mat(0, 1);
			mColumns[1][1] = Mat(1, 1);
			mColumns[1][2] = Mat(2, 1);
			mColumns[1][3] = Mat(3, 1);

			mColumns[2][0] = Mat(0, 2);
			mColumns[2][1] = Mat(1, 2);
			mColumns[2][2] = Mat(2, 2);
			mColumns[2][3] = Mat(3, 2);

			mColumns[3][0] = Mat(0, 3);
			mColumns[3][1] = Mat(1, 3);
			mColumns[3][2] = Mat(2, 3);
			mColumns[3][3] = Mat(3, 3);
			return *this;
		}
	};
}
