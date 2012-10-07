// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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