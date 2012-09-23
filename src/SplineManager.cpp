#include "SplineManager.h"
#include "RenderSystem.h"
#include "VectorMath.h"

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

StatusCode SplineManager::Initialize() {
    mProgram = RenderSystem::LoadShaderProgram("src/cubic.glsl");
    glUseProgram(mProgram);
    mAttribParameterAndSide = glGetAttribLocation(mProgram, "parameterAndSide");
    mUniformThickness = glGetUniformLocation(mProgram, "thickness");
    mUniformPositionMatrix = glGetUniformLocation(mProgram, "positionMatrix");
    mUniformNormalMatrix = glGetUniformLocation(mProgram, "normalMatrix");
    mUniformColor = glGetUniformLocation(mProgram, "color");
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
	return OK;
}

StatusCode SplineManager::CreateComponent(ID node) {
	mComponents.Alloc(node);
	mComponents[node].refCount = 0;
	return OK;
}

StatusCode SplineManager::DestroyComponent(ID node) {
	// remove any segments using this node
	if (mComponents[node].refCount > 0) {
		auto p = mSegments.Begin();
		while(p != mSegments.End()) {
			if (p->start == node || p->end == node) {
				DestroySegment(mSegments.GetID(p));
			} else {
				++p;
			}
		}
	}
	mComponents.Free(node);
	return OK;
}

ID SplineManager::CreateMaterial(float weight, color_t color) {
	auto result = mMaterials.TakeOut();
	mMaterials[result].refCount = 0;
	mMaterials[result].mat.weight = weight;
	mMaterials[result].mat.color = color;
	return result;
}

Material& SplineManager::GetMaterial(ID mid) {
	return mMaterials[mid].mat;
}

void SplineManager::DestroyMaterial(ID mid) {
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

ID SplineManager::CreateSegment(ID start, ID end, ID mid) {
	using namespace SceneSystem;
	ASSERT(start != end);
	if (!HasComponent(start, kComponentSpline)) {
		AddComponent(start, kComponentSpline);
	}
	if (!HasComponent(end, kComponentSpline)) {
		AddComponent(end, kComponentSpline);
	}
	mComponents[start].refCount++;
	mComponents[end].refCount--;

	auto result = mSegments.TakeOut();
	mSegments[result].start = start;
	mSegments[result].end = end;
	mSegments[result].material = mid;
	return result;
}

void SplineManager::SetMaterial(ID sid, ID mid) {
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

void SplineManager::DestroySegment(ID sid) {
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

StatusCode SplineManager::Update(RenderBuffer *vbuf) {
	// TODO: Culling
	for(auto p=mMaterials.Begin(); p!=mMaterials.End(); ++p) {
		vbuf->materials[vbuf->materialCount] = p->mat;
		vbuf->materialCount++;
	}
	for(auto p=mSegments.Begin(); p!=mSegments.End(); ++p) {
		SegmentCommand cmd = { 0, mMaterials.Index(p->material), SceneSystem::Index(p->start), SceneSystem::Index(p->end) };
		vbuf->segments[vbuf->segmentCount] = cmd;
		vbuf->segmentCount++;
	}
	return OK;
}

StatusCode SplineManager::Render(RenderBuffer *vbuf) {
	if (vbuf->segmentCount) {
	    glUseProgram(mProgram);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableVertexAttribArray(mAttribParameterAndSide);
	    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	    glLoadIdentity();
	    // TODO: sort by material?
	    for(int i=0; i<vbuf->segmentCount; ++i) {
	    	auto& segment = vbuf->segments[i];
	    	auto& mat = vbuf->materials[segment.material];
	    	auto& start = vbuf->transforms[segment.start].t;
	    	auto& end = vbuf->transforms[segment.end].t;

	    	auto p0 = Vec4(start.t);
	    	auto p1 = Vec4(end.t);
	    	auto t0 = Vec4(start.q);
	    	auto t1 = Vec4(end.q);

	    	auto posMatrix = HermiteMat(p0, p1, t0, t1);
	    	auto normMatrix = HermiteNormMat(p0, p1, t0, t1);
	    	//LOG_FLOAT(mat.weight);

		    glUniform1f(mUniformThickness, mat.weight);
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
	return OK;
}

StatusCode SplineManager::Destroy() {
	return OK;
}


