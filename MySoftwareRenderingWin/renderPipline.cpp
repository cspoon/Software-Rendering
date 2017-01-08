
#include "renderPipline.h"
#include "Camera.h"
#include "geometry.h"
#include "render.h"

enum ClipState
{
	ClipLessZ,
	ClipGreaterZ,
	ClipInRange
};

void TransformRenderList(list<Triangle> &renderList, Matrix4x4 &m) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			for (int j = 0; j < 3; j++) {
				i->vertices[j].pos *= m;
			}
		}
	}
}

void ModelToWorldRenderList(list<Triangle> &renderList, Point &worldPos) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			for (int j = 0; j < 3; j++) {
				i->transformedVertices[j].pos = i->vertices[j].pos + worldPos;
			}
		}
	}
}

void BuildCameraMatrixEuler(Camera &cam) {
	Matrix4x4 tanslationInv = Matrix4x4(1, 0, 0, 0,
										0, 1, 0, 0,
										0, 0, 1, 0,
							-cam.pos.x, -cam.pos.y, -cam.pos.z, 1);
	
	Matrix4x4 yInv = GetRotateMatrixAlongY(-cam.dir.y);
	Matrix4x4 xInv = GetRotateMatrixAlongX(-cam.dir.x);
	Matrix4x4 zInv = GetRotateMatrixAlongZ(-cam.dir.z);
	Matrix4x4 rot = yInv * xInv * zInv;
	cam.worldToCamera = tanslationInv * yInv * xInv * zInv;	//take care of oder
}

void CullBackfacesRenderList(list<Triangle> &renderList, Camera &cam) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			Vector4 u = Vector4(i->transformedVertices[0].pos, i->transformedVertices[1].pos);
			Vector4 v = Vector4(i->transformedVertices[0].pos, i->transformedVertices[2].pos);
			Vector4 n = VectorCross(u, v);
			Vector4 view = Vector4(i->transformedVertices[0].pos, cam.pos);
			if (n * view < 0.f)
				i->state = TriangleBackFace;
		}
	}
}

void WorldToCameraRenderList(list<Triangle> &renderList, Camera &cam) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			for (int j = 0; j < 3; j++) {
				i->transformedVertices[j].pos *= cam.worldToCamera;
			}
		}
	}
}

void CameraToPerspectiveRenderList(list<Triangle> &renderList, Camera &cam) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			for (int j = 0; j < 3; j++) {
				float z = i->transformedVertices[j].pos.z;
				i->transformedVertices[j].pos.x = cam.viewDist * i->transformedVertices[j].pos.x / z;
				i->transformedVertices[j].pos.y = cam.viewDist * i->transformedVertices[j].pos.y * cam.aspectRation / z;
			}
		}
	}
}

void PerspectiveToScreenRenderList(list<Triangle> &renderList, Camera &cam) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			float alpha = (0.5 * cam.viewPortWidth - 0.5);
			float beta = (0.5 * cam.viewPortHeight - 0.5);
			for (int j = 0; j < 3; j++) {
				i->transformedVertices[j].pos.x = alpha + alpha * i->transformedVertices[j].pos.x;
				i->transformedVertices[j].pos.y = beta + beta * i->transformedVertices[j].pos.y;
			}
		}
	}
}

void ResetColorUV(Triangle &tri, int v0, int v1, float t, float &ui, float &vi, float &ri, float &gi, float &bi) {
	ui = tri.transformedVertices[v0].u + (tri.transformedVertices[v1].u - tri.transformedVertices[v0].u)*t;
	vi = tri.transformedVertices[v0].v + (tri.transformedVertices[v1].v - tri.transformedVertices[v0].v)*t;
	tri.transformedVertices[v1].u = ui;
	tri.transformedVertices[v1].v = vi;

	ri = tri.transformedVertices[v0].color.r + (tri.transformedVertices[v1].color.r - tri.transformedVertices[v0].color.r)*t;
	gi = tri.transformedVertices[v0].color.g + (tri.transformedVertices[v1].color.g - tri.transformedVertices[v0].color.g)*t;
	bi = tri.transformedVertices[v0].color.b + (tri.transformedVertices[v1].color.b - tri.transformedVertices[v0].color.b)*t;
	tri.transformedVertices[v1].color.r = ri;
	tri.transformedVertices[v1].color.b = bi;
	tri.transformedVertices[v1].color.g = gi;
}


void ClipRenderList(list<Triangle> &renderList, Camera &cam) {
	int numVertexIn = 0;
	ClipState clipStates[3];
	int v0, v1, v2 = 0;
	float t1 = 0.0; float t2 = 0.0;
	float xi = 0.0; float yi = 0.0;
	float ui, vi, ri, gi, bi = 0.0;
	float x01i = 0.0; float y01i = 0.0; float x02i = 0.0; float y02i = 0.0;
	float u01i = 0.0; float  v01i = 0.0; float  r01i = 0.0; float  g01i = 0.0; float  b01i = 0.0;
	float u02i = 0.0; float  v02i = 0.0; float  r02i = 0.0; float  g02i = 0.0; float  b02i = 0.0;
	int tempV = 0;

	
	for (list<Triangle>::iterator currTri = renderList.begin(); currTri != renderList.end(); currTri++) {
		numVertexIn = 0;
		memset(clipStates, ClipInRange, 3 * sizeof(int));
		v0 = 0; v1 = 0; v2 = 0;
		t1 = 0.0; t2 = 0.0;
		xi = yi = 0.0;
		ui = vi = ri = gi = bi = 0.0;
		x01i = y01i = x02i = y02i = 0.0;
		u01i = v01i = r01i = g01i = b01i = 0.0;
		u02i = v02i = r02i = g02i = b02i = 0.0;
		tempV = 0;
		for (int j = 0; j < 3; j++) {
			if (currTri->transformedVertices[j].pos.z < cam.nearClip) {
				clipStates[j] = ClipLessZ;
			}else {
				clipStates[j] = ClipInRange;
				numVertexIn++;
			}
		}
		if (numVertexIn == 0) {		//3 vertices'z less than camera's nearClipZ
			currTri->state = TriangleClipped;
			continue;
		}
		//				0
		//
		//	    -----02---01------- nearClipZ
		//			2		1
		if (numVertexIn == 1) {		//2 vertices'z less than camera's nearClipZ
			if (clipStates[0] == ClipInRange) {
				v0 = 0; v1 = 1; v2 = 2;
			}
			else if (clipStates[1] == ClipInRange) {
				v0 = 1; v1 = 2; v2 = 0;
			}
			else {
				v0 = 2; v1 = 0; v2 = 1;
			}
			Vector4 vec = Vector4(currTri->transformedVertices[v0].pos, currTri->transformedVertices[v1].pos);
			t1 = ((cam.nearClip - currTri->transformedVertices[v0].pos.z) / vec.z);
			xi = currTri->transformedVertices[v0].pos.x + vec.x * t1;
			yi = currTri->transformedVertices[v0].pos.y + vec.y * t1;
			currTri->transformedVertices[v1].pos.x = xi;
			currTri->transformedVertices[v1].pos.y = yi;
			currTri->transformedVertices[v1].pos.z = cam.nearClip;

			vec = Vector4(currTri->transformedVertices[v0].pos, currTri->transformedVertices[v2].pos);
			t2 = (cam.nearClip - currTri->transformedVertices[v0].pos.z) / vec.z;
			xi = currTri->transformedVertices[v0].pos.x + vec.x * t2;
			yi = currTri->transformedVertices[v0].pos.y + vec.y * t2;
			currTri->transformedVertices[v2].pos.x = xi;
			currTri->transformedVertices[v2].pos.y = yi;
			currTri->transformedVertices[v2].pos.z = cam.nearClip;

			ResetColorUV(*currTri, v0, v1, t1, ui, vi, ri, gi, bi);
			ResetColorUV(*currTri, v0, v2, t2, ui, vi, ri, gi, bi);

			currTri->EnableWireFrame(v0, true);
			currTri->EnableWireFrame(v1, false);
			currTri->EnableWireFrame(v2, true);
		}
		//			1	    2
		//
		//	    ------01--02-------- nearClipZ
		//			 	0	 
		else if (numVertexIn == 2) {	//1 vertex'z less than camera's nearClipZ
			Triangle tempTri(*currTri);
			if (clipStates[0] == ClipLessZ) {
				v0 = 0; v1 = 1; v2 = 2;
			}
			else if (clipStates[1] == ClipLessZ) {
				v0 = 1; v1 = 2; v2 = 0;
			}
			else {
				v0 = 2; v1 = 0; v2 = 1;
			}
			Vector4 vec = Vector4(currTri->transformedVertices[v0].pos, currTri->transformedVertices[v1].pos);
			t1 = (cam.nearClip - currTri->transformedVertices[v0].pos.z) / vec.z;
			x01i = currTri->transformedVertices[v0].pos.x + vec.x * t1;
			y01i = currTri->transformedVertices[v0].pos.y + vec.y * t1;
			
			vec = Vector4(currTri->transformedVertices[v0].pos, currTri->transformedVertices[v2].pos);
			t2 = (cam.nearClip - currTri->transformedVertices[v0].pos.z) / vec.z;
			x02i = currTri->transformedVertices[v0].pos.x + vec.x * t2;
			y02i = currTri->transformedVertices[v0].pos.y + vec.y * t2;

			//updates v0 by value of 01
			currTri->transformedVertices[v0].pos.x = x01i;
			currTri->transformedVertices[v0].pos.y = y01i;
			currTri->transformedVertices[v0].pos.z = cam.nearClip;
			currTri->EnableWireFrame(v0, true);
			currTri->EnableWireFrame(v1, true);
			currTri->EnableWireFrame(v2, false);

			//dont update tempTri' v2, set new values of v01 and v02
			tempTri.transformedVertices[v1].pos.x = x01i;
			tempTri.transformedVertices[v1].pos.y = y01i;
			tempTri.transformedVertices[v1].pos.z = cam.nearClip;
			tempTri.transformedVertices[v0].pos.x = x02i;
			tempTri.transformedVertices[v0].pos.y = y02i;
			tempTri.transformedVertices[v0].pos.z = cam.nearClip;
			currTri->EnableWireFrame(v0, false);
			currTri->EnableWireFrame(v1, true);
			currTri->EnableWireFrame(v2, true);

			u01i = currTri->transformedVertices[v0].u + (currTri->transformedVertices[v1].u - currTri->transformedVertices[v0].u)*t1;
			v01i = currTri->transformedVertices[v0].v + (currTri->transformedVertices[v1].v - currTri->transformedVertices[v0].v)*t1;

			u02i = currTri->transformedVertices[v0].u + (currTri->transformedVertices[v2].u - currTri->transformedVertices[v0].u)*t2;
			v02i = currTri->transformedVertices[v0].v + (currTri->transformedVertices[v2].v - currTri->transformedVertices[v0].v)*t2;

			currTri->transformedVertices[v0].u = u01i;
			currTri->transformedVertices[v0].v = v01i;
			tempTri.transformedVertices[v0].u = u02i;
			tempTri.transformedVertices[v0].v = v02i;
			tempTri.transformedVertices[v1].u = u01i;
			tempTri.transformedVertices[v1].v = v01i;

			r01i = currTri->transformedVertices[v0].color.r + (currTri->transformedVertices[v1].color.r - currTri->transformedVertices[v0].color.r)*t1;
			g01i = currTri->transformedVertices[v0].color.g + (currTri->transformedVertices[v1].color.g - currTri->transformedVertices[v0].color.g)*t1;
			b01i = currTri->transformedVertices[v0].color.b + (currTri->transformedVertices[v1].color.b - currTri->transformedVertices[v0].color.b)*t1;

			r02i = currTri->transformedVertices[v0].color.r + (currTri->transformedVertices[v2].color.r - currTri->transformedVertices[v0].color.r)*t2;
			g02i = currTri->transformedVertices[v0].color.g + (currTri->transformedVertices[v2].color.g - currTri->transformedVertices[v0].color.g)*t2;
			b02i = currTri->transformedVertices[v0].color.b + (currTri->transformedVertices[v2].color.b - currTri->transformedVertices[v0].color.b)*t2;

			currTri->transformedVertices[v0].color.r = r01i;
			currTri->transformedVertices[v0].color.g = g01i;
			currTri->transformedVertices[v0].color.b = b01i;
			tempTri.transformedVertices[v0].color.r = r02i;
			tempTri.transformedVertices[v0].color.g = g02i;
			tempTri.transformedVertices[v0].color.b = b02i;
			tempTri.transformedVertices[v1].color.r = r01i;
			tempTri.transformedVertices[v1].color.g = g01i;
			tempTri.transformedVertices[v1].color.b = b01i;

			InsertTriangleToRenderList(renderList, tempTri);
		}
	}
}

void DrawRenderList(list<Triangle> &renderList, RenderType rt) {
	for (list<Triangle>::iterator i = renderList.begin(); i != renderList.end(); i++) {
		if (i->isValid()) {
			i->Draw(rt);
		}
	}
}
