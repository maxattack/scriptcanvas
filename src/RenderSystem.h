#pragma once
#include "SceneSystem.h"
#include "CircleSystem.h"

enum RenderType {
	kRenderTypeCirle
};

struct RenderBuffer {
	Transform transforms[MAX_NODES];
	CircleGeometry circleGeometry[MAX_NODES];
	CircleMaterial circleMaterials[MAX_NODES];
	CircleCommand circleCommands[MAX_NODES];
	int circleCount;
};

void InitializeRenderSystem();


// For SCENE THREAD
void SubmitToRenderSystem(RenderBuffer* vbuf);
void RetrieveFromRenderSystem(RenderBuffer** out);

// For RENDERING THREAD
void RetrieveFromSceneSystem(RenderBuffer** out);
void SubmitToSceneSystem(RenderBuffer* vbuf);
void Render(RenderBuffer* vbuf);
