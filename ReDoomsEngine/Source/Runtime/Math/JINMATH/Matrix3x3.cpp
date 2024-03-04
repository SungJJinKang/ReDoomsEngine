#include "Matrix3x3.h"

#include "Matrix2x2.h"
#include "Matrix4x4.h"

math::Matrix3x3::Matrix3x3(const Matrix2x2& matrix) noexcept
	: columns{ col_type{ matrix.columns[0]}, col_type{matrix.columns[1]}, col_type{0, 0, 1} }
{
}

math::Matrix3x3::Matrix3x3(const Matrix4x4& matrix) noexcept
	: columns{ col_type{matrix.columns[0]}, col_type{matrix.columns[1]}, col_type{matrix.columns[2]} }
{
}

math::Matrix3x3& math::Matrix3x3::operator=(const Matrix2x2& matrix) noexcept
{
	columns[0] = matrix.columns[0];
	columns[1] = matrix.columns[1];
	columns[2] = 0;
	return *this;
}

math::Matrix3x3& math::Matrix3x3::operator=(const Matrix4x4& matrix) noexcept
{
	columns[0] = matrix.columns[0];
	columns[1] = matrix.columns[1];
	columns[2] = matrix.columns[2];
	return *this;
}
