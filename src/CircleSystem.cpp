// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
CompactComponentPool<Circle> mSlots;

void CircleSystem::Initialize() {
    mProgram = RenderSystem::LoadShaderProgram("src/circle.glsl");
    // lookup shader storage locations
    glUseProgram(mProgram);
    mAttribUnit = glGetAttribLocation(mProgram, "unit");
    mUniformRadius = glGetUniformLocation(mProgram, "radius");
    mUniformColor = glGetUniformLocation(mProgram, "color");
    // create circle vertex buffer
    vec2_t unit = Vec2(1,0);
    vec2_t rotor = Polar(1.f, kTau / (64-2.f));
    vec2_t buffer[64];
    buffer[0] = Vec2(0,0);
    for(int i=1; i<64; ++i) {
        buffer[i] = unit;
        unit *= rotor;
    }
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2_t)*64, buffer, GL_STATIC_DRAW);
    glUseProgram(0);
}

void CircleSystem::Destroy() {
    // todo: cleanup
}

Circle& CircleSystem::GetCircle(ID node) {
    return mSlots[node];
}

void CircleSystem::Update(CommandBuffer* vbuf) {
    for(auto p=mSlots.Begin(); p!=mSlots.End(); ++p) {
        CircleCommand cmd = { 
            MaterialSystem::Index(p->component.material),
            NodeSystem::Index(p->node)
        };
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
            auto mat = vbuf->materials[cmd.mid];
            glLoadMatrixf(Mat4(vbuf->transforms[cmd.tid]).m);
            glUniform1f(mUniformRadius, mat.weight);
            float r,g,b;
            mat.color.ToFloatRGB(&r, &g, &b);
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

void CircleSystem::Create(ID node, ID mid) {
    ASSERT(NodeSystem::NodeValid(node));
    ASSERT(MaterialSystem::MaterialValid(mid));
    NodeSystem::AddComponent(node, kComponentCircle);
    mSlots.Alloc(node);
    mSlots[node].material = mid;
}

Material& GetMaterial(ID node) {
    return MaterialSystem::GetMaterial(mSlots[node].material);
}

void CircleSystem::OnNodeDestroyed(ID node) {
    mSlots.Free(node);
}

void CircleSystem::Destroy(ID node) { 
    NodeSystem::RemoveComponent(node, kComponentCircle);
}
