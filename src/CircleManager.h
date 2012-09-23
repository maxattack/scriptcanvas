#pragma once
#include "SceneSystem.h"
#include "util/CompactComponentPool.h"
#include "Color.h"

struct Circle {
	float radius;
	color_t fill;
};

struct CircleCommand {
	uint16_t queue;
	uint16_t transform;
	Circle properties;
};

class CircleManager : public IManager {
private:
	struct CircleSlot {
		ID node;
		Circle circle;
	};

	GLuint mProgram;
	GLuint mAttribUnit;
	GLuint mUniformRadius;
	GLuint mUniformColor;
	GLuint mVertexBuffer;
	CompactComponentPool<CircleSlot> mSlots;

public:

	StatusCode Initialize();
	StatusCode Destroy();

	StatusCode CreateComponent(ID node);
	StatusCode DestroyComponent(ID node);
	
	Circle& operator[](ID node) { return mSlots[node].circle; }
	Circle operator[](ID node) const { return mSlots[node].circle; }
	
	StatusCode Update(RenderBuffer *vbuf);
	StatusCode Render(RenderBuffer *vbuf);
};