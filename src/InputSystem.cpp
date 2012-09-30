#include "InputSystem.h"
#include <GL/glfw.h>

// TODO - Add an atomic "dirty bit" and copy

static GLFWmutex lock;
static double seconds;
static bool wantsQuit;
static float2_t mousePosition;

static void OnMouseMoved(int x, int y) {
	glfwLockMutex(lock);
	mousePosition = Float2(x, y);
	glfwUnlockMutex(lock);
}

void InputSystem::Initialize() {
	lock = glfwCreateMutex();
	wantsQuit = false;
	seconds = glfwGetTime();
	mousePosition = Float2(0,0);
    glfwSetMousePosCallback(OnMouseMoved);
}

void InputSystem::Update() {
	glfwLockMutex(lock);
	wantsQuit = (glfwGetKey('Q') == GLFW_PRESS);
	seconds = glfwGetTime();
	glfwUnlockMutex(lock);
}

float2_t InputSystem::MousePosition() {
	glfwLockMutex(lock);
	float2_t result = mousePosition;
	glfwUnlockMutex(lock);
	return result;
}

double InputSystem::Time() {
	glfwLockMutex(lock);
	double result = seconds;
	glfwUnlockMutex(lock);
	return seconds;
}

void InputSystem::Destroy() {
	glfwDestroyMutex(lock);
}

bool InputSystem::Quit() {
	glfwLockMutex(lock);
	bool result = wantsQuit;
	glfwUnlockMutex(lock);
	return result;
}
