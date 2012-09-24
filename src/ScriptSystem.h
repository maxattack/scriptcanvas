#pragma once
#include "SceneSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace ScriptSystem {
void Bind(lua_State* L);
}

namespace SceneSystem {

void Paint();
inline float2 Position(ID node) { return LocalToParent(node).t.t; }
inline float Rotation(ID node) { return LocalToParent(node).t.q.Radians() * (180.0f/M_PI); }
inline float2 Direction(ID node) { return LocalToParent(node).t.q; }
inline void SetPosition(ID node, float2 p) { LocalToParent(node).t.t = p; }
inline void SetRotation(ID node, float degrees) { LocalToParent(node).t.q = Polar(1.f, degrees * (M_PI/180.0f)); }
inline void SetDirection(ID node, float2 d) { LocalToParent(node).t.q = d; }

} // SceneSystem

namespace CircleSystem {

extern CircleManager gInst;

inline void Create(ID node, color_t color, float radius) {
	SceneSystem::AddComponent(node, kComponentCircle);
	gInst[node].fill = color;
	gInst[node].radius = radius;

}

inline void Destroy(ID node) { gInst.DestroyComponent(node); }
inline color_t Fill(ID node) { return gInst[node].fill; }
inline float Radius(ID node) { return gInst[node].radius; }
inline void SetFill(ID node, color_t fill) { gInst[node].fill = fill; }
inline void SetRadius(ID node, float r) { gInst[node].radius = r; }

} // CircleSystem

namespace SplineSystem {

extern SplineManager gInst;

inline ID CreateMaterial(float weight, color_t color) { return gInst.CreateMaterial(weight, color); }
inline float Weight(ID mat) { return gInst.GetMaterial(mat).weight; }
inline color_t Color(ID mat) { return gInst.GetMaterial(mat).color; }
inline void SetWeight(ID mat, float weight) { gInst.GetMaterial(mat).weight = weight; }
inline void SetColor(ID mat, color_t color) { gInst.GetMaterial(mat).color = color; }
inline ID CreateHermiteSegment(ID start, ID end, ID mat) { return gInst.CreateHermiteSegment(start, end, mat); }

} // SplineSystem
