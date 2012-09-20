#pragma once
#include "SceneSystem.h"
#include "CircleSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"

#define COMPONENT_CIRCLE	0

namespace SceneSystem {

void Paint();

inline void GetPosition(ID node, float *x=0, float *y=0) {
	auto position = LocalToParent(node).t;
	*x = position.x;
	*y = position.y;
}

inline void SetPosition(ID node, float x, float y) {
	LocalToParent(node).t = Float2(x,y);
}

inline float GetRotation(ID node) {
	return LocalToParent(node).q.Radians() * (180.0f/M_PI);
}

inline void SetRotation(ID node, float degrees) {
	LocalToParent(node).q = Polar(1.f, degrees * (M_PI/180.0f));
}

} // SceneSystem

namespace CircleSystem {

extern CircleManager inst;

inline void Create(ID node, uint32_t color, float radius) {
	SceneSystem::AddComponent(node, COMPONENT_CIRCLE);
	inst[node].fill = RGB(color);
	inst[node].radius = radius;

}

inline void Destroy(ID node) {
	inst.DestroyComponent(node);
}

inline void Fill(ID node, uint32_t *color=0) {
	*color = inst[node].fill.RGB();
}

inline void Radius(ID node, float *radius=0) {
	*radius = inst[node].radius;
}

inline void SetFill(ID node, uint32_t fill) {
	inst[node].fill = RGB(fill);
}

inline void SetRadius(ID node, float r) {
	inst[node].radius = r;
}

} // CircleSystem

namespace Script {
	
inline void Initialize() {
	SceneSystem::RegisterComponentManager(COMPONENT_CIRCLE, &CircleSystem::inst);
}

inline void Update(RenderBuffer* vbuf) {
	CircleSystem::inst.Update(vbuf);
}

inline void Render(RenderBuffer* vbuf) {
	CircleSystem::inst.Render(vbuf);
}
	
} // Script

