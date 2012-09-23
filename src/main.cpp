#include "Script.h"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}
#include "tolua++.h"
#include "binding.h"

void game(void* ctxt);

#include "VectorMath.h"
struct vertex {
    float uuu;
    float uu;
    float u;
    float s;
};

inline vertex Vertex(float u, bool side) {
    vertex result = { u*u*u, u*u, u, side ? 0.5f : -0.5f };
    return result;
}

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
    glfwSetMousePosCallback(InputSystem::SetMousePosition);
    ScriptInitialize();

    // initialize communication channels
    static RenderBuffer buf0;
    static RenderBuffer buf1;
    RenderSystem::SubmitToSceneSystem(&buf0);
    RenderSystem::SubmitToSceneSystem(&buf1);
    InputSystem::SetTime(glfwGetTime());
    glfwCreateThread(game, 0);

    // TEST hermite curves
    auto cubicProgram = RenderSystem::LoadShaderProgram("src/cubic.glsl");
    glUseProgram(cubicProgram);
    auto attribParameterAndSide = glGetAttribLocation(cubicProgram, "parameterAndSide");
    auto uniformThickness = glGetUniformLocation(cubicProgram, "thickness");
    auto uniformPositionMatrix = glGetUniformLocation(cubicProgram, "positionMatrix");
    auto uniformNormalMatrix = glGetUniformLocation(cubicProgram, "normalMatrix");
    auto uniformColor = glGetUniformLocation(cubicProgram, "color");
    GLuint vertexBuffer;
    {
        vertex buf[256];
        float sideBuffer[256];
        float du = 1.f / 127.f;
        float u = 0;
        for(int i=0; i<128; ++i) {
            buf[i+i] = Vertex(u, true);
            buf[i+i+1] = Vertex(u, false);
            u += du;
        }
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*256, buf, GL_STATIC_DRAW);
    }
    glUseProgram(0);


    // render loop
    while (glfwGetKey('Q') != GLFW_PRESS) {
        RenderBuffer *vbuf;
        RenderSystem::RetrieveFromSceneSystem(&vbuf);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderSystem::Render(vbuf);

        // TEST hermite curves
        glUseProgram(cubicProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(attribParameterAndSide);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glLoadIdentity();

        auto p0 = Vec4(200, 100);
        auto p1 = Vec4(200, 700);

        auto mp = InputSystem::MousePosition();
        auto t = InputSystem::Time();

        auto t0 = Vec4(mp.x - 200, mp.y - 100);
        auto t1 = -Vec4(mp.x - 200, mp.y - 700);

        auto posMatrix = HermiteMat(p0, p1, t0, t1);
        auto normMatrix = HermiteNormMat(p0, p1, t0, t1);

        glUniform1f(uniformThickness, 16.f + 32.f * (0.5f + 0.5f * sin(kTau * t)));
        glUniformMatrix4fv(uniformPositionMatrix, 1, GL_FALSE, posMatrix.m);
        glUniformMatrix4fv(uniformNormalMatrix, 1, GL_FALSE, normMatrix.m);
        glUniform4f(uniformColor, 1.f, 1.f, 0.5f, 1.f);

        glVertexAttribPointer(attribParameterAndSide, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 256);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableVertexAttribArray(attribParameterAndSide);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);


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
    auto virtualMachine = luaL_newstate();    // todo: hook memory allocator
    luaL_openlibs(virtualMachine);                  // todo: limit libs
    tolua_binding_open(virtualMachine);
    luaL_loadfile(virtualMachine, "src/main.lua");  // todo: hook physFS
    lua_call(virtualMachine, 0, 0);                 // todo: handle panic
    lua_close(virtualMachine);
    // todo: terminate at script end
}