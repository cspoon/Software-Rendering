#pragma once

#include "stdafx.h"
#include "Color.h"
#include "geometry.h"
#include  "mesh.h"

enum LightType
{
	Ambient,
	Infinite,
};

class Light
{
public:
	bool state;
	LightType type;
	Color ambient;
	Color diffuse;
	Vector4 dir;
	
	Light(){state = true;}
	Light(LightType t, Color amb, Color diff, Vector4 d) 
		: type(t), ambient(amb), diffuse(diff), dir(d){
		state = true;
	}

	void Init(LightType t, Color amb, Color diff, Vector4 d){
		type = t; ambient = amb; diffuse = diff; dir = d;
	}

	void RenderLight(list<Triangle> &renderList) {
		float rSum = 0.0;
		float gSum = 0.0;
		float bSum = 0.0;
		float rBase = 0.0;
		float gBase = 0.0;
		float bBase = 0.0;
		float vertexColor[3][3];
		Point sphereCenter = { 0.0, 0.0, 0.0, 1 };
		Vector4 spVertex[3];
		Vector4 spVerNor[3];
		float dp = 0.0; float normalLength = 0.0; float par = 0.0;
		for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
			Triangle *triangle = &*i;
			if (!triangle->isValid())
				continue;
			rSum = 0.0; gSum = 0.0; bSum = 0.0;
			rBase = triangle->baseColor.r; gBase = triangle->baseColor.g; bBase = triangle->baseColor.b;
			if (modelType == CubeModel) {
				if (!state) {
					rSum += 0; gSum += 0; bSum += 0;
					continue;
				}
				if (type == Ambient) {
					rSum += ((ambient.r * rBase) / 256);
					gSum += ((ambient.g * gBase) / 256);
					bSum += ((ambient.b * bBase) / 256);
				}
				else if (type == Infinite) {
					Vector4 u = Vector4(triangle->transformedVertices[0].pos, triangle->transformedVertices[1].pos);
					Vector4 v = Vector4(triangle->transformedVertices[0].pos, triangle->transformedVertices[2].pos);
					Vector4 normal = VectorCross(u, v);
					dp = normal * dir;
					if (dp > 0) {
						normalLength = normal.Length();
						par = 128 * dp / normalLength * dp / normalLength;
						rSum += (diffuse.r * rBase * par) / (255 * 128);
						gSum += (diffuse.g * gBase * par) / (255 * 128);
						bSum += (diffuse.b * bBase * par) / (255 * 128);
					}
				}
				triangle->lightedColor.r = rSum;
				triangle->lightedColor.g = gSum;
				triangle->lightedColor.b = bSum;
			}
			else if (modelType == SphereModel) {
				int i = 0; int j = 0;
				for (i = 0; i < 3; i++)
					for (j = 0; j < 3; j++) {
						vertexColor[i][j] = 0.0;
					}
				if (!state) {
					for (i = 0; i < 3; i++)
						for (j = 0; j < 3; j++) {
							vertexColor[i][j] += 0.0;
						}
					continue;
				}
				if (type == Ambient) {
					rSum += ((ambient.r * rBase) / 256);
					gSum += ((ambient.g * gBase) / 256);
					bSum += ((ambient.b * bBase) / 256);
					for (i = 0; i < 3; i++) {
						vertexColor[i][0] += rSum;
						vertexColor[i][1] += gSum;
						vertexColor[i][2] += bSum;
					}
				}
				else if (type == Infinite) {
					for (i = 0; i < 3; i++) {
						spVertex[i] = Vector4(sphereCenter, triangle->transformedVertices[i].pos);
						dp = spVertex[i] * dir;
						if (dp > 0) {
							normalLength = spVertex[i].Length();
							par = 128 * dp / normalLength * dp / normalLength;
							vertexColor[i][0] += (diffuse.r * rBase * par) / (255 * 128);
							vertexColor[i][1] += (diffuse.g * gBase * par) / (255 * 128);
							vertexColor[i][2] += (diffuse.b * bBase * par) / (255 * 128);
						}
					}
				}
				for (i = 0; i < 3; i++) {
					triangle->transformedVertices[i].lightedColor.r = vertexColor[i][0];
					triangle->transformedVertices[i].lightedColor.g = vertexColor[i][1];
					triangle->transformedVertices[i].lightedColor.b = vertexColor[i][2];
				}
			}
			triangle->lightedColor.r = rSum;
			triangle->lightedColor.g = gSum;
			triangle->lightedColor.b = bSum;
		}
	}
};

//void RenderLight(list<Triangle> &renderList) {
//	float rSum = 0.0;
//	float gSum = 0.0;
//	float bSum = 0.0;
//	float rBase = 0.0;
//	float gBase = 0.0;
//	float bBase = 0.0;
//	float vertexColor[3][3];
//	Point sphereCenter = { 0.0, 0.0, 0.0, 1 };
//	Vector4 spVertex[3];
//	Vector4 spVerNor[3];
//	float dp = 0.0; float normalLength = 0.0; float par = 0.0;
//	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
//		Triangle *triangle = &*i;
//		if (!triangle->isValid())
//			continue;
//		rSum = 0.0; gSum = 0.0; bSum = 0.0;
//		rBase = triangle->baseColor.r; gBase = triangle->baseColor.g; bBase = triangle->baseColor.b;
//		if (modelType == CubeModel) {
//			for (int j = 0; j < 2; j++) {
//				if (!lights[j].state) {
//					rSum += 0; gSum += 0; bSum += 0;
//					continue;
//				}
//				if (lights[j].type == Ambient) {
//					rSum += ((lights[j].ambient.r * rBase) / 256);
//					gSum += ((lights[j].ambient.g * gBase) / 256);
//					bSum += ((lights[j].ambient.b * bBase) / 256);
//				}
//				else if (lights[j].type == Infinite) {
//					Vector4 u = Vector4(triangle->transformedVertices[0].pos, triangle->transformedVertices[1].pos);
//					Vector4 v = Vector4(triangle->transformedVertices[0].pos, triangle->transformedVertices[2].pos);
//					Vector4 normal = VectorCross(u, v);
//					dp = normal * lights[j].dir;
//					if (dp > 0) {
//						normalLength = normal.Length();
//						par = 128 * dp / normalLength * dp / normalLength;
//						rSum += (lights[j].diffuse.r * rBase * par) / (255 * 128);
//						gSum += (lights[j].diffuse.g * gBase * par) / (255 * 128);
//						bSum += (lights[j].diffuse.b * bBase * par) / (255 * 128);
//					}
//				}
//			}
//			triangle->lightedColor.r = rSum;
//			triangle->lightedColor.g = gSum;
//			triangle->lightedColor.b = bSum;
//		}
//		else if (modelType == SphereModel) {
//			int i = 0; int j = 0;
//			for (i = 0; i < 3; i++)
//				for (j = 0; j < 3; j++) {
//					vertexColor[i][j] = 0.0;
//				}
//			for (int l = 0; l < 2; l++) {
//				if (!lights[l].state) {
//					for (i = 0; i < 3; i++)
//						for (j = 0; j < 3; j++) {
//							vertexColor[i][j] += 0.0;
//						}
//					continue;
//				}
//				if (lights[l].type == Ambient) {
//					rSum += ((lights[l].ambient.r * rBase) / 256);
//					gSum += ((lights[l].ambient.g * gBase) / 256);
//					bSum += ((lights[l].ambient.b * bBase) / 256);
//					for (i = 0; i < 3; i++) {
//						vertexColor[i][0] += rSum;
//						vertexColor[i][1] += gSum;
//						vertexColor[i][2] += bSum;
//					}
//				}
//				else if (lights[l].type == Infinite) {
//					for (i = 0; i < 3; i++) {
//						spVertex[i] = Vector4(sphereCenter, triangle->transformedVertices[i].pos);
//						dp = spVertex[i] * lights[l].dir;
//						if (dp > 0) {
//							normalLength = spVertex[i].Length();
//							par = 128 * dp / normalLength * dp / normalLength;
//							vertexColor[i][0] += (lights[l].diffuse.r * rBase * par) / (255 * 128);
//							vertexColor[i][1] += (lights[l].diffuse.g * gBase * par) / (255 * 128);
//							vertexColor[i][2] += (lights[l].diffuse.b * bBase * par) / (255 * 128);
//						}
//					}
//				}
//				for (i = 0; i < 3; i++) {
//					triangle->transformedVertices[i].lightedColor.r = vertexColor[i][0];
//					triangle->transformedVertices[i].lightedColor.g = vertexColor[i][1];
//					triangle->transformedVertices[i].lightedColor.b = vertexColor[i][2];
//				}
//			}
//		}
//		triangle->lightedColor.r = rSum;
//		triangle->lightedColor.g = gSum;
//		triangle->lightedColor.b = bSum;
//	}
//}