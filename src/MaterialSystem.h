#pragma once
#include "Foundation.h"
#include "Color.h"

struct Material {
	float weight;
	color_t color;
};

namespace MaterialSystem {

void Initialize();
void Destroy();

void Update(CommandBuffer *vbuf);

ID CreateMaterial(color_t color=Color(), float weight=1.f);
bool MaterialValid(ID mid);
Material& GetMaterial(ID mid);
uint16_t Index(ID mid);
void DestroyMaterial(ID mid);

inline float Weight(ID mat) { return GetMaterial(mat).weight; }
inline color_t Color(ID mat) { return GetMaterial(mat).color; }

inline void SetWeight(ID mat, float weight) { GetMaterial(mat).weight = weight; }
inline void SetColor(ID mat, color_t color) { GetMaterial(mat).color = color; }



}