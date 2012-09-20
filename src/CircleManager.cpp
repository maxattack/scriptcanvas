#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "CircleManager.h"
#include "RenderSystem.h"
#include "util/Macros.h"

static GLuint LoadShaderProgram(const char* filename); // make more accessible

void CircleManager::Initialize() {
    mProgram = LoadShaderProgram("src/circle.glsl");
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

Circle& CircleManager::operator[](ID node) {
    return mSlots[node].circle;
}

Circle CircleManager::operator[](ID node) const {
    return mSlots[node].circle;
}

void CircleManager::Update(RenderBuffer* vbuf) {
    for(auto p=mSlots.Begin(); p!=mSlots.End(); ++p) {
        // todo: culling?
        CircleCommand cmd = { 0, SceneSystem::Index(p->node), p->circle.radius, p->circle.fill };
        vbuf->circles[vbuf->circleCount++] = cmd;
    }
}

void CircleManager::Render(RenderBuffer* vbuf) {
    LOG_INT(vbuf->circleCount);
    if (vbuf->circleCount) {
        glUseProgram(mProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(mAttribUnit);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

        for(int i=0; i<vbuf->circleCount; ++i) {
            auto cmd = vbuf->circles[i];
            transform w = vbuf->transforms[cmd.transform];
            float mat[16] = {
                w.q.x, w.q.y, 0, 0,
                -w.q.y, w.q.x, 0, 0,
                0, 0, 1, 0,
                w.t.x, w.t.y, 0, 1
            };
            glLoadMatrixf(mat);
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

static GLuint LoadShaderProgram(const char* filename) {
    GLuint prog, vert, frag;
    GLint cnt = 0;
    GLchar* buf = 0;
    {
        FILE* file = fopen(filename, "r");
        if (!file) { 
            return 0; 
        }
        fseek(file, 0, SEEK_END);
        cnt = ftell(file);
        rewind(file);
        buf = new char[cnt+1];
        if (buf) {
          unsigned acnt = fread(buf, 1, cnt, file);
          fclose(file);
          if (cnt != acnt) {
            delete[] buf;
            return 0;
          } else {
            buf[cnt] = 0;
          }
        }
    }
     // initialize shader program
    prog = glCreateProgram();
    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);
    {
        const GLchar sCondVert[] = "#define VERTEX\n";
        const GLchar sCondFrag[] = "#define FRAGMENT\n";
        const GLchar *vsrc[] = { sCondVert, buf };
        const GLchar *fsrc[] = { sCondFrag, buf };
        GLint vcnt[] = { static_cast<GLint>(strlen(sCondVert)), cnt };
        GLint fcnt[] = { static_cast<GLint>(strlen(sCondFrag)), cnt };
        glShaderSource(vert, 2, vsrc, vcnt);
        glShaderSource(frag, 2, fsrc, fcnt);
        glCompileShader(vert);
        glCompileShader(frag);
        delete[] buf;

        GLint result;
        glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE) {
            GLchar buf[256];
            int len;
            glGetShaderInfoLog(vert, 256, &len, buf);
            printf("%s\n", buf);
            return 0;
        }

        glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE) {
            GLchar buf[256];
            int len;
            glGetShaderInfoLog(frag, 256, &len, buf);
            printf("%s\n", buf);
            return 0;
        }

        glAttachShader(prog, vert);
        glAttachShader(prog, frag);
        glLinkProgram(prog);
    }
    return prog;
}
