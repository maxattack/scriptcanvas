#pragma once
#include "SceneSystem.h"
#include "CircleManager.h"
#include "SplineManager.h"

// TODO: remove explicit commands from this header -- instead
// individual manager write to a generic buffer which is
// identified with their component ID.  Only the transforms
// are known ahead of time.

struct RenderBuffer {
	// counts
	int circleCount;

	int materialCount;
	int segmentCount;

	// buffers (one shared, compact buffer?)
	ztransform transforms[kMaxNodes];
	CircleCommand circles[kMaxNodes];
	Material materials[kMaxMaterials];
	SegmentCommand segments[kMaxSegments];
	
};

namespace RenderSystem {

void Initialize();
void Clear(RenderBuffer *vbuf);

// For SCENE THREAD
void SubmitToRenderSystem(RenderBuffer* vbuf);
void RetrieveFromRenderSystem(RenderBuffer** out);

// For RENDERING THREAD
void RetrieveFromSceneSystem(RenderBuffer** out);
void SubmitToSceneSystem(RenderBuffer* vbuf);
void Render(RenderBuffer* vbuf);


// utils
GLuint LoadShaderProgram(const char* filename);

}