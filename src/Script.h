#pragma once
#include "SceneSystem.h"
#include "CircleSystem.h"
#include "InputSystem.h"

namespace SceneSystem {

void Paint();

void GetPosition(ID node, float *x=0, float *y=0) {
	auto position = LocalToParent(node).t;
	*x = position.x;
	*y = position.y;
}

void SetPosition(ID node, float x, float y) {
	LocalToParent(node).t = Float2(x,y);
}

float GetRotation(ID node) {
	return LocalToParent(node).q.Radians() * (180.0f/M_PI);
}

void SetRotation(ID node, float degrees) {
	LocalToParent(node).q = Polar(1.f, degrees * (M_PI/180.0f));
}

}