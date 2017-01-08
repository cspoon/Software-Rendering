#pragma once

#include "stdafx.h"
#include "mesh.h"
#include "transform.h"
class Camera;

void TransformRenderList(list<Triangle> &renderList, Matrix4x4 &m);
void ModelToWorldRenderList(list<Triangle> &renderList, Point &worldPos);
void BuildCameraMatrixEuler(Camera &cam);
void CullBackfacesRenderList(list<Triangle> &renderList, Camera &cam);
void WorldToCameraRenderList(list<Triangle> &renderList, Camera &cam);
void ClipRenderList(list<Triangle> &renderList, Camera &cam);
void CameraToPerspectiveRenderList(list<Triangle> &renderList, Camera &cam);
void PerspectiveToScreenRenderList(list<Triangle> &renderList, Camera &cam);
void DrawRenderList(list<Triangle> &renderList, RenderType rt);