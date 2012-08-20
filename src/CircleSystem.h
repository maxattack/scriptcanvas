#pragma once
#include <GL/glfw.h>
#include "util/SceneUtil.h"
#include "util/CompactPool.h"

namespace CircleSystem {

struct Component {
	ID node;
	ID geometry;
	ID material;
};

struct Geometry {
	float radius;
};

struct Material {
	float r, g, b, a;
};

union Command {
	uint64_t id;
	struct {
		uint16_t queue;
		uint16_t material;
		uint16_t geometry;
		uint16_t transform;
	} fields;
};

ID CreateMaterial(float r=1, float g=1, float b=1);
void SetMaterial(ID node, ID mat);
ID GetMaterial(ID node);
void SetMaterialColor(ID mat, float r, float g, float b);
void DestroyMaterial(ID matId);

ID CreateGeometry(float radius=1);
void SetGeometry(ID node, ID geom);
ID GetGeometry(ID node);
void SetGeometryRadius(ID geomId, float radius);
void DestroyGeometry(ID geomId);

void Update(RenderBuffer *vbuf);
void Render(RenderBuffer *vbuf);

struct Manager : public IManager {
	bool CreateComponent(ID node);
	bool DestroyComponent(ID node);
	void Update(RenderBuffer *vbuf) { CircleSystem::Update(vbuf); }
	void Render(RenderBuffer *vbuf) { CircleSystem::Render(vbuf); }
};

}