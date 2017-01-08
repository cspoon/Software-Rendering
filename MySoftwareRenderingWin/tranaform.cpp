
#include "transform.h"
#include "Utils.h"

Matrix4x4::Matrix4x4(float mat[4][4]) {
	memcpy(m, mat, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(float t00, float t01, float t02, float t03,
	float t10, float t11, float t12, float t13,
	float t20, float t21, float t22, float t23,
	float t30, float t31, float t32, float t33) {
	m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
	m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
	m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
	m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
}

float* Matrix4x4::operator[](int k){
	return m[k];
}

bool Matrix4x4::operator==(const Matrix4x4 &m2) const {
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (m[i][j] != m2.m[i][j]) return false;
	return true;
}

bool Matrix4x4::operator!=(const Matrix4x4 &m2) const{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (m[i][j] != m2.m[i][j]) return true;
	return false;
}

Matrix4x4& Matrix4x4::operator= (Matrix4x4 &rm){
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m[i][j] = rm[i][j];
	return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &rm) const {
	Matrix4x4 r;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			r.m[i][j] = m[i][0] * rm.m[0][j] +
			m[i][1] * rm.m[1][j] +
			m[i][2] * rm.m[2][j] +
			m[i][3] * rm.m[3][j];
	return r;
};

Matrix4x4& Matrix4x4::operator *= (Matrix4x4 &rm){
	Matrix4x4 r;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			r.m[i][j] = m[i][0] * rm.m[0][j] +
			m[i][1] * rm.m[1][j] +
			m[i][2] * rm.m[2][j] +
			m[i][3] * rm.m[3][j];
	*this = r;
	return *this;
}

Matrix4x4 Matrix4x4::Transpose(const Matrix4x4 &m) {
	return Matrix4x4(m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
}

Matrix4x4 GetRotateMatrixAlongX(float degreeX){
	float cosTheta = cos(DEG_TO_RAD(degreeX));		//cos(-x) = cos(x)
	float sinTheta = sin(DEG_TO_RAD(degreeX));	//sin(-x) = -sin(x)
	return Matrix4x4(1, 0, 0, 0,
		0, cosTheta, sinTheta, 0,
		0, -sinTheta, cosTheta, 0,
		0, 0, 0, 1);
}

Matrix4x4 GetRotateMatrixAlongY(float degreeY){
	float cosTheta = cos(DEG_TO_RAD(degreeY));
	float sinTheta = sin(DEG_TO_RAD(degreeY));
	return Matrix4x4(cosTheta, 0, -sinTheta, 0,
		0, 1, 0, 0,
		sinTheta, 0, cosTheta, 0,
		0, 0, 0, 1);
}

Matrix4x4 GetRotateMatrixAlongZ(float degreeZ){
	float cosTheta = cos(DEG_TO_RAD(degreeZ));
	float sinTheta = sin(DEG_TO_RAD(degreeZ));
	return Matrix4x4(cosTheta, sinTheta, 0, 0,
		-sinTheta, cosTheta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

Matrix4x4 BuildXyzRotationMatrix(float degreeX, float degreeY, float degreeZ){
	Matrix4x4 ret;
	if (fabs(degreeX) > EPSILON_E5)
		ret *= GetRotateMatrixAlongX(degreeX);
	if (fabs(degreeY) > EPSILON_E5)
		ret *= GetRotateMatrixAlongY(degreeY);
	if (fabs(degreeZ) > EPSILON_E5)
		ret *= GetRotateMatrixAlongZ(degreeZ);
	return ret;
}