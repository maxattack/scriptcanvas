#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "CircleManager.h"
#include "RenderSystem.h"
#include "VectorMath.h"

void CircleManager::Initialize() {
    mProgram = RenderSystem::LoadShaderProgram("src/circle.glsl");
    // lookup shader storage locations
    glUseProgram(mProgram);
    mAttribUnit = glGetAttribLocation(mProgram, "unit");
    mUniformRadius = glGetUniformLocation(mProgram, "radius");
    mUniformColor = glGetUniformLocation(mProgram, "color");
    // create circle vertex buffer
    float2 unit = Float2(1,0);
    float2 rotor = Polar(1.f, kTau / (64-2.f));
    float2 buffer[64];
    buffer[0] = Float2(0,0);
    for(int i=1; i<64; ++i) {
        buffer[i] = unit;
        unit *= rotor;
    }
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float2)*64, buffer, GL_STATIC_DRAW);
    glUseProgram(0);
}

void CircleManager::Destroy() {
    // todo: cleanup
}

bool CircleManager::CreateComponent(ID node) {
	mSlots.Alloc(node);
    auto& circle = mSlots[node].circle;
    circle.radius = 1.f;
    circle.fill = RGB(0xffffff);
    mSlots[node].node = node;
    return true;
}

bool CircleManager::DestroyComponent(ID node) {
    mSlots.Free(node);
    return true;
}

void CircleManager::Update(RenderBuffer* vbuf) {
    // TODO: Culling
    for(auto p=mSlots.Begin(); p!=mSlots.End(); ++p) {
        CircleCommand cmd = { 0, SceneSystem::Index(p->node), p->circle };
        vbuf->circles[vbuf->circleCount++] = cmd;
    }
}

void CircleManager::Render(RenderBuffer* vbuf) {
    if (vbuf->circleCount) {
        glUseProgram(mProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(mAttribUnit);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        for(int i=0; i<vbuf->circleCount; ++i) {
            auto cmd = vbuf->circles[i];
            glLoadMatrixf(Mat4(vbuf->transforms[cmd.transform]).m);
            glUniform1f(mUniformRadius, cmd.properties.radius);
            float r,g,b;
            cmd.properties.fill.ToFloatRGB(&r, &g, &b);
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

