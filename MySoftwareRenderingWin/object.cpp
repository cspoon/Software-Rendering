
#include "stdafx.h"
#include "object.h"


Cube::Cube() {
	mesh = new Mesh(24, 12);
	Vertex temp[8];
	temp[0] = Vertex(Point(50, -50, 50), Color(255, 51, 51));
	temp[1] = Vertex(Point(-50, -50, 50), Color(51, 255, 51));
	temp[2] = Vertex(Point(-50, 50, 50), Color(51, 255, 51));
	temp[3] = Vertex(Point(50, 50, 50), Color(255, 51, 255));
	temp[4] = Vertex(Point(50, -50, -50), Color(255, 255, 51));
	temp[5] = Vertex(Point(-50, -50, -50), Color(51, 255, 255));
	temp[6] = Vertex(Point(-50, 50, -50), Color(255, 77, 77));
	temp[7] = Vertex(Point(50, 50, -50), Color(51, 255, 77));
	AddPlan(temp[3], temp[2], temp[1], temp[0], 0);
	AddPlan(temp[7], temp[3], temp[0], temp[4], 2);
	AddPlan(temp[6], temp[7], temp[4], temp[5], 4);
	AddPlan(temp[2], temp[6], temp[5], temp[1], 6);
	AddPlan(temp[5], temp[4], temp[0], temp[1], 8);
	AddPlan(temp[2], temp[3], temp[7], temp[6], 10);
}

void Cube::AddPlan(Vertex a, Vertex b, Vertex c, Vertex d, int planIndex) {
	a.u = 0, a.v = 1, b.u = 1, b.v = 1;
	c.u = 1, c.v = 0, d.u = 0, d.v = 0;
	mesh->vertices[planIndex*2] = a;
	mesh->vertices[planIndex*2+1] = b;
	mesh->vertices[planIndex*2+2] = c;
	mesh->vertices[planIndex*2+3] = d;
	mesh->tris[planIndex].vertices[0] = a;
	mesh->tris[planIndex].vertices[1] = b;
	mesh->tris[planIndex].vertices[2] = c;
	for (int i = 0; i < 3; i++)
		mesh->tris[planIndex].transformedVertices[i] = mesh->tris[planIndex].vertices[i];
	mesh->tris[planIndex + 1].vertices[0] = c;
	mesh->tris[planIndex + 1].vertices[1] = d;
	mesh->tris[planIndex + 1].vertices[2] = a;
	for (int i = 0; i < 3; i++)
		mesh->tris[planIndex + 1].transformedVertices[i] = mesh->tris[planIndex + 1].vertices[i];
}

Point GetSpherePoint(float radius, float a, float b)
{
	float x = radius*sin(DEG_TO_RAD(a)) * sin(DEG_TO_RAD(b));
	float y = radius*cos(DEG_TO_RAD(a));
	float z = radius*sin(DEG_TO_RAD(a)) * cos(DEG_TO_RAD(b));
	float w = 1;
	return Point(x, y, z, w);
}

Vertex* GetPointArray(float radius, int slices)
{
	int i, j, w = 2 * slices, h = slices;
	float a = 0.0, b = 0.0;
	float hStep = 180.0 / (h - 1);
	float wStep = 360.0 / w;
	float uStep = 1.0 / (h - 1);
	float vStep = 1.0 / w;

	Vertex *arr = new Vertex[w*h];
	Point vec;
	Matrix4x4 matrix;
	for (a = 0.0, i = 0; i < h; i++, a += hStep)
		for (b = 0.0, j = 0; j < w; j++, b += wStep) {
			vec = GetSpherePoint(radius, a, b);
			arr[i*w + j].pos = vec;
			arr[i*w + j].u = 1 - vStep * j;
			arr[i*w + j].v = 1 - uStep * i;
			arr[i*w + j].color = ColorGray;
		}
	return arr;
}

Sphere::Sphere(float radious, int slices)
{
	int i = 0, j = 0, w = 2 * slices, h = slices;
	mesh = new Mesh(0, 2*w*(h-1));
	Vertex *points;
	points = GetPointArray(radious, slices);
	Vertex temp1, temp2;	//用于拼接圆形最后一圈纹理坐标
	int index = 0;
	for (; i < h - 1; i++) {
		for (j = 0; j < w - 1; j++) {
			mesh->tris[index].vertices[0] = points[(i + 1)*w + j];
			mesh->tris[index].vertices[1] = points[(i + 1)*w + j + 1];
			mesh->tris[index].vertices[2] = points[i*w + j];
			for (int i = 0; i < 3; i++)
				mesh->tris[index].transformedVertices[i] = mesh->tris[index].vertices[i];
			index++;
			mesh->tris[index].vertices[0] = points[(i + 1)*w + j + 1];
			mesh->tris[index].vertices[1] = points[i*w + j + 1];
			mesh->tris[index].vertices[2] = points[i*w + j];
			for (int i = 0; i < 3; i++)
				mesh->tris[index].transformedVertices[i] = mesh->tris[index].vertices[i];
			index++;
		}
		temp1 = points[(i + 1)*w];
		temp1.u = 0;
		temp2 = points[i*w];
		temp2.u = 0;
		mesh->tris[index].vertices[0] = points[(i + 1)*w + j];
		mesh->tris[index].vertices[1] = temp1;
		mesh->tris[index].vertices[2] = points[i*w + j];
		for (int i = 0; i < 3; i++)
			mesh->tris[index].transformedVertices[i] = mesh->tris[index].vertices[i];
		index++;
		mesh->tris[index].vertices[0] = temp1;
		mesh->tris[index].vertices[1] = temp2;
		mesh->tris[index].vertices[2] = points[i*w + j];
		for (int i = 0; i < 3; i++)
			mesh->tris[index].transformedVertices[i] = mesh->tris[index].vertices[i];
		index++;
	}
	delete(points);
}