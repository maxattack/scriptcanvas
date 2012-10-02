#pragma once
#include "Foundation.h"
#include "MaterialSystem.h"

struct CircleCommand {
    //uint16_t queue;
    uint16_t mid;
    uint16_t tid;
};

struct Circle {
    ID material;
};

namespace CircleSystem {

void Initialize();
void Destroy();

void Create(ID node, ID mid);
void Destroy(ID node);

void OnNodeDestroyed(ID node);

void Update(CommandBuffer* buf);
void Render(CommandBuffer *buf);

Circle& GetCircle(ID node);

// Helper Functions

inline ID GetMaterialID(ID node) { return GetCircle(node).material; }
inline Material& GetMaterial(ID node) { return MaterialSystem::GetMaterial(GetCircle(node).material); }

inline color_t Fill(ID node) { return GetMaterial(node).color; }
inline float Radius(ID node) { return GetMaterial(node).weight; }
inline void SetFill(ID node, color_t fill) { GetMaterial(node).color = fill; }
inline void SetRadius(ID node, float r) { GetMaterial(node).weight = r; }

}
