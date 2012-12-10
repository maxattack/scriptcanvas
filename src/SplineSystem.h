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
	NodeID start;
	NodeID end;
	MatID material;
	StrokeID stroke;
	float eccentricity;
};

typedef ID SegmentID;

namespace SplineSystem {

void Initialize();
void Destroy();

void Update(CommandBuffer *vbuf);
void Render(CommandBuffer *vbuf);

void OnNodeDestroyed(ID node);

ID CreateSegment(ID start, ID end, ID mat, ID stroke=0, float eccentricity=INFINITY);
Segment& GetSegment(ID sid);
void DestroySegment(ID sid);

inline NodeID Start(SegmentID sid) { return GetSegment(sid).start; }
inline NodeID End(SegmentID sid) { return GetSegment(sid).end; }
inline StrokeID GetStrokeID(SegmentID sid) { return GetSegment(sid).stroke; }
inline Stroke& GetStroke(SegmentID sid) { return StrokeSystem::GetStroke(GetSegment(sid).stroke); }
inline MatID GetMaterialID(SegmentID sid) { return GetSegment(sid).material; }
inline Material& GetMaterial(SegmentID sid) { return MaterialSystem::GetMaterial(GetSegment(sid).material); }
inline void SetMaterial(SegmentID sid, MatID mid) { ASSERT(MaterialSystem::MaterialValid(mid)); GetSegment(sid).material = mid; }
inline void SetStroke(SegmentID sid, StrokeID stroke) { ASSERT(StrokeSystem::StrokeValid(stroke)); GetSegment(sid).stroke = stroke; }
inline float Eccentricity(SegmentID sid) { return GetSegment(sid).eccentricity; }
inline void SetEccentricity(SegmentID sid, float e) { GetSegment(sid).eccentricity = e; }
inline void ClearEccentricity(SegmentID sid) { GetSegment(sid).eccentricity = INFINITY; }

}
