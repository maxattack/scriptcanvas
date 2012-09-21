#pragma once
#include "SceneSystem.h"
#include "CircleManager.h"

struct CircleCommand {
	uint16_t queue;
	uint16_t transform;
	Circle properties;
};

struct RenderBuffer {
	// counts
	int circleCount;

	// buffers (one shared, compact buffer?)
	transform transforms[MAX_NODES];
	CircleCommand circles[MAX_NODES];
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

}