#pragma once
#include "SceneSystem.h"
#include "CircleSystem.h"

struct RenderBuffer {
	// counts
	int circleCount;

	// buffers (one shared, compact buffer?)
	transform transforms[MAX_NODES];
	CircleManager::Command circles[MAX_NODES];
};

namespace RenderSystem {

void Initialize();

// For SCENE THREAD
void SubmitToRenderSystem(RenderBuffer* vbuf);
void RetrieveFromRenderSystem(RenderBuffer** out);

// For RENDERING THREAD
void RetrieveFromSceneSystem(RenderBuffer** out);
void SubmitToSceneSystem(RenderBuffer* vbuf);
void Render(RenderBuffer* vbuf);

}