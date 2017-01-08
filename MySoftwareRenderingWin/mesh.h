#pragma once

#include "render.h"
#include "texture.h"
#include "geometry.h"
#include "color.h"

class Mesh;

enum TriangleState
{
	TriangleActive,
	TriangleClipped,
	TriangleBackFace,
};

class Vertex 
{
public:
	Point pos;
	Vector4 normal;
	float u, v;
	Color color;
	Color lightedColor;
	Vertex() {}
	Vertex(Point pp, Color cc) : pos(pp), color(cc) {}
	Vertex(Point pp, float uu, float vv, Color cc) : pos(pp), u(uu), v(vv), color(cc) {}
};

class ScanlineTriangle {
public:
	class Edge {
	public:
		Vertex current, start, end;
	};
	Edge left, right;
	float topY, bottomY;
	Color lightedColor;
};

class Scanline {
public:
	Vertex current, step; 
	int x, y, width; 
	Color lightedColor;
};

class Triangle 
{
public:
	Triangle() { state = TriangleActive; }
	Triangle(Mesh *m) {
		owner = m;
		state = TriangleActive;
	}
	Color baseColor = ColorWhite;
	Color lightedColor;
	Mesh *owner;
	Vertex vertices[3];
	Vertex transformedVertices[3];
	TriangleState state;
	bool wireframeState[3] = {true, true, true};
	bool isValid() { return state == TriangleActive; }
	void EnableWireFrame(int index, bool enble) { wireframeState[index] = enble; }
	void Draw(RenderType rt);
};

class Mesh 
{
public:
	Vertex *vertices;
	int *vertexIndex;
	Texture *texture;
	int verticesCount;
	int triCount;
	Triangle *tris;

	Mesh() {}
	Mesh(const Mesh& m) {
		verticesCount = m.verticesCount;
		triCount = m.triCount;
		vertices = new Vertex[verticesCount];
		vertexIndex = new int[3 * triCount];
		tris = new Triangle[triCount];
		memcpy(vertices, m.vertices, sizeof(m.vertices));
		memcpy(texture, m.texture, sizeof(m.texture));
		memcpy(tris, m.tris, sizeof(m.tris));
	}
	//Mesh& operator= (const Mesh& m) {}

	Mesh(int verticesCount, int triCount) {
		this->verticesCount = verticesCount;
		this->triCount = triCount;
		vertices = new Vertex[verticesCount];
		vertexIndex = new int[3 * triCount];
		tris = new Triangle[triCount];
	}

	void AddVertex(int, Vertex);
	~Mesh() {
		if(vertices != NULL)
			delete[] vertices;
		if(vertexIndex != NULL)
			delete[] vertexIndex;
		if (tris != NULL)
			delete[] tris;
	}
};

int DrawLine(int x0, int y0, int x1, int y1, DWORD color);
void DrawScanline(Scanline *scanline);
void RenderTriangle(ScanlineTriangle *tri);
void TriangleEdgeInterp(ScanlineTriangle *tri, float y);
void VertexAdd(Vertex *y, const Vertex *x);
void InitScanLine(const ScanlineTriangle *tri, Scanline *scanline, int y);