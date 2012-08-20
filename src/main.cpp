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

#define COMPONENT_CIRCLE 0

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
    
    InitializeRenderSystem();
    InitializeInputSystem();
    glfwSetMousePosCallback(SetMousePosition);

    static RenderBuffer buf0;
    static RenderBuffer buf1;
    
    SubmitToSceneSystem(&buf0);
    SubmitToSceneSystem(&buf1);
    SetTime(glfwGetTime());
    glfwCreateThread(game, 0);

    while (glfwGetKey('Q') != GLFW_PRESS) {
        RenderBuffer *vbuf;
        RetrieveFromSceneSystem(&vbuf);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render(vbuf);
        glfwSwapBuffers();
        SetTime(glfwGetTime());
        SubmitToSceneSystem(vbuf);
    }

    // Teardown
    FinalizeInputSystem();
    glfwTerminate();

    return 0;
}

void game(void* ctxt) {
    static CircleSystem gCircles;
    RegisterComponentManager(COMPONENT_CIRCLE, &gCircles);

    // Initialize Scripts
    // lua_State* virtualMachine = lua_open();
    // luaL_openlibs(virtualMachine);
    // tolua_bubbles_open(virtualMachine);
    // const char* script_path = argc == 2 ? argv[1] : "src/main.lua";
    // luaL_loadfile(virtualMachine, script_path);
    // lua_call(virtualMachine, 0, 0);    

    // lua_getglobal(virtualMachine, "init");
    // if (lua_isfunction(virtualMachine, -1)) {
    //     lua_call(virtualMachine, 0, 0);    
    // } else {
    //     lua_pop(virtualMachine, 1);
    // }

    // "Bootstrap Script"
    ID root = CreateNode();
    Pose(root).t = vec(400, 400);
    AddComponent(root, COMPONENT_CIRCLE);
    GetCircleComponent(root).geometry = CreateCircleGeometry(32);
    GetCircleComponent(root).material = CreateCircleMaterial(1, 1, 0.25);


    ID urth = CreateNode(root);
    Pose(urth).t = vec(128, 0);
    AddComponent(urth, COMPONENT_CIRCLE);
    GetCircleComponent(urth).geometry = CreateCircleGeometry(24);
    GetCircleComponent(urth).material = CreateCircleMaterial(0.5, 0.5, 0.8);

    ID moon = CreateNode(urth);
    Pose(moon).t.x = 48;
    AddComponent(moon, COMPONENT_CIRCLE);
    GetCircleComponent(moon).geometry = CreateCircleGeometry(12);
    GetCircleComponent(moon).material = CreateCircleMaterial(0.4, 0.4, 0.4);

    while(1) {
        RenderBuffer *vbuf;
        RetrieveFromRenderSystem(&vbuf);
        Input input;
        GetInput(&input);

        // "Update Script"
        GetCircleGeometry(GetCircleComponent(root).geometry).radius = 32 + 4.f * sin(2.1f * kTau * input.seconds);
        Pose(root).t = input.mousePosition;
        Pose(root).q = Polar(1.f, -0.11f * kTau * input.seconds);
        Pose(urth).q = Polar(1.f, kTau * input.seconds);

        // lua_getglobal(virtualMachine, "update");
        // if (lua_isfunction(virtualMachine, -1)) {
        //     lua_call(virtualMachine, 0, 0);    
        // } else {
        //     lua_pop(virtualMachine, 1);
        // }

        // Render Shtuff
        UpdateSceneSystem(vbuf);
        UpdateCircleSystem(vbuf);
        SubmitToRenderSystem(vbuf);
    }

}