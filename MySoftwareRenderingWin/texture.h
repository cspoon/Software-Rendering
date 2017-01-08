#pragma once

#include "color.h"
#include "Utils.h"
using std::string;

class Texture
{
public:
	int width, height;
	DWORD *colors;

	Texture() {
		HBITMAP texHbmp = (HBITMAP)LoadImage(NULL, L"lena.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		BITMAP texBmp;
		GetObject(texHbmp, sizeof(BITMAP), &texBmp);
		width = texBmp.bmWidth;
		height = texBmp.bmHeight;
		texBmp.bmBits;
		UCHAR * p = (UCHAR*)(texBmp.bmBits);
		int index = 0;
		UCHAR b = 0; UCHAR g = 0; UCHAR r = 0;
		colors = new DWORD[width*height];
		for (int i = 0; i < width * height; i++) {
			UCHAR b = p[i * 3];
			UCHAR g = p[i * 3 + 1];
			UCHAR r = p[i * 3 + 2];
			colors[i] = RGBA(r, g, b);
		}
	}

	~Texture() {
		if (colors != NULL)
			delete[] colors;
	}

	inline DWORD GetTextureValue(int x, int y) {
		x = Clamp(x, 0, width-1);
		y = Clamp(y, 0, height-1);
 		return colors[y * width + x];
	}
};