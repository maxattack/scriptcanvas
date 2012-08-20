#pragma once
#include "SceneSystem.h"

enum RenderType {
	kRenderTypeCirle
};

struct CircleGeometry {
	float radius;
};

struct CircleMaterial {
	float r, g, b, a;
};

union CircleCommand {
	uint64_t id;
	struct {
		uint16_t queue;
		uint16_t transform;
		uint16_t material;
		uint16_t geometry;
	} fields;
};

struct RenderBuffer {
	Transform transforms[MAX_NODES];
	CircleGeometry circleGeometry[MAX_NODES];
	CircleMaterial circleMaterials[MAX_NODES];
	CircleCommand circleCommands[MAX_NODES];


	int circleCount;
};


// For SCENE THREAD
bool SubmitToRenderSystem(RenderBuffer* vbuf);
bool RetrieveFromRenderSystem(RenderBuffer** out);

// For RENDERING THREAD
bool RetrieveFromSceneSystem(RenderBuffer** out);
bool SubmitToSceneSystem(RenderBuffer* vbuf);
void Render(RenderBuffer* vbuf);
