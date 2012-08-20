#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "tolua++.h"
#include "binding.h"

#include "RenderSystem.h"
#include "InputSystem.h"

void game(void* ctxt);

int main(int argc, char* argv[]) {
    
    // Initialize Display
    glfwInit();
    if (glfwOpenWindow(800, 500,8, 8, 8, 8, 8, 0, GLFW_WINDOW) != GL_TRUE) {
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
    RenderSystem::Initialize();
    InputSystem::Initialize();
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
        RenderSystem::Render(vbuf);
        glfwSwapBuffers();
        InputSystem::SetTime(glfwGetTime());
        RenderSystem::SubmitToSceneSystem(vbuf);
    }

    // Teardown
    InputSystem::Finalize();
    glfwTerminate();

    return 0;
}

void game(void* ctxt) {
    // It feels weird that this is the only non-hard-coded place...
    /// I suppose it's just until we have scripted components to mix with.
    static CircleSystem::Manager gCircles;
    SceneSystem::RegisterComponentManager(0, &gCircles);

    // Initialize Scripts
    // TODO handle errors
    lua_State* virtualMachine = lua_open();
    luaL_openlibs(virtualMachine);
    tolua_bubbles_open(virtualMachine);
    luaL_loadfile(virtualMachine, "src/main.lua");
    lua_call(virtualMachine, 0, 0);    
    lua_getglobal(virtualMachine, "init");
    if (lua_isfunction(virtualMachine, -1)) {
        lua_call(virtualMachine, 0, 0);    
    } else {
        lua_pop(virtualMachine, 1);
    }

    RenderBuffer *vbuf;

    while(1) {
        lua_getglobal(virtualMachine, "update");
        if (lua_isfunction(virtualMachine, -1)) {
            lua_call(virtualMachine, 0, 0);    
        } else {
            lua_pop(virtualMachine, 1);
        }
        // Render Shtuff
        RenderSystem::RetrieveFromRenderSystem(&vbuf);
        SceneSystem::Update(vbuf);
        CircleSystem::Update(vbuf);
        RenderSystem::SubmitToRenderSystem(vbuf);
    }

}