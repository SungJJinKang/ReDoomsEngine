#include "Matrix2x2.h"

#include "Vector2.h"
#include "Vector3.h"

#include "Matrix3x3.h"
#include "Matrix4x4.h"


math::Matrix2x2::Matrix2x2(const Matrix3x3& matrix) noexcept
	: columns{ col_type{matrix.columns[0]}, col_type{matrix.columns[1]} }
{
}

math::Matrix2x2::Matrix2x2(const Matrix4x4& matrix) noexcept
: columns{ col_type{matrix.columns[0]}, col_type{matrix.columns[1]} }
{
}

math::Matrix2x2& math::Matrix2x2::operator=(const ::math::Matrix3x3& matrix) noexcept
{
	columns[0] = matrix.columns[0];
	columns[1] = matrix.columns[1];
	return *this;
}

math::Matrix2x2& math::Matrix2x2::operator=(const ::math::Matrix4x4& matrix) noexcept
{
	columns[0] = matrix.columns[0];
	columns[1] = matrix.columns[1];
	return *this;
}

math::Matrix2x2 math::Matrix2x2::operator*(const Matrix2x2& rhs) const noexcept
{
	const col_type SrcA0 = col_type{ columns[0] };
	const col_type SrcA1 = col_type{ columns[1] };

	const col_type SrcB0 = col_type{ rhs[0] };
	const col_type SrcB1 = col_type{ rhs[1] };

	Matrix2x2 Result{};
	Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1];
	Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1];
	return type{ Result };
}
