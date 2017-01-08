// MySoftwareRendering.cpp : 定义控制台应用程序的入口点。
//

#include "main.h"
#include "mesh.h"
#include "render.h"
#include "renderPipline.h"
#include "Camera.h"
#include "Light.h"

list<Triangle> renderList;
int frames;
double deltaTime;
float timeInterval = 1.0;
double time;
float timeLeft = timeInterval;
char onStr[] = "On";
char offStr[] = "Off";
char sphereStr[] = "Sphere";
char cubeStr[] = "Cube";
char textureStr[] = "Texture";
char gouraudStr[] = "GouraudColor";
char wireFrameStr[] = "WireFrame";

char str[100];
char str1[] = "Press Arrow Up,Donw,Left,Right To Rotate Model";
char str2[] = "Press 'W','S','A','D' To Move Camera";
char str3[] = "Press 'I','K','J','L' To Rotate Camera";
char str4[100];
char str5[100];
char str6[100];
char str7[100];
char str8[100];
char str9[100];
char str10[100];
void DrawHelpText(HDC hdc);
void UpdateFPS();
void Reset();
void ClientInit();
void GetInput(HWND hwnd);

LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg) {
	case WM_CREATE: {
		return 0;
	}break;
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}break;
	case WM_KEYDOWN:
		switch (wparam) {
		case VK_ESCAPE: {
			::DestroyWindow(hwnd);
		}
		case VK_SPACE: {
			 renderTypeIndex++;
			renderType = renderTypes[renderTypeIndex % 3];
		}break;
		case 'E': {
			enableLight = !enableLight;
		}break;
		case 'C': {
			enableCullBackFace = !enableCullBackFace;
		}break;
		case '1': {
			modelType = modelType == CubeModel ? SphereModel : CubeModel;
		}break;
		case '2': {
			lights[0].state = !lights[0].state;
		}break;
		case '3': {
			lights[1].state = !lights[1].state;
		}break;
		case '4': {
			camSpeed++;
		}break;
		case '5': {
			camSpeed--;
		}break;
		case VK_F1: {
			needHelpMenu = !needHelpMenu;
		}break;
		default:
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	WNDCLASSEX winclass;
	HWND	   hwnd;
	MSG		   msg;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC |
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = L"WINCLASS1";
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


	memset(colorBuffer, ColorWhiteDW, Height * Width * sizeof(DWORD));
	for (int i = 0; i < Width * Height; i++) {
		zBuffer[i] = 0.0;
	}

	if (!RegisterClassEx(&winclass)) {
		return 0;
	}
	if (!(hwnd = CreateWindowEx(NULL,                  // extended style
		L"WINCLASS1",		// class
		L"This is a Test", // title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 250,	  // initial x,y
		Width, Height,  // initial width, height
		NULL,	  // handle to parent 
		NULL,	  // handle to menu
		hinstance,// instance of this application
		NULL))) {	//// extra creation parms
		return 0;
	}
	ClientInit();

	HDC hdc = GetDC(hwnd);

	HDC compatibleDC = CreateCompatibleDC(hdc);
	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biWidth = Width;
	bmpInfo.bmiHeader.biHeight = Height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	HBITMAP mBitmap = CreateCompatibleBitmap(hdc, Width, Height);
	while (TRUE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		GetInput(hwnd);
		SelectObject(compatibleDC, mBitmap);
		Reset();

		Matrix4x4 rot = BuildXyzRotationMatrix(objRotX, objRotY, 0);
		
		TransformRenderList(renderList, rot);
		
		ModelToWorldRenderList(renderList, objPos);
		
		if (enableLight)
			for(int i=0; i<2; i++)
				lights[i].RenderLight(renderList);

		BuildCameraMatrixEuler(camera);

		if (enableCullBackFace)
			CullBackfacesRenderList(renderList, camera);

		WorldToCameraRenderList(renderList, camera);

		ClipRenderList(renderList, camera);

		CameraToPerspectiveRenderList(renderList, camera);

		PerspectiveToScreenRenderList(renderList, camera);

 		DrawRenderList(renderList, renderType);

		SetDIBits(compatibleDC, mBitmap, 0, Height, colorBuffer,
		&bmpInfo, DIB_PAL_COLORS);
		DrawHelpText(compatibleDC);

		BitBlt(hdc, 0, 0, Width, Height, compatibleDC, 0, 0, SRCCOPY);
		UpdateFPS();

	}
	DeleteObject(mBitmap);
	DeleteDC(compatibleDC);
	ReleaseDC(hwnd, hdc);
	return(msg.wParam);
}

void Reset() {
	renderList.clear();
	memset(colorBuffer, ColorWhiteDW, Height * Width * sizeof(DWORD));
	for (int i = 0; i < Width * Height; i++) {
		zBuffer[i] = 0.0;
	}
	InsertModel(modelType);
}

void ClientInit() {
	renderTypes[0] = WireFrame;
	renderTypes[1] = VertexColor;
	renderTypes[2] = Textured;
	camera.IniCameraEuler(Euler, camPos, camDirEuler,
		100.0, 500.0, 90.0, Width, Height);

	Color diffColor = { 255.0, 255.0, 255.0 };
	Color amb = Color(25, 25, 25);
	Vector4 dir = Vector4(0, 0, -1, 0);
	
	lights[0].Init(Ambient, amb, ColorBlack, dir);
	lights[1].Init(Infinite, ColorWhite, diffColor, dir);
	renderType = renderTypes[renderTypeIndex % 3];
};

void GetInput(HWND hwnd){
	if (::GetAsyncKeyState(VK_UP) & 0x8000f) {
		objRotX += 200 * deltaTime;
	}
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f) {
		objRotX -= 200 * deltaTime;
	}
	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) {
		objRotY += 200 * deltaTime;
	}
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) {
		objRotY -= 200 * deltaTime;
	}
	if (::GetAsyncKeyState('W') & 0x8000f){
		camera.Move(VecForward, camSpeed, deltaTime);
	}
	if (::GetAsyncKeyState('S') & 0x8000f) {
		camera.Move(VecBackWard, camSpeed, deltaTime);
	}
	if (::GetAsyncKeyState('A') & 0x8000f) {
		camera.Move(VecLeft, camSpeed, deltaTime);
	}
	if (::GetAsyncKeyState('D') & 0x8000f) {
		camera.Move(VecRight, camSpeed, deltaTime);
	}
	if (::GetAsyncKeyState('I') & 0x8000f) {
		camera.dir.x -= 70 * deltaTime;
	}
	if (::GetAsyncKeyState('K') & 0x8000f) {
		camera.dir.x += 70 * deltaTime;
	}
	if (::GetAsyncKeyState('J') & 0x8000f) {
		camera.dir.y -= 70 * deltaTime;
	}
	if (::GetAsyncKeyState('L') & 0x8000f) {
		camera.dir.y += 70 * deltaTime;
	}
}

double SystemTimeToTime(SYSTEMTIME st)
{
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	LONGLONG nLL;
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	nLL = (ft.dwHighDateTime << 32) | ft.dwLowDateTime;
	double pt = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000.0);
	return pt;
}

double GetCTime()
{
	SYSTEMTIME Time = { 0 };
	GetLocalTime(&Time);
	double time = SystemTimeToTime(Time);
	return time;
}

void DrawLable(HDC hdc, LPCSTR str, int lineNum){
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT);
	TextOutA(hdc, 0, 17 * lineNum, str, strlen(str));
}

void UpdateFPS(){
	frames++;
	double currTime = GetCTime();
	deltaTime = currTime - time;
	timeLeft -= deltaTime;
	time = currTime;
	if (timeLeft <= 0) {
		fpsCount = frames;
		frames = 0;
		timeLeft = timeInterval;
	}
}

void DrawHelpText(HDC hdc){
	if (!needHelpMenu) {
		sprintf_s(str, "FPS:%d   Press F1 To Show Key Control", fpsCount);
	}else {
		sprintf_s(str, "FPS:%d   ", fpsCount);
		DrawLable(hdc, str1, 1);
		DrawLable(hdc, str2, 2);
		DrawLable(hdc, str3, 3);
		if (enableLight)
			sprintf_s(str4, "Press 'E' To Toggle Lights   %s", onStr);
		else
			sprintf_s(str4, "Press 'E' To Toggle Lights   %s", offStr);
		DrawLable(hdc, str4, 4);
		if (modelType == CubeModel)
			sprintf_s(str5, "Press '1' To Toggle Model    %s", cubeStr);
		else
			sprintf_s(str5, "Press '1' To Toggle Model    %s", sphereStr);
		DrawLable(hdc, str5, 5);
		if (renderType == Textured)
			sprintf_s(str6, "Press Space To Change Render State   %s", textureStr);
		else if (renderType == VertexColor)
			sprintf_s(str6, "Press Space To Change Render State   %s", gouraudStr);
		else
			sprintf_s(str6, "Press Space To Change Render State   %s", wireFrameStr);
		DrawLable(hdc, str6, 6);
		if (lights[0].state)
			sprintf_s(str7, "Press '2' To Toggle Ambient Light   %s", onStr);
		else
			sprintf_s(str7, "Press '2' To Toggle Ambient Light   %s", offStr);
		DrawLable(hdc, str7, 7);
		if (lights[1].state)
			sprintf_s(str8, "Press '3' To Toggle Infinite Light   %s", onStr);
		else
			sprintf_s(str8, "Press '3' To Toggle Infinite Light   %s", offStr);
		DrawLable(hdc, str8, 8);
		if (enableCullBackFace)
			sprintf_s(str9, "Press 'C' To Toggle Cull Backface   %s", onStr);
		else
			sprintf_s(str9, "Press 'C' To Toggle Cull Backface   %s", offStr);
		DrawLable(hdc, str9, 9);
		sprintf_s(str10, "Press '4','5' To Control Camera Speed   %d", camSpeed);
		DrawLable(hdc, str10, 10);
	}
	DrawLable(hdc, str, 0);
}