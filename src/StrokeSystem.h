#pragma once

#include <GL/glfw.h>
#include "util/SceneUtil.h"
#include "util/CompactPool.h"

namespace StrokeSystem {

struct Component {
	ID node;
	ID geometry;
	ID material;
};

struct Geometry {
	int firstVertex;
	int vertexCount;
	float eccentricity;
	float strokeWeight;
};

struct Material {
	float r;
	float g;
	float b;
};

}
