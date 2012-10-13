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
#include "Shaders.h"
#include "CommandSystem.h"
#include "VectorMath.h"
#include "util/CompactComponentPool.h"

static Shader::circle_t shader;
static GLuint unitVertexBuffer;
static CompactComponentPool<Circle> circles;

static Shader::eye_t eye;

void CircleSystem::Initialize() {
    shader.Initialize();
    // create circle vertex buffer
    vec2_t unit = Vec2(1,0);
    vec2_t rotor = Polar(1.f, kTau / (64-2.f));
    vec2_t buffer[64];
    buffer[0] = Vec2(0,0);
    for(int i=1; i<64; ++i) {
        buffer[i] = unit;
        unit *= rotor;
    }
    glGenBuffers(1, &unitVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, unitVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2_t)*64, buffer, GL_STATIC_DRAW);

    eye.Initialize();
}

void CircleSystem::Destroy() {
    shader.Destroy();
    // TODO: Teardown VB
}

Circle& CircleSystem::GetCircle(ID node) {
    return circles[node];
}

void CircleSystem::Update(CommandBuffer* vbuf) {
    for(auto p=circles.Begin(); p!=circles.End(); ++p) {
        CircleCommand cmd = { 
            MaterialSystem::Index(p->component.material),
            NodeSystem::Index(p->node)
        };
        vbuf->circles[vbuf->circleCount++] = cmd;
    }
}

#include "InputSystem.h"

void CircleSystem::Render(CommandBuffer *vbuf) {
    if (vbuf->circleCount) {
        glUseProgram(shader.handle);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(shader.unit);
        glBindBuffer(GL_ARRAY_BUFFER, unitVertexBuffer);
        glVertexAttribPointer(shader.unit, 2, GL_FLOAT, GL_FALSE, 0, 0);

        for(int i=0; i<vbuf->circleCount; ++i) {
            auto cmd = vbuf->circles[i];
            auto mat = vbuf->materials[cmd.mid];
            glLoadMatrixf(Mat4(vbuf->transforms[cmd.tid]).m);
            glUniform1f(shader.radius, mat.weight);
            float r,g,b;
            mat.color.ToFloatRGB(&r, &g, &b);
            glUniform4f(shader.color, r, g, b, 1.f);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 64);
        }
        // clean up opengl state
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableVertexAttribArray(shader.unit);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    // test: eye
    glUseProgram(eye.handle);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(shader.unit);
    glBindBuffer(GL_ARRAY_BUFFER, unitVertexBuffer);
    glVertexAttribPointer(shader.unit, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glLoadIdentity();
    glTranslatef(400.f, 200.f, 0.9f);
    float radius = 64;
    glUniform1f(eye.radius, radius);
    glUniform4f(eye.color, 1.f, 1.f, 1.f, 1.f);
    glUniform4f(eye.irisColor, 0.f, 1.f, 0.f, 1.f);
    glUniform1f(eye.irisRadiusSq, 0.33f*0.33f);

    vec2_t offsetToMouse = (InputSystem::MousePosition() - Vec2(400, 200)) / radius;
    if (offsetToMouse.Norm() > 1.f) {
        offsetToMouse = offsetToMouse.Normalized();
    }
    offsetToMouse *= 0.6f;
    glUniform2f(eye.irisPosition, offsetToMouse.x, offsetToMouse.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 64);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(shader.unit);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

}

void CircleSystem::Create(ID node, ID mid) {
    ASSERT(NodeSystem::NodeValid(node));
    ASSERT(MaterialSystem::MaterialValid(mid));
    NodeSystem::AddComponent(node, kComponentCircle);
    circles.Alloc(node);
    circles[node].material = mid;
}

Material& GetMaterial(ID node) {
    return MaterialSystem::GetMaterial(circles[node].material);
}

void CircleSystem::OnNodeDestroyed(ID node) {
    circles.Free(node);
}

void CircleSystem::Destroy(ID node) { 
    NodeSystem::RemoveComponent(node, kComponentCircle);
}
