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
#include "MaterialSystem.h"

struct CircleCommand {
    //uint16_t queue;
    uint16_t mid;
    uint16_t tid;
};

struct Circle {
    MatID material;
};

namespace CircleSystem {

void Initialize();
void Destroy();

void Create(NodeID node, MatID mid);
void Destroy(NodeID node);

void OnNodeDestroyed(NodeID node);

void Update(CommandBuffer* buf);
void Render(CommandBuffer *buf);

Circle& GetCircle(NodeID node);

// Helper Functions

inline MatID GetMaterialID(NodeID node) { return GetCircle(node).material; }
inline Material& GetMaterial(NodeID node) { return MaterialSystem::GetMaterial(GetCircle(node).material); }

inline color_t Fill(NodeID node) { return GetMaterial(node).color; }
inline float Radius(NodeID node) { return GetMaterial(node).weight; }
inline void SetFill(NodeID node, color_t fill) { GetMaterial(node).color = fill; }
inline void SetRadius(NodeID node, float r) { GetMaterial(node).weight = r; }

}
