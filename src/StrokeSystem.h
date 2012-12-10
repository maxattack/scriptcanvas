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

struct Stroke {
	float start;
	float eccentricity;
	float end;
};

typedef ID StrokeID;

namespace StrokeSystem {

void Initialize();
void Destroy();

void Update(CommandBuffer *vbuf);

StrokeID CreateStroke(float start, float end, float eccen=0.f);
bool StrokeValid(StrokeID sid);
Stroke& GetStroke(StrokeID strokeId);
uint16_t Index(StrokeID strokeId);
void DestroyStroke(StrokeID strokeId);


inline StrokeID CreateUniformStroke(float uniform) { return CreateStroke(uniform, uniform); }
inline StrokeID CreateEccentricStroke(float uniform, float eccen) { return CreateStroke(uniform, uniform, eccen); }

inline float Start(StrokeID strokeId) { return GetStroke(strokeId).start; }
inline float End(StrokeID strokeId) { return GetStroke(strokeId).end; }
inline float Eccentricity(StrokeID strokeId) { return GetStroke(strokeId).eccentricity; }
inline void SetStart(StrokeID strokeId, float s) { GetStroke(strokeId).start = s; }
inline void SetEnd(StrokeID strokeId, float e) { GetStroke(strokeId).end = e; }
inline void SetEccentricity(StrokeID strokeId, float e) { GetStroke(strokeId).eccentricity = e; }



}
