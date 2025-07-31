#pragma once
#include <array>
#include <math.h>
#include "Matrix3x3.hpp"
#include "Vector4.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "MathConstants.hpp"

namespace Math
{
	template <class T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(const T a11, const T a12, const T a13, const T a14, const T a21, const T a22, const T a23, const T a24, const T a31, const T a32, const T a33, const T a34, const T a41, const T a42, const T a43, const T a44);
		Matrix4x4<T>(const Vector4<T> vectorA, const Vector4<T> vectorB, const Vector4<T> vectorC, const Vector4<T> vectorD);
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);
		Matrix4x4(const Matrix3x3<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		T& operator[](const int aIndex);
		const T& operator[](const int aIndex) const;
		void operator=(const Matrix4x4<T>& aMatrix);

		void Transpose();
		const Matrix4x4<T> GetTranspose() const;
		void FastInverse();
		const Matrix4x4<T> GetFastInverse() const;
		const Matrix4x4<T> GetInverse() const;
		const Vector3<T> GetRotationsInRadians() const;
		const Vector3<T> GetRotationsInDegrees() const;

		const Vector4<T> GetRowVector(int aIndex) const;

		const Vector3<T> GetRightVector() const;
		const Vector3<T> GetUpVector() const;
		const Vector3<T> GetForwardVector() const;

		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);
		static Matrix4x4<T> CreateRollPitchYawMatrix(Vector3<T> aPitchYawRollVector);
		static Matrix4x4<T> CreateRotationMatrixFromQuaternionVectorXYZW(Vector4<T> aVector);

		static Matrix4x4<T> CreateScaleMatrix(Vector3<T> aScaleVector);
		static Vector3<T> CreateScaleVector(Matrix4x4<T> aMatrix);
		static Matrix4x4<T> CreateTranslationMatrix(Vector3<T> aTranslationVector);
		static Vector3<T> CreateTranslationVector(Matrix4x4<T> aMatrix);

	private:
		std::array<std::array<T, 4>, 4> myMatrix;
	};

	template <class T>
	Matrix4x4<T>::Matrix4x4()
	{
		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				this->operator()(rowIndex, columnIndex) = 0;
			}
		}

		this->operator()(1, 1) = 1;
		this->operator()(2, 2) = 1;
		this->operator()(3, 3) = 1;
		this->operator()(4, 4) = 1;
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(const T a11, const T a12, const T a13, const T a14, const T a21, const T a22, const T a23, const T a24, const T a31, const T a32, const T a33, const T a34, const T a41, const T a42, const T a43, const T a44)
	{
		this->operator()(1, 1) = a11;
		this->operator()(1, 2) = a12;
		this->operator()(1, 3) = a13;
		this->operator()(1, 4) = a14;

		this->operator()(2, 1) = a21;
		this->operator()(2, 2) = a22;
		this->operator()(2, 3) = a23;
		this->operator()(2, 4) = a24;

		this->operator()(3, 1) = a31;
		this->operator()(3, 2) = a32;
		this->operator()(3, 3) = a33;
		this->operator()(3, 4) = a34;

		this->operator()(4, 1) = a41;
		this->operator()(4, 2) = a42;
		this->operator()(4, 3) = a43;
		this->operator()(4, 4) = a44;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Vector4<T> vectorA, const Vector4<T> vectorB, const Vector4<T> vectorC, const Vector4<T> vectorD)
	{
		this->operator()(1, 1) = vectorA.x;
		this->operator()(1, 2) = vectorA.y;
		this->operator()(1, 3) = vectorA.z;
		this->operator()(1, 4) = vectorA.w;

		this->operator()(2, 1) = vectorB.x;
		this->operator()(2, 2) = vectorB.y;
		this->operator()(2, 3) = vectorB.z;
		this->operator()(2, 4) = vectorB.w;

		this->operator()(3, 1) = vectorC.x;
		this->operator()(3, 2) = vectorC.y;
		this->operator()(3, 3) = vectorC.z;
		this->operator()(3, 4) = vectorC.w;

		this->operator()(4, 1) = vectorD.x;
		this->operator()(4, 2) = vectorD.y;
		this->operator()(4, 3) = vectorD.z;
		this->operator()(4, 4) = vectorD.w;
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		*this = aMatrix;
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(const Matrix3x3<T>& aMatrix)
	{
		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				this->operator()(rowIndex, columnIndex) = aMatrix(rowIndex, columnIndex);
			}
		}

		this->operator()(1, 4) = 0;
		this->operator()(2, 4) = 0;
		this->operator()(3, 4) = 0;
		this->operator()(4, 4) = 1;
		this->operator()(4, 1) = 0;
		this->operator()(4, 2) = 0;
		this->operator()(4, 3) = 0;
	}

	template <class T>
	T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow <= 4);
		assert(aColumn > 0 && aColumn <= 4);

		return myMatrix[aRow - 1][aColumn - 1];
	}

	template <class T>
	const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow <= 4);
		assert(aColumn > 0 && aColumn <= 4);

		return myMatrix[aRow - 1][aColumn - 1];
	}

	template <class T>
	T& Matrix4x4<T>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < 16);

		return myMatrix[aIndex / 4][aIndex % 4];
	}

	template <class T>
	const T& Matrix4x4<T>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < 16);

		return myMatrix[aIndex / 4][aIndex % 4];
	}

	template <class T>
	void Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				this->operator()(rowIndex, columnIndex) = aMatrix(rowIndex, columnIndex);
			}
		}
	}

	template <class T>
	void Matrix4x4<T>::Transpose()
	{
		Matrix4x4<T> transposedMatrix;

		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				transposedMatrix(rowIndex, columnIndex) = this->operator()(columnIndex, rowIndex);
			}
		}

		*this = transposedMatrix;
	}

	template <class T>
	const Matrix4x4<T> Matrix4x4<T>::GetTranspose() const
	{
		Matrix4x4<T> newMatrix(*this);
		newMatrix.Transpose();
		return newMatrix;
	}

	template <class T>
	void Matrix4x4<T>::FastInverse()
	{
		Matrix3x3<T> inverseRotation3x3 = this->GetTranspose();
		Matrix4x4<T> inverseRotation4x4(inverseRotation3x3);

		Matrix4x4<T> inverseTranslationMatrix;
		inverseTranslationMatrix(4, 1) = -this->operator()(4, 1);
		inverseTranslationMatrix(4, 2) = -this->operator()(4, 2);
		inverseTranslationMatrix(4, 3) = -this->operator()(4, 3);

		*this = inverseTranslationMatrix * inverseRotation4x4;
	}

	template <class T>
	const Matrix4x4<T> Matrix4x4<T>::GetFastInverse() const
	{
		Matrix4x4 inverseMatrix(*this);
		inverseMatrix.FastInverse();
		return inverseMatrix;
	}

	template <class T>
	const Matrix4x4<T> Matrix4x4<T>::GetInverse() const
	{
		T inv[16];

		Matrix4x4<T> aMatrixToInverse = *this;

		inv[0] =
			aMatrixToInverse[5] * aMatrixToInverse[10] * aMatrixToInverse[15] -
			aMatrixToInverse[5] * aMatrixToInverse[11] * aMatrixToInverse[14] -
			aMatrixToInverse[9] * aMatrixToInverse[6] * aMatrixToInverse[15] +
			aMatrixToInverse[9] * aMatrixToInverse[7] * aMatrixToInverse[14] +
			aMatrixToInverse[13] * aMatrixToInverse[6] * aMatrixToInverse[11] -
			aMatrixToInverse[13] * aMatrixToInverse[7] * aMatrixToInverse[10];

		inv[4] = -aMatrixToInverse[4] * aMatrixToInverse[10] * aMatrixToInverse[15] +
			aMatrixToInverse[4] * aMatrixToInverse[11] * aMatrixToInverse[14] +
			aMatrixToInverse[8] * aMatrixToInverse[6] * aMatrixToInverse[15] -
			aMatrixToInverse[8] * aMatrixToInverse[7] * aMatrixToInverse[14] -
			aMatrixToInverse[12] * aMatrixToInverse[6] * aMatrixToInverse[11] +
			aMatrixToInverse[12] * aMatrixToInverse[7] * aMatrixToInverse[10];

		inv[8] = aMatrixToInverse[4] * aMatrixToInverse[9] * aMatrixToInverse[15] -
			aMatrixToInverse[4] * aMatrixToInverse[11] * aMatrixToInverse[13] -
			aMatrixToInverse[8] * aMatrixToInverse[5] * aMatrixToInverse[15] +
			aMatrixToInverse[8] * aMatrixToInverse[7] * aMatrixToInverse[13] +
			aMatrixToInverse[12] * aMatrixToInverse[5] * aMatrixToInverse[11] -
			aMatrixToInverse[12] * aMatrixToInverse[7] * aMatrixToInverse[9];

		inv[12] = -aMatrixToInverse[4] * aMatrixToInverse[9] * aMatrixToInverse[14] +
			aMatrixToInverse[4] * aMatrixToInverse[10] * aMatrixToInverse[13] +
			aMatrixToInverse[8] * aMatrixToInverse[5] * aMatrixToInverse[14] -
			aMatrixToInverse[8] * aMatrixToInverse[6] * aMatrixToInverse[13] -
			aMatrixToInverse[12] * aMatrixToInverse[5] * aMatrixToInverse[10] +
			aMatrixToInverse[12] * aMatrixToInverse[6] * aMatrixToInverse[9];

		inv[1] = -aMatrixToInverse[1] * aMatrixToInverse[10] * aMatrixToInverse[15] +
			aMatrixToInverse[1] * aMatrixToInverse[11] * aMatrixToInverse[14] +
			aMatrixToInverse[9] * aMatrixToInverse[2] * aMatrixToInverse[15] -
			aMatrixToInverse[9] * aMatrixToInverse[3] * aMatrixToInverse[14] -
			aMatrixToInverse[13] * aMatrixToInverse[2] * aMatrixToInverse[11] +
			aMatrixToInverse[13] * aMatrixToInverse[3] * aMatrixToInverse[10];

		inv[5] = aMatrixToInverse[0] * aMatrixToInverse[10] * aMatrixToInverse[15] -
			aMatrixToInverse[0] * aMatrixToInverse[11] * aMatrixToInverse[14] -
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[15] +
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[14] +
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[11] -
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[10];

		inv[9] = -aMatrixToInverse[0] * aMatrixToInverse[9] * aMatrixToInverse[15] +
			aMatrixToInverse[0] * aMatrixToInverse[11] * aMatrixToInverse[13] +
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[15] -
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[13] -
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[11] +
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[9];

		inv[13] = aMatrixToInverse[0] * aMatrixToInverse[9] * aMatrixToInverse[14] -
			aMatrixToInverse[0] * aMatrixToInverse[10] * aMatrixToInverse[13] -
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[14] +
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[13] +
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[10] -
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[9];

		inv[2] = aMatrixToInverse[1] * aMatrixToInverse[6] * aMatrixToInverse[15] -
			aMatrixToInverse[1] * aMatrixToInverse[7] * aMatrixToInverse[14] -
			aMatrixToInverse[5] * aMatrixToInverse[2] * aMatrixToInverse[15] +
			aMatrixToInverse[5] * aMatrixToInverse[3] * aMatrixToInverse[14] +
			aMatrixToInverse[13] * aMatrixToInverse[2] * aMatrixToInverse[7] -
			aMatrixToInverse[13] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[6] = -aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[15] +
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[14] +
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[15] -
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[14] -
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[7] +
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[10] = aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[15] -
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[13] -
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[15] +
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[13] +
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[7] -
			aMatrixToInverse[12] * aMatrixToInverse[3] * aMatrixToInverse[5];

		inv[14] = -aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[14] +
			aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[13] +
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[14] -
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[13] -
			aMatrixToInverse[12] * aMatrixToInverse[1] * aMatrixToInverse[6] +
			aMatrixToInverse[12] * aMatrixToInverse[2] * aMatrixToInverse[5];

		inv[3] = -aMatrixToInverse[1] * aMatrixToInverse[6] * aMatrixToInverse[11] +
			aMatrixToInverse[1] * aMatrixToInverse[7] * aMatrixToInverse[10] +
			aMatrixToInverse[5] * aMatrixToInverse[2] * aMatrixToInverse[11] -
			aMatrixToInverse[5] * aMatrixToInverse[3] * aMatrixToInverse[10] -
			aMatrixToInverse[9] * aMatrixToInverse[2] * aMatrixToInverse[7] +
			aMatrixToInverse[9] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[7] = aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[11] -
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[10] -
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[11] +
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[10] +
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[7] -
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[6];

		inv[11] = -aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[11] +
			aMatrixToInverse[0] * aMatrixToInverse[7] * aMatrixToInverse[9] +
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[11] -
			aMatrixToInverse[4] * aMatrixToInverse[3] * aMatrixToInverse[9] -
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[7] +
			aMatrixToInverse[8] * aMatrixToInverse[3] * aMatrixToInverse[5];

		inv[15] = aMatrixToInverse[0] * aMatrixToInverse[5] * aMatrixToInverse[10] -
			aMatrixToInverse[0] * aMatrixToInverse[6] * aMatrixToInverse[9] -
			aMatrixToInverse[4] * aMatrixToInverse[1] * aMatrixToInverse[10] +
			aMatrixToInverse[4] * aMatrixToInverse[2] * aMatrixToInverse[9] +
			aMatrixToInverse[8] * aMatrixToInverse[1] * aMatrixToInverse[6] -
			aMatrixToInverse[8] * aMatrixToInverse[2] * aMatrixToInverse[5];

		T det = aMatrixToInverse[0] * inv[0] + aMatrixToInverse[1] * inv[4] + aMatrixToInverse[2] * inv[8] + aMatrixToInverse[3] * inv[12];

		det = T(1.0) / det;

		Matrix4x4<T> returnMatrix;
		for (int i = 0; i < 16; i++)
		{
			returnMatrix[i] = inv[i] * det;
		}

		return returnMatrix;
	}

	template<class T>
	const inline Vector3<T> Matrix4x4<T>::GetRotationsInRadians() const
	{
		T xRot = static_cast<T>(std::atan2(this->operator()(3, 2), this->operator()(2, 3)));
		T yRot = static_cast<T>(std::asin(this->operator()(1, 2)));
		T zRot = static_cast<T>(std::atan2(this->operator()(1, 2), this->operator()(2, 2)));
		return Vector3<T>(xRot, yRot, zRot);
	}

	template<class T>
	const inline Vector3<T> Matrix4x4<T>::GetRotationsInDegrees() const
	{
		Vector3<T> rotations = GetRotationsInRadians();
		rotations.x *= static_cast<T>(RADIANS_TO_DEGREES);
		rotations.y *= static_cast<T>(RADIANS_TO_DEGREES);
		rotations.z *= static_cast<T>(RADIANS_TO_DEGREES);
		return rotations;
	}

	template<class T>
	const inline Vector4<T> Matrix4x4<T>::GetRowVector(int aIndex) const
	{
		assert(aIndex > 0 && aIndex <= 4);

		return Vector4<T>(this->operator()(aIndex, 1), this->operator()(aIndex, 2), this->operator()(aIndex, 3), this->operator()(aIndex, 4));
	}

	template<class T>
	inline const Vector3<T> Matrix4x4<T>::GetRightVector() const
	{
		return Vector3<T>(this->operator()(1, 1), this->operator()(1, 2), this->operator()(1, 3)).GetNormalized();
	}

	template<class T>
	inline const Vector3<T> Matrix4x4<T>::GetUpVector() const
	{
		return Vector3<T>(this->operator()(2, 1), this->operator()(2, 2), this->operator()(2, 3)).GetNormalized();
	}

	template<class T>
	inline const Vector3<T> Matrix4x4<T>::GetForwardVector() const
	{
		return Vector3<T>(this->operator()(3, 1), this->operator()(3, 2), this->operator()(3, 3)).GetNormalized();
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		T cosRadians = cos(aAngleInRadians);
		T sinRadians = sin(aAngleInRadians);

		Matrix4x4<T> newMatrix(1, 0, 0, 0,
								0, cosRadians, sinRadians, 0,
								0, -sinRadians, cosRadians, 0,
								0, 0, 0, 1);
		return newMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		T cosRadians = cos(aAngleInRadians);
		T sinRadians = sin(aAngleInRadians);

		Matrix4x4<T> newMatrix(cosRadians, 0, -sinRadians, 0,
								0, 1, 0, 0,
								sinRadians, 0, cosRadians, 0,
								0, 0, 0, 1);
		return newMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		T cosRadians = cos(aAngleInRadians);
		T sinRadians = sin(aAngleInRadians);

		Matrix4x4<T> newMatrix(cosRadians, sinRadians, 0, 0,
								-sinRadians, cosRadians, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1);
		return newMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRollPitchYawMatrix(Vector3<T> aPitchYawRollVector)
	{
		//pitch * roll * yaw

		aPitchYawRollVector.x *= static_cast<T>(DEGREES_TO_RADIANS);
		aPitchYawRollVector.y *= static_cast<T>(DEGREES_TO_RADIANS);
		aPitchYawRollVector.z *= static_cast<T>(DEGREES_TO_RADIANS);

		Quaternion<T> rotation = Quaternion<T>(aPitchYawRollVector);
		Vector4<T> values = { rotation.x, rotation.y, rotation.z, rotation.w };
		return CreateRotationMatrixFromQuaternionVectorXYZW(values);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrixFromQuaternionVectorXYZW(Vector4<T> aVector)
	{
		Matrix4x4<T> result;
		Vector4<T> v = { aVector.w, aVector.x, aVector.y, aVector.z };
		aVector = v;
		T qxx(aVector.y * aVector.y);
		T qyy(aVector.z * aVector.z);
		T qzz(aVector.w * aVector.w);

		T qxz(aVector.y * aVector.w);
		T qxy(aVector.y * aVector.z);
		T qyz(aVector.z * aVector.w);

		T qwx(aVector.x * aVector.y);
		T qwy(aVector.x * aVector.z);
		T qwz(aVector.x * aVector.w);

		result(1, 1) = T(1) - T(2) * (qyy + qzz);
		result(1, 2) = T(2) * (qxy + qwz);
		result(1, 3) = T(2) * (qxz - qwy);

		result(2, 1) = T(2) * (qxy - qwz);
		result(2, 2) = T(1) - T(2) * (qxx + qzz);
		result(2, 3) = T(2) * (qyz + qwx);

		result(3, 1) = T(2) * (qxz + qwy);
		result(3, 2) = T(2) * (qyz - qwx);
		result(3, 3) = T(1) - T(2) * (qxx + qyy);
		return result;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(Vector3<T> aScaleVector)
	{
		Matrix4x4<T> result;
		result(1, 1) = aScaleVector.x;
		result(2, 2) = aScaleVector.y;
		result(3, 3) = aScaleVector.z;
		return result;
	}

	template<class T>
	inline Vector3<T> Matrix4x4<T>::CreateScaleVector(Matrix4x4<T> aMatrix)
	{
		return { aMatrix(1, 1), aMatrix(2, 2), aMatrix(3, 3) };
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(Vector3<T> aTranslationVector)
	{
		Matrix4x4<T> result;
		result(4, 1) = aTranslationVector.x;
		result(4, 2) = aTranslationVector.y;
		result(4, 3) = aTranslationVector.z;
		return result;
	}

	template<class T>
	inline Vector3<T> Matrix4x4<T>::CreateTranslationVector(Matrix4x4<T> aMatrix)
	{
		return { aMatrix(4, 1), aMatrix(4, 2), aMatrix(4, 3) };
	}

	template <class T>
	const Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> newMatrix;

		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				newMatrix(rowIndex, columnIndex) = aMatrix0(rowIndex, columnIndex) + aMatrix1(rowIndex, columnIndex);
			}
		}

		return newMatrix;
	}

	template <class T>
	const void operator+=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 + aMatrix1;
	}

	template <class T>
	const Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> newMatrix;

		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				newMatrix(rowIndex, columnIndex) = aMatrix0(rowIndex, columnIndex) - aMatrix1(rowIndex, columnIndex);
			}
		}

		return newMatrix;
	}

	template <class T>
	const void operator-=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 - aMatrix1;
	}

	template <class T>
	const Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> newMatrix;

		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				T x = aMatrix0(rowIndex, 1) * aMatrix1(1, columnIndex);
				T y = aMatrix0(rowIndex, 2) * aMatrix1(2, columnIndex);
				T z = aMatrix0(rowIndex, 3) * aMatrix1(3, columnIndex);
				T w = aMatrix0(rowIndex, 4) * aMatrix1(4, columnIndex);
				newMatrix(rowIndex, columnIndex) = x + y + z + w;
			}
		}

		return newMatrix;
	}

	template <class T>
	const void operator*=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 * aMatrix1;
	}

	template <class T>
	const Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> newVector;

		newVector.x = aVector.x * aMatrix(1, 1) +
						aVector.y * aMatrix(2, 1) +
						aVector.z * aMatrix(3, 1) +
						aVector.w * aMatrix(4, 1);

		newVector.y = aVector.x * aMatrix(1, 2) +
						aVector.y * aMatrix(2, 2) +
						aVector.z * aMatrix(3, 2) +
						aVector.w * aMatrix(4, 2);

		newVector.z = aVector.x * aMatrix(1, 3) +
						aVector.y * aMatrix(2, 3) +
						aVector.z * aMatrix(3, 3) +
						aVector.w * aMatrix(4, 3);

		newVector.w = aVector.x * aMatrix(1, 4) +
						aVector.y * aMatrix(2, 4) +
						aVector.z * aMatrix(3, 4) +
						aVector.w * aMatrix(4, 4);

		return newVector;
	}

	template <class T>
	const bool operator==(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		for (int rowIndex = 1; rowIndex <= 4; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 4; columnIndex++)
			{
				if (aMatrix0(rowIndex, columnIndex) != aMatrix1(rowIndex, columnIndex))
				{
					return false;
				}
			}
		}

		return true;
	}


	typedef Matrix4x4<float> Matrix4x4f;
}