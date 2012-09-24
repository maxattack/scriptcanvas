#pragma once
#include "SceneSystem.h"
#include "util/CompactComponentPool.h"

class NameManager : IManager {
private:
	struct NameSlot {
		ID node;
		const char* name;
	};
	CompactComponentPool<NameSlot> mNames;
	// std::map<std::string, ID> mHash; ?
	
public:
	StatusCode Initialize();
	StatusCode Destroy();

	StatusCode CreateComponent(ID node);
	StatusCode DestroyComponent(ID node);
	
	const char* NodeName(ID node);
	ID FindNode(const char* name);

	StatusCode Update(RenderBuffer *vbuf) {}
	StatusCode Render(RenderBuffer *vbuf) {}

};