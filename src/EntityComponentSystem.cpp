#include "EntityComponentSystem.h"
#include <vector>
#include <iostream>
/*
A mind-numbingly stupid implementation just to get the wheels turning.
*/

namespace EntityComponentSystem {

struct EntityComponentRecord {
	EntityHandle e;
	ComponentType t;
	ComponentID c;
};

static std::vector<ISystem*> gSystems;
static std::vector<EntityComponentRecord> gDatabase;

ComponentType RegisterSystem(ISystem *s) {
	gSystems.push_back(s);
	return gSystems.size();
}

EntityHandle CreateEntity() {
	static EntityHandle mNext = 0;
	// ASSERT(mNext != 0xffffffff);
	return ++mNext;
}

ComponentID AddComponent(EntityHandle e, ComponentType t) {
	ComponentID result = gSystems[t-1]->CreateComponent();
	if (result) {
		EntityComponentRecord record;
		record.e = e;
		record.t = t;
		record.c = result;
		gDatabase.push_back(record);
	}
	return result;
}

ComponentID GetComponent(EntityHandle e, ComponentType t) {
	for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
		if (p->e == e and p->t == t) {
			return p->c;
		}
	}
	return 0;
}

ComponentIterator::ComponentIterator(EntityHandle e) : e(e), i(0) {
}

bool ComponentIterator::Next(ComponentType* outType, ComponentID* outID) {
	while(i++ < gDatabase.size()) {
		if (gDatabase[i-1].e == e) {
			*outType = gDatabase[i-1].t;
			*outID = gDatabase[i-1].c;
			return true;
		}
	}
	return false;
}

void DestroyComponent(ComponentType t, ComponentID id) {
	gSystems[t-1]->DestroyComponent(id);
	for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
		if (p->t == t && p->c == id) {
			gDatabase.erase(p);
			break;
		}
	}
}

void DestroyEntity(EntityHandle e) {
	ComponentIterator i(e);
	ComponentType t;
	ComponentID id;
	while(i.Next(&t, &id)) {
		DestroyComponent(t, id);
	}
}

}
