#pragma once
#include <limits.h>
#include <stdint.h>

typedef uint32_t ID;

struct RenderBuffer;

// Interface for component systems
struct IManager {
	// SCENE THREAD
	virtual bool CreateComponent(ID node) = 0;
	virtual bool DestroyComponent(ID node) = 0;
	virtual void Update(RenderBuffer *vbuf) = 0;

	// RENDER THREAD
	virtual void Render(RenderBuffer *vbuf) = 0;
};
