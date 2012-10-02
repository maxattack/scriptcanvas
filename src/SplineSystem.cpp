#include "CommandSystem.h"
#include "util/CompactPool.h"
#include "util/CompactComponentPool.h"
#include "VectorMath.h"

// TYPES

struct MaterialSlot {
	Material mat;
	int refCount;
};

struct vertex_t {
    float uuu;
    float uu;
    float u;
    float s;

    void SetValue(float param, float side) {
    	u = param;
    	uu = u * u;
    	uuu = uu * u;
    	s = side;
    }
};

// GLOBALS

static GLuint mProgram;
static GLuint mAttribParameterAndSide;
static GLuint mUniformThickness;
static GLuint mUniformPositionMatrix;
static GLuint mUniformNormalMatrix;
static GLuint mUniformColor;
static GLuint mUniformTaperStart;
static GLuint mUniformTaperDelta;
static GLuint mVertexBuffer;

static CompactComponentPool<ControlVertex> mComponents;
static CompactPool<MaterialSlot, kMaxMaterials> mMaterials;
static CompactPool<CubicSegment, kMaxSegments> mSegments;

// FUNCTIONS

void SplineSystem::Initialize() {
    mProgram = RenderSystem::LoadShaderProgram("src/cubic.glsl");
    glUseProgram(mProgram);
    mAttribParameterAndSide = glGetAttribLocation(mProgram, "parameterAndSide");
    mUniformThickness = glGetUniformLocation(mProgram, "thickness");
    mUniformPositionMatrix = glGetUniformLocation(mProgram, "positionMatrix");
    mUniformNormalMatrix = glGetUniformLocation(mProgram, "normalMatrix");
    mUniformColor = glGetUniformLocation(mProgram, "color");
    mUniformTaperStart = glGetUniformLocation(mProgram, "taperStart");
    mUniformTaperDelta = glGetUniformLocation(mProgram, "taperDelta");
    {
        vertex_t buf[kSegmentResolution];
        float sideBuffer[kSegmentResolution];
        float du = 1.f / 127.f;
        float u = 0;
        for(int i=0; i<kSegmentResolution>>1; ++i) {
            buf[i+i].SetValue(u, 0.5f);
            buf[i+i+1].SetValue(u, -0.5f);
            u += du;
        }
        glGenBuffers(1, &mVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * kSegmentResolution, buf, GL_STATIC_DRAW);
    }
    glUseProgram(0);
}

void SplineSystem::Destroy() {
	// TODO: Teardown
}

void SplineSystem::Update(CommandBuffer *vbuf) {
	// TODO: Culling
	for(auto p=mMaterials.Begin(); p!=mMaterials.End(); ++p) {
		vbuf->materials[vbuf->materialCount] = p->mat;
		vbuf->materialCount++;
	}
	for(auto p=mSegments.Begin(); p!=mSegments.End(); ++p) {
		float taperStart = Taper(p->start);
		CubicSegmentCommand cmd = { 
			0, 
			mMaterials.Index(p->material), 
			SceneSystem::Index(p->start), 
			SceneSystem::Index(p->end),
			taperStart,
			Taper(p->end)-taperStart
		 };
		vbuf->cubicSegments[vbuf->cubicSegmentCount] = cmd;
		vbuf->cubicSegmentCount++;
	}
}

void SplineSystem::Render(CommandBuffer *vbuf) {
	if (vbuf->cubicSegmentCount) {
	    glUseProgram(mProgram);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableVertexAttribArray(mAttribParameterAndSide);
	    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	    glLoadIdentity();
	    // TODO: sort by material?
	    for(int i=0; i<vbuf->cubicSegmentCount; ++i) {
	    	auto& segment = vbuf->cubicSegments[i];
	    	auto& mat = vbuf->materials[segment.material];
	    	auto& start = vbuf->transform_ts[segment.start].t;
	    	auto& end = vbuf->transform_ts[segment.end].t;

	    	auto p0 = Vec4(start.t);
	    	auto p1 = Vec4(end.t);
	    	auto t0 = Vec4(start.q);
	    	auto t1 = Vec4(end.q);

	    	//auto posMatrix = HermiteMatrix(p0, p1, t0, t1);
	    	//auto normMatrix = HermiteNormMatrix(p0, p1, t0, t1);

	    	auto d = Vec4((end.t - start.t).Clockwise());

	    	float e = Lerp(-0.2f, 0.2f, 0.5f + 0.5f * sinf(4.f * 3.14159f * glfwGetTime()));
	    	auto m = p0 + 0.5f * (p1 - p0) + e * d;

	    	auto posMatrix = QuadraticBezierMatrix(p0, m, p1);
	    	auto normMatrix = QuadraticBezierNormMatrix(p0, m, p1);

	    	//LOG_FLOAT(mat.weight);

		    glUniform1f(mUniformThickness, mat.weight);
		    glUniform1f(mUniformTaperStart, segment.taperStart);
		    glUniform1f(mUniformTaperDelta, segment.taperDelta);
		    glUniformMatrix4fv(mUniformPositionMatrix, 1, GL_FALSE, posMatrix.m);
		    glUniformMatrix4fv(mUniformNormalMatrix, 1, GL_FALSE, normMatrix.m);
			
			float r,g,b;
			mat.color.ToFloatRGB(&r, &g, &b);
			glUniform4f(mUniformColor, r, g, b, 1.f);

		    glVertexAttribPointer(mAttribParameterAndSide, 4, GL_FLOAT, GL_FALSE, 0, 0);

		    glDrawArrays(GL_TRIANGLE_STRIP, 0, kSegmentResolution);
	    }

	    glDisableClientState(GL_VERTEX_ARRAY);
	    glDisableVertexAttribArray(mAttribParameterAndSide);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glUseProgram(0);
	}
}


ID SplineSystem::CreateMaterial(float weight, color_t color) {
	auto result = mMaterials.TakeOut();
	mMaterials[result].refCount = 0;
	mMaterials[result].mat.weight = weight;
	mMaterials[result].mat.color = color;
	return result;
}

Material& SplineSystem::GetMaterial(ID mid) {
	return mMaterials[mid].mat;
}

void SplineSystem::DestroyMaterial(ID mid) {
	// remove references from segments using this material
	if (mMaterials[mid].refCount > 0) {
		for(auto p=mSegments.Begin(); p!=mSegments.End(); ++p) {
			if (p->material == mid) {
				p->material = 0;
			}
		}
	}
	mMaterials.PutBack(mid);
}

static void CreateControlVertex(ID node) {
	mComponents.Alloc(node);
	mComponents[node].refCount = 0;
	mComponents[node].taper = 1.f;
}

ControlVertex& SplineSystem::GetControlVertex(ID node) {
	return mComponents[node];
}

ID SplineSystem::CreateCubicSegment(ID start, ID end, ID mat) {
	using namespace SceneSystem;
	ASSERT(start != end);
	if (!HasComponent(start, kComponentSpline)) {
		AddComponent(start, kComponentSpline);
		CreateControlVertex(start);
	}
	if (!HasComponent(end, kComponentSpline)) {
		AddComponent(end, kComponentSpline);
		CreateControlVertex(end);
	}
	mComponents[start].refCount++;
	mComponents[end].refCount--;

	auto result = mSegments.TakeOut();
	mSegments[result].start = start;
	mSegments[result].end = end;
	mSegments[result].material = mat;
	return result;
}

void SplineSystem::SetCubicSegmentMaterial(ID sid, ID mid) {
	ASSERT(mSegments.IsActive(sid));
	ASSERT(mMaterials.IsActive(mid));
	auto& seg = mSegments[sid];
	if (!seg.material || seg.material != mid) {
		ASSERT(mMaterials[seg.material].refCount > 0);
		mMaterials[seg.material].refCount--;
		mSegments[sid].material = mid;
		mMaterials[mid].refCount++;
	}
}

CubicSegment SplineSystem::GetCubicSegment(ID sid) {
	return mSegments[sid];
}

void SplineSystem::DestroyCubicSegment(ID sid) {
	using namespace SceneSystem;
	auto& seg = mSegments[sid];
	if (seg.material) {
		mMaterials[seg.material].refCount--;
	}
	mComponents[seg.start].refCount--;
	if (mComponents[seg.start].refCount == 0) {
		RemoveComponent(seg.start, kComponentSpline);
	}
	mComponents[seg.end].refCount--;
	if (mComponents[seg.end].refCount == 0) {
		RemoveComponent(seg.end, kComponentSpline);
	}
	mSegments.PutBack(sid);
}

void SplineSystem::OnNodeDestroyed(ID node) {
	// remove any segments using this node
	if (mComponents[node].refCount > 0) {
		auto p = mSegments.Begin();
		while(p != mSegments.End()) {
			if (p->start == node || p->end == node) {
				DestroyCubicSegment(mSegments.GetID(p));
			} else {
				++p;
			}
		}
	}
	mComponents.Free(node);
}


