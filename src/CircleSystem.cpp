#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "RenderSystem.h"

static bool mInitialized = false;
static GLuint mProgram;
static GLuint mAttribUnit;
static GLuint mUniformRadius;
static GLuint mUniformColor;
static GLuint mVertexBuffer;
static CompactComponentPool<CircleComponent> mComponents;
static CompactPool<CircleGeometry, MAX_NODES> mGeometry;
static CompactPool<CircleMaterial, MAX_NODES> mMaterial;

static GLuint LoadShaderProgram(const char* filename);

bool CreateCircleComponent(ID node) {
	mComponents.Alloc(node);
    mComponents[node].node = node;
    return true;
}

bool DestroyCircleComponent(ID node) {
	mComponents.Free(node);
    return true;
}

void UpdateCircleSystem(RenderBuffer* vbuf) {
    for(int i=0; i<mGeometry.Count(); ++i) {
        vbuf->circleGeometry[i] = mGeometry.Begin()[i];
    }
    for(int i=0; i<mMaterial.Count(); ++i) {
        vbuf->circleMaterials[i] = mMaterial.Begin()[i];
    }
    for(int i=0; i<mComponents.Count(); ++i) {
        auto& c = mComponents.Begin()[i];
        CircleCommand cmd;
        cmd.fields.queue = 0;
        cmd.fields.material = mMaterial.GetIndex(c.material);
        cmd.fields.geometry = mGeometry.GetIndex(c.geometry);
        cmd.fields.transform = GetIndex(c.node);
        vbuf->circleCommands[i] = cmd;
    }
    vbuf->circleCount = mComponents.Count();
}

void RenderCircleSystem(RenderBuffer* vbuf) {
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

        // TODO: Batch!!
        // std::sort(
        //  &vbuf->circleCommands[0].id, 
        //  &vbuf->circleCommands[vbuf->circleCount].id,
        //  std::less<uint64_t>()
        // );
        // construct circle batches until we overflow some capacity:
        //  -- 32 total circles
        //  -- 8 distinct transforms
        //  -- 8 distinct geometries
        //  -- 8 distinct materials
        // int batchCircleCount = 0;
        // int batchTransformCount = 0;
        // int batchGeometryCount = 0;
        // int batchMaterialCount = 0;
        // for(int i=0; i<vbuf->circleCount; ++i) {
        // }        

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

CircleComponent& GetCircleComponent(ID node) {
	return mComponents[node];
}

ID CreateCircleMaterial(float r, float g, float b) { 
    ID result = mMaterial.TakeOut(); 
    mMaterial[result].r = r;
    mMaterial[result].g = g;
    mMaterial[result].b = b;
    mMaterial[result].a = 1.f;
    return result;
}

CircleMaterial& GetCircleMaterial(ID matId) { 
    return mMaterial[matId]; 
}

void DestroyCircleMaterial(ID matId) { 
    mMaterial.PutBack(matId); 
    for(auto p=mComponents.Begin(); p!=mComponents.End(); ++p) {
        p->material *= (p->material == matId);
    }
}

ID CreateCircleGeometry(float radius) { 
    ID result = mGeometry.TakeOut(); 
    mGeometry[result].radius = radius;
    return result;
}

CircleGeometry& GetCircleGeometry(ID geomId) { 
    return mGeometry[geomId]; 
}

void DestroyCircleGeometry(ID geomId) { 
    mGeometry.PutBack(geomId); 
    for (auto p=mComponents.Begin(); p!=mComponents.End(); ++p) {
        p->geometry *=  (p->geometry == geomId);
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
