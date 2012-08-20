#pragma once
#include "SceneSystem.h"
#include "CircleSystem.h"

struct Node;

union ptr_as_id {
	Node* p;
	ID h;
	ptr_as_id(Node* ptr) : p(ptr) {}
	ptr_as_id(ID id) : p(0) { h=id; }
};

inline Node* ToNode(ID node) {
	return ptr_as_id(node).p;
}

struct Node {
	ID id() { return ptr_as_id(this).h; }
	void attach(Node child) { AttachNode(id(), child.id()); }
	void detach() { DetachNode(id()); }
	Node* parent() { return reinterpret_cast<Node*>(Parent(id())); }
	void add_component(ID componentType) { AddComponent(id(), componentType); }
	void remove_component(ID componentType) { RemoveComponent(id(), componentType); }
	void destroy() { DestroyNode(id()); }

	void set_position(float x, float y) { 
		Pose(id()).t = vec(x, y); 
	}

	void get_position(float* x, float* y) { 
		Vec t = Pose(id()).t;
		*x = t.x;
		*y = t.y;
	}
	void set_rotation(float radians) { Pose(id()).q = Polar(1.f, radians); }
};

inline Node* create_node(Node* parent=0) { 
	return ToNode( CreateNode(ptr_as_id(parent).h) );
}

