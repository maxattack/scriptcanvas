#pragma once
#include "Foundation.h"
#include "Color.h"

struct HermiteSegmentCommand {
	uint16_t queue;
	uint16_t material;
	uint16_t start;
	uint16_t end;
	float taperStart;
	float taperDelta;
};

struct Material {
	float weight;
	color_t color;
};

struct HermiteSegment {
	ID start;
	ID end;
	ID material;
};

struct ControlVertex {
	int refCount;
	float taper;
};

namespace SplineSystem {

void Initialize();
void Destroy();

void Update(CommandBuffer *vbuf);
void Render(CommandBuffer *vbuf);


ID CreateMaterial(float weight=1.f, color_t color=Color());
Material& GetMaterial(ID mid);
void DestroyMaterial(ID mid);

ID CreateSegment(ID start, ID end, ID mat);
void SetSegmentMaterial(ID sid, ID mid);
HermiteSegment GetSegment(ID sid);
void DestroySegment(ID sid);

void OnNodeDestroyed(ID node);

// Helper Functions

ControlVertex& GetControlVertex(ID node);

inline float Weight(ID mat) { return GetMaterial(mat).weight; }
inline color_t Color(ID mat) { return GetMaterial(mat).color; }

inline void SetWeight(ID mat, float weight) { GetMaterial(mat).weight = weight; }
inline void SetColor(ID mat, color_t color) { GetMaterial(mat).color = color; }

inline ID Start(ID sid) { return GetSegment(sid).start; }
inline ID End(ID sid) { return GetSegment(sid).end; }
inline ID SegmentMaterialID(ID sid) { return GetSegment(sid).material; }
inline Material& SegmentMaterial(ID sid) { return GetMaterial(GetSegment(sid).material); }

inline float Taper(ID node) { return GetControlVertex(node).taper; }
inline void SetTaper(ID node, float taper) { GetControlVertex(node).taper = taper; }

}
