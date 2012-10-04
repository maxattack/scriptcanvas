#pragma once
#include "Foundation.h"

struct Stroke {
	float start;
	float eccentricity;
	float end;
};

namespace StrokeSystem {

void Initialize();
void Destroy();

void Update(CommandBuffer *vbuf);

ID CreateStroke(float start, float end, float eccen=0.f);
bool StrokeValid(ID sid);
Stroke& GetStroke(ID strokeId);
uint16_t Index(ID strokeId);
void DestroyStroke(ID strokeId);


inline ID CreateUniformStroke(float uniform) { return CreateStroke(uniform, uniform); }
inline ID CreateEccentricStroke(float uniform, float eccen) { return CreateStroke(uniform, uniform, eccen); }

inline float Start(ID strokeId) { return GetStroke(strokeId).start; }
inline float End(ID strokeId) { return GetStroke(strokeId).end; }
inline float Eccentricity(ID strokeId) { return GetStroke(strokeId).eccentricity; }
inline void SetStart(ID strokeId, float s) { GetStroke(strokeId).start = s; }
inline void SetEnd(ID strokeId, float e) { GetStroke(strokeId).end = e; }
inline void SetEccentricity(ID strokeId, float e) { GetStroke(strokeId).eccentricity = e; }



}
