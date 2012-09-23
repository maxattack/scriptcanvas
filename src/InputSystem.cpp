#include "InputSystem.h"
#include <GL/glfw.h>

// TODO - Add an atomic "dirty bit" and copy

static GLFWmutex lock;
static float2 mousePosition;
static double seconds;

void InputSystem::Initialize() {
	lock = glfwCreateMutex();
	mousePosition = Float2(0,0);
	seconds = 0;
}

void InputSystem::SetMousePosition(int x, int y) {
	glfwLockMutex(lock);
	mousePosition = Float2(x, y);
	glfwUnlockMutex(lock);
}

void InputSystem::SetTime(double t) {
	glfwLockMutex(lock);
	seconds = t;
	glfwUnlockMutex(lock);
}

float2 InputSystem::MousePosition() {
	glfwLockMutex(lock);
	float2 result = mousePosition;
	glfwUnlockMutex(lock);
	return result;
}

void InputSystem::GetMousePosition(int *x, int *y) {
	auto position = MousePosition();
	*x = position.x;
	*y = position.y;
}

double InputSystem::Time() {
	glfwLockMutex(lock);
	double result = seconds;
	glfwUnlockMutex(lock);
	return seconds;
}

void InputSystem::Finalize() {
	glfwDestroyMutex(lock);
}
