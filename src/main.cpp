#include "CircleSystem.h"
#include "util/Macros.h"
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

#define COMPONENT_CIRCLE 0

CircleSystem gCircles;


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
    
    // Initialize Systems
    gCircles.Init();
    RegisterComponentManager(COMPONENT_CIRCLE, &gCircles);

    // Initialize Scripts
    lua_State* virtualMachine = lua_open();
    luaL_openlibs(virtualMachine);
    tolua_bubbles_open(virtualMachine);
    const char* script_path = argc == 2 ? argv[1] : "src/main.lua";
    luaL_loadfile(virtualMachine, script_path);
    lua_call(virtualMachine, 0, 0);    

    lua_getglobal(virtualMachine, "init");
    if (lua_isfunction(virtualMachine, -1)) {
        lua_call(virtualMachine, 0, 0);    
    } else {
        lua_pop(virtualMachine, 1);
    }

    // "Bootstrap Script"
    ID root = CreateNode();
    AddComponent(root, COMPONENT_CIRCLE);
    gCircles[root].Init(32, 1, 1, 0.25);

    ID urth = CreateNode(root);
    Pose(urth).t = vec(128, 0);
    AddComponent(urth, COMPONENT_CIRCLE);
    gCircles[urth].Init(24, 0.5, 0.5, 0.8);

    ID moon = CreateNode(urth);
    Pose(moon).t.x = 48;
    AddComponent(moon, COMPONENT_CIRCLE);
    gCircles[moon].Init(12, 0.4, 0.4, 0.4);

    while (glfwGetKey('Q') != GLFW_PRESS) {

        lua_getglobal(virtualMachine, "update");
        if (lua_isfunction(virtualMachine, -1)) {
            lua_call(virtualMachine, 0, 0);    
        } else {
            lua_pop(virtualMachine, 1);
        }
        // "Update Script"
        gCircles[root].radius = 32 + 4.f * sin(2.1f * kTau * glfwGetTime());
        int mx, my;
        glfwGetMousePos( &mx, &my );
        Pose(root).t = vec(mx, my);
        Pose(root).q = Polar(1.f, -0.11f * kTau * glfwGetTime());
        Pose(urth).q = Polar(1.f, kTau * glfwGetTime());

        // Render Shtuff
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gCircles.Render();
        glfwSwapBuffers();
    }

    // Teardown
    DestroyNode(root);
    gCircles.Dispose();
    glfwTerminate();

    return 0;
}
