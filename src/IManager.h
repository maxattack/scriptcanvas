#pragma once
#include "Foundation.h"

struct RenderBuffer;

struct IManager {

	// SCENE THREAD
	virtual StatusCode CreateComponent(ID node) = 0;
	virtual StatusCode DestroyComponent(ID node) = 0;
	virtual StatusCode Update(RenderBuffer *vbuf) = 0;

	// RENDER THREAD
	virtual StatusCode Initialize();
	virtual StatusCode Render(RenderBuffer *vbuf) = 0;
	virtual StatusCode Destroy();

};
