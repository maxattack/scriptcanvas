#include "SceneSystem.h"
#include "util/Macros.h"
#include "util/CompactPool.h"
#include <vector>
#include <iostream>

/*
A mind-numbingly stupid implementation just to get the wheels turning.
*/

namespace SceneSystem {

struct EntityComponentRecord {
    EntityID e;
    ComponentID c;
};

// Do not want this to really be object-oriented.
// This record should be broken into separate arrays
struct EntityRecord {
	EntityID parent;
	EntityID firstChild;
	EntityID nextSibling;
	EntityID prevSibling;
	Transform localToParent;
};

static std::vector<IComponentFactory*> gFactories;
static std::vector<EntityComponentRecord> gDatabase;
static StaticCompactPool<EntityRecord, 256> gEntities;

TypeID RegisterComponentType(IComponentFactory *s) {
    ASSERT(gFactories.size() < 254);
    gFactories.push_back(s);
    return gFactories.size();
}

EntityID CreateEntity(EntityID parent) {
    auto result = gEntities.TakeOut();
	auto& rec = gEntities[result];
	rec.parent = 0;
	rec.firstChild = 0;
	rec.nextSibling = 0;
	rec.prevSibling = 0;
	rec.localToParent = Transform::Identity();
	if (parent) {
		AttachChild(parent, result);
	}
	return result;
}

EntityID GetParent(EntityID e) {
	return gEntities[e].parent;
}

void AttachChild(EntityID e, EntityID child) {
	DetachFromParent(child);
	auto& childRec = gEntities[child];
	if (childRec.parent) {
		if (childRec.parent == e) { 
			return; 
		} else {
			DetachFromParent(child);
		}
	}
	auto& rec = gEntities[e];
	childRec.nextSibling = rec.firstChild;
	rec.firstChild = child;
	if (childRec.nextSibling) {
		gEntities[childRec.nextSibling].prevSibling = child;
	}
}

void DetachFromParent(EntityID e) {
	auto& rec = gEntities[e];
	if (rec.parent) {
		auto& parent = gEntities[rec.parent];
		if (parent.firstChild == e) {
			parent.firstChild = rec.nextSibling;
		}
		if (rec.nextSibling) {
			gEntities[rec.nextSibling].prevSibling = rec.prevSibling;
		}
		if (rec.prevSibling) {
			gEntities[rec.prevSibling].nextSibling = rec.nextSibling;
		}
		rec.parent = 0;
		rec.prevSibling = 0;
		rec.nextSibling = 0;
	}
}

void ComputeWorldTransforms() {
	// Nothing doing -- all the transforms are lazy in this crappy impl
}

Transform& GetLocalTransform(EntityID e) {
	return gEntities[e].localToParent;
}

Transform GetWorldTransform(EntityID e) {
	// compute on demand -- would rather have a computed-in-one-pass buffer
	auto result = gEntities[e].localToParent;
	while(gEntities[e].parent) {
		e = gEntities[e].parent;
		result = gEntities[e].localToParent * result;
	}
	return result;
}

TypeID GetType(ComponentID c) {
    return c >> 24;
}

ComponentID GetComponent(EntityID e, TypeID t) {
    for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
        if (p->e == e and GetType(p->c) == t) {
            return p->c;
        }
    }
    return 0;
}

ComponentID AddComponent(EntityID e, TypeID t) {
    ComponentID result = gFactories[t-1]->CreateComponent();
    if (result) {
        ASSERT(GetType(result) == 0);
        result |= t << 24;
        EntityComponentRecord record = { e, result };
        gDatabase.push_back(record);
    }
    return result;
}

void DestroyComponent(ComponentID id) {
    gFactories[GetType(id)-1]->DestroyComponent(id);
    for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
        if (p->c == id) {
            gDatabase.erase(p);
            break;
        }
    }
}

void DestroyEntity(EntityID e) {
	DetachFromParent(e);
	while(gEntities[e].firstChild) {
		DestroyEntity(gEntities[e].firstChild);
	}
    ComponentIterator i(e);
    ComponentID id;
    while(i.Next(&id)) {
        DestroyComponent(id);
    }
}


ComponentIterator::ComponentIterator(EntityID e) : e(e), i(0) {
}

bool ComponentIterator::Next(ComponentID* outID) {
    while(i++ < gDatabase.size()) {
        if (gDatabase[i-1].e == e) {
            *outID = gDatabase[i-1].c;
            return true;
        }
    }
    return false;
}

EntityIterator::EntityIterator() : i(0) {
}

bool EntityIterator::Next(EntityID *outID) {
	if (i < gEntities.Count()) {
		*outID = gEntities.GetID(gEntities.Begin() + i);
		i++;
		return true;
	} else {
		return false;
	}
}

EntityChildIterator::EntityChildIterator(EntityID parent, bool recursive) : e(gEntities[parent].firstChild) {
	ASSERT(!recursive);
}

bool EntityChildIterator::Next(EntityID* outID) {
	if (e) {
		*outID = e;
		e = gEntities[e].nextSibling;
		return true;
	} else {
		return false;
	}
}


}
