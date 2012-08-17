#pragma once
#include <stdint.h>
#include "math/Transform.h"

namespace SceneSystem {
   
/*
	Definitions of the basic numeric ComponentID types.
	The only requirement is that these handles match 
	Lua's native word size so they can be passed around
	inexpensively in scripts.
	
	Right now the TypeID is encoded in the MSB of the 
	ComponentID, requiring CIDs to only be 24-bit.
*/

typedef uint32_t EntityID;
typedef uint32_t ComponentID;
typedef uint32_t TypeID;

/*
	Abstract interface for batch-processing Component Factories.
*/

class IComponentFactory {
public:
	virtual ComponentID CreateComponent() = 0;
	virtual void DestroyComponent(ComponentID i) = 0;
};

/*
	Core SceneSystem Functions
*/

TypeID RegisterComponentType(IComponentFactory *s);

EntityID CreateEntity(EntityID parent=0);
EntityID GetParent(EntityID e);
void AttachChild(EntityID e, EntityID child);
void DetachFromParent(EntityID e);
void ComputeWorldTransforms();

Transform& GetLocalTransform(EntityID e);
Transform GetWorldTransform(EntityID e);

TypeID GetType(ComponentID c);
ComponentID GetComponent(EntityID e, TypeID t);
ComponentID AddComponent(EntityID e, TypeID t);

void DestroyComponent(ComponentID id);
void DestroyEntity(EntityID e);

/*
	Iterators
	
	Kind of annoying that I have to expose some internal implementation
	details here.  On the other hand... is there any other way to implement
	this efficiently?
*/

class EntityIterator {
private:
	int32_t i;
public:
	EntityIterator();
	bool Next(EntityID *outID);
};

class EntityChildIterator {
private:
	EntityID e;
public:
	EntityChildIterator(EntityID parent, bool recursive=false);
	bool Next(EntityID* outID);	
};

class ComponentIterator {
private:
	EntityID e;
	int32_t i;
public:
	ComponentIterator(EntityID e);
	bool Next(ComponentID* outID);
};

/*
	Entity status?  Uninitialized | Awake | Sleeping | Destroyed?
	Serialization?
*/

}


/*
Example C++ Class:

class Ogre {
private:
    EntityID hEntity;
    ComponentID hPhys;
    ComponentID hRend;
    ComponentID hOgre;
    
public:
    Ogre() {
        hEntity = CreateEntity();
        hPhys = AddComponent(hEntity, kPhysics);
        hRend = AddComponent(hEntity, kRendering);
        hOgre = AddComponent(hEntity, kOgre);
    }

    ~Ogre() {
        DestroyEntity(hEntity);
    }
    
    PhysComponent& Phys() { return PhysicsSystem::GetComponent(hPhys); }
    RendComponent& Gfx() { return RenderSystem::GetComponent(hRend); }
    OgreComponent& Ogre() { return OgreSystem::GetComponent(hOgre); }
    
};

Example Lua Script:

function CreateOgre() 
    result = CreateEntity()
    result:AddComponent(Physics)
    result:AddComponent(Rendering)
    result:AddComponent(Ogre)
    return result
end

*/