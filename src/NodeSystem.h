// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include "Math.h"
#include "RenderSystem.h"

typedef ID NodeID;
typedef ID CompID;

namespace NodeSystem {

//------------------------------------------------------------------------------
// SCENE SYSTEM
//
// The core interface for composing a scene from a node heirarchy
// and attaching components to nodes.  Nodes themselves are refered
// to using plain-old numeric handles.  As with pointers, "0" indicates
// a "NULL" node, for the purpose of conditional testing.
//
// Components themselves are completely managed in separate systems.  
// The NodeSystem only manages the rapid manipulation of the scene
// and efficient computation of a worldspace-transform_t buffer for 
// the RenderSystem.
//
// The NodeSystem itself performs no dynamic memory allocation.  All
// data records are preallocated in self-defragmenting pools, and should
// be considered cache-friendly and concurrency-friendly.
//------------------------------------------------------------------------------

// Initialize the scene system
void Initialize();
void Destroy();

// Is this ID defined for the current scene?
bool NodeValid(NodeID id);

// How many nodes are currently active in the scene?
int NodeCount();

// Create a new node, potentially rooted to another node and
// retrieve it's handle.
ID CreateNode(NodeID parent=0, vec2_t position=Vec2(0,0), vec2_t attitude=Vec2(1,0), float depth=0.f);

// Attach a node as a child of another node
void AttachNode(NodeID parent, NodeID child);

// Detach a node from it's parent
void DetachNode(NodeID child);

// Lookup a node's parent
NodeID Parent(NodeID node);

// Iterate over a node's children
struct ChildIterator {
	NodeID current;
	ChildIterator(NodeID node);
	bool Next(NodeID *outNode);
};

// Lookup a node's local-to-parent transform_t
transform_t& LocalToParent(NodeID node);

uint16_t Index(NodeID node);

// For mid-frame one-shots -- the RenderQueue will get this batched.
transform_t LocalToWorld(NodeID node);

// write render state to vbuf
void Update(CommandBuffer *buf);

// Add a component to a node.  Each node can have multiple components,
// but only one component of a given type.
void AddComponent(NodeID node, CompID componentType);

// Check whether this node has a component of that type
bool HasComponent(NodeID node, CompID componentType);

// Remove a component from a node.  Components cannot be reparented like
// nodes, only destroyed
void RemoveComponent(NodeID node, CompID componentType);

// Iterate over the components attached to a given node
struct ComponentIterator {
	uint32_t mask;
	ComponentIterator(NodeID node);
	bool Next(CompID *outComponentType);
};

// Recursively destroy a node's children, then the node's
// components, and then the node itself.
void DestroyNode(NodeID node);

// Helper Methods
inline vec2_t Position(NodeID node) { 
	return LocalToParent(node).translation; 
}

inline float Rotation(NodeID node) { 
	return LocalToParent(node).attitude.Radians(); 
}

inline float Depth(NodeID node) {
	return LocalToParent(node).depth;
}

inline vec2_t Direction(NodeID node) { 
	return LocalToParent(node).attitude; 
}

inline void SetPosition(NodeID node, vec2_t p) { 
	LocalToParent(node).translation = p; 
}

inline void SetRotation(NodeID node, float angle) { 
	LocalToParent(node).attitude = Polar(1.f, angle); 
}

inline void SetDirection(NodeID node, vec2_t d) { 
	LocalToParent(node).attitude = d; 
}

inline void SetDepth(NodeID node, float d) {
	LocalToParent(node).depth = d;
}

//------------------------------------------------------------------------------
// TODO
// Worldspace Reparenting
// Serialization of Node Manager and Component Managers
//------------------------------------------------------------------------------

}

