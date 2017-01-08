
#include "stdafx.h"
#include "mesh.h"
#include "Utils.h"
#include "geometry.h"

void Mesh::AddVertex(int index, Vertex v) {
	if (index < this->verticesCount)
		vertices[index] = v;
}

void VertexReverseZ(Vertex *v) {
	float rz = 1.0f / v->pos.z;
	v->pos.z = rz;
	v->u = v->u*rz;
	v->v = v->v*rz;
	v->color.r = v->color.r*rz;
	v->color.g = v->color.g*rz;
	v->color.b = v->color.b*rz;
	v->lightedColor.r = v->lightedColor.r*rz;
	v->lightedColor.g = v->lightedColor.g*rz;
	v->lightedColor.b = v->lightedColor.b*rz;
}

//y	  3
//	1	2	
void IniTopTriangle(ScanlineTriangle *tri, const Vertex *v1, const Vertex *v2, const Vertex *v3) {
	const Vertex * temp = NULL;
	if (v1->pos.x > v2->pos.x){
		temp = v1; v1 = v2; v2 = temp;
	}
	tri->topY = v3->pos.y;
	tri->bottomY = v1->pos.y;
	tri->left.start = *v3;
	tri->left.end = *v1;
	tri->right.start = *v3;
	tri->right.end = *v2;
}

//y		2 3
//		 1	
void IniBottomTriangle(ScanlineTriangle *tri, const Vertex *v1, const Vertex *v2, const Vertex *v3) {
	const Vertex * temp = NULL;
	if (v2->pos.x > v3->pos.x){
		temp = v2; v2 = v3; v3 = temp;
	}
	tri->topY = v2->pos.y;
	tri->bottomY = v1->pos.y;
	tri->left.start = *v2;
	tri->left.end = *v1;
	tri->right.start = *v3;
	tri->right.end = *v1;
}

//    3          3
//   2     or      2
// (t)  1      1   (t)
void IniNormalTriangle(ScanlineTriangle *tri1, ScanlineTriangle *tri2, const Vertex *v1,
	const Vertex * v2, const Vertex * v3) {
	tri1->topY = v3->pos.y; tri1->bottomY = v2->pos.y;
	tri2->topY = v2->pos.y; tri2->bottomY = v1->pos.y;

	float k = (v1->pos.y - v3->pos.y) / (v2->pos.y - v3->pos.y);
	float newTx = v3->pos.x + (v2->pos.x - v3->pos.x) * k;
	if (newTx < v1->pos.x) {	//left
		tri1->left.start = *v3;
		tri1->left.end = *v2;
		tri1->right.start = *v3;
		tri1->right.end = *v1;
		tri2->left.start = *v2;
		tri2->left.end = *v1;
		tri2->right.start = *v3;
		tri2->right.end = *v1;
	}
	else {
		tri1->left.start = *v3;
		tri1->left.end = *v1;
		tri1->right.start = *v3;
		tri1->right.end = *v2;
		tri2->left.start = *v3;
		tri2->left.end = *v1;
		tri2->right.start = *v2;
		tri2->right.end = *v1;
	}
}

void Triangle::Draw(RenderType rt) {
	Vertex *v1 = &transformedVertices[0], *v2 = &transformedVertices[1], *v3 = &transformedVertices[2], *temp;
	ScanlineTriangle t1, t2;

	//order v1 < v2 < v3
	if (v1->pos.y > v2->pos.y) temp = v1, v1 = v2, v2 = temp;
	if (v1->pos.y > v3->pos.y) temp = v1, v1 = v3, v3 = temp;
	if (v2->pos.y > v3->pos.y) temp = v2, v2 = v3, v3 = temp;
	if (FloatEqual(v1->pos.y, v2->pos.y) && FloatEqual(v1->pos.y, v3->pos.y)) return;	// come to one line

	VertexReverseZ(v1);
	VertexReverseZ(v2);
	VertexReverseZ(v3);
	
	if (rt == Textured || rt == VertexColor) {
		if (FloatEqual(v1->pos.y, v2->pos.y)) {
			IniTopTriangle(&t1, v1, v2, v3);
			t1.lightedColor = lightedColor;
			RenderTriangle(&t1);
		}
		else if (FloatEqual(v2->pos.y, v3->pos.y)) {
			IniBottomTriangle(&t1, v1, v2, v3);
			t1.lightedColor = lightedColor;
			RenderTriangle(&t1);
		}
		else {
			IniNormalTriangle(&t1, &t2, v1, v2, v3);
			t1.lightedColor = lightedColor;
			t2.lightedColor = lightedColor;
			RenderTriangle(&t1);
			RenderTriangle(&t2);
		}
	}
	else {
		if (wireframeState[0]) {
			DrawLine(transformedVertices[0].pos.x, transformedVertices[0].pos.y,
				transformedVertices[1].pos.x, transformedVertices[1].pos.y, ColorBlackDW);
		}
		if (wireframeState[1]) {
			DrawLine(transformedVertices[1].pos.x, transformedVertices[1].pos.y,
				transformedVertices[2].pos.x, transformedVertices[2].pos.y, ColorBlackDW);
		}if (wireframeState[2]) {
			DrawLine(transformedVertices[2].pos.x, transformedVertices[2].pos.y,
				transformedVertices[0].pos.x, transformedVertices[0].pos.y, ColorBlackDW);
		}
	}
}

void RenderTriangle(ScanlineTriangle *tri) {
	Scanline scanline;
	int j, top, bottom;
	top = (int)(tri->topY + 0.5f);
	bottom = (int)(tri->bottomY + 0.5f);
	for (j = bottom; j < top; j++) {
		if (j >= 0 && j < Height) {
			TriangleEdgeInterp(tri, (float)j + 0.5);
			InitScanLine(tri, &scanline, j);
			DrawScanline(&scanline);
		}
		if (j >= Height) break;
	}
}


void ClampRGBA(float &v) {
	if (v < 0) v = 0;
	if (v > 255) v = 255;
}

void VertexInterp(Vertex *res, const Vertex *v1, const Vertex *v2, float f) {
	PointInterp(&res->pos, &v1->pos, &v2->pos, f);
	res->color.r = Interp(v1->color.r, v2->color.r, f);
	res->color.g = Interp(v1->color.g, v2->color.g, f);
	res->color.b = Interp(v1->color.b, v2->color.b, f);
	res->lightedColor.r = Interp(v1->lightedColor.r, v2->lightedColor.r, f);
	res->lightedColor.g = Interp(v1->lightedColor.g, v2->lightedColor.g, f);
	res->lightedColor.b = Interp(v1->lightedColor.b, v2->lightedColor.b, f);
	res->u = Interp(v1->u, v2->u, f);
	res->v = Interp(v1->v, v2->v, f);
}

void VertexDivision(Vertex *y, const Vertex *x1, const Vertex *x2, float w) {
	float inv = 1.0f / w;
	y->pos.x = (x2->pos.x - x1->pos.x) * inv;
	y->pos.y = (x2->pos.y - x1->pos.y) * inv;
	y->pos.z = (x2->pos.z - x1->pos.z) * inv;
	y->pos.w = (x2->pos.w - x1->pos.w) * inv;
	y->u = (x2->u - x1->u) * inv;
	y->v = (x2->v - x1->v) * inv;
	y->color.r = (x2->color.r - x1->color.r) * inv;
	y->color.g = (x2->color.g - x1->color.g) * inv;
	y->color.b = (x2->color.b - x1->color.b) * inv;
	y->lightedColor.r = (x2->lightedColor.r - x1->lightedColor.r) * inv;
	y->lightedColor.g = (x2->lightedColor.g - x1->lightedColor.g) * inv;
	y->lightedColor.b = (x2->lightedColor.b - x1->lightedColor.b) * inv;
	y->pos.z = (x2->pos.z - x1->pos.z) * inv;
}

void TriangleEdgeInterp(ScanlineTriangle *tri, float y) {
	float s1 = tri->left.end.pos.y - tri->left.start.pos.y;
	float s2 = tri->right.end.pos.y - tri->right.start.pos.y;
	float t1 = (y - tri->left.start.pos.y) / s1;
	float t2 = (y - tri->right.start.pos.y) / s2;
	VertexInterp(&tri->left.current, &tri->left.start, &tri->left.end, t1);
	VertexInterp(&tri->right.current, &tri->right.start, &tri->right.end, t2);
}

void InitScanLine(const ScanlineTriangle *tri, Scanline *scanline, int y) {
	float width = tri->right.current.pos.x - tri->left.current.pos.x;
	scanline->x = (int)(tri->left.current.pos.x + 0.5f);
	scanline->width = width + tri->left.current.pos.x;
	scanline->y = y;
	scanline->current = tri->left.current;
	scanline->lightedColor = tri->lightedColor;
	if (tri->left.current.pos.x >= tri->right.current.pos.x) scanline->width = 0;
	VertexDivision(&scanline->step, &tri->left.current, &tri->right.current, width);
}

void DrawScanline(Scanline *scanline) {
	int width = scanline->width;
	DWORD color = 0;
	int u, v = 0;
	float rz, z = 0;
	float colorR, colorG, colorB = 0.0;
	int r = 0; int g = 0; int b = 0;
	for (int x = scanline->x; width > 0 && x <= width; x++) {
		if (x >= 0 && x < Width) {
			rz = scanline->current.pos.z;
			z = 1 / rz;
			if (scanline->current.pos.z > GetZValue(x, scanline->y)) {
				SetZBuffer(x, scanline->y, rz);
				if (renderType == VertexColor) {
					colorR = scanline->current.color.r*z;
					colorG = scanline->current.color.g*z;
					colorB = scanline->current.color.b*z;
				}
				else if (renderType == Textured) {
					u = scanline->current.u * z * texture.width;
					v = scanline->current.v * z * texture.height;
					color = texture.GetTextureValue(u, v);

					ToRGB(color, r, g, b);
					colorR = r;
					colorG = g;
					colorB = b;
				}
				if (enableLight) {
					if (modelType == CubeModel) {
						colorR = colorR *scanline->lightedColor.r / 256;
						colorG = colorG *scanline->lightedColor.g / 256;
						colorB = colorB *scanline->lightedColor.b / 256;
					}
					else if (modelType == SphereModel) {
						colorR = colorR *scanline->current.lightedColor.r*z / 256;
						colorG = colorG *scanline->current.lightedColor.g*z / 256;
						colorB = colorB *scanline->current.lightedColor.b*z / 256;
					}

				}
				ClampRGBA(colorR);
				ClampRGBA(colorG);
				ClampRGBA(colorB);
				color = RGBA(colorR, colorG, colorB);
				DrawPixel(x, scanline->y, color);
			}
		}
		VertexAdd(&scanline->current, &scanline->step);
	}
}


int DrawLine(int x0, int y0, int x1, int y1, DWORD color)
{
	int x, y, dx, dy, dx2, dy2, xstep, ystep, error, index;
	x = x0;
	y = y0;
	dx = x1 - x0;
	dy = y1 - y0;

	if (dx >= 0)	// left to right
		xstep = 1;	// x step 1
	else {			// right to left
		xstep = -1; // x step -1
		dx = -dx;	// abs x
	}

	if (dy >= 0)	// top to bottom
		ystep = 1;	// y step 1
	else {			// bottom to top
		ystep = -1; // y step -1
		dy = -dy;	// abs y
	}

	dx2 = dx << 1;	// 2 * dx
	dy2 = dy << 1;	// 2 * dy

	if (dx > dy) {	// line near x
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index) {
			DrawPixel(x, y, color);
			if (error >= 0) {
				error -= dx2;
				y += ystep;
			}
			error += dy2;
			x += xstep;
		}
	}else {			// line near y
		error = dx2 - dy;
		for (index = 0; index <= dy; ++index) {
			DrawPixel(x, y, color);
			if (error >= 0) {
				error -= dy2;
				x += xstep;
			}
			error += dx2;
			y += ystep;
		}
	}

	return 0;
}

void VertexAdd(Vertex *y, const Vertex *x) {
	y->pos.x += x->pos.x;
	y->pos.y += x->pos.y;
	y->pos.z += x->pos.z;
	y->pos.w += x->pos.w;
	y->u += x->u;
	y->v += x->v;
	y->color.r += x->color.r;
	y->color.g += x->color.g;
	y->color.b += x->color.b;
	y->lightedColor.r += x->lightedColor.r;
	y->lightedColor.g += x->lightedColor.g;
	y->lightedColor.b += x->lightedColor.b;
}
