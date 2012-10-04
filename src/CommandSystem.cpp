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