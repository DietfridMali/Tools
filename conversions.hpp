#pragma once

#define _USE_MATH_DEFINES // for C++

#include <cmath>

#ifndef M_PI
#   define M_PI 3.14159265358979323846f
#   define HAVE_M_PI
#endif


class Conversions
{
public:
    static float DegToRad(float deg) {
        return (float)(deg / 180.0 * (float) M_PI);
    }

    static float RadToDeg(float rad) {
        return (float)(rad * 180.0 / (float) M_PI);
    }

    static float DotToRad(float dot) {
        return (float)acos(dot);
    }

    static float DotToDeg(float dot) {
        return RadToDeg(DotToRad(dot));
    }
};
