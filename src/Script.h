#pragma once
#include "SceneSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"

namespace SceneSystem {

inline void GetPosition(ID node, float *x=0, float *y=0) {
	auto position = LocalToParent(node).t;
	*x = position.t.x;
	*y = position.t.y;
}

inline void SetPosition(ID node, float x, float y) {
	LocalToParent(node).t.t = Float2(x,y);
}

inline float GetRotation(ID node) {
	return LocalToParent(node).t.q.Radians() * (180.0f/M_PI);
}

inline void SetRotation(ID node, float degrees) {
	LocalToParent(node).t.q = Polar(1.f, degrees * (M_PI/180.0f));
}

inline void SetDirection(ID node, float dx, float dy) {
	LocalToParent(node).t.q = Float2(dx, dy);
}

} // SceneSystem

namespace CircleSystem {

extern CircleManager gInst;

inline void Create(ID node, uint32_t color, float radius) {
	SceneSystem::AddComponent(node, kComponentCircle);
	gInst[node].fill = RGB(color);
	gInst[node].radius = radius;

}

inline void Destroy(ID node) {
	gInst.DestroyComponent(node);
}

inline void Fill(ID node, uint32_t *color=0) {
	*color = gInst[node].fill.RGB();
}

inline void Radius(ID node, float *radius=0) {
	*radius = gInst[node].radius;
}

inline void SetFill(ID node, uint32_t fill) {
	gInst[node].fill = RGB(fill);
}

inline void SetRadius(ID node, float r) {
	gInst[node].radius = r;
}

} // CircleSystem

namespace SplineSystem {

extern SplineManager gInst;

inline ID CreateMaterial(float weight, uint32_t color) {
	return gInst.CreateMaterial(weight, RGB(color));
}

inline float Weight(ID mat) {
	return gInst.GetMaterial(mat).weight;
}

inline uint32_t Color(ID mat) {
	return gInst.GetMaterial(mat).color.RGB();
}

inline void SetWeight(ID mat, float weight) {
	gInst.GetMaterial(mat).weight = weight;
}

inline void SetColor(ID mat, uint32_t color) {
	gInst.GetMaterial(mat).color = RGB(color);
}

inline ID CreateHermiteSegment(ID start, ID end, ID mat) {
	return gInst.CreateHermiteSegment(start, end, mat);
}

} // SplineSystem

void ScriptPaint();
