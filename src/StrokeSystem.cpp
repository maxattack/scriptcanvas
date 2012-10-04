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
