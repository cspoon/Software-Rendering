
#include "stdafx.h"
#include "render.h"
#include "object.h"
#include "main.h"
#include "Camera.h"
#include "geometry.h"
#include "Light.h"

Light lights[2];
int fpsCount;
float objRotX = 0;
float objRotY = 0;
bool needHelpMenu;
int camSpeed = 1;
int renderTypeIndex = 1;
RenderType renderTypes[3];
int Width = 640;
int Height = 480;
DWORD colorBuffer[640 * 480];
float zBuffer[640 * 480];
RenderType renderType;
bool enableLight;
bool enableCullBackFace;
Point objPos;
Point  camPos = Point(0, 0, -300);
Vector4 camDirEuler = Vector4(0, 0, 0, 0);
Camera camera;
Texture texture;
ModelType modelType = CubeModel;
Cube cube;
Sphere sphere = Sphere(50.0, 18);


int DrawPixel(int x, int y, DWORD color) {
	if (x < 0 || y < 0 || x >= Width || y >= Height) {
		return -1;
	}
	colorBuffer[y * Width + x] = color;
	return 0;
}

void InsertTriangleToRenderList(list<Triangle> &renderList, Triangle &tri) {
	renderList.push_back(tri);
}

void InsertModel(ModelType modelType) {
	if(modelType == CubeModel)
		for (int i = 0; i < cube.mesh->triCount; i++)
			InsertTriangleToRenderList(renderList, cube.mesh->tris[i]);
	else
		for (int i = 0; i < sphere.mesh->triCount; i++)
			InsertTriangleToRenderList(renderList, sphere.mesh->tris[i]);
}

