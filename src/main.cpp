#include "Script.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}
#include "tolua++.h"
#include "binding.h"

CircleManager CircleSystem::gInst;
SplineManager SplineSystem::gInst;

void game(void* ctxt);

int main(int argc, char* argv[]) {
    
    // Initialize Display
    glfwInit();
    if (glfwOpenWindow(800, 800,8, 8, 8, 8, 8, 0, GLFW_WINDOW) != GL_TRUE) {
        printf("[ERROR] Window Failed to Open.\n");
        return -1;
    }
    glfwSetWindowTitle("Bubbles!?");
    int w,h;
    glfwGetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    
    // init systems
    SceneSystem::Initialize();
    RenderSystem::Initialize();
    InputSystem::Initialize();

    SceneSystem::RegisterComponentManager(kComponentCircle, &CircleSystem::gInst);
    SceneSystem::RegisterComponentManager(kComponentSpline, &SplineSystem::gInst);

    glfwSetMousePosCallback(InputSystem::SetMousePosition);

    // initialize communication channels
    static RenderBuffer buf0;
    static RenderBuffer buf1;
    RenderSystem::SubmitToSceneSystem(&buf0);
    RenderSystem::SubmitToSceneSystem(&buf1);
    InputSystem::SetTime(glfwGetTime());
    glfwCreateThread(game, 0);


    // render loop
    while (glfwGetKey('Q') != GLFW_PRESS) {
        RenderBuffer *vbuf;
        RenderSystem::RetrieveFromSceneSystem(&vbuf);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: dispatch rendering through SceneSystem
        CircleSystem::gInst.Render(vbuf);
        SplineSystem::gInst.Render(vbuf);

        glfwSwapBuffers();
        InputSystem::SetTime(glfwGetTime());
        RenderSystem::SubmitToSceneSystem(vbuf);
    }

    // Teardown
    InputSystem::Finalize();
    // todo: other system teardowns
    glfwTerminate();

    return 0;
}

void game(void* ctxt) {
    // run scripts
    auto virtualMachine = luaL_newstate();          // todo: hook memory allocator
    luaL_openlibs(virtualMachine);                  // todo: limit libs
    tolua_binding_open(virtualMachine);
    luaL_loadfile(virtualMachine, "src/main.lua");  // todo: hook physFS
    lua_call(virtualMachine, 0, 0);                 // todo: handle panic
    lua_close(virtualMachine);
    // todo: terminate at script end
}

void ScriptPaint() {
    RenderBuffer *vbuf;
    RenderSystem::RetrieveFromRenderSystem(&vbuf);
    RenderSystem::Clear(vbuf);
    SceneSystem::Update(vbuf);

    // TODO: dispatch updating through SceneSystem
    CircleSystem::gInst.Update(vbuf);
    SplineSystem::gInst.Update(vbuf);

    RenderSystem::SubmitToRenderSystem(vbuf);
}
