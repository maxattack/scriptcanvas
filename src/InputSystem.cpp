// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "InputSystem.h"

// TODO - Add an atomic "dirty bit" and copy

static GLFWmutex lock;
static double seconds;
static bool wantsQuit;
static vec2_t mousePosition;

static void OnMouseMoved(int x, int y) {
	glfwLockMutex(lock);
	mousePosition = Vec2(x, y);
	glfwUnlockMutex(lock);
}

void InputSystem::Initialize() {
	lock = glfwCreateMutex();
	wantsQuit = false;
	seconds = glfwGetTime();
	mousePosition = Vec2(0,0);
    glfwSetMousePosCallback(OnMouseMoved);
}

void InputSystem::Update() {
	glfwLockMutex(lock);
	wantsQuit = (glfwGetKey('Q') == GLFW_PRESS);
	seconds = glfwGetTime();
	glfwUnlockMutex(lock);
}

vec2_t InputSystem::MousePosition() {
	glfwLockMutex(lock);
	vec2_t result = mousePosition;
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
