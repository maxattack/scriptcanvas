#pragma once
#include "Math.h"

struct color_t {
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

inline color_t Color() {
		color_t result = { 255, 255, 255, 255 };
		return result;
}

inline color_t Color(float r, float g, float b, float a=1.f) {
	color_t result = {
		uint8_t(255 * Clamp01(a)),
		uint8_t(255 * Clamp01(r)),
		uint8_t(255 * Clamp01(g)),
		uint8_t(255 * Clamp01(b))
	};
	return result;
}

inline color_t RGB(uint32_t rgb) {
	color_t result = {
		255,
		uint8_t((rgb & 0xff0000) >> 16),
		uint8_t((rgb & 0x00ff00) >> 8),
		uint8_t(rgb & 0x0000ff)
	};
	return result;
}

inline color_t ARGB(uint32_t argb) {
	color_t result = {
		uint8_t((argb & 0xff000000) >> 24),
		uint8_t((argb & 0x00ff0000) >> 16),
		uint8_t((argb & 0x0000ff00) >> 8),
		uint8_t(argb & 0x000000ff)
	};
	return result;
}

color_t HSB(float h, float s, float b, float a=1.f);
