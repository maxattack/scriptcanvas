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

#include "CommandSystem.h"
#include "Shaders.h"
#include "util/CompactPool.h"
#include "util/CompactComponentPool.h"
#include "VectorMath.h"

// TYPES

struct vertex_t {
    float x;
    float y;
    float z;
    float w;

    inline void SetValue(float u) {
    	x = u*u*u;
    	y = u*u;
    	z = u;
    	w = 1.f;
    }
};

struct ControlVertex {
	int refCount;
};

// VECTORIZED PARAMETRIC CURVES
// These compute curves based on linear multiplication by a "cubic parameterc vector":
// U = < u^3, u^2, u, 1 >,

#define ORTH_ROTATION_MAT  (Mat4(0, 1, 0, 0, -1, 0, 0, 0))

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

static Shader::cubic_t mShader;
static GLuint mParameterBuffer;
static GLuint mSideBuffer;

static CompactComponentPool<ControlVertex> mVertices;
static CompactPool<Segment, kMaxSegments> mSegments;

// FUNCTIONS

void SplineSystem::Initialize() {
	mShader.Initialize();
    
    vertex_t buf[kSegmentResolution];
    float sbuf[kSegmentResolution];
    float du = 1.f / (kSegmentResolution/2.f-1.f);
    float u = 0;
    for(int i=0; i<kSegmentResolution>>1; ++i) {
        buf[i+i].SetValue(u);
        buf[i+i+1].SetValue(u);
        sbuf[i+i] = 1.f;
        sbuf[i+i+1] = -1.f;
        u += du;
    }

    glGenBuffers(1, &mParameterBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mParameterBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * kSegmentResolution, buf, GL_STATIC_DRAW);

    glGenBuffers(1, &mSideBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mSideBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * kSegmentResolution, sbuf, GL_STATIC_DRAW);
}

void SplineSystem::Destroy() {
	mShader.Destroy();
	// TODO: Teardown VB
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
	    glUseProgram(mShader.handle);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableVertexAttribArray(mShader.parameter);
	    glEnableVertexAttribArray(mShader.side);
	    glLoadIdentity();
	    
	    glBindBuffer(GL_ARRAY_BUFFER, mParameterBuffer);
	    glVertexAttribPointer(mShader.parameter, 4, GL_FLOAT, GL_FALSE, 0, 0);
	    glBindBuffer(GL_ARRAY_BUFFER, mSideBuffer);
	    glVertexAttribPointer(mShader.side, 1, GL_FLOAT, GL_FALSE, 0, 0);


	    // TODO: sort by material?
	    // --> if we sort by 'eccentricity' we can avoid the isfinite() branch
	    // --> if we store the stroke differently (standard default?)

	    for(int i=0; i<vbuf->segmentCount; ++i) {
	    	auto& segment = vbuf->segments[i];
	    	auto& mat = vbuf->materials[segment.mid];
	    	auto& start = vbuf->transforms[segment.start];
	    	auto& end = vbuf->transforms[segment.end];
	    	if (isfinite(segment.eccentricity)) {
		    	auto p0 = Vec4(start.translation.x, start.translation.y, start.depth);
		    	auto p1 = Vec4(end.translation.x, end.translation.y, end.depth);
		    	auto offset = end.translation - start.translation;
		    	auto m = Vec4( start.translation + 0.5f * offset + segment.eccentricity * offset.Clockwise() );
		    	auto posMatrix = QuadraticBezierMatrix(p0, m, p1);
		    	auto normMatrix = QuadraticBezierNormMatrix(p0, m, p1);
			    glUniformMatrix4fv(mShader.positionMatrix, 1, GL_FALSE, posMatrix.m);
			    glUniformMatrix4fv(mShader.normalMatrix, 1, GL_FALSE, normMatrix.m);
	    	} else {
		    	auto p0 = Vec4(start.translation.x, start.translation.y, start.depth, 1);
		    	auto p1 = Vec4(end.translation.x, end.translation.y, end.depth, 1);
		    	auto t0 = Vec4(start.attitude);
		    	auto t1 = Vec4(end.attitude);
		    	auto posMatrix = HermiteMatrix(p0, p1, t0, t1);
		    	auto normMatrix = HermiteNormMatrix(p0, p1, t0, t1);
			    glUniformMatrix4fv(mShader.positionMatrix, 1, GL_FALSE, posMatrix.m);
			    glUniformMatrix4fv(mShader.normalMatrix, 1, GL_FALSE, normMatrix.m);
	    	}
	    	if (segment.stroke != 0xffff) {
		    	auto& stroke = vbuf->strokes[segment.stroke];
		    	auto strokeVector = mat.weight * EccentricStrokeVector(stroke.start, stroke.eccentricity, stroke.end);
		    	glUniform4f(mShader.strokeVector, strokeVector.x, strokeVector.y, strokeVector.z, strokeVector.w);
	    	} else {
	    		glUniform4f(mShader.strokeVector, 0, 0, 0, mat.weight);
	    	}

			float r,g,b;
			mat.color.ToFloatRGB(&r, &g, &b);
			glUniform4f(mShader.color, r, g, b, 1.f);
		    glDrawArrays(GL_TRIANGLE_STRIP, 0, kSegmentResolution);
	    }

	    glDisableVertexAttribArray(mShader.parameter);
	    glDisableVertexAttribArray(mShader.side);
	    glDisableClientState(GL_VERTEX_ARRAY);
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
