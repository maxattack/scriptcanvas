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
#include "util/CompactPool.h"

struct MaterialSlot {
	int refCount;
	Material mat;
};

static CompactPool<MaterialSlot, kMaxMaterials> mMaterials;

// TODO: Handle RefCounts

void MaterialSystem::Initialize() {
	// NOOP
}

void MaterialSystem::Destroy() {
	// NOOP
}

void MaterialSystem::Update(CommandBuffer *vbuf) {
	int i=0;
	for(auto p=mMaterials.Begin(); p!=mMaterials.End(); ++p) {
		vbuf->materials[i++] = p->mat;
	}
}

ID MaterialSystem::CreateMaterial(color_t color, float weight) {
	ID result = mMaterials.TakeOut();
	mMaterials[result].refCount = 0;
	mMaterials[result].mat.weight = weight;
	mMaterials[result].mat.color = color;
	return result;

}

bool MaterialSystem::MaterialValid(ID mid) {
	return mMaterials.IsActive(mid);
}

Material& MaterialSystem::GetMaterial(ID mid) {
	return mMaterials[mid].mat;
}

uint16_t MaterialSystem::Index(ID mid) {
	return mMaterials.Index(mid);
}

void MaterialSystem::DestroyMaterial(ID mid) {
	ASSERT(mMaterials[mid].refCount == 0);
	mMaterials.PutBack(mid);
}

