#pragma once

#include "geometry.h"
#include "Utils.h"

enum CameraType
{
	Euler,
	UVN,
};

class Camera {
public:
	CameraType type;
	Point pos;
	Vector4 dir;
	Point target;
	float nearClip, farClip, fov, viewPlanWidth, viewPlanHeight, viewPortWidth, viewPortHeight, 
		aspectRation, viewDist;
	Matrix4x4 worldToCamera;
	Camera() {};
	Camera(CameraType t, Point p, Vector4 d, Point tar, float nearz, float farz,
		float fov, float width, float height) : type(t), pos(p), dir(d), target(tar), nearClip(nearz),
		farClip(farz), fov(fov), viewPortWidth(width), viewPortHeight(height){
		aspectRation = width / height;
		viewPlanWidth = 2.0f;
		viewPlanHeight = 2.0 / aspectRation;
		float tanFovDiv2 = tan(DEG_TO_RAD(fov / 2));
		viewDist = (0.5)*(viewPlanWidth) / tanFovDiv2;
	}
	void IniCameraEuler(CameraType t, Point p, Vector4 d, float nearz, float farz,
		float fov, float width, float height) {
		type = t; pos = p; dir = d; nearClip = nearz;
		farClip = farz; this->fov = fov; viewPortWidth = width; viewPortHeight = height;
		aspectRation = width / height;
		viewPlanWidth = 2.0f;
		viewPlanHeight = 2.0 / aspectRation;
		float tanFovDiv2 = tan(DEG_TO_RAD(fov / 2));
		viewDist = (0.5)*(viewPlanWidth) / tanFovDiv2;
	}

	void Move(const Vector4 &localDir, int speed, double deltaTime){
		Matrix4x4 rotM = BuildXyzRotationMatrix(dir.x, dir.y, dir.z);
		Vector4 camDir = localDir * rotM;
		Vector4 camStep = camDir * (speed * 100 * deltaTime);
		pos += camStep;
	}

};