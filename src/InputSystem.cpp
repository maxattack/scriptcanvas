#include "InputSystem.h"
#include <GL/glfw.h>

namespace InputSystem {

// TODO - Add an atomic "dirty bit" and copy

static GLFWmutex lock;
static float2 mousePosition;
static double seconds;

void Initialize() {
	lock = glfwCreateMutex();
	mousePosition = Float2(0,0);
	seconds = 0;
}

void SetMousePosition(int x, int y) {
	glfwLockMutex(lock);
	mousePosition = Float2(x, y);
	glfwUnlockMutex(lock);
}

void SetTime(double t) {
	glfwLockMutex(lock);
	seconds = t;
	glfwUnlockMutex(lock);
}

float2 MousePosition() {
	glfwLockMutex(lock);
	float2 result = mousePosition;
	glfwUnlockMutex(lock);
	return result;
}

void GetMousePosition(int *x, int *y) {
	auto position = MousePosition();
	*x = position.x;
	*y = position.y;
}

double GetTime() {
	glfwLockMutex(lock);
	double result = seconds;
	glfwUnlockMutex(lock);
	return seconds;
}

void Finalize() {
	glfwDestroyMutex(lock);
}

}