#pragma once
#include "transform.h"
#include "mesh.h"

class Object 
{
public:
	Transform transform;
	Mesh *mesh;
	Object() {};
	~Object() {
		if (mesh)
			delete mesh;
	};
};

class Cube : public Object 
{
public:
	Cube();
	void AddPlan(Vertex a, Vertex b, Vertex c, Vertex d, int planIndex);
};

class Sphere : public Object
{
public:
	Sphere(float radious, int slices);
};