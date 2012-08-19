#pragma once
#include "SceneSystem.h"
#include "RenderSystem.h"

struct Node;

union ptr_as_id {
	Node* p;
	ID h;
	ptr_as_id(Node* ptr) : p(ptr) {}
	ptr_as_id(ID id) : h(id) {}
};

inline Node* ToNode(ID node) {
	return ((ptr_as_id)node).p;
}

RenderSystem::CircleSystem* GetCircleSystem();

struct Node {
	ID id() { return ptr_as_id(this).h; }
	void attach(Node child) { SceneSystem::AttachNode(id(), child.id()); }
	void detach() { SceneSystem::DetachNode(id()); }
	Node* parent() { return reinterpret_cast<Node*>(SceneSystem::Parent(id())); }
	void add_component(ID componentType) { SceneSystem::AddComponent(id(), componentType); }
	void remove_component(ID componentType) { SceneSystem::RemoveComponent(id(), componentType); }
	void destroy() { SceneSystem::DestroyNode(id()); }

	void set_position(float x, float y) { 
		SceneSystem::Pose(id()).t = vec(x, y); 
	}

	void get_position(float* x, float* y) { 
		Vec t = SceneSystem::Pose(id()).t;
		*x = t.x;
		*y = t.y;
	}
	void set_rotation(float radians) { SceneSystem::Pose(id()).q = Polar(1.f, radians); }

	void init_circle(float radius, float r, float g, float b) { 
		(*GetCircleSystem())[id()].Init(radius, r, g, b); 
	}
};

inline Node* create_node(Node* parent=0) { 
	return ToNode( SceneSystem::CreateNode(ptr_as_id(parent).h) );
}

