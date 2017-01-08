#pragma once

class Matrix4x4;
class Point;

#include "math.h"

#define VecForward Vector4(0, 0, 1, 1)
#define VecBackWard Vector4(0, 0, -1, 1)
#define VecLeft Vector4(-1, 0, 0, 1)
#define VecRight Vector4(1, 0, 0, 1)

class Vector4 
{
public:
	Vector4() { x = y = z = w = 0.f; }

	Vector4(float xx, float yy, float zz, float ww)
		: x(xx), y(yy), z(zz), w(ww) {}

	Vector4(Point &from, Point &to);
	Vector4(const Vector4 &v) {
		x = v.x; y = v.y; z = v.z; w = v.w;
	}

	bool HasNaNs() const { return isnan(x) || isnan(y) || isnan(z); }

	Vector4& operator=(const Vector4 &v) {
		x = v.x; y = v.y; z = v.z; w = v.w;
		return *this;
	}

	Vector4 operator+(const Vector4 &v) const {
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4& operator+=(const Vector4 &v) {
		x += v.x; y += v.y; z += v.z; w += v.w;
		return *this;
	}

	Vector4 operator-(const Vector4 &v) const {
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4& operator-=(const Vector4 &v) {
		x -= v.x; y -= v.y; z -= v.z; w -= v.w;
		return *this;
	}

	Vector4 operator*(float f) const { return Vector4(f*x, f*y, f*z, f*w); }
	
	float operator*(Vector4 v) const { return x*v.x + y*v.y + z*v.z; }
	
	Vector4& operator*=(float f) {
		x *= f; y *= f; z *= f; w *= w;
		return *this;
	}

	Vector4 operator*(Matrix4x4 &m) const;
	
	Vector4 operator/(float f) const {
		float inv = 1.f / f;
		return Vector4(x * inv, y * inv, z * inv, w * inv);
	}

	Vector4& operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv; w *= inv;
		return *this;
	}
	Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }
	float operator[](int i) const {
		return (&x)[i];
	}

	float& operator[](int i) {
		return (&x)[i];
	}
	float LengthSquared() const { return x*x + y*y + z*z + w*w; }
	float Length() const { return sqrtf(LengthSquared()); }

	bool operator==(const Vector4 &v) const {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}
	bool operator!=(const Vector4 &v) const {
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	float x, y, z, w;
};

class Point 
{
public:
	Point() { x = y = z = 0.f; w = 1.0f; }
	Point(float xx, float yy, float zz) : x(xx), y(yy), z(zz) { w = 1.f; }
	Point(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww){}

	Point& operator=(const Point &p) {
		x = p.x; y = p.y; z = p.z; w = p.w;
		return *this;
	}
	Point(const Point &p) {
		x = p.x; y = p.y; z = p.z; w = p.w;
	}

	Point operator+(const Vector4 &v) const {
		return Point(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Point& operator+=(const Vector4 &v) {
		x += v.x; y += v.y; z += v.z; w += v.w;
		return *this;
	}
	Vector4 operator-(const Point &p) const {
		return Vector4(x - p.x, y - p.y, z - p.z, w - p.w);
	}

	Point operator-(const Vector4 &v) const {
		return Point(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Point& operator-=(const Vector4 &v) {
		x -= v.x; y -= v.y; z -= v.z; w -= v.w;
		return *this;
	}
	Point& operator+=(const Point &p) {
		x += p.x; y += p.y; z += p.z;
		return *this;
	}
	Point operator+(const Point &p) const {
		return Point(x + p.x, y + p.y, z + p.z, 1);
	}
	Point operator* (float f) const {
		return Point(f*x, f*y, f*z, f*w);
	}
	Point& operator*=(float f) {
		x *= f; y *= f; z *= f; w *= f;
		return *this;
	}
	Point operator*(Matrix4x4 &m) const;
	Point& operator*=(Matrix4x4 &m);

	Point operator/ (float f) const {
		float inv = 1.f / f;
		return Point(inv*x, inv*y, inv*z, inv*w);
	}
	Point& operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv; w *= inv;
		return *this;
	}
	float operator[](int i) const {
		return (&x)[i];
	}

	float& operator[](int i) {
		return (&x)[i];
	}
	bool HasNaNs() const {
		return isnan(x) || isnan(y) || isnan(z);
	}

	bool operator==(const Point &p) const {
		return x == p.x && y == p.y && z == p.z && w == p.w;
	}
	bool operator!=(const Point &p) const {
		return x != p.x || y != p.y || z != p.z || w != p.w;
	}

	float x, y, z, w;
};

Vector4 VectorCross(Vector4 a, Vector4 b);
void VectorInterp(Vector4 *z, const Vector4 *x1, const Vector4 *x2, float t);
void PointInterp(Point *z, const Point *x1, const Point *x2, float t);