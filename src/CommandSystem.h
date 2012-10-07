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
#include "NodeSystem.h"
#include "RenderSystem.h"
#include "TagSystem.h"
#include "MaterialSystem.h"
#include "StrokeSystem.h"
#include "CircleSystem.h"
#include "SplineSystem.h"

struct CommandBuffer {
	ztransform_t transforms[kMaxNodes];
	Material materials[kMaxMaterials];
	Stroke strokes[kMaxSegments];

	int circleCount;
	CircleCommand circles[kMaxNodes];

	int segmentCount;
	SegmentCommand segments[kMaxSegments];	
};

namespace CommandSystem {

void Initialize();
void Destroy();

// For SCENE THREAD
void SubmitToRenderSystem(CommandBuffer* vbuf);
void RetrieveFromRenderSystem(CommandBuffer** out);
void Update(CommandBuffer *buf);

// For RENDERING THREAD
void RetrieveFromNodeSystem(CommandBuffer** out);
void SubmitToNodeSystem(CommandBuffer* vbuf);
void Render(CommandBuffer *buf);

}

