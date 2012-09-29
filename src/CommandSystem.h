#pragma once
#include "SceneSystem.h"
#include "RenderSystem.h"
#include "CircleSystem.h"
#include "SplineSystem.h"

struct CommandBuffer {
	ztransform_t transform_ts[kMaxNodes];

	int circleCount;
	CircleCommand circles[kMaxNodes];

	int materialCount;
	int segmentCount;
	Material materials[kMaxMaterials];
	HermiteSegmentCommand hermiteSegments[kMaxSegments];	
};

namespace CommandSystem {

void Initialize();
void Destroy();

// For SCENE THREAD
void SubmitToRenderSystem(CommandBuffer* vbuf);
void RetrieveFromRenderSystem(CommandBuffer** out);
void Update(CommandBuffer *buf);

// For RENDERING THREAD
void RetrieveFromSceneSystem(CommandBuffer** out);
void SubmitToSceneSystem(CommandBuffer* vbuf);
void Render(CommandBuffer *buf);

}

