#pragma once
#include "SceneSystem.h"
#include "util/CompactComponentPool.h"
#include "util/CompactPool.h"
#include "math/Color.h"

struct StrokeMaterial {
	float thickness;
	color stroke;
};

struct RegularPolygon {
	int sideCount;
	float radius;
	ID material;
};

class RegularPolygonManager : public IManager {
private:
	struct RegularPolygonSlot {
		ID node;
		RegularPolygon polygon;
	};

	CompactPool<StrokeMaterial, 256> mMaterials;
	CompactComponentPool<RegularPolygonSlot> mPolygons;

public:

	void Initialize();
	void Destroy();

	bool CreateComponent(ID node);
	bool DestroyComponent(ID node);
	
	void Update(RenderBuffer *vbuf);
	void Render(RenderBuffer *vbuf);
};
