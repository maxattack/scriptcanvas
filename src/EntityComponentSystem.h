#pragma once
#include <stdint.h>

namespace EntityComponentSystem {
	
	/*
	Definitions of the basic numeric ComponentID types.  The only
	functional requirements is that these handles match 
	Lua's native word size so they can be passed around
	inexpensively in scripts.
	*/
	
	typedef uint32_t EntityHandle;
	typedef uint32_t ComponentType;
	typedef uint32_t ComponentID;

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

	ComponentType RegisterSystem(ISystem *s);

	/*
	All the basic interface for creating logic entities, attaching components,
	querying components, and destroying things.
	
	Typically, scripts will invoke these functions "like methods", so the
	first parameter should be the object's "self" ref.  
	
	E.g.: someEntity:GetComponent(Physics)
	*/
	
	EntityHandle CreateEntity();

	ComponentID AddComponent(EntityHandle e, ComponentType t);
	ComponentID GetComponent(EntityHandle e, ComponentType t);

	class ComponentIterator {
	private:
		EntityHandle e;
		unsigned i;
	public:
		ComponentIterator(EntityHandle e);
		bool Next(ComponentType* outType, ComponentID* outID);
	};

	void DestroyComponent(ComponentType t, ComponentID id);
	void DestroyEntity(EntityHandle e);

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
	EntityHandle hEntity;
	ComponentHandle hPhys;
	ComponentHandle hRend;
	ComponentHandle hOgre;
	
public:
	Ogre() {
		hEntity = CreateEntity();
		// Precache component ComponentIDs, to avoid per-frame lookups
		hPhys = AddComponent(hEntity, hPhysics);
		hRend = AddComponent(hEntity, hRendering);
		hOgre = AddComponent(hEntity, hOgre);
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