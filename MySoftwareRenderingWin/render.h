#pragma once

#include "stdafx.h"
#include "texture.h"


class Triangle;
class Cube;
class Camera;
class Point;
class Vector4;
class Light;
class Sphere;

enum ModelType {
	CubeModel,
	SphereModel,
};

enum RenderType {
	VertexColor,
	WireFrame,
	Textured,
};

extern Light lights[2];
extern int fpsCount;
extern float objRotX;
extern float objRotY;
extern bool needHelpMenu;
extern int camSpeed;
extern int renderTypeIndex;
extern RenderType renderTypes[3];
extern int Width;
extern int Height;
extern Point objPos;
extern Point camPos;
extern Vector4 camDirEuler;
extern Camera camera;
extern DWORD colorBuffer[640 * 480];
extern float zBuffer[640 * 480];
extern RenderType renderType;
extern bool enableLight;
extern bool enableCullBackFace;
extern Texture texture;
extern ModelType modelType;
extern Cube cube;
extern Sphere sphere;

inline void SetZBuffer(int x, int y, float zValue) {
	zBuffer[y * Width + x] = zValue;
}

inline float GetZValue(int x, int y) {
	return zBuffer[y * Width + x];
}

int DrawPixel(int x, int y, DWORD color);
void InsertModel(ModelType modelType);
void InsertTriangleToRenderList(list<Triangle> &renderList, Triangle &tri);