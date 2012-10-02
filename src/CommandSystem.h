#pragma once
#include "NodeSystem.h"
#include "RenderSystem.h"
#include "TagSystem.h"
#include "MaterialSystem.h"
#include "CircleSystem.h"
#include "SplineSystem.h"

struct CommandBuffer {
	ztransform_t transforms[kMaxNodes];
	Material materials[kMaxMaterials];

	int circleCount;
	CircleCommand circles[kMaxNodes];

	int cubicSegmentCount;
	SegmentCommand cubicSegments[kMaxSegments];	

	int eccentricSegmentCount;
	EccentricSegmentCommand eccentricSegments[kMaxSegments];
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

