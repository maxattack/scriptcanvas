#pragma once
#include "Foundation.h"
#include "Geometry.h"

struct CircleCommand {
    uint16_t queue;
    uint16_t transform_t;
    float radius;
    color_t fill;
};

namespace CircleSystem {

void Initialize();
void Destroy();

void Create(ID node, color_t color, float radius);
void Destroy(ID node);

void OnNodeDestroyed(ID node);

void Update(CommandBuffer* buf);
void Render(CommandBuffer *buf);

circle_t& GetCircle(ID node);

// Helper Functions

inline color_t Fill(ID node) { return GetCircle(node).fill; }
inline float Radius(ID node) { return GetCircle(node).radius; }
inline void SetFill(ID node, color_t fill) { GetCircle(node).fill = fill; }
inline void SetRadius(ID node, float r) { GetCircle(node).radius = r; }

}
