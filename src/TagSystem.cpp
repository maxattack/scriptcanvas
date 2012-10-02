#include "CommandSystem.h"
#include "util/CompactComponentPool.h"
#include <map>
#include <string>

struct Name {
	char name[kMaxNameLength];
};

CompactComponentPool<Name> mNames;
std::map<std::string, ID> mDict; 	// TODO: Replace with something ninja?

void TagSystem::Initialize() {
	// noop
}
void TagSystem::Destroy() {
	mDict.clear();
}

void TagSystem::SetName(ID node, std::string name) {
	ASSERT(name.size() < kMaxNameLength);
	NodeSystem::AddComponent(node, kComponentName);
	mNames.Alloc(node);
	strcpy(mNames[node].name,  name.c_str());
	mDict[name] = node;
}

void TagSystem::ClearName(ID node) {
	NodeSystem::RemoveComponent(node, kComponentName);
}

void TagSystem::OnNodeDestroyed(ID node) {
	mDict.erase(mNames[node].name);
	mNames.Free(node);
}

std::string TagSystem::Name(ID node) {
	return mNames[node].name;
}

ID TagSystem::FindNode(std::string name) {
	auto result = mDict.find(name);
	return result == mDict.end() ? 0 : result->second;
}
