#include "InputSystem.h"
#include <GL/glfw.h>

namespace InputSystem {

// TODO - Add an atomic "dirty bit" and copy

static GLFWmutex lock;
static Vec mousePosition;
static double seconds;

void Initialize() {
	lock = glfwCreateMutex();
	mousePosition = vec(0,0);
	seconds = 0;
}

void SetMousePosition(int x, int y) {
	glfwLockMutex(lock);
	mousePosition = vec(x, y);
	glfwUnlockMutex(lock);
}

void SetTime(double t) {
	glfwLockMutex(lock);
	seconds = t;
	glfwUnlockMutex(lock);
}

Vec MousePosition() {
	glfwLockMutex(lock);
	Vec result = mousePosition;
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