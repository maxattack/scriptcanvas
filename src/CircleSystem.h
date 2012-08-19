#pragma once
#include <GL/glfw.h>
#include "util/SceneUtil.h"

struct Circle {
	ID node;
	float radius;
	float r, g, b;

	void Init(float aradius, float ar, float ag, float ab) { 
		radius = aradius;
		r=ar; 
		g=ag; 
		b=ab; 
	}
};

class CircleSystem : public IComponentManager {
private:
	GLuint mProgram;
	GLuint mAttribUnit;
	GLuint mUniformRadius;
	GLuint mUniformColor;
	GLuint mVertexBuffer;
	CompactComponentPool<Circle> mData;

public:
	void Init();
	void Render();
	void Dispose();

	void CreateComponent(ID node);
	void DestroyComponent(ID node);
	Circle& operator[](ID node);
};
