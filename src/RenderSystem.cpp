#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "RenderSystem.h"

namespace RenderSystem {

static GLuint LoadShaderProgram(const char* filename);

void CircleSystem::Init() {
	mProgram = LoadShaderProgram("src/circle.glsl");
	// lookup shader storage locations
	glUseProgram(mProgram);
	mAttribUnit = glGetAttribLocation(mProgram, "unit");
	mUniformRadius = glGetUniformLocation(mProgram, "radius");
	mUniformColor = glGetUniformLocation(mProgram, "color");
	// create circle vertex buffer
	Vec unit = vec(1,0);
	Vec rotor = Polar(1.f, kTau / (64-2.f));
	Vec buffer[64];
	buffer[0] = vec(0,0);
	for(int i=1; i<64; ++i) {
		buffer[i] = unit;
		unit *= rotor;
	}
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec)*64, buffer, GL_STATIC_DRAW);
}

void CircleSystem::Render() {
    // A kinda fake function.  In reality, we will queue render
    // work from a game thread using a lockless queue.  In that case,
    // this method will perform those queue, not the actual opengl calls.

    // For now we only render one circle at a time.  We would really wanto
    // batch up to 32 at a time for better performance, but I'd rather get
    // the real RenderQueue implemented first before thinking about that kind
    // of performance.
    glUseProgram(mProgram);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(mAttribUnit);

    // iterate through all the circles
    for(auto *p=mData.Begin(); p!=mData.End(); ++p) {
        
        // THIS IS WHAT REALLY NEEDS TO CHANGE -- World transforms
        // need to be batch-computed, not redone per-renderable per-frame
        // in an arbitrary order (want data-oriented, not object-oriented).
        Transform w = SceneSystem::WorldPose(p->node);
        float mat[16] = {
            w.q.x, w.q.y, 0, 0,
            -w.q.y, w.q.x, 0, 0,
            0, 0, 1, 0,
            w.t.x, w.t.y, 0, 1
        };
        glLoadMatrixf(mat);
        //

        glUniform1f(mUniformRadius, p->radius);
        glUniform4f(mUniformColor, p->r, p->g, p->b, 1.f);

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glVertexAttribPointer(mAttribUnit, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 64);
    }

    // clean up opengl state
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(mAttribUnit);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void CircleSystem::Dispose() {
    // TODO: Cleanup OpenGL Stuff
}

void CircleSystem::CreateComponent(ID node) {
	mData.Alloc(node);
    mData[node].node = node;
}

void CircleSystem::DestroyComponent(ID node) {
	mData.Free(node);
}

Circle& CircleSystem::operator[](ID node) {
	return mData[node];
}

static GLuint LoadShaderProgram(const char* filename) {
    GLuint prog, vert, frag;
    unsigned cnt = 0;
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
        GLint vcnt[] = { strlen(sCondVert), cnt };
        GLint fcnt[] = { strlen(sCondFrag), cnt };
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

}