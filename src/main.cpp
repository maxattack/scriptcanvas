#include "RenderSystem.h"
#include "util/Macros.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace SceneSystem;
using namespace RenderSystem;

#define COMPONENT_CIRCLE 0

#define M_TAU (2.0 * M_PI)

static CircleSystem gCircles;

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

        // "Update Script"
        gCircles[root].radius = 32 + 4.f * sin(2.1f * M_TAU * glfwGetTime());
        int mx, my;
        glfwGetMousePos( &mx, &my );
        Pose(root).t = vec(mx, my);
        Pose(root).q = Polar(1.f, -0.11f * M_TAU * glfwGetTime());
        Pose(urth).q = Polar(1.f, M_TAU * glfwGetTime());

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
