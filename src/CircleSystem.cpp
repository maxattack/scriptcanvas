#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "CommandSystem.h"
#include "VectorMath.h"
#include "util/CompactComponentPool.h"

GLuint mProgram;
GLuint mAttribUnit;
GLuint mUniformRadius;
GLuint mUniformColor;
GLuint mVertexBuffer;
CompactComponentPool<circle_t> mSlots;

void CircleSystem::Initialize() {
    mProgram = RenderSystem::LoadShaderProgram("src/circle.glsl");
    // lookup shader storage locations
    glUseProgram(mProgram);
    mAttribUnit = glGetAttribLocation(mProgram, "unit");
    mUniformRadius = glGetUniformLocation(mProgram, "radius");
    mUniformColor = glGetUniformLocation(mProgram, "color");
    // create circle vertex buffer
    float2_t unit = Float2(1,0);
    float2_t rotor = Polar(1.f, kTau / (64-2.f));
    float2_t buffer[64];
    buffer[0] = Float2(0,0);
    for(int i=1; i<64; ++i) {
        buffer[i] = unit;
        unit *= rotor;
    }
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float2_t)*64, buffer, GL_STATIC_DRAW);
    glUseProgram(0);
}

void CircleSystem::Destroy() {
    // todo: cleanup
}

void CircleSystem::Update(CommandBuffer* vbuf) {
    for(auto p=mSlots.Begin(); p!=mSlots.End(); ++p) {
        CircleCommand cmd = { 0, SceneSystem::Index(p->node), p->component.radius, p->component.fill };
        vbuf->circles[vbuf->circleCount++] = cmd;
    }
}

void CircleSystem::Render(CommandBuffer *vbuf) {
    if (vbuf->circleCount) {
        glUseProgram(mProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(mAttribUnit);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        for(int i=0; i<vbuf->circleCount; ++i) {
            auto cmd = vbuf->circles[i];
            glLoadMatrixf(Mat4(vbuf->transform_ts[cmd.transform_t]).m);
            glUniform1f(mUniformRadius, cmd.radius);
            float r,g,b;
            cmd.fill.ToFloatRGB(&r, &g, &b);
            glUniform4f(mUniformColor, r, g, b, 1.f);
            glVertexAttribPointer(mAttribUnit, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 64);
        }
        // clean up opengl state
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableVertexAttribArray(mAttribUnit);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}

void CircleSystem::Create(ID node, color_t color, float radius) {
    SceneSystem::AddComponent(node, kComponentCircle);
    mSlots.Alloc(node);
    mSlots[node].fill = color;
    mSlots[node].radius = radius;
}

circle_t& CircleSystem::GetCircle(ID node) {
    return mSlots[node];
}

void CircleSystem::OnNodeDestroyed(ID node) {
    mSlots.Free(node);
}

void CircleSystem::Destroy(ID node) { 
    SceneSystem::RemoveComponent(node, kComponentCircle);
}
