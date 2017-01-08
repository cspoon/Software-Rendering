#pragma once

#define PI         ((float)3.141592654f)
#define DEG_TO_RAD(ang) ((ang)*PI/180.0)
#define RAD_TO_DEG(rads) ((rads)*180.0/PI)
#define EPSILON_E5 (float)(1E-5)

inline float Interp(float x1, float x2, float t) { return x1 + (x2 - x1) * t; }
inline bool FloatEqual(float a, float b){ return fabs(a - b)< EPSILON_E5;}
inline int Clamp(int value, int a, int b) {return min(max(a, value), b);}