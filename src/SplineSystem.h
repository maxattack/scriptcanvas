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

struct SegmentCommand {
	uint16_t mid;
	uint16_t stroke;
	uint16_t start;
	uint16_t end;
	float eccentricity;
};

struct Segment {
	ID start;
	ID end;
	ID material;
	ID stroke;
	float eccentricity;
};

namespace SplineSystem {

void Initialize();
void Destroy();

void Update(CommandBuffer *vbuf);
void Render(CommandBuffer *vbuf);

void OnNodeDestroyed(ID node);

ID CreateSegment(ID start, ID end, ID mat, ID stroke=0, float eccentricity=INFINITY);
Segment& GetSegment(ID sid);
void DestroySegment(ID sid);

inline ID Start(ID sid) { return GetSegment(sid).start; }
inline ID End(ID sid) { return GetSegment(sid).end; }
inline ID GetStrokeID(ID sid) { return GetSegment(sid).stroke; }
inline Stroke& GetStroke(ID sid) { return StrokeSystem::GetStroke(GetSegment(sid).stroke); }
inline ID GetMaterialID(ID sid) { return GetSegment(sid).material; }
inline Material& GetMaterial(ID sid) { return MaterialSystem::GetMaterial(GetSegment(sid).material); }
inline void SetMaterial(ID sid, ID mid) { ASSERT(MaterialSystem::MaterialValid(mid)); GetSegment(sid).material = mid; }
inline void SetStroke(ID sid, ID stroke) { ASSERT(StrokeSystem::StrokeValid(stroke)); GetSegment(sid).stroke = stroke; }
inline float Eccentricity(ID sid) { return GetSegment(sid).eccentricity; }
inline void SetEccentricity(ID sid, float e) { GetSegment(sid).eccentricity = e; }
inline void ClearEccentricity(ID sid) { GetSegment(sid).eccentricity = INFINITY; }

}
