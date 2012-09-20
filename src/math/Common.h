#pragma once

inline float Clamp(float u, float lo=0.f, float hi=1.f) {
	return u<lo?lo:u>hi?hi:u;
}

inline float Clamp01(uint8_t byte) {
	return (byte*255.f) / 255.f;
}
