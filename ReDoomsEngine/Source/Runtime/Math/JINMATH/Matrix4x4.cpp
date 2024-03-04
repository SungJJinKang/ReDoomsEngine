#include "Matrix4x4.h"

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "EAStdC/EAMemory.h"

const math::Matrix4x4 math::Matrix4x4::identify{ 1 };

math::Matrix4x4::Matrix4x4(const Matrix2x2& matrix) noexcept
	:
	columns
	{
		col_type{ matrix.columns[0]},
		col_type{matrix.columns[1]},
		math::Vector4{0, 0, 1, 0},
		math::Vector4{0, 0, 0, 1}
	}
{
}

math::Matrix4x4::Matrix4x4(const Matrix4x4& matrix) noexcept 
	: columns{}
{
	EA::StdC::Memcpy(data(), matrix.data(), sizeof(Matrix4x4));
}

JINMATH_NO_DISCARD math::Matrix4x4 math::Matrix4x4::operator*(const Matrix4x4& rhs) const noexcept
{
	JINMATH_M256F _REULST_MAT4[2];
	JINMATH_M128F TEMP_M128F;

	const JINMATH_M128F* A = reinterpret_cast<const JINMATH_M128F*>(this);
	//const JINMATH_M128F* A = (const JINMATH_M128F*)this->data(); // this is slower
	const JINMATH_M128F* B = reinterpret_cast<const JINMATH_M128F*>(&rhs);
	JINMATH_M128F* R = reinterpret_cast<JINMATH_M128F*>(&_REULST_MAT4);

	// First row of result (Matrix1[0] * Matrix2).
	TEMP_M128F = M128F_MUL(JINMATH_M128F_REPLICATE(B[0], 0), A[0]);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[0], 1), A[1], TEMP_M128F);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[0], 2), A[2], TEMP_M128F);
	R[0] = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[0], 3), A[3], TEMP_M128F);

	// Second row of result (Matrix1[1] * Matrix2).
	TEMP_M128F = M128F_MUL(JINMATH_M128F_REPLICATE(B[1], 0), A[0]);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[1], 1), A[1], TEMP_M128F);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[1], 2), A[2], TEMP_M128F);
	R[1] = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[1], 3), A[3], TEMP_M128F);

	// Third row of result (Matrix1[2] * Matrix2).
	TEMP_M128F = M128F_MUL(JINMATH_M128F_REPLICATE(B[2], 0), A[0]);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[2], 1), A[1], TEMP_M128F);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[2], 2), A[2], TEMP_M128F);
	R[2] = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[2], 3), A[3], TEMP_M128F);

	// Fourth row of result (Matrix1[3] * Matrix2).
	TEMP_M128F = M128F_MUL(JINMATH_M128F_REPLICATE(B[3], 0), A[0]);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[3], 1), A[1], TEMP_M128F);
	TEMP_M128F = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[3], 2), A[2], TEMP_M128F);
	R[3] = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(B[3], 3), A[3], TEMP_M128F);

	return Matrix4x4{ *reinterpret_cast<Matrix4x4*>(&_REULST_MAT4) };
}

math::Vector4 math::Matrix4x4::operator*(const Vector4& vector) const noexcept
{
	Vector4 TEMP_VEC4{};

	const JINMATH_M128F* A = reinterpret_cast<const JINMATH_M128F*>(this);
	const JINMATH_M128F* B = reinterpret_cast<const JINMATH_M128F*>(&vector);
	JINMATH_M128F* R = reinterpret_cast<JINMATH_M128F*>(&TEMP_VEC4);

	// First row of result (Matrix1[0] * Matrix2).
	*R = M128F_MUL(JINMATH_M128F_REPLICATE(*B, 0), A[0]);
	*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 1), A[1], *R);
	*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 2), A[2], *R);
	*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 3), A[3], *R);

	return Vector4{ TEMP_VEC4 };
}

math::Vector4 math::Matrix4x4::operator*(const Vector3& vector) const noexcept
{
	const Vector4 temp_vec4{ vector };
	Vector4 result{};

	const JINMATH_M128F* A = reinterpret_cast<const JINMATH_M128F*>(this);
	const JINMATH_M128F* B = reinterpret_cast<const JINMATH_M128F*>(temp_vec4.data());
	JINMATH_M128F* R = reinterpret_cast<JINMATH_M128F*>(result.data());

	// First row of result (Matrix1[0] * Matrix2).
	*R = M128F_MUL(JINMATH_M128F_REPLICATE(*B, 0), A[0]);
	*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 1), A[1], *R);
	*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 2), A[2], *R);
	*R = M128F_MUL_AND_ADD(AllOne, A[3], *R);

	return result;
}

math::Matrix4x4::Matrix4x4(const Matrix3x3& matrix) noexcept
	:
	columns
	{
		col_type{matrix.columns[0], 0.0f},
		col_type{matrix.columns[1], 0.0f},
		col_type{matrix.columns[2], 0.0f},
		math::Vector4{0, 0, 0, 1}
	}
{
}

math::Matrix4x4& math::Matrix4x4::operator=(const Matrix2x2& matrix) noexcept
{
	columns[0] = matrix.columns[0];
	columns[1] = matrix.columns[1];
	columns[2] = 0;
	columns[3] = { 0,0,0,1 };
	return *this;
}

math::Matrix4x4& math::Matrix4x4::operator=(const Matrix3x3& matrix) noexcept
{
	columns[0] = matrix.columns[0];
	columns[1] = matrix.columns[1];
	columns[2] = matrix.columns[2];
	columns[3] = { 0,0,0,1 };
	return *this;
}

