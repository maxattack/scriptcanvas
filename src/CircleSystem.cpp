#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "RenderSystem.h"

namespace CircleSystem {

static bool mInitialized = false;
static GLuint mProgram;
static GLuint mAttribUnit;
static GLuint mUniformRadius;
static GLuint mUniformColor;
static GLuint mVertexBuffer;
static CompactComponentPool<Component> mComponents;
static CompactPool<Geometry, MAX_NODES> mGeometry;
static CompactPool<Material, MAX_NODES> mMaterial;

static GLuint LoadShaderProgram(const char* filename);

bool Manager::CreateComponent(ID node) {
	mComponents.Alloc(node);
    mComponents[node].node = node;
    return true;
}

bool Manager::DestroyComponent(ID node) {
    mComponents.Free(node);
    return true;
}

ID CreateMaterial(float r, float g, float b) { 
    ID result = mMaterial.TakeOut(); 
    mMaterial[result].r = r;
    mMaterial[result].g = g;
    mMaterial[result].b = b;
    mMaterial[result].a = 1.f;
    return result;
}

void SetMaterial(ID node, ID mat) {
    ASSERT(mMaterial.IsActive(mat));
    mComponents[node].material = mat;
}

ID GetMaterial(ID node) {
    return mComponents[node].material;
}

void SetMaterialColor(ID mat, float r, float g, float b){
    mMaterial[mat].r = r;
    mMaterial[mat].g = g;
    mMaterial[mat].b = b;
}

void DestroyMaterial(ID matId) { 
    mMaterial.PutBack(matId); 
    for(auto p=mComponents.Begin(); p!=mComponents.End(); ++p) {
        p->material *= (p->material == matId);
    }
}

ID CreateGeometry(float radius) { 
    ID result = mGeometry.TakeOut(); 
    mGeometry[result].radius = radius;
    return result;
}

void SetGeometry(ID node, ID geom) {
    ASSERT(mGeometry.IsActive(geom));
    mComponents[node].geometry = geom;
}

ID GetGeometry(ID node) {
    return mComponents[node].geometry;
}

void SetGeometryRadius(ID geomId, float radius) {
    mGeometry[geomId].radius = radius;
}

void DestroyGeometry(ID geomId) { 
    mGeometry.PutBack(geomId); 
    for (auto p=mComponents.Begin(); p!=mComponents.End(); ++p) {
        p->geometry *=  (p->geometry == geomId);
    }
}

void Update(RenderBuffer* vbuf) {
    for(int i=0; i<mGeometry.Count(); ++i) {
        vbuf->circleGeometry[i] = mGeometry.Begin()[i];
    }
    for(int i=0; i<mMaterial.Count(); ++i) {
        vbuf->circleMaterials[i] = mMaterial.Begin()[i];
    }
    int idx=0;
    for(auto p=mComponents.Begin(); p!=mComponents.End(); ++p) {
        if (p->material && p->geometry) {
            Command cmd;
            cmd.fields.queue = 0;
            cmd.fields.material = mMaterial.GetIndex(p->material);
            cmd.fields.geometry = mGeometry.GetIndex(p->geometry);
            cmd.fields.transform = SceneSystem::GetIndex(p->node);
            vbuf->circleCommands[idx++] = cmd;
        }
    }
    vbuf->circleCount = idx;
}

void Render(RenderBuffer* vbuf) {
    if (vbuf->circleCount) {
        if (!mInitialized) {
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
            mInitialized = true;
        }

        glUseProgram(mProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableVertexAttribArray(mAttribUnit);

        for(int i=0; i<vbuf->circleCount; ++i) {
            auto cmd = vbuf->circleCommands[i];
            Transform w = vbuf->transforms[cmd.fields.transform];
            float mat[16] = {
                w.q.x, w.q.y, 0, 0,
                -w.q.y, w.q.x, 0, 0,
                0, 0, 1, 0,
                w.t.x, w.t.y, 0, 1
            };
            glLoadMatrixf(mat);
            glUniform1f(mUniformRadius, vbuf->circleGeometry[cmd.fields.geometry].radius);
            auto material = vbuf->circleMaterials[cmd.fields.material];
            glUniform4f(mUniformColor, material.r, material.g, material.b, 1.f);

            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glVertexAttribPointer(mAttribUnit, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 64);
        }

        // clean up opengl state
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableVertexAttribArray(mAttribUnit);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);

    } else { /* teardown when not in use? */ }
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

}
