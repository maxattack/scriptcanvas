#include "RenderSystem.h"
#include "ProducerConsumerQueue.h"
#include <algorithm>
#include <functional>

static folly::ProducerConsumerQueue<RenderBuffer*, 2> mRenderQueue;
static folly::ProducerConsumerQueue<RenderBuffer*, 2> mSceneQueue;

bool SubmitToRenderSystem(RenderBuffer* vbuf) {
	return mRenderQueue.write(vbuf);
}

bool RetrieveFromRenderSystem(RenderBuffer** out) {
	return mRenderQueue.read(*out);
}

bool RetrieveFromSceneSystem(RenderBuffer** out) {
	return mSceneQueue.read(*out);
}

bool SubmitToSceneSystem(RenderBuffer* vbuf) {
	return mSceneQueue.write(vbuf);
}

void Render(RenderBuffer* vbuf) {
	std::sort(
		&vbuf->circleCommands[0].id, 
		&vbuf->circleCommands[vbuf->circleCount].id,
		std::less<uint64_t>()
	);
	
	// construct circle batches until we overflow some capacity:
	//  -- 32 total circles
	//	-- 8 distinct transforms
	//  -- 8 distinct geometries
	//	-- 8 distinct materials
	int batchCircleCount = 0;
	int batchTransformCount = 0;
	int batchGeometryCount = 0;
	int batchMaterialCount = 0;
	for(int i=0; i<vbuf->circleCount; ++i) {
		// TODO
	}
}
