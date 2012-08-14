#pragma once
#include <stdint.h>

namespace EntityComponentSystem {
    
    /*
    Definitions of the basic numeric ComponentID types.  The only
    functional requirements is that these handles match 
    Lua's native word size so they can be passed around
    inexpensively in scripts.
    */
    
    typedef uint32_t EntityID;
    typedef uint32_t ComponentID;
    typedef uint8_t TypeID;

    // Should we roll "Type" and "ID" into one word?  E.g. 8-bit type, 24-bit ID.
    // Would require modifying the object pools to ignore the most significant byte.

    /*
    Systems are "Batch Operators" which do things like graphics,
    physics, AI, etc.  The only requirement is that they are able
    to assign valid database ComponentIDs to their components.
    
    I'm actually not too keen on this interface, but I figure I'll
    implement it and see how it goes.
    */

    class ISystem {
    public:
        virtual ComponentID CreateComponent() = 0;
        virtual void DestroyComponent(ComponentID i) = 0;
    };

    TypeID RegisterComponentType(ISystem *s);

    /*
    All the basic interface for creating logic entities, attaching components,
    querying components, and destroying things.
    
    Typically, scripts will invoke these functions "like methods", so the
    first parameter should be the object's "self" ref.  
    
    E.g.: someEntity:GetComponent(Physics)
    */
    
    EntityID CreateEntity();
    TypeID GetType(ComponentID c);

    ComponentID AddComponent(EntityID e, TypeID t);
    ComponentID GetComponent(EntityID e, TypeID t);

    class ComponentIterator {
    private:
        EntityID e;
        unsigned i;
    public:
        ComponentIterator(EntityID e);
        bool Next(ComponentID* outID);
    };

    void DestroyComponent(ComponentID id);
    void DestroyEntity(EntityID e);

    /*
    EntityIterators?
    Scene Queries?
    Heirarchical entity relationships?
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