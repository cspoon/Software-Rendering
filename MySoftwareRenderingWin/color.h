#pragma once

#define ColorRed RGBA(255, 0, 0)
#define ColorBlackDW RGBA(0, 0, 0)
#define ColorBlack Color{0, 0, 0}
#define ColorWhite Color{255, 255, 255}
#define ColorWhiteDW RGBA(255, 255, 255)

#define ColorGray Color{100, 100, 100}
#define ColorYellow Color{255, 255, 100}


class Color
{
public:
	Color() { r = g = b = .0f; }
	Color(float rr, float gg, float bb) : r(rr), g(gg), b(bb) {}
	float r, g, b;
};

inline DWORD RGBA(int r, int g, int b, int a = 0) {
	return ((b)+((g) << 8) + ((r) << 16) + ((a) << 24));
}

inline void ToRGB(DWORD rbg, int &r, int &g, int &b) {
	r = rbg >> 16 & 0xff;
	g = rbg >> 8 & 0xff;
	b = rbg & 0xff;
}
