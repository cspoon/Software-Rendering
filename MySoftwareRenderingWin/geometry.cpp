
#include "geometry.h"
#include "transform.h"
#include "Utils.h"

Vector4::Vector4(Point &from, Point &to) {
	x = to.x - from.x;
	y = to.y - from.y;
	z = to.z - from.z;
	w = 0.f;
}

Vector4 Vector4::operator*(Matrix4x4 &m) const {
	Vector4 v;
	float temp[4];
	for (int col = 0; col < 4; col++) {
		temp[col] = m[col][0]*x + m[col][1]*y + m[col][2]*z + m[col][3]*z;
	}
	v.x = temp[0]; v.y = temp[1]; v.z = temp[2]; v.w = temp[3];
	return v;
}

Point Point::operator*(Matrix4x4 &m) const {
	Point p;
	float temp[4];
	for (int col = 0; col < 4; col++) {
		temp[col] = m[0][col] * x + m[1][col] * y + m[2][col] * z + m[3][col] * w;
	}
	p.x = temp[0]; p.y = temp[1]; p.z = temp[2]; p.w = temp[3];
	return p;
}

Point& Point::operator*=(Matrix4x4 &m){
	float temp[4];
	for (int col = 0; col < 4; col++) {
		temp[col] = m[0][col] * x + m[1][col] * y + m[2][col] * z + m[3][col] * w;
	}
	x = temp[0]; y = temp[1]; z = temp[2]; w = temp[3];
	return *this;
}

Vector4 VectorCross(Vector4 a, Vector4 b) {
	Vector4 ret;
	ret.x = a.y*b.z - a.z*b.y;
	ret.y = -(a.x*b.z - a.z*b.x);
	ret.z = a.x*b.y - a.y*b.x;
	ret.w = 1;
	return ret;
}

void VectorInterp(Vector4 *z, const Vector4 *x1, const Vector4 *x2, float t) {
	z->x = Interp(x1->x, x2->x, t);
	z->y = Interp(x1->y, x2->y, t);
	z->z = Interp(x1->z, x2->z, t);
	z->w = 1.0f;
}

void PointInterp(Point *z, const Point *x1, const Point *x2, float t) {
	z->x = Interp(x1->x, x2->x, t);
	z->y = Interp(x1->y, x2->y, t);
	z->z = Interp(x1->z, x2->z, t);
	z->w = 1.0f;
}