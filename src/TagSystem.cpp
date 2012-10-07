// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
