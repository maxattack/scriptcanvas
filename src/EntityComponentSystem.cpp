#include "EntityComponentSystem.h"
#include "util/Macros.h"
#include <vector>
#include <iostream>
/*
A mind-numbingly stupid implementation just to get the wheels turning.
*/

namespace EntityComponentSystem {

struct EntityComponentRecord {
    EntityID e;
    ComponentID c;
};

static std::vector<ISystem*> gSystems;
static std::vector<EntityComponentRecord> gDatabase;

TypeID RegisterSystem(ISystem *s) {
    ASSERT(gSystems.size() < 254);
    gSystems.push_back(s);
    return gSystems.size();
}

EntityID CreateEntity() {
    static EntityID mNext = 0;
    ASSERT(mNext != 0xffffffff);
    return ++mNext;
}

ComponentID AddComponent(EntityID e, TypeID t) {
    ComponentID result = gSystems[t-1]->CreateComponent();
    if (result) {
        ASSERT(GetType(result) == 0);
        result |= t << 24;
        EntityComponentRecord record = { e, result };
        gDatabase.push_back(record);
    }
    return result;
}

ComponentID GetComponent(EntityID e, TypeID t) {
    for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
        if (p->e == e and GetType(p->c) == t) {
            return p->c;
        }
    }
    return 0;
}

TypeID GetType(ComponentID c) {
    return c >> 24;
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

void DestroyComponent(ComponentID id) {
    gSystems[GetType(id)-1]->DestroyComponent(id);
    for(auto p=gDatabase.begin(); p!=gDatabase.end(); ++p) {
        if (p->c == id) {
            gDatabase.erase(p);
            break;
        }
    }
}

void DestroyEntity(EntityID e) {
    ComponentIterator i(e);
    ComponentID id;
    while(i.Next(&id)) {
        DestroyComponent(id);
    }
}

}
