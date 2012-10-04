#include "ScriptSystem.h"

void game(void* ctxt);

int main(int argc, char* argv[]) {
    // init systems
    RenderSystem::Initialize();
    NodeSystem::Initialize();
    MaterialSystem::Initialize();
    StrokeSystem::Initialize();
    CommandSystem::Initialize();
    TagSystem::Initialize();
    CircleSystem::Initialize();
    SplineSystem::Initialize();
    InputSystem::Initialize();

    // initialize communication channels
    static CommandBuffer buf0;
    static CommandBuffer buf1;
    CommandSystem::SubmitToNodeSystem(&buf0);
    CommandSystem::SubmitToNodeSystem(&buf1);
    GLFWthread hThread = glfwCreateThread(game, 0);

    // render loop
    do {
        CommandBuffer *vbuf;
        CommandSystem::RetrieveFromNodeSystem(&vbuf);
        RenderSystem::Render(vbuf);
        CircleSystem::Render(vbuf);
        SplineSystem::Render(vbuf);
        glfwSwapBuffers();
        CommandSystem::SubmitToNodeSystem(vbuf);
        InputSystem::Update();
    } while(glfwWaitThread(hThread, GLFW_NOWAIT) == GL_FALSE);

    // Teardown
    InputSystem::Destroy();
    SplineSystem::Destroy();
    CircleSystem::Destroy();
    TagSystem::Destroy();
    CommandSystem::Destroy();
    StrokeSystem::Destroy();
    MaterialSystem::Destroy();
    NodeSystem::Destroy();
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
    // yield one more time to prevent a potential deadlock
    ScriptSystem::Yield();    
}

void ScriptSystem::Yield() {
    CommandBuffer *vbuf;
    CommandSystem::RetrieveFromRenderSystem(&vbuf);
    NodeSystem::Update(vbuf);
    MaterialSystem::Update(vbuf);
    StrokeSystem::Update(vbuf);
    CircleSystem::Update(vbuf);
    SplineSystem::Update(vbuf);
    CommandSystem::SubmitToRenderSystem(vbuf);
}
