#include "RenderSystem.h"
#include "ProducerConsumerQueue.h"
#include "util/Macros.h"
#include "Script.h"
#include <algorithm>
#include <functional>

namespace RenderSystem {

static folly::ProducerConsumerQueue<RenderBuffer*, 4> mRenderQueue;
static folly::ProducerConsumerQueue<RenderBuffer*, 4> mSceneQueue;
static GLFWcond gRenderCondition;
static GLFWcond gSceneCondition;
static GLFWmutex gRenderMutex;
static GLFWmutex gSceneMutex;

void Initialize() {
	gRenderCondition = glfwCreateCond();
	gSceneCondition = glfwCreateCond();
	gRenderMutex = glfwCreateMutex();
	gSceneMutex = glfwCreateMutex();
}

void SubmitToRenderSystem(RenderBuffer* vbuf) {
	glfwLockMutex(gSceneMutex);
	auto result = mRenderQueue.write(vbuf);
	ASSERT(result); // eek?
	glfwUnlockMutex(gSceneMutex);
	glfwSignalCond(gSceneCondition);
}

void RetrieveFromRenderSystem(RenderBuffer** out) {
	if (!mSceneQueue.read(*out)) {
		//puts("waiting on renderer...");
		glfwLockMutex(gRenderMutex);
		glfwWaitCond(gRenderCondition, gRenderMutex, GLFW_INFINITY);
		glfwUnlockMutex(gRenderMutex);
		mSceneQueue.read(*out);
	}
}

void RetrieveFromSceneSystem(RenderBuffer** out) {
	if (!mRenderQueue.read(*out)) {
		//puts("waiting on scene...");
		glfwLockMutex(gSceneMutex);
		glfwWaitCond(gSceneCondition, gSceneMutex, GLFW_INFINITY);
		glfwUnlockMutex(gSceneMutex);
		mRenderQueue.read(*out);
	}
}

void SubmitToSceneSystem(RenderBuffer* vbuf) {
	glfwLockMutex(gRenderMutex);
	auto result = mSceneQueue.write(vbuf);
	ASSERT(result); // eek?
	glfwUnlockMutex(gRenderMutex);
	glfwSignalCond(gRenderCondition);
}

void Render(RenderBuffer* vbuf) {
	ScriptRender(vbuf);
}

}
