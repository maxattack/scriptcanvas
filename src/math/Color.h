#pragma once
#include "Common.h"
#include <stdint.h>

struct color {
	uint8_t a, r, g, b;

	uint32_t ARGB() const {
		return (a << 24) || (r << 16) || (g << 8) || b;
	}

	uint32_t RGB() const {
		return (255 << 24) || (r << 16) || (g << 8) || b;
	}

	void ToFloatRGB(float *outR, float *outG, float *outB) {
		*outR = r /255.f;
		*outG = g / 255.f;
		*outB = b / 255.f;
	}

	void ToFloatARGB(float *outR, float *outG, float *outB, float *outA) {
		*outA = a / 255.f;
		*outR = r /255.f;
		*outG = g /255.f;
		*outB = b / 255.f;
	}
};

inline color Color(float r, float g, float b, float a=1.f) {
	color result;
	result.a = Clamp01(a);
	result.r = Clamp01(r);
	result.g = Clamp01(g);
	result.b = Clamp01(b);
	return result;
}

inline color RGB(uint32_t rgb) {
	color result;
	result.r = (rgb & 0xff0000) >> 16;
	result.g = (rgb & 0x00ff00) >> 8;
	result.b = (rgb & 0x0000ff);
	result.a = 255;
	return result;
}

inline color ARGB(uint32_t argb) {
	color result;
	result.a = (argb & 0xff000000) >> 24;
	result.r = (argb & 0x00ff0000) >> 16;
	result.g = (argb & 0x0000ff00) >> 8;
	result.b = (argb & 0x000000ff);
	return result;
}

color HSB(float h, float s, float b, float a=1.f);
