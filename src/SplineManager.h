#pragma once
#include "SceneSystem.h"
#include "util/CompactPool.h"
#include "util/CompactComponentPool.h"
#include "Color.h"

struct Material {
	float weight;
	color_t color;
};


struct Segment {
	ID start;
	ID end;
	ID material;
};

struct SegmentCommand {
	uint16_t queue;
	uint16_t material;
	uint16_t start;
	uint16_t end;
};

class SplineManager : public IManager {
private:
	struct ControlVertexSlot {
		ID node;
		int refCount;
	};

	struct MaterialSlot {
		Material mat;
		int refCount;
	};

	GLuint mProgram;
	GLuint mAttribParameterAndSide;
    GLuint mUniformThickness;
    GLuint mUniformPositionMatrix;
    GLuint mUniformNormalMatrix;
    GLuint mUniformColor;
    GLuint mVertexBuffer;

	CompactComponentPool<ControlVertexSlot> mComponents;
	CompactPool<MaterialSlot, kMaxMaterials> mMaterials;
	CompactPool<Segment, kMaxSegments> mSegments;

public:
	StatusCode Initialize();

	StatusCode CreateComponent(ID node);
	StatusCode DestroyComponent(ID node);

	ID CreateMaterial(float weight=8.f, color_t color=ARGB(0xffffffff));
	Material& GetMaterial(ID mid);
	Material GetMaterial(ID mid) const;
	void DestroyMaterial(ID mid);

	ID CreateSegment(ID start, ID end, ID mid=0);
	void SetMaterial(ID sid, ID mid);
	void DestroySegment(ID sid);

	void CleanUpMaterials();

	StatusCode Update(RenderBuffer *vbuf);
	StatusCode Render(RenderBuffer *vbuf);

	StatusCode Destroy();

};
