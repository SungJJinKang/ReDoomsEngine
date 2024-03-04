#pragma once
#include "JINMATHCore.h"

#include "Vector4.h"
#include "SIMD_Core.h"

namespace math
{
	struct Matrix2x2;
	struct Matrix3x3;
	struct Matrix4x4;

	struct Vector2;
	struct Vector3;
	struct Vector4;
	
	/// <summary>
	/// This class is made for SIMD computation. So it's aligned to 32 byte
	/// Because Matrix4X4Floag is aligned to 32 byte, It will works well with __m128 functions
	/// </summary>
	struct alignas(32) Matrix4x4
	{
		using value_type = typename float;
		using type = typename Matrix4x4;
		using col_type = Vector4;

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE static size_t columnCount()  noexcept { return 4; }

		/// <summary>
		/// All columns always is aligned to 16 byte, because Matrix4x4 class is aligned to 16byte
		/// columns[0] start from address of Matrix class
		/// Vector4 is 16 byte -> columns[1] is also aligned to 16 byte
		/// </summary>
		///

		col_type columns[4];

		JINMATH_FORCE_INLINE float* data() noexcept
		{
			return columns[0].data();
		}

		const JINMATH_FORCE_INLINE float* data() const noexcept
		{
			return columns[0].data();
		}

		static const Matrix4x4 identify;

		JINMATH_FORCE_INLINE void InitializeSIMD(const Matrix4x4& matrix) noexcept
		{
			//std::memcpy(this->data(), matrix.data(), sizeof(Matrix4x4)); // this is slower than SIMD

			JINMATH_M256F* A = reinterpret_cast<JINMATH_M256F*>(this);
			const float* B = reinterpret_cast<const float*>(&matrix);
			A[0] = _mm256_load_ps(B); // copy 0 ~ 256 OF B to 0 ~ 256 this
			A[1] = _mm256_load_ps(B + 8); // B + 8 -> B + sizeof(float) * 8  , copy 256 ~ 512 OF B to 256 ~ 512 this

		}

		JINMATH_FORCE_INLINE void InitializeSIMD(const col_type& column) noexcept
		{
			JINMATH_M256F* A = reinterpret_cast<JINMATH_M256F*>(this);
			const JINMATH_M128F* B = reinterpret_cast<const JINMATH_M128F*>(&column);
			A[0] = _mm256_broadcast_ps(&(*B)); // copy 0 ~ 256 OF B to 0 ~ 256 this
			A[1] = _mm256_broadcast_ps(&(*B)); // B + 8 -> B + sizeof(float) * 8  , copy 256 ~ 512 OF B to 256 ~ 512 this
		}

		JINMATH_FORCE_INLINE Matrix4x4() {}

		/// <summary>
		/// diagonal matrix
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE explicit Matrix4x4(value_type value) noexcept
			: columns{
			col_type(value, 0, 0, 0),
			col_type(0, value, 0, 0),
			col_type(0, 0, value, 0),
			col_type(0, 0, 0, value) }
		{
		}
		
		JINMATH_FORCE_INLINE Matrix4x4
		(
			value_type x0, value_type y0, value_type z0, value_type w0,
			value_type x1, value_type y1, value_type z1, value_type w1,
			value_type x2, value_type y2, value_type z2, value_type w2,
			value_type x3, value_type y3, value_type z3, value_type w3
		) noexcept : columns{
			col_type(x0, x1, x2, x3),
			col_type(y0, y1, y2, y3),
			col_type(z0, z1, z2, z3),
			col_type(w0, w1, w2, w3) }
		{
		}

		JINMATH_FORCE_INLINE explicit Matrix4x4(const col_type& columnValue) : columns{}
		{
			this->InitializeSIMD(columnValue);
		}

		JINMATH_FORCE_INLINE Matrix4x4(const col_type& column0Value, const col_type& column1Value, const col_type& column2Value, const col_type& column3Value) noexcept
			: columns{ col_type{column0Value}, col_type{ column1Value}, col_type{column2Value}, col_type{column3Value} }
		{
		}
		
		Matrix4x4(const Matrix2x2& matrix) noexcept;
		Matrix4x4(const Matrix3x3& matrix) noexcept;


		Matrix4x4(const Matrix4x4& matrix) noexcept;
		JINMATH_FORCE_INLINE Matrix4x4& operator=(value_type value) noexcept
		{
			columns[0] = value;
			columns[1] = value;
			columns[2] = value;
			columns[3] = value;
			return *this;
		}

		JINMATH_FORCE_INLINE Matrix4x4& operator=(const col_type& column) noexcept
		{
			this->InitializeSIMD(column);
			return *this;
		}
		
		Matrix4x4& operator=(const Matrix2x2& matrix) noexcept;


		Matrix4x4& operator=(const Matrix3x3& matrix) noexcept;


		JINMATH_FORCE_INLINE Matrix4x4& operator=(const Matrix4x4& matrix) noexcept
		{
			columns[0] = matrix.columns[0];
			columns[1] = matrix.columns[1];
			columns[2] = matrix.columns[2];
			columns[3] = matrix.columns[3];
			return *this;
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE col_type& operator[](size_t i)
		{
			assert(i >= 0 || i < columnCount());
			return columns[i];
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE const col_type& operator[](size_t i) const
		{
			assert(i >= 0 || i < columnCount());
			return columns[i];
		}


		JINMATH_FORCE_INLINE Matrix4x4 operator+(const Matrix4x4& rhs) const noexcept
		{
			return Matrix4x4(columns[0] + rhs.columns[0], columns[1] + rhs.columns[1], columns[2] + rhs.columns[2], columns[3] + rhs.columns[3]);
		}


		JINMATH_FORCE_INLINE Matrix4x4 operator-(const Matrix4x4& rhs) const noexcept
		{
			return Matrix4x4(columns[0] - rhs.columns[0], columns[1] - rhs.columns[1], columns[2] - rhs.columns[2], columns[3] - rhs.columns[3]);
		}
		
		JINMATH_NO_DISCARD Matrix4x4 operator*(const Matrix4x4& rhs) const noexcept;
		JINMATH_NO_DISCARD Vector4 operator*(const Vector4& vector) const noexcept;
		JINMATH_NO_DISCARD Vector4 operator*(const Vector3& vector) const noexcept;

		inline static JINMATH_M128F AllOne{ _mm_set1_ps(1.0f) };

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="X"></typeparam>
		/// <param name="vector"></param>
		/// <returns></returns>
		




		// 		/ <summary>
		// 		/// 
		// 		/// </summary>
		// 		/// <param name="vector"></param>
		// 		/// <returns></returns>
		// 		
		// 		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator*(const Vector3& vector) const noexcept
		// 		{
		// 			Vector4 Result{};
		// 			Matrix4x4 Vec4{ vector };
		// 
		// 			const JINMATH_M128F* A = (const JINMATH_M128F*)this;
		// 			const JINMATH_M128F* B = (const JINMATH_M128F*)Vec4.data();
		// 			JINMATH_M128F* R = reinterpret_cast<JINMATH_M128F*>(&Result);
		// 
		// 			// First row of result (Matrix1[0] * Matrix2).
		// 			*R = M128F_MUL(JINMATH_M128F_REPLICATE(*B, 0), A[0]);
		// 			*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 1), A[1], *R);
		// 			*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 2), A[2], *R);
		// 			*R = M128F_MUL_AND_ADD(JINMATH_M128F_REPLICATE(*B, 3), A[3], *R);
		// 
		// 			return Vector3
		// 			{
		// 				Result
		// 			};
		//   		}

		JINMATH_FORCE_INLINE Matrix4x4 operator+(float rhs) const noexcept
		{
			return Matrix4x4(columns[0] + rhs, columns[1] + rhs, columns[2] + rhs, columns[3] + rhs);
		}

		JINMATH_FORCE_INLINE Matrix4x4 operator-(float rhs) const noexcept
		{
			return Matrix4x4(columns[0] - rhs, columns[1] - rhs, columns[2] - rhs, columns[3] - rhs);
		}

		JINMATH_FORCE_INLINE Matrix4x4 operator*(float rhs) const noexcept
		{
			return Matrix4x4(columns[0] * rhs, columns[1] * rhs, columns[2] * rhs, columns[3] * rhs);
		}

		/*

		JINMATH_FORCE_INLINE Matrix4x4 operator/(const Matrix<4, X>& rhs)
		{
			return Matrix4x4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}


		JINMATH_FORCE_INLINE Matrix4x4 operator%(const Matrix<4, X>& rhs)
		{
			return Matrix4x4(x % rhs.x, y % rhs.y, z % rhs.z, w % rhs.w);
		}
		*/


		JINMATH_FORCE_INLINE Matrix4x4& operator+=(const Matrix4x4& rhs) noexcept
		{
			columns[0] += rhs.columns[0];
			columns[1] += rhs.columns[1];
			columns[2] += rhs.columns[2];
			columns[3] += rhs.columns[3];
			return *this;
		}


		JINMATH_FORCE_INLINE Matrix4x4& operator-=(const Matrix4x4& rhs) noexcept
		{
			columns[0] -= rhs.columns[0];
			columns[1] -= rhs.columns[1];
			columns[2] -= rhs.columns[2];
			columns[3] -= rhs.columns[3];
			return *this;
		}



		JINMATH_FORCE_INLINE Matrix4x4& operator*=(const Matrix4x4& rhs) noexcept
		{
			return (*this = *this * rhs);
		}



		/*

		JINMATH_FORCE_INLINE Matrix4x4& operator/=(const Matrix<4, X>& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}


		JINMATH_FORCE_INLINE Matrix4x4& operator%=(const Matrix<4, X>& rhs)
		{
			x %= rhs.x;
			y %= rhs.y;
			z %= rhs.z;
			w %= rhs.w;
			return *this;
		}
		*/
		//

		JINMATH_FORCE_INLINE Matrix4x4& operator+=(float scalar) noexcept
		{
			columns[0] += scalar;
			columns[1] += scalar;
			columns[2] += scalar;
			columns[3] += scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Matrix4x4& operator-=(float scalar) noexcept
		{
			columns[0] -= scalar;
			columns[1] -= scalar;
			columns[2] -= scalar;
			columns[3] -= scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Matrix4x4& operator*=(float scalar) noexcept
		{
			columns[0] *= scalar;
			columns[1] *= scalar;
			columns[2] *= scalar;
			columns[3] *= scalar;
			return *this;
		}

		/*

		JINMATH_FORCE_INLINE Matrix4x4& operator/=(const X& scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;
			return *this;
		}


		template <typename X, std::enable_if_t<std::is_integral_v<X>, bool> = true>
		JINMATH_FORCE_INLINE Matrix4x4& operator%=(const X& scalar)
		{
			x %= scalar;
			y %= scalar;
			z %= scalar;
			w %= scalar;
			return *this;
		}

		template <typename X, std::enable_if_t<std::is_floating_point_v<X>, bool> = true>
		JINMATH_FORCE_INLINE Matrix4x4& operator%=(const X& scalar)
		{

			x %= std::fmod(x, scalar);
			y %= std::fmod(y, scalar);
			z %= std::fmod(z, scalar);
			w %= std::fmod(w, scalar);
			return *this;
		}
		*/

		//

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Matrix4x4& rhs) const noexcept
		{
			return this->columns[0] == rhs.columns[0] && this->columns[1] == rhs.columns[1] && this->columns[2] == rhs.columns[2] && this->columns[3] == rhs.columns[3];
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Matrix4x4& rhs) const noexcept
		{
			return this->columns[0] != rhs.columns[0] || this->columns[1] != rhs.columns[1] || this->columns[2] != rhs.columns[2] || this->columns[3] != rhs.columns[3];
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(float number) const noexcept
		{
			return this->columns[0] == number && this->columns[1] == number && this->columns[2] == number && this->columns[3] == number;
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(float number) const noexcept
		{
			return this->columns[0] != number || this->columns[1] != number || this->columns[2] != number || this->columns[3] != number;
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix4x4& operator++() noexcept
		{
			++columns[0];
			++columns[1];
			++columns[2];
			++columns[3];
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix4x4 operator++(int32_t) noexcept
		{
			Matrix4x4 Matrix{ *this };
			++* this;
			return Matrix4x4{ Matrix };
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix4x4& operator--() noexcept
		{
			--columns[0];
			--columns[1];
			--columns[2];
			--columns[3];
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix4x4 operator--(int32_t) noexcept
		{
			Matrix4x4 Matrix{ *this };
			--* this;
			return Matrix4x4{ Matrix };
		}

		inline Matrix4x4 inverse() const noexcept
		{
			value_type Coef00 = columns[2][2] * columns[3][3] - columns[3][2] * columns[2][3];
			value_type Coef02 = columns[1][2] * columns[3][3] - columns[3][2] * columns[1][3];
			value_type Coef03 = columns[1][2] * columns[2][3] - columns[2][2] * columns[1][3];

			value_type Coef04 = columns[2][1] * columns[3][3] - columns[3][1] * columns[2][3];
			value_type Coef06 = columns[1][1] * columns[3][3] - columns[3][1] * columns[1][3];
			value_type Coef07 = columns[1][1] * columns[2][3] - columns[2][1] * columns[1][3];

			value_type Coef08 = columns[2][1] * columns[3][2] - columns[3][1] * columns[2][2];
			value_type Coef10 = columns[1][1] * columns[3][2] - columns[3][1] * columns[1][2];
			value_type Coef11 = columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2];

			value_type Coef12 = columns[2][0] * columns[3][3] - columns[3][0] * columns[2][3];
			value_type Coef14 = columns[1][0] * columns[3][3] - columns[3][0] * columns[1][3];
			value_type Coef15 = columns[1][0] * columns[2][3] - columns[2][0] * columns[1][3];

			value_type Coef16 = columns[2][0] * columns[3][2] - columns[3][0] * columns[2][2];
			value_type Coef18 = columns[1][0] * columns[3][2] - columns[3][0] * columns[1][2];
			value_type Coef19 = columns[1][0] * columns[2][2] - columns[2][0] * columns[1][2];

			value_type Coef20 = columns[2][0] * columns[3][1] - columns[3][0] * columns[2][1];
			value_type Coef22 = columns[1][0] * columns[3][1] - columns[3][0] * columns[1][1];
			value_type Coef23 = columns[1][0] * columns[2][1] - columns[2][0] * columns[1][1];

			col_type Fac0(Coef00, Coef00, Coef02, Coef03);
			col_type Fac1(Coef04, Coef04, Coef06, Coef07);
			col_type Fac2(Coef08, Coef08, Coef10, Coef11);
			col_type Fac3(Coef12, Coef12, Coef14, Coef15);
			col_type Fac4(Coef16, Coef16, Coef18, Coef19);
			col_type Fac5(Coef20, Coef20, Coef22, Coef23);

			col_type Vec0(columns[1][0], columns[0][0], columns[0][0], columns[0][0]);
			col_type Vec1(columns[1][1], columns[0][1], columns[0][1], columns[0][1]);
			col_type Vec2(columns[1][2], columns[0][2], columns[0][2], columns[0][2]);
			col_type Vec3(columns[1][3], columns[0][3], columns[0][3], columns[0][3]);

			col_type Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			col_type Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			col_type Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			col_type Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			col_type SignA(+1, -1, +1, -1);
			col_type SignB(-1, +1, -1, +1);
			Matrix4x4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

			col_type Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

			col_type Dot0(columns[0] * Row0);
			value_type Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

			value_type OneOverDeterminant = static_cast<value_type>(1) / Dot1;

			return Matrix4x4{ Inverse * OneOverDeterminant };
		}

		inline Matrix4x4 transpose() const noexcept
		{
			Matrix4x4 Result{};
			Result[0][0] = columns[0][0];
			Result[0][1] = columns[1][0];
			Result[0][2] = columns[2][0];
			Result[0][3] = columns[3][0];

			Result[1][0] = columns[0][1];
			Result[1][1] = columns[1][1];
			Result[1][2] = columns[2][1];
			Result[1][3] = columns[3][1];

			Result[2][0] = columns[0][2];
			Result[2][1] = columns[1][2];
			Result[2][2] = columns[2][2];
			Result[2][3] = columns[3][2];

			Result[3][0] = columns[0][3];
			Result[3][1] = columns[1][3];
			Result[3][2] = columns[2][3];
			Result[3][3] = columns[3][3];
			return Matrix4x4{ Result };
		}

		inline value_type determinant() const noexcept
		{
			value_type SubFactor00 = columns[2][2] * columns[3][3] - columns[3][2] * columns[2][3];
			value_type SubFactor01 = columns[2][1] * columns[3][3] - columns[3][1] * columns[2][3];
			value_type SubFactor02 = columns[2][1] * columns[3][2] - columns[3][1] * columns[2][2];
			value_type SubFactor03 = columns[2][0] * columns[3][3] - columns[3][0] * columns[2][3];
			value_type SubFactor04 = columns[2][0] * columns[3][2] - columns[3][0] * columns[2][2];
			value_type SubFactor05 = columns[2][0] * columns[3][1] - columns[3][0] * columns[2][1];

			col_type DetCof(
				+(columns[1][1] * SubFactor00 - columns[1][2] * SubFactor01 + columns[1][3] * SubFactor02),
				-(columns[1][0] * SubFactor00 - columns[1][2] * SubFactor03 + columns[1][3] * SubFactor04),
				+(columns[1][0] * SubFactor01 - columns[1][1] * SubFactor03 + columns[1][3] * SubFactor05),
				-(columns[1][0] * SubFactor02 - columns[1][1] * SubFactor04 + columns[1][2] * SubFactor05));

			return
				columns[0][0] * DetCof[0] + columns[0][1] * DetCof[1] +
				columns[0][2] * DetCof[2] + columns[0][3] * DetCof[3];
		}


		JINMATH_FORCE_INLINE float trace() const noexcept
		{
			return columns[0][0] + columns[1][1] + columns[2][2] + columns[3][3];
		}
	};

	JINMATH_NO_DISCARD inline Matrix4x4 operator+(const Matrix4x4& matrix) noexcept
	{
		return Matrix4x4{ matrix };
	}

	JINMATH_NO_DISCARD inline Matrix4x4 operator-(const Matrix4x4& matrix) noexcept
	{
		return Matrix4x4(
			-matrix.columns[0],
			-matrix.columns[1],
			-matrix.columns[2],
			-matrix.columns[3]);
	}
	
}
