#pragma once
#include <GL/glfw.h>
#include "util/SceneUtil.h"
#include "util/CompactPool.h"

struct CircleComponent {
	ID node;
	ID geometry;
	ID material;
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
		uint16_t material;
		uint16_t geometry;
		uint16_t transform;
	} fields;
};

bool CreateCircleComponent(ID node);
CircleComponent& GetCircleComponent(ID node);
bool DestroyCircleComponent(ID node);

ID CreateCircleMaterial(float r=1, float g=1, float b=1);
CircleMaterial& GetCircleMaterial(ID matId);
void DestroyCircleMaterial(ID matId);

ID CreateCircleGeometry(float radius=1);
CircleGeometry& GetCircleGeometry(ID geomId);
void DestroyCircleGeometry(ID geomId);

void UpdateCircleSystem(RenderBuffer *vbuf);
void RenderCircleSystem(RenderBuffer *vbuf);

struct CircleSystem : public IManager {
	bool CreateComponent(ID node) { return CreateCircleComponent(node); }
	bool DestroyComponent(ID node) { return DestroyCircleComponent(node); }
	void Update(RenderBuffer *vbuf) { UpdateCircleSystem(vbuf); }
	void Render(RenderBuffer *vbuf) { RenderCircleSystem(vbuf); }
};
