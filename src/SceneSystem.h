#pragma once
#include "Math.h"
#include "RenderSystem.h"

namespace SceneSystem {

//------------------------------------------------------------------------------
// SCENE SYSTEM
//
// The core interface for composing a scene from a node heirarchy
// and attaching components to nodes.  Nodes themselves are refered
// to using plain-old numeric handles.  As with pointers, "0" indicates
// a "NULL" node, for the purpose of conditional testing.
//
// Components themselves are completely managed in separate systems.  
// The SceneSystem only manages the rapid manipulation of the scene
// and efficient computation of a worldspace-transform_t buffer for 
// the RenderSystem.
//
// The SceneSystem itself performs no dynamic memory allocation.  All
// data records are preallocated in self-defragmenting pools, and should
// be considered cache-friendly and concurrency-friendly.
//------------------------------------------------------------------------------

// Initialize the scene system
void Initialize();
void Destroy();

// Is this ID defined for the current scene?
bool NodeValid(ID id);

// How many nodes are currently active in the scene?
int NodeCount();

// Create a new node, potentially rooted to another node and
// retrieve it's handle.
ID CreateNode(ID parent=0);

// Attach a node as a child of another node
void AttachNode(ID parent, ID child);

// Detach a node from it's parent
void DetachNode(ID child);

// Lookup a node's parent
ID Parent(ID node);

// Iterate over a node's children
struct ChildIterator {
	ID current;
	ChildIterator(ID node);
	bool Next(ID *outNode);
};

// Lookup a node's local-to-parent transform_t
ztransform_t& LocalToParent(ID node);

uint16_t Index(ID node);

// For mid-frame one-shots -- the RenderQueue will get this batched.
ztransform_t LocalToWorld(ID node);

// write render state to vbuf
void Update(CommandBuffer *buf);

// Add a component to a node.  Each node can have multiple components,
// but only one component of a given type.
void AddComponent(ID node, ID componentType);

// Check whether this node has a component of that type
bool HasComponent(ID node, ID componentType);

// Remove a component from a node.  Components cannot be reparented like
// nodes, only destroyed
void RemoveComponent(ID node, ID componentType);

// Iterate over the components attached to a given node
struct ComponentIterator {
	uint32_t mask;
	ComponentIterator(ID node);
	bool Next(ID *outComponentType);
};

// Recursively destroy a node's children, then the node's
// components, and then the node itself.
void DestroyNode(ID node);

// Helper Methods
inline float2_t Position(ID node) { 
	return LocalToParent(node).t.t; 
}

inline float Rotation(ID node) { 
	return LocalToParent(node).t.q.Radians(); 
}

inline float2_t Direction(ID node) { 
	return LocalToParent(node).t.q; 
}

inline void SetPosition(ID node, float2_t p) { 
	LocalToParent(node).t.t = p; 
}

inline void SetRotation(ID node, float angle) { 
	LocalToParent(node).t.q = Polar(1.f, angle); 
}

inline void SetDirection(ID node, float2_t d) { 
	LocalToParent(node).t.q = d; 
}

//------------------------------------------------------------------------------
// TODO
// Worldspace Reparenting
// Serialization of Node Manager and Component Managers
// Scripting Interface / Component Retrieval
//------------------------------------------------------------------------------

}

