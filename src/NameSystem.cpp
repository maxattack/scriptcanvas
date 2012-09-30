#include "CommandSystem.h"
#include "util/CompactComponentPool.h"
#include <map>
#include <string>

struct Name {
	char name[kMaxNameLength];
};

CompactComponentPool<Name> mNames;
std::map<std::string, ID> mDict; 	// TODO: Replace with something ninja?

void NameSystem::Initialize() {
	// noop
}
void NameSystem::Destroy() {
	mDict.clear();
}

void NameSystem::SetName(ID node, std::string name) {
	ASSERT(name.size() < kMaxNameLength);
	SceneSystem::AddComponent(node, kComponentName);
	mNames.Alloc(node);
	strcpy(mNames[node].name,  name.c_str());
	mDict[name] = node;
}

void NameSystem::ClearName(ID node) {
	SceneSystem::RemoveComponent(node, kComponentName);
}

void NameSystem::OnNodeDestroyed(ID node) {
	mDict.erase(mNames[node].name);
	mNames.Free(node);
}

std::string NameSystem::Name(ID node) {
	return mNames[node].name;
}

ID NameSystem::FindNode(std::string name) {
	auto result = mDict.find(name);
	return result == mDict.end() ? 0 : result->second;
}
