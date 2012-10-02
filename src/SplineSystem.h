#pragma once
#include "Foundation.h"
#include "MaterialSystem.h"

// Does it make sense to have a concept of a generic segment,
// and then to endow it with linear, cubic, or eccentric nature?
// It would reduce the number of syscalls...

struct SegmentCommand {
	uint16_t queue;
	uint16_t mid;
	uint16_t start;
	uint16_t end;
	float taperStart;
	float taperDelta;
};

struct EccentricSegmentCommand : SegmentCommand {
	float eccentricity;
};

struct Segment {
	ID start;
	ID end;
	ID material;
};

struct EccentricSegment : Segment {
	float eccentricity;
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

ControlVertex& GetControlVertex(ID node);
void OnNodeDestroyed(ID node);

inline float Taper(ID node) { return GetControlVertex(node).taper; }
inline void SetTaper(ID node, float taper) { GetControlVertex(node).taper = taper; }

ID CreateCubicSegment(ID start, ID end, ID mat);
void SetCubicSegmentMaterial(ID csid, ID mid);
Segment GetCubicSegment(ID csid);
void DestroyCubicSegment(ID csid);

inline ID CubicSegmentStart(ID csid) { return GetCubicSegment(csid).start; }
inline ID CubicSegmentEnd(ID csid) { return GetCubicSegment(csid).end; }
inline ID CubicSegmentMaterialID(ID csid) { return GetCubicSegment(csid).material; }
inline Material& CubicSegmentMaterial(ID csid) { return MaterialSystem::GetMaterial(GetCubicSegment(csid).material); }

ID CreateEccentricSegment(ID start, ID end, ID mat, float eccentricity=0.f);
void SetEccentricSegmentMaterial(ID esid, ID mid);
float Eccentricity(ID esid);
void SetEccentricity(ID esid, float e);
void DestroyEccentricSegment(ID esid);

EccentricSegment GetEccentricSegment(ID esid);

inline ID EccentricSegmentStart(ID esid) { return GetEccentricSegment(esid).start; }
inline ID EccentricSegmentEnd(ID esid) { return GetEccentricSegment(esid).end; }
inline ID EccentricSegmentMaterialID(ID esid) { return GetEccentricSegment(esid).material; }
inline Material& EccentricSegmentMaterial(ID esid) { return MaterialSystem::GetMaterial(GetEccentricSegment(esid).material); }




}
