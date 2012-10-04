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

