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

#include "StrokeSystem.h"
#include "CommandSystem.h"
#include "util/CompactPool.h"

struct StrokeSlot {
	int refCount;
	Stroke stroke;
};

static CompactPool<StrokeSlot, kMaxSegments> mStrokes;

// TODO: Handle RefCounts

void StrokeSystem::Initialize() {
	// NOOP
}

void StrokeSystem::Update(CommandBuffer *vbuf) {
	auto cmd = vbuf->strokes;
	for (auto p=mStrokes.Begin(); p!=mStrokes.End(); ++p) {
		*cmd = p->stroke;
		cmd++;
	}
}

void StrokeSystem::Destroy() {
	// NOOP
}

ID StrokeSystem::CreateStroke(float start, float end, float eccentricity) {
	ID result = mStrokes.TakeOut();
	mStrokes[result].refCount = 0;
	mStrokes[result].stroke.start = start;
	mStrokes[result].stroke.end = end;
	mStrokes[result].stroke.eccentricity = eccentricity;
	return result;
}

uint16_t StrokeSystem::Index(ID sid) {
	return mStrokes.Index(sid);
}

Stroke& StrokeSystem::GetStroke(ID strokeId) {
	return mStrokes[strokeId].stroke;
}

bool StrokeSystem::StrokeValid(ID sid) {
	return mStrokes.IsActive(sid);
}

void StrokeSystem::DestroyStroke(ID strokeId) {
	ASSERT(mStrokes[strokeId].refCount == 0);
	mStrokes.PutBack(strokeId);
}
