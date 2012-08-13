#include "EntityComponentSystem.h"
#include <vector>

/*
A mind-numbingly stupid implementation just to get the wheels turning.
*/

namespace EntityComponentSystem {

struct EntityComponentRecord {
	EntityHandle e;
	ComponentHandle c;
};

static std::vector<ISystem*> gSystems;
static std::vector<EntityComponentRecord> gDatabase;

SystemType RegisterSystem(ISystem *s) {
	gSystems.push_back(s);
	return gSystems.size() - 1;
}

EntityHandle CreateEntity() {
	static EntityHandle mNext = 0;
	// ASSERT(mNext != 0xffffffff);
	return ++mNext;
}

ComponentHandle AddComponent(EntityHandle e, SystemType type) {
	unsigned result = gSystems[type]->AddComponent();
	if (result) {
		result |= (type<<24);
		EntityComponentRecord record;
		record.e = e;
		record.c = result;
		gDatabase.push_back(record);
	}
	return result;
}

SystemType GetType(ComponentHandle c) { 
	return c >> 24; 
}

ID GetID(ComponentHandle c) {
	return c & 0x00ffffff;
}


ComponentHandle GetComponent(EntityHandle e, SystemType t) {
	for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
		if (p->e == e and GetType(p->c) == t) {
			return p->c;
		}
	}
	return 0;
}

ComponentIterator::ComponentIterator(EntityHandle e) : e(e), i(0) {
}

ComponentHandle ComponentIterator::Next() {
	while(i++ < gDatabase.size()) {
		if (gDatabase[i-1].e == e) {
			return gDatabase[i-1].c;
		}
	}
	return 0;
}


void DestroyComponent(ComponentHandle c) {
	gSystems[GetType(c)]->DestroyComponent(GetID(c));
	for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
		if (p->c == c) {
			gDatabase.erase(p);
			break;
		}
	}
}

void DestroyEntity(EntityHandle e) {
	ComponentIterator i(e);
	ComponentHandle c;
	while((c = i.Next())) {
		DestroyComponent(c);
	}
}

}
