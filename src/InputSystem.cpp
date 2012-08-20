#include "InputSystem.h"
#include <GL/glfw.h>

static GLFWmutex gInputLock;
static Input gInput;

void InitializeInputSystem() {
	gInputLock = glfwCreateMutex();
	gInput.mousePosition = vec(0,0);
	gInput.seconds = 0;
}

void SetMousePosition(int x, int y) {
	glfwLockMutex(gInputLock);
	gInput.mousePosition = vec(x, y);
	glfwUnlockMutex(gInputLock);
}

void SetTime(double t) {
	glfwLockMutex(gInputLock);
	gInput.seconds = t;
	glfwUnlockMutex(gInputLock);
}

void GetInput(Input* input) {
	glfwLockMutex(gInputLock);
	*input = gInput;
	glfwUnlockMutex(gInputLock);
}

void FinalizeInputSystem() {
	glfwDestroyMutex(gInputLock);
}