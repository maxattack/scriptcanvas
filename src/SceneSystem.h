#pragma once
#include "BaseSystem.h"
#include "math/transform.h"

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
// and efficient computation of a worldspace-transform buffer for 
// the RenderSystem.
//
// The SceneSystem itself performs no dynamic memory allocation.  All
// data records are preallocated in self-defragmenting pools, and should
// be considered cache-friendly and concurrency-friendly.
//------------------------------------------------------------------------------

// Constants
#define MAX_NODES 				1024
#define MAX_COMPONENT_TYPES		32

// Initialize the scene system
void Initialize();

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

// Lookup a node's local-to-parent transform
transform& LocalToParent(ID node);

int Index(ID node);

// For mid-frame one-shots -- the RenderQueue will get this batched.
transform LocalToWorld(ID node);

void Update(RenderBuffer *vbuf);

// Register a component system with the scene system (belongs in basesystem?)
void RegisterComponentManager(ID componentType, IManager* pMgr);

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

//------------------------------------------------------------------------------
// TODO
// Worldspace Reparenting
// Serialization of Node Manager and Component Managers
// Batch Render transformation
// Scripting Interface / Component Retrieval
//------------------------------------------------------------------------------

}

