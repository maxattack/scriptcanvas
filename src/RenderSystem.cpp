#include "RenderSystem.h"
#include "util/ProducerConsumerQueue.h"
#include "Script.h"
#include <algorithm>
#include <functional>

static folly::ProducerConsumerQueue<RenderBuffer*, 4> mRenderQueue;
static folly::ProducerConsumerQueue<RenderBuffer*, 4> mSceneQueue;
static GLFWcond gRenderCondition;
static GLFWcond gSceneCondition;
static GLFWmutex gRenderMutex;
static GLFWmutex gSceneMutex;

void RenderSystem::Initialize() {
	gRenderCondition = glfwCreateCond();
	gSceneCondition = glfwCreateCond();
	gRenderMutex = glfwCreateMutex();
	gSceneMutex = glfwCreateMutex();
}

void RenderSystem::Clear(RenderBuffer *vbuf) {
	vbuf->circleCount = 0;
    vbuf->materialCount = 0;
    vbuf->hermiteSegmentCount = 0;
}

void RenderSystem::SubmitToRenderSystem(RenderBuffer* vbuf) {
	glfwLockMutex(gSceneMutex);
	auto result = mRenderQueue.write(vbuf);
	ASSERT(result); // eek?
	glfwUnlockMutex(gSceneMutex);
	glfwSignalCond(gSceneCondition);
}

void RenderSystem::RetrieveFromRenderSystem(RenderBuffer** out) {
	if (!mSceneQueue.read(*out)) {
		//puts("waiting on renderer...");
		glfwLockMutex(gRenderMutex);
		glfwWaitCond(gRenderCondition, gRenderMutex, GLFW_INFINITY);
		glfwUnlockMutex(gRenderMutex);
		mSceneQueue.read(*out);
	}
}

void RenderSystem::RetrieveFromSceneSystem(RenderBuffer** out) {
	if (!mRenderQueue.read(*out)) {
		//puts("waiting on scene...");
		glfwLockMutex(gSceneMutex);
		glfwWaitCond(gSceneCondition, gSceneMutex, GLFW_INFINITY);
		glfwUnlockMutex(gSceneMutex);
		mRenderQueue.read(*out);
	}
}

void RenderSystem::SubmitToSceneSystem(RenderBuffer* vbuf) {
	glfwLockMutex(gRenderMutex);
	auto result = mSceneQueue.write(vbuf);
	ASSERT(result); // eek?
	glfwUnlockMutex(gRenderMutex);
	glfwSignalCond(gRenderCondition);
}

GLuint RenderSystem::LoadShaderProgram(const char* filename) {
    GLuint prog, vert, frag;
    GLint cnt = 0;
    GLchar* buf = 0;
    {
        FILE* file = fopen(filename, "r");
        if (!file) { 
            return 0; 
        }
        fseek(file, 0, SEEK_END);
        cnt = ftell(file);
        rewind(file);
        buf = new char[cnt+1];
        if (buf) {
          unsigned acnt = fread(buf, 1, cnt, file);
          fclose(file);
          if (cnt != acnt) {
            delete[] buf;
            return 0;
          } else {
            buf[cnt] = 0;
          }
        }
    }
     // initialize shader program
    prog = glCreateProgram();
    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);
    {
        const GLchar sCondVert[] = "#define VERTEX\n";
        const GLchar sCondFrag[] = "#define FRAGMENT\n";
        const GLchar *vsrc[] = { sCondVert, buf };
        const GLchar *fsrc[] = { sCondFrag, buf };
        GLint vcnt[] = { static_cast<GLint>(strlen(sCondVert)), cnt };
        GLint fcnt[] = { static_cast<GLint>(strlen(sCondFrag)), cnt };
        glShaderSource(vert, 2, vsrc, vcnt);
        glShaderSource(frag, 2, fsrc, fcnt);
        glCompileShader(vert);
        glCompileShader(frag);
        delete[] buf;

        GLint result;
        glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE) {
            GLchar buf[256];
            int len;
            glGetShaderInfoLog(vert, 256, &len, buf);
            printf("%s\n", buf);
            return 0;
        }

        glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE) {
            GLchar buf[256];
            int len;
            glGetShaderInfoLog(frag, 256, &len, buf);
            printf("%s\n", buf);
            return 0;
        }

        glAttachShader(prog, vert);
        glAttachShader(prog, frag);
        glLinkProgram(prog);
    }
    return prog;
}
