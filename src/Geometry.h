#pragma once
#include "Math.h"
#include "Color.h"

struct circle_t {
    float radius;
    color_t fill;
};

inline circle_t Circle(float radius=1.f, color_t color=Color()) {
	circle_t result = { radius, color };
	return result;
}
