#pragma once
#include "JINMATHCore.h"

#include "Vector3.h"

namespace math
{
	struct Matrix2x2;
	struct Matrix3x3;
	struct Matrix4x4;

	struct Vector2;
	struct Vector3;
	struct Vector4;

	struct Matrix3x3
	{
		using value_type = float;
		using type = typename Matrix3x3;
		using col_type = Vector3;

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE static size_t columnCount()  noexcept { return 3; }

		col_type columns[3];

		JINMATH_FORCE_INLINE float* data() noexcept
		{
			return columns[0].data();
		}

		const JINMATH_FORCE_INLINE float* data() const noexcept
		{
			return columns[0].data();
		}

		static const Matrix3x3 identify;

		JINMATH_FORCE_INLINE Matrix3x3() {}
		/// <summary>
		/// diagonal matrix
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE explicit Matrix3x3(value_type value) noexcept
			: columns{ 
			col_type(value, 0, 0),
			col_type(0, value, 0), 
			col_type(0, 0, value) }
		{
		}

		JINMATH_FORCE_INLINE Matrix3x3
		(
			value_type x0, value_type y0, value_type z0,
			value_type x1, value_type y1, value_type z1,
			value_type x2, value_type y2, value_type z2
		) noexcept : columns{
			col_type(x0, x1, x2),
			col_type(y0, y1, y2),
			col_type(z0, z1, z2) }
		{
		}

		Matrix3x3(const col_type& column0Value, const col_type& column1Value, const col_type& column2Value) noexcept
			: columns{ col_type{column0Value}, col_type{column1Value}, col_type{column2Value} }
		{
		}
		
		explicit Matrix3x3(const Matrix2x2& matrix) noexcept;
		JINMATH_FORCE_INLINE Matrix3x3(const Matrix3x3& matrix) noexcept
			: columns{ col_type{matrix.columns[0]}, col_type{matrix.columns[1]}, col_type{matrix.columns[2]} }
		{
		}
		explicit Matrix3x3(const Matrix4x4& matrix) noexcept;

		JINMATH_FORCE_INLINE Matrix3x3& operator=(value_type value) noexcept
		{
			columns[0] = value;
			columns[1] = value;
			columns[2] = value;
			return *this;
		}

		JINMATH_FORCE_INLINE Matrix3x3& operator=(const col_type& column) noexcept
		{
			columns[0] = column;
			columns[1] = column;
			columns[2] = column;
			return *this;
		}
		
		Matrix3x3& operator=(const Matrix2x2& matrix) noexcept;
		JINMATH_FORCE_INLINE Matrix3x3& operator=(const Matrix3x3& matrix) noexcept
		{
			columns[0] = matrix.columns[0];
			columns[1] = matrix.columns[1];
			columns[2] = matrix.columns[2];
			return *this;
		}
		Matrix3x3& operator=(const Matrix4x4& matrix) noexcept;

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



		
		JINMATH_FORCE_INLINE Matrix3x3 operator+(const Matrix3x3& rhs) const noexcept
		{
			return Matrix3x3(columns[0] + rhs.columns[0], columns[1] + rhs.columns[1], columns[2] + rhs.columns[2]);
		}

		
		JINMATH_FORCE_INLINE Matrix3x3 operator-(const Matrix3x3& rhs) const noexcept
		{
			return Matrix3x3(columns[0] - rhs.columns[0], columns[1] - rhs.columns[1], columns[2] - rhs.columns[2]);
		}

		
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Matrix3x3 operator*(const Matrix3x3& rhs) const noexcept
		{
			const col_type SrcA0 = col_type{ columns[0] };
			const col_type SrcA1 = col_type{ columns[1] };
			const col_type SrcA2 = col_type{ columns[2] };

			const col_type SrcB0 = col_type{ rhs[0] };
			const col_type SrcB1 = col_type{ rhs[1] };
			const col_type SrcB2 = col_type{ rhs[2] };

			Matrix3x3 Result{};
			Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2];
			Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2];
			Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2];
			return Matrix3x3{ Result };
		}

		
		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE Vector3 operator*(const Vector3& vector) const noexcept
		{
			return  Vector3
			{
				this->columns[0][0] * vector[0] + this->columns[1][0] * vector[1] + this->columns[2][0] * vector[2],
					this->columns[0][1] * vector[0] + this->columns[1][1] * vector[1] + this->columns[2][1] * vector[2],
					this->columns[0][2] * vector[0] + this->columns[1][2] * vector[1] + this->columns[2][2] * vector[2]
			};
		}

		JINMATH_FORCE_INLINE Matrix3x3 operator+(float rhs) const noexcept
		{
			return Matrix3x3(columns[0] + rhs, columns[1] + rhs, columns[2] + rhs);
		}

		JINMATH_FORCE_INLINE Matrix3x3 operator-(float rhs) const noexcept
		{
			return Matrix3x3(columns[0] - rhs, columns[1] - rhs, columns[2] - rhs);
		}

		JINMATH_FORCE_INLINE Matrix3x3 operator*(float rhs) const noexcept
		{
			return Matrix3x3(columns[0] * rhs, columns[1] * rhs, columns[2] * rhs);
		}
		/*
		
		JINMATH_FORCE_INLINE Matrix3x3 operator/(const Matrix<4, X>& rhs)
		{
			return Matrix3x3(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}

		
		JINMATH_FORCE_INLINE Matrix3x3 operator%(const Matrix<4, X>& rhs)
		{
			return Matrix3x3(x % rhs.x, y % rhs.y, z % rhs.z, w % rhs.w);
		}
		*/

		
		JINMATH_FORCE_INLINE Matrix3x3& operator+=(const Matrix3x3& rhs) noexcept
		{
			columns[0] += rhs.columns[0];
			columns[1] += rhs.columns[1];
			columns[2] += rhs.columns[2];
			return *this;
		}

		
		JINMATH_FORCE_INLINE Matrix3x3& operator-=(const Matrix3x3& rhs) noexcept
		{
			columns[0] -= rhs.columns[0];
			columns[1] -= rhs.columns[1];
			columns[2] -= rhs.columns[2];
			return *this;
		}


		
		JINMATH_FORCE_INLINE Matrix3x3& operator*=(const Matrix3x3& rhs) noexcept
		{
			return (*this = *this * rhs);
		}



		/*
		
		JINMATH_FORCE_INLINE Matrix3x3& operator/=(const Matrix<4, X>& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		
		JINMATH_FORCE_INLINE Matrix3x3& operator%=(const Matrix<4, X>& rhs)
		{
			x %= rhs.x;
			y %= rhs.y;
			z %= rhs.z;
			w %= rhs.w;
			return *this;
		}
		*/
		//

		JINMATH_FORCE_INLINE Matrix3x3& operator+=(float scalar) noexcept
		{
			columns[0] += scalar;
			columns[1] += scalar;
			columns[2] += scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Matrix3x3& operator-=(float scalar) noexcept
		{
			columns[0] -= scalar;
			columns[1] -= scalar;
			columns[2] -= scalar;
			return *this;
		}

		JINMATH_FORCE_INLINE Matrix3x3& operator*=(float scalar) noexcept
		{
			columns[0] *= scalar;
			columns[1] *= scalar;
			columns[2] *= scalar;
			return *this;
		}

		/*
		
		JINMATH_FORCE_INLINE Matrix3x3& operator/=(const X& scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			w /= scalar;
			return *this;
		}


		template <typename X, std::enable_if_t<std::is_integral_v<X>, bool> = true>
		JINMATH_FORCE_INLINE Matrix3x3& operator%=(const X& scalar)
		{
			x %= scalar;
			y %= scalar;
			z %= scalar;
			w %= scalar;
			return *this;
		}

		template <typename X, std::enable_if_t<std::is_floating_point_v<X>, bool> = true>
		JINMATH_FORCE_INLINE Matrix3x3& operator%=(const X& scalar)
		{

			x %= std::fmod(x, scalar);
			y %= std::fmod(y, scalar);
			z %= std::fmod(z, scalar);
			w %= std::fmod(w, scalar);
			return *this;
		}
		*/

		//

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(const Matrix3x3& rhs) const noexcept
		{
			return this->columns[0] == rhs.columns[0] && this->columns[1] == rhs.columns[1] && this->columns[2] == rhs.columns[2];
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(const Matrix3x3& rhs) const noexcept
		{
			return this->columns[0] != rhs.columns[0] || this->columns[1] != rhs.columns[1] || this->columns[2] != rhs.columns[2];
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator==(float number) const noexcept
		{
			return this->columns[0] == number && this->columns[1] == number && this->columns[2] == number;
		}

		JINMATH_NO_DISCARD JINMATH_FORCE_INLINE bool operator!=(float number) const noexcept
		{
			return this->columns[0] != number || this->columns[1] != number || this->columns[2] != number;
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix3x3& operator++() noexcept
		{
			++columns[0];
			++columns[1];
			++columns[2];
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix3x3 operator++(int32_t) noexcept
		{
			Matrix3x3 Matrix{ *this };
			++* this;
			return Matrix;
		}

		/// <summary>
		/// prefix
		/// </summary>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix3x3& operator--() noexcept
		{
			--columns[0];
			--columns[1];
			--columns[2];
			return *this;
		}

		/// <summary>
		/// postfix
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		JINMATH_FORCE_INLINE Matrix3x3 operator--(int32_t) noexcept
		{
			Matrix3x3 Matrix{ *this };
			--* this;
			return Matrix;
		}

		template <typename U = float, std::enable_if_t<std::is_signed_v<U>, bool> = true>
		inline Matrix3x3 inverse() const noexcept
		{
			value_type OneOverDetercolumnsinant = static_cast<value_type>(1) / (
				+columns[0][0] * (columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2])
				- columns[1][0] * (columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2])
				+ columns[2][0] * (columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2]));

			Matrix3x3 Inverse;
			Inverse[0][0] = +(columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2]) * OneOverDetercolumnsinant;
			Inverse[1][0] = -(columns[1][0] * columns[2][2] - columns[2][0] * columns[1][2]) * OneOverDetercolumnsinant;
			Inverse[2][0] = +(columns[1][0] * columns[2][1] - columns[2][0] * columns[1][1]) * OneOverDetercolumnsinant;
			Inverse[0][1] = -(columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2]) * OneOverDetercolumnsinant;
			Inverse[1][1] = +(columns[0][0] * columns[2][2] - columns[2][0] * columns[0][2]) * OneOverDetercolumnsinant;
			Inverse[2][1] = -(columns[0][0] * columns[2][1] - columns[2][0] * columns[0][1]) * OneOverDetercolumnsinant;
			Inverse[0][2] = +(columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2]) * OneOverDetercolumnsinant;
			Inverse[1][2] = -(columns[0][0] * columns[1][2] - columns[1][0] * columns[0][2]) * OneOverDetercolumnsinant;
			Inverse[2][2] = +(columns[0][0] * columns[1][1] - columns[1][0] * columns[0][1]) * OneOverDetercolumnsinant;

			return Inverse;
		}

		inline Matrix3x3 transpose() const noexcept
		{
			Matrix3x3 Result{};
			Result[0][0] = columns[0][0];
			Result[0][1] = columns[1][0];
			Result[0][2] = columns[2][0];

			Result[1][0] = columns[0][1];
			Result[1][1] = columns[1][1];
			Result[1][2] = columns[2][1];

			Result[2][0] = columns[0][2];
			Result[2][1] = columns[1][2];
			Result[2][2] = columns[2][2];
			return Result;
		}

		inline value_type determinant() const noexcept
		{
			return
				+columns[0][0] * (columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2])
				- columns[1][0] * (columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2])
				+ columns[2][0] * (columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2]);
		}

		inline float trace() const noexcept
		{
			return columns[0][0] + columns[1][1] + columns[2][2];
		}
	};

	JINMATH_NO_DISCARD inline Matrix3x3 operator+(const Matrix3x3& matrix) noexcept
	{
		return matrix;
	}

	JINMATH_NO_DISCARD inline Matrix3x3 operator-(const Matrix3x3& matrix) noexcept
	{
		return Matrix3x3(
			-matrix.columns[0],
			-matrix.columns[1],
			-matrix.columns[2]);
	}
}

