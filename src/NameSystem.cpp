#include "CommandSystem.h"
#include "util/CompactComponentPool.h"
#include <map>
#include <string>

struct Name {
	const char* name;
};

CompactComponentPool<Name> mNames;
std::map<std::string, ID> mDict; 	// TODO: Replace with something ninja?

void NameSystem::Initialize() {
	// noop
}
void NameSystem::Destroy() {
	mDict.clear();
}

void NameSystem::SetName(ID node, const char* name) {
	SceneSystem::AddComponent(node, kComponentName);
	mNames.Alloc(node);
	mNames[node].name = name;
	mDict[name] = node;
}

void NameSystem::ClearName(ID node) {
	SceneSystem::RemoveComponent(node, kComponentName);
}

void NameSystem::OnNodeDestroyed(ID node) {
	mDict.erase(mNames[node].name);
	mNames.Free(node);
}

const char* NameSystem::Name(ID node) {
	return mNames[node].name;
}

ID NameSystem::FindNode(const char *name) {
	auto result = mDict.find(name);
	return result == mDict.end() ? 0 : result->second;
}
