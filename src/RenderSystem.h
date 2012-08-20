#pragma once
#include "SceneSystem.h"
#include "CircleSystem.h"

enum RenderType {
	kRenderTypeCirle
};

struct RenderBuffer {
	Transform transforms[MAX_NODES];
	CircleSystem::Geometry circleGeometry[MAX_NODES];
	CircleSystem::Material circleMaterials[MAX_NODES];
	CircleSystem::Command circleCommands[MAX_NODES];
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
