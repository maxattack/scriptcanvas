#include "CommandSystem.h"
#include "util/CompactPool.h"
#include "util/CompactComponentPool.h"
#include "VectorMath.h"

// TYPES

struct vertex_t {
    float uuu;
    float uu;
    float u;
    float s;

    inline void SetValue(float param, float side) {
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
static CompactPool<Segment, kMaxSegments> mCubicSegments;
static CompactPool<EccentricSegment, kMaxSegments> mEccentricSegments;

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
        float du = 1.f / 127.f;
        float u = 0;
        for(int i=0; i<kSegmentResolution>>1; ++i) {
            buf[i+i].SetValue(u, 1.f);
            buf[i+i+1].SetValue(u, -1.f);
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
	auto cmd = &(vbuf->cubicSegments[vbuf->cubicSegmentCount]);
	for(auto p=mCubicSegments.Begin(); p!=mCubicSegments.End(); ++p) {
		float taperStart = Taper(p->start);
		cmd->mid = MaterialSystem::Index(p->material);
		cmd->start = SceneSystem::Index(p->start);
		cmd->end = SceneSystem::Index(p->end);
		cmd->taperStart = taperStart;
		cmd->taperDelta = Taper(p->end)-taperStart;
		vbuf->cubicSegmentCount++;
		cmd++;
	}
	auto ecmd = &(vbuf->eccentricSegments[vbuf->eccentricSegmentCount]);
	for(auto p=mEccentricSegments.Begin(); p!=mEccentricSegments.End(); ++p) {
		float taperStart = Taper(p->start);
		ecmd->mid = MaterialSystem::Index(p->material);
		ecmd->start = SceneSystem::Index(p->start);
		ecmd->end = SceneSystem::Index(p->end);
		ecmd->taperStart = taperStart;
		ecmd->taperDelta = Taper(p->end)-taperStart;
		ecmd->eccentricity = p->eccentricity;
		vbuf->eccentricSegmentCount++;
		ecmd++;
	}
}

void SplineSystem::Render(CommandBuffer *vbuf) {
	if (vbuf->cubicSegmentCount || vbuf->eccentricSegmentCount) {
	    glUseProgram(mProgram);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableVertexAttribArray(mAttribParameterAndSide);
	    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	    glLoadIdentity();
	    // TODO: sort by material?
	    for(int i=0; i<vbuf->cubicSegmentCount; ++i) {
	    	auto& segment = vbuf->cubicSegments[i];
	    	auto& mat = vbuf->materials[segment.mid];
	    	auto& start = vbuf->transforms[segment.start].t;
	    	auto& end = vbuf->transforms[segment.end].t;

	    	auto p0 = Vec4(start.t);
	    	auto p1 = Vec4(end.t);
	    	auto t0 = Vec4(start.q);
	    	auto t1 = Vec4(end.q);

	    	auto posMatrix = HermiteMatrix(p0, p1, t0, t1);
	    	auto normMatrix = HermiteNormMatrix(p0, p1, t0, t1);

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

	    for(int i=0; i<vbuf->eccentricSegmentCount; ++i) {
	    	auto& segment = vbuf->eccentricSegments[i];
	    	auto& mat = vbuf->materials[segment.mid];
	    	auto& start = vbuf->transforms[segment.start].t;
	    	auto& end = vbuf->transforms[segment.end].t;

	    	auto p0 = Vec4(start.t);
	    	auto p1 = Vec4(end.t);

	    	auto offset = end.t - start.t;
	    	auto m = Vec4( start.t + 0.5f * offset + segment.eccentricity * offset.Clockwise() );

	    	auto posMatrix = QuadraticBezierMatrix(p0, m, p1);
	    	auto normMatrix = QuadraticBezierNormMatrix(p0, m, p1);

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


static void CreateControlVertex(ID node) {
	SceneSystem::AddComponent(node, kComponentSpline);
	mComponents.Alloc(node);
	mComponents[node].refCount = 0;
	mComponents[node].taper = 1.f;
}

ControlVertex& SplineSystem::GetControlVertex(ID node) {
	return mComponents[node];
}


void SplineSystem::OnNodeDestroyed(ID node) {
	// remove any segments using this node
	if (mComponents[node].refCount > 0) {
		mComponents[node].refCount++; // back to make sure we don't get recursively called
		auto p = mCubicSegments.Begin();
		while(p != mCubicSegments.End()) {
			if (p->start == node || p->end == node) {
				DestroyCubicSegment(mCubicSegments.GetID(p));
			} else {
				++p;
			}
		}
		auto i = mEccentricSegments.Begin();
		while(i != mEccentricSegments.End()) {
			if (i->start == node || i->end == node) {
				DestroyEccentricSegment(mEccentricSegments.GetID(i));
			} else {
				++i;
			}
		}
	}
	mComponents.Free(node);
}

ID SplineSystem::CreateCubicSegment(ID start, ID end, ID mat) {
	ASSERT(start != end);
	if (!SceneSystem::HasComponent(start, kComponentSpline)) { CreateControlVertex(start); }
	if (!SceneSystem::HasComponent(end, kComponentSpline)) { CreateControlVertex(end); }
	mComponents[start].refCount++;
	mComponents[end].refCount--;
	auto result = mCubicSegments.TakeOut();
	mCubicSegments[result].start = start;
	mCubicSegments[result].end = end;
	mCubicSegments[result].material = mat;
	return result;
}

void SplineSystem::SetCubicSegmentMaterial(ID csid, ID mid) {
	ASSERT(mCubicSegments.IsActive(csid));
	ASSERT(MaterialSystem::MaterialValid(mid));
	auto& seg = mCubicSegments[csid].material = mid;
}

Segment SplineSystem::GetCubicSegment(ID csid) {
	return mCubicSegments[csid];
}

void SplineSystem::DestroyCubicSegment(ID csid) {
	auto& seg = mCubicSegments[csid];
	mComponents[seg.start].refCount--;
	if (mComponents[seg.start].refCount == 0) {
		SceneSystem::RemoveComponent(seg.start, kComponentSpline);
	}
	mComponents[seg.end].refCount--;
	if (mComponents[seg.end].refCount == 0) {
		SceneSystem::RemoveComponent(seg.end, kComponentSpline);
	}
	mCubicSegments.PutBack(csid);
}


ID SplineSystem::CreateEccentricSegment(ID start, ID end, ID mat, float eccentricity) {
	ASSERT(start != end);
	if (!SceneSystem::HasComponent(start, kComponentSpline)) { CreateControlVertex(start); }
	if (!SceneSystem::HasComponent(end, kComponentSpline)) { CreateControlVertex(end); }
	mComponents[start].refCount++;
	mComponents[end].refCount--;
	auto result = mEccentricSegments.TakeOut();
	mEccentricSegments[result].start = start;
	mEccentricSegments[result].end = end;
	mEccentricSegments[result].material = mat;
	mEccentricSegments[result].eccentricity = eccentricity;
	return result;
}

void SplineSystem::SetEccentricSegmentMaterial(ID esid, ID mid) {
	ASSERT(mEccentricSegments.IsActive(esid));
	ASSERT(MaterialSystem::MaterialValid(mid));
	auto& seg = mCubicSegments[esid].material = mid;
}

float SplineSystem::Eccentricity(ID esid) {
	return mEccentricSegments[esid].eccentricity;
}

void SplineSystem::SetEccentricity(ID esid, float e) {
	mEccentricSegments[esid].eccentricity = e;
}

void SplineSystem::DestroyEccentricSegment(ID esid) {
	auto& seg = mEccentricSegments[esid];
	mComponents[seg.start].refCount--;
	if (mComponents[seg.start].refCount == 0) {
		SceneSystem::RemoveComponent(seg.start, kComponentSpline);
	}
	mComponents[seg.end].refCount--;
	if (mComponents[seg.end].refCount == 0) {
		SceneSystem::RemoveComponent(seg.end, kComponentSpline);
	}
	mCubicSegments.PutBack(esid);
}

EccentricSegment SplineSystem::GetEccentricSegment(ID esid) {
	return mEccentricSegments[esid];
}


