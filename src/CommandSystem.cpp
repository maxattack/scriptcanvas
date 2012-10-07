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

#include "CommandSystem.h"
#include "util/ProducerConsumerQueue.h"

static folly::ProducerConsumerQueue<CommandBuffer*, 4> mRenderQueue;
static folly::ProducerConsumerQueue<CommandBuffer*, 4> mSceneQueue;
static GLFWcond gRenderCondition;
static GLFWcond gSceneCondition;
static GLFWmutex gRenderMutex;
static GLFWmutex gSceneMutex;

static void Clear(CommandBuffer *buf) {
	buf->circleCount = 0;
	buf->segmentCount = 0;
}

void CommandSystem::Initialize() {
	gRenderCondition = glfwCreateCond();
	gSceneCondition = glfwCreateCond();
	gRenderMutex = glfwCreateMutex();
	gSceneMutex = glfwCreateMutex();
}

void CommandSystem::SubmitToRenderSystem(CommandBuffer* vbuf) {
	glfwLockMutex(gSceneMutex);
	auto result = mRenderQueue.write(vbuf);
	ASSERT(result); // eek?
	glfwUnlockMutex(gSceneMutex);
	glfwSignalCond(gSceneCondition);
}

void CommandSystem::RetrieveFromRenderSystem(CommandBuffer** out) {
	if (!mSceneQueue.read(*out)) {
		//puts("waiting on renderer...");
		glfwLockMutex(gRenderMutex);
		glfwWaitCond(gRenderCondition, gRenderMutex, GLFW_INFINITY);
		glfwUnlockMutex(gRenderMutex);
		mSceneQueue.read(*out);
	}
}

void CommandSystem::RetrieveFromNodeSystem(CommandBuffer** out) {
	if (!mRenderQueue.read(*out)) {
		//puts("waiting on scene...");
		glfwLockMutex(gSceneMutex);
		glfwWaitCond(gSceneCondition, gSceneMutex, GLFW_INFINITY);
		glfwUnlockMutex(gSceneMutex);
		mRenderQueue.read(*out);
	}
}

void CommandSystem::SubmitToNodeSystem(CommandBuffer* vbuf) {
	Clear(vbuf);
	glfwLockMutex(gRenderMutex);
	auto result = mSceneQueue.write(vbuf);
	ASSERT(result); // eek?
	glfwUnlockMutex(gRenderMutex);
	glfwSignalCond(gRenderCondition);
}

void CommandSystem::Destroy() {
	// TODO: Teardown
}