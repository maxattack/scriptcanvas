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

struct ControlVertex {
	int refCount;
};

// VECTORIZED PARAMETRIC CURVES
// These compute curves based on linear multiplication by a "cubic parameterc vector":
// U = < u^3, u^2, u, 1 >,

#define ORTH_ROTATION_MAT  (Mat4(0, 1, 0, 0, -1, 0, 0, 0))
//#define IDENTITY_MAT  (Mat4())

inline mat4 HermiteMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
	return Mat4(p0, p1, t0, t1) * Mat4(
    2, -2, 1, 1, 
    -3, 3, -2, -1, 
    0, 0, 1, 0, 
    1, 0, 0, 0
  );
}

inline mat4 HermiteDerivMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return Mat4(p0, p1, t0, t1) * Mat4(
    0, 0, 0, 0, 
    6, -6, 3, 3, 
    -6, 6, -4, -2, 
    0, 0, 1, 0
  );
}

inline mat4 HermiteNormMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return ORTH_ROTATION_MAT * HermiteDerivMatrix(p0, p1, t0, t1);
}

inline mat4 BezierMatrix(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
	return Mat4(p0, p1, p2, p3) * Mat4(
    -1, 3, -3, 1, 
    3, -6, 3, 0, 
    -3, 3, 0, 0, 
    1, 0, 0, 0
  );
}

inline mat4 BezierDerivMatrix(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
  return Mat4(p0, p1, p2, p3) * Mat4(
    0, 0, 0, 0, 
    -3, 9, -9, 3, 
    6, -12, 6, 0, 
    -3, 3, 0, 0
  );
}

inline mat4 BezierNormMatrix(vec4 p0, vec4 p1, vec4 t0, vec4 t1) {
  return ORTH_ROTATION_MAT * BezierDerivMatrix(p0, p1, t0, t1);
}

inline mat4 QuadraticBezierMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return Mat4(Vec4(), p0, p1, p2) * Mat4(
    0, 0, 0, 0,
    0, 1, -2, 1,
    0, -2, 2, 0,
    0, 1, 0, 0
  );
}

inline mat4 QuadraticBezierDerivMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return Mat4(Vec4(), p0, p1, p2) * Mat4(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 2, -4, 2,
    0, -2, 2, 0
  );
}

inline mat4 QuadraticBezierNormMatrix(vec4 p0, vec4 p1, vec4 p2) {
  return ORTH_ROTATION_MAT * QuadraticBezierDerivMatrix(p0, p1, p2);
}

inline vec4 NonStrokeingVector(float t) {
	// booooring non-strokeing thickness vector
	return Vec4(0, 0, 0, t);
}

inline vec4 StrokeVector(float t0, float t1) {
	// dotted with cubic parametric vector to produce a linear strokeing curve
	return Vec4(0, 0, t1-t0, t0);
}

inline vec4 EccentricStrokeVector(float t0, float e, float t1) {
	// dotted with cubic parametric vector to produce a parabolic strokeing curve
	return Vec4(0, -e-e-e-e, e+e+e+e+t1-t0, t0);
}

// Use cubic parameter for teardrop strokeing?  InflectedStrokeVector?

// GLOBALS

static GLuint mProgram;
static GLuint mAttribParameterAndSide;
static GLuint mUniformPositionMatrix;
static GLuint mUniformNormalMatrix;
static GLuint mUniformColor;
static GLuint mUniformStrokeVector;
static GLuint mVertexBuffer;

static CompactComponentPool<ControlVertex> mVertices;
static CompactPool<Segment, kMaxSegments> mSegments;

// FUNCTIONS

void SplineSystem::Initialize() {
    mProgram = RenderSystem::LoadShaderProgram("src/cubic.glsl");
    glUseProgram(mProgram);
    mAttribParameterAndSide = glGetAttribLocation(mProgram, "parameterAndSide");
    mUniformPositionMatrix = glGetUniformLocation(mProgram, "positionMatrix");
    mUniformNormalMatrix = glGetUniformLocation(mProgram, "normalMatrix");
    mUniformColor = glGetUniformLocation(mProgram, "color");
    mUniformStrokeVector = glGetUniformLocation(mProgram, "strokeVector");
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
    glUseProgram(0);
}

void SplineSystem::Destroy() {
	// TODO: Teardown
}

void SplineSystem::Update(CommandBuffer *vbuf) {
	// TODO: Culling?

	// write segments
	auto cmd = &(vbuf->segments[vbuf->segmentCount]);
	for(auto p=mSegments.Begin(); p!=mSegments.End(); ++p) {
		cmd->mid = MaterialSystem::Index(p->material);
		cmd->start = NodeSystem::Index(p->start);
		cmd->end = NodeSystem::Index(p->end);
		cmd->stroke = p->stroke ? StrokeSystem::Index(p->stroke) : 0xffff;
		cmd->eccentricity = p->eccentricity;
		vbuf->segmentCount++;
		cmd++;
	}
}

void SplineSystem::Render(CommandBuffer *vbuf) {
	if (vbuf->segmentCount) {
	    glUseProgram(mProgram);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableVertexAttribArray(mAttribParameterAndSide);
	    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	    glLoadIdentity();
	    
	    // TODO: sort by material?

	    for(int i=0; i<vbuf->segmentCount; ++i) {
	    	auto& segment = vbuf->segments[i];
	    	auto& mat = vbuf->materials[segment.mid];
	    	auto& start = vbuf->transforms[segment.start].t;
	    	auto& end = vbuf->transforms[segment.end].t;
	    	if (isfinite(segment.eccentricity)) {
		    	auto p0 = Vec4(start.translation);
		    	auto p1 = Vec4(end.translation);
		    	auto offset = end.translation - start.translation;
		    	auto m = Vec4( start.translation + 0.5f * offset + segment.eccentricity * offset.Clockwise() );
		    	auto posMatrix = QuadraticBezierMatrix(p0, m, p1);
		    	auto normMatrix = QuadraticBezierNormMatrix(p0, m, p1);
			    glUniformMatrix4fv(mUniformPositionMatrix, 1, GL_FALSE, posMatrix.m);
			    glUniformMatrix4fv(mUniformNormalMatrix, 1, GL_FALSE, normMatrix.m);
	    	} else {
		    	auto p0 = Vec4(start.translation);
		    	auto p1 = Vec4(end.translation);
		    	auto t0 = Vec4(start.attitude);
		    	auto t1 = Vec4(end.attitude);
		    	auto posMatrix = HermiteMatrix(p0, p1, t0, t1);
		    	auto normMatrix = HermiteNormMatrix(p0, p1, t0, t1);
			    glUniformMatrix4fv(mUniformPositionMatrix, 1, GL_FALSE, posMatrix.m);
			    glUniformMatrix4fv(mUniformNormalMatrix, 1, GL_FALSE, normMatrix.m);
	    	}
	    	if (segment.stroke != 0xffff) {
		    	auto& stroke = vbuf->strokes[segment.stroke];
		    	auto strokeVector = mat.weight * EccentricStrokeVector(stroke.start, stroke.eccentricity, stroke.end);
		    	glUniform4f(mUniformStrokeVector, strokeVector.x, strokeVector.y, strokeVector.z, strokeVector.w);
	    	} else {
	    		glUniform4f(mUniformStrokeVector, 0, 0, 0, mat.weight);
	    	}

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
	NodeSystem::AddComponent(node, kComponentSpline);
	mVertices.Alloc(node);
	mVertices[node].refCount = 0;
}

void SplineSystem::OnNodeDestroyed(ID node) {
	// remove any segments using this node
	if (mVertices[node].refCount > 0) {
		mVertices[node].refCount++; // back to make sure we don't get recursively called
		auto p = mSegments.Begin();
		while(p != mSegments.End()) {
			if (p->start == node || p->end == node) {
				DestroySegment(mSegments.GetID(p));
			} else {
				++p;
			}
		}
	}
	mVertices.Free(node);
}

ID SplineSystem::CreateSegment(ID start, ID end, ID mat, ID stroke, float eccentricity) {
	ASSERT(start != end);
	ASSERT(MaterialSystem::MaterialValid(mat));
	ASSERT(stroke == 0 || StrokeSystem::StrokeValid(stroke));
	if (!NodeSystem::HasComponent(start, kComponentSpline)) { CreateControlVertex(start); }
	if (!NodeSystem::HasComponent(end, kComponentSpline)) { CreateControlVertex(end); }
	mVertices[start].refCount++;
	mVertices[end].refCount++;
	auto result = mSegments.TakeOut();
	mSegments[result].start = start;
	mSegments[result].end = end;
	mSegments[result].material = mat;
	mSegments[result].stroke = stroke;
	mSegments[result].eccentricity = eccentricity;
	return result;
}

Segment& SplineSystem::GetSegment(ID sid) {
	return mSegments[sid];
}

void SplineSystem::DestroySegment(ID sid) {
	auto& seg = mSegments[sid];
	mVertices[seg.start].refCount--;
	if (mVertices[seg.start].refCount == 0) {
		NodeSystem::RemoveComponent(seg.start, kComponentSpline);
	}
	mVertices[seg.end].refCount--;
	if (mVertices[seg.end].refCount == 0) {
		NodeSystem::RemoveComponent(seg.end, kComponentSpline);
	}
	mSegments.PutBack(sid);
}
