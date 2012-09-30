#include "ScriptSystem.h"

void game(void* ctxt);

int main(int argc, char* argv[]) {
    // init systems
    RenderSystem::Initialize();
    SceneSystem::Initialize();
    CommandSystem::Initialize();
    NameSystem::Initialize();
    CircleSystem::Initialize();
    SplineSystem::Initialize();
    InputSystem::Initialize();

    // initialize communication channels
    static CommandBuffer buf0;
    static CommandBuffer buf1;
    CommandSystem::SubmitToSceneSystem(&buf0);
    CommandSystem::SubmitToSceneSystem(&buf1);
    GLFWthread hThread = glfwCreateThread(game, 0);

    // render loop
    do {
        CommandBuffer *vbuf;
        CommandSystem::RetrieveFromSceneSystem(&vbuf);
        RenderSystem::Render(vbuf);
        CircleSystem::Render(vbuf);
        SplineSystem::Render(vbuf);
        glfwSwapBuffers();
        CommandSystem::SubmitToSceneSystem(vbuf);
        InputSystem::Update();
    } while(glfwWaitThread(hThread, GLFW_NOWAIT) == GL_FALSE);

    // Teardown
    InputSystem::Destroy();
    SplineSystem::Destroy();
    CircleSystem::Destroy();
    NameSystem::Destroy();
    CommandSystem::Destroy();
    SceneSystem::Destroy();
    RenderSystem::Destroy();
    return 0;
}

void game(void* ctxt) {
    auto virtualMachine = luaL_newstate();          // TODO: hook memory allocator
    luaL_openlibs(virtualMachine);                  // TODO: limit libs
    ScriptSystem::Bind(virtualMachine);
    luaL_loadfile(virtualMachine, "src/main.lua");  // TODO: hook physFS
    lua_call(virtualMachine, 0, 0);                 // TODO: handle panic
    lua_close(virtualMachine);
}

void ScriptSystem::Yield() {
    CommandBuffer *vbuf;
    CommandSystem::RetrieveFromRenderSystem(&vbuf);
    SceneSystem::Update(vbuf);
    CircleSystem::Update(vbuf);
    SplineSystem::Update(vbuf);
    CommandSystem::SubmitToRenderSystem(vbuf);
}
