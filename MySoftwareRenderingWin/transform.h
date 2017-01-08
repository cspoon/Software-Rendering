#pragma once

#include "stdafx.h"
#include "geometry.h"

class Matrix4x4
{
public:
	Matrix4x4() {
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
		m[0][1] = m[0][2] = m[0][3] = 
		m[1][0] = m[1][2] = m[1][3] = 
		m[2][0] = m[2][1] = m[2][3] = 
		m[3][0] = m[3][1] = m[3][2] = 0.f;
	}
	Matrix4x4(float mat[4][4]);
	Matrix4x4(float t00, float t01, float t02, float t03,
		float t10, float t11, float t12, float t13,
		float t20, float t21, float t22, float t23,
		float t30, float t31, float t32, float t33);

	bool operator==(const Matrix4x4 &) const;
	bool operator!=(const Matrix4x4 &) const;
	float* operator[](int k);
	Matrix4x4 operator*(const Matrix4x4 &rm) const;
	Matrix4x4& operator*=(Matrix4x4 &rm);
	Matrix4x4& operator=(Matrix4x4 &rm);
	void print() const {
		printf("[ ");
		for (int i = 0; i < 4; ++i) {
			printf("  [ ");
			for (int j = 0; j < 4; ++j) {
				printf("%f", m[i][j]);
				if (j != 3) printf(", ");
			}
			printf(" ]\n");
		}
		printf(" ] ");
	}
	Matrix4x4 Transpose(const Matrix4x4 &);
	Matrix4x4 Inverse(const Matrix4x4 &);
	float m[4][4];
};

class Transform {
public:
	void Rotate(Vector4 vec);
	void Rotate(float xAngle, float yAngle, float zAngle);
	Matrix4x4 localToWorldMatrix;
	Matrix4x4 worldToLocalMatrix;

};

Matrix4x4 GetRotateMatrixAlongX(float radX);
Matrix4x4 GetRotateMatrixAlongY(float radY);
Matrix4x4 GetRotateMatrixAlongZ(float radZ);
Matrix4x4 BuildXyzRotationMatrix(float radX, float radY, float radZ);