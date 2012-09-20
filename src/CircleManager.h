#pragma once
#include <GL/glfw.h>
#include "util/SceneUtil.h"
#include "math/color.h"

struct Circle {
	float radius;
	color fill;
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

	struct Command {
		uint16_t queue;
		uint16_t transform;
		color fill;
		float radius;
	};


	void Initialize();
	void Destroy();

	bool CreateComponent(ID node);
	bool DestroyComponent(ID node);
	
	Circle& operator[](ID node);
	Circle operator[](ID node) const;
	
	void Update(RenderBuffer *vbuf);
	void Render(RenderBuffer *vbuf);
};