#include "RenderSystem.h"
#include <algorithm>
#include <cstring>

//------------------------------------------------------
// INTERNAL DATA
//------------------------------------------------------

struct NodeSlot {
	// for thumbprint validation (in case slot is reused)
	ID id;

	// poses are stored in a separate DAG-sorted buffer
	// for fast per-frame world-coordinate computation
	uint16_t poseIndex;

	// freelist linked list
	uint16_t nextFreeSlot;

	// tree data (just store the slot index?)
	ID parent;
	ID firstChild;
	ID nextSibling;
	ID prevSibling;

	// component data
	uint32_t componentMask;
};

struct PoseData {
	uint16_t parentIndex;
	uint16_t slotIndex;
	ztransform localToParent;
};

//------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------

static uint32_t sNodeCount;
static uint16_t sNodeFreelistEnqueue;
static uint16_t sNodeFreelistDequeue;
static NodeSlot sNodeSlots[MAX_NODES];
static PoseData sNodePoses[MAX_NODES];
static IManager* sComponentManagers[MAX_COMPONENT_TYPES];

#if !NO_DAG_SORT
static int mFirstInvalidDagIndex;
static int mLastInvalidDagIndex;
#endif

//------------------------------------------------------
// INTERNAL METHODS
//------------------------------------------------------

static NodeSlot& Slot(ID id) {
	ASSERT((id&0xffff)<MAX_NODES);
	return sNodeSlots[id & 0xffff];
}

static PoseData& Pose(ID id) {
	ASSERT(SceneSystem::NodeValid(id));
	return sNodePoses[sNodeSlots[id&0xffff].poseIndex];
}

//------------------------------------------------------
// PUBLIC METHODS
//------------------------------------------------------

void SceneSystem::Initialize() {
	memset(sComponentManagers, 0, MAX_COMPONENT_TYPES * sizeof(IManager*));
	#if !NO_DAG_SORT
	mFirstInvalidDagIndex = -1;
	mLastInvalidDagIndex = -1;
	#endif
	sNodeCount = 0;
	sNodeFreelistEnqueue = MAX_NODES-1;
	sNodeFreelistDequeue = 0;
	for (unsigned i=0; i<MAX_NODES; ++i) {
		sNodeSlots[i].id = i;
		sNodeSlots[i].poseIndex = USHRT_MAX;
		sNodeSlots[i].nextFreeSlot = i+1;
	}
}

bool SceneSystem::NodeValid(ID id) {
	NodeSlot& slot = sNodeSlots[id & 0xffff];
	return slot.id == id && slot.poseIndex != USHRT_MAX;
}

int SceneSystem::NodeCount() {
	return sNodeCount;
}

ID SceneSystem::CreateNode(ID parent) {
	ASSERT(sNodeCount < MAX_NODES);
	// Allocate a new node at the end of the buffer
	// Dequeue a slot
	auto& slot = sNodeSlots[sNodeFreelistDequeue];
	sNodeFreelistDequeue = slot.nextFreeSlot;
	// Give the ID a unique thumbprint
	slot.id = (0xffff & slot.id) + ((((0xffff0000&slot.id)>>16)%0xffff)<<16) + 0x10000;
	// Allocate a new buffer location
	slot.poseIndex = sNodeCount++;
	// initialize records
	sNodePoses[slot.poseIndex] = { USHRT_MAX, uint16_t(0xffff&slot.id), ZTransform() };
	slot.componentMask = 0;
	slot.parent = 0;
	slot.firstChild = 0;
	slot.nextSibling = 0;
	slot.prevSibling = 0;
	if (parent) {
		AttachNode(parent, slot.id);
	}
	return slot.id;
}

void SceneSystem::AttachNode(ID parent, ID child) {
	ASSERT(parent != child);
	auto& childData = Slot(child);
	// check existing parents
	if (childData.parent) {
		if (childData.parent == parent) {
			return;
		} else {
			DetachNode(child);
		}
	}
	// update records
	childData.parent = parent;
	auto& parentData = Slot(parent);
	if (parentData.firstChild) {
		childData.nextSibling = parentData.firstChild;
		Slot(parentData.firstChild).prevSibling = child;
	}
	int pIndex = Slot(parent).poseIndex;
	int chIndex = Slot(child).poseIndex;
	parentData.firstChild = child;
	Pose(child).parentIndex = pIndex;
	#if !NO_DAG_SORT
	// check if the parent is listed after the child
	if (chIndex < pIndex) {
		if (mFirstInvalidDagIndex == -1 || chIndex < mFirstInvalidDagIndex) {
			mFirstInvalidDagIndex = chIndex;
		}
		if (mLastInvalidDagIndex == -1 || pIndex > mLastInvalidDagIndex) {
			mLastInvalidDagIndex = pIndex;
		}
	}
	#endif
}

void SceneSystem::DetachNode(ID child) {
	auto& data = Slot(child);
	if (data.parent) {
		auto& parent = Slot(data.parent);
		if (parent.firstChild == child) parent.firstChild = data.nextSibling;
		if (data.nextSibling) Slot(data.nextSibling).prevSibling = data.prevSibling;
		if (data.prevSibling) Slot(data.prevSibling).nextSibling = data.nextSibling;
		data.parent = 0;
		data.nextSibling = 0;
		data.prevSibling = 0;
		Pose(child).parentIndex = -1;
	}
}

ID SceneSystem::Parent(ID node) {
	return Slot(node).parent;
}

SceneSystem::ChildIterator::ChildIterator(ID node) {
	current = Slot(node).firstChild;
}

bool SceneSystem::ChildIterator::Next(ID *outNode) {
	if (current) {
		*outNode = current;
		current = Slot(current).nextSibling;
		return true;
	} else {
		return false;
	}
}

uint16_t SceneSystem::Index(ID node) {
	return Slot(node).poseIndex;
}

ztransform& SceneSystem::LocalToParent(ID node) {
	return Pose(node).localToParent;
}

static ztransform ComputeLocalToWorld(const PoseData& pose) {
	if (pose.parentIndex == USHRT_MAX) {
		return pose.localToParent;
	} else {
		return pose.localToParent * ComputeLocalToWorld(sNodePoses[pose.parentIndex]);
	}
}

ztransform SceneSystem::LocalToWorld(ID node) {
	return ComputeLocalToWorld(Pose(node));
}

#if NO_DAG_SORT

static void Update(RenderBuffer *vbuf, uint16_t i) {
	auto& pose = sNodePoses[i];
	vbuf->transforms[i] = pose.localToParent * vbuf->transforms[pose.parentIndex];
	for(ID cid=Slot(pose.slotIndex).firstChild; cid; cid=Slot(cid).nextSibling) {
		Update(vbuf, Slot(cid).poseIndex);
	}
}

#endif

void SceneSystem::Update(RenderBuffer *vbuf) {
	
	#if NO_DAG_SORT
	// have to walk the heirarchy recursively :P
	for(int i=0; i<sNodeCount; ++i) {
		auto& pose = sNodePoses[i];
		if (pose.parentIndex == USHRT_MAX) {
			vbuf->transforms[i] = pose.localToParent;
			for(ID cid=Slot(pose.slotIndex).firstChild; cid; cid=Slot(cid).nextSibling) {
				Update(vbuf, Slot(cid).poseIndex);
			}
			
		}
	}
	#else

	// dag-sort slice?
	if(mFirstInvalidDagIndex >= 0) {
		// Basic Parents-Before-Children Algorithm (impl could be better):
		//	- start reading at the first invalid DAG index
		//	- write all parents that are in the invalid DAG slice first in DAG-order
		//	- write the current readPos
		//	- increment read pos passed all entires that have already been written
		
		// could stack-alloc just enough for the slice?
		static int scratchpad[MAX_NODES];
		static uint8_t scratchpadMarks[MAX_NODES];
		memset(scratchpadMarks+mFirstInvalidDagIndex, 0, mLastInvalidDagIndex-mFirstInvalidDagIndex+1);
		int readPos = mFirstInvalidDagIndex;
		int writePos = readPos;
		do {
			// parent(s) in the invalid-dag-slice, need to write it first
			int parentIndex = sNodePoses[readPos].parentIndex;
			int parentCount = 0;
			while (parentIndex != USHRT_MAX && parentIndex >= mFirstInvalidDagIndex && !scratchpadMarks[parentIndex]) {
				scratchpad[writePos] = parentIndex;
				scratchpadMarks[parentIndex] = 1;
				writePos++;
				parentIndex = sNodePoses[parentIndex].parentIndex;
				parentCount++;
			}
			if (parentCount > 1) {
				// parents are in reverse order >__<
				int halfCount = parentCount >> 1;
				for(int i=0; i<halfCount; ++i) {
					std::swap(scratchpad[writePos-parentCount+i], scratchpad[writePos-1-i]);
				}
			}
			scratchpad[writePos++] = readPos;
			scratchpadMarks[readPos] = 1;
			// skip past elements that have already been written
			while(readPos <= mLastInvalidDagIndex && scratchpadMarks[readPos]) {
				readPos++;
			}
		} while(readPos <= mLastInvalidDagIndex);
		// move records to reflect scratchpad
		//	- first update pose parents
		//	- swap record in first position with correct record
		//	  modifying scratchpad as we go
		for(int i=mFirstInvalidDagIndex; i<=mLastInvalidDagIndex; ++i) {
			if (scratchpad[i] != i) {
				std::swap(
					sNodeSlots[sNodePoses[i].slotIndex].poseIndex, 
					sNodeSlots[sNodePoses[scratchpad[i]].slotIndex].poseIndex
				);
				std::swap(sNodePoses[i], sNodePoses[scratchpad[i]]);
				int j=i+1;
				while(scratchpad[j] != i && j <= mLastInvalidDagIndex) {
					++j;
				}
				if (j <= mLastInvalidDagIndex) {
					scratchpad[j] = scratchpad[i];
				}
			}
		}
		for(int i=mFirstInvalidDagIndex; i<=mLastInvalidDagIndex; ++i) {
			ID parent = Slot(sNodePoses[i].slotIndex).parent;
			if (parent) {
				sNodePoses[i].parentIndex = Slot(parent).poseIndex;
			}
			
		}
		mFirstInvalidDagIndex = -1;
		mLastInvalidDagIndex = -1;
	}

	// fast-compute local-to-world buffer
	for(int i=0; i<sNodeCount; ++i) {
		if (sNodePoses[i].parentIndex == USHRT_MAX) {
			vbuf->transforms[i] = sNodePoses[i].localToParent;
		} else {
			vbuf->transforms[i] = sNodePoses[i].localToParent * vbuf->transforms[sNodePoses[i].parentIndex];
		}
	}

	#endif
}

void SceneSystem::RegisterComponentManager(ID componentType, IManager* pMgr) {
	ASSERT((componentType) < MAX_COMPONENT_TYPES);
	ASSERT(sComponentManagers[componentType] == 0);
	pMgr->Initialize();
	sComponentManagers[componentType] = pMgr;
}

void SceneSystem::AddComponent(ID node, ID componentType) {
	ASSERT(sComponentManagers[componentType]);
	ASSERT(!HasComponent(node, componentType));
	Slot(node).componentMask |= (0x80000000 >> componentType);
	sComponentManagers[componentType]->CreateComponent(node);
}

bool SceneSystem::HasComponent(ID node, ID componentType) {
	return Slot(node).componentMask & (0x80000000 >> componentType);
}

void SceneSystem::RemoveComponent(ID node, ID componentType) {
	ASSERT(sComponentManagers[componentType]);
	ASSERT(HasComponent(node, componentType));
	sComponentManagers[componentType]->DestroyComponent(node);
	Slot(node).componentMask ^= (0x80000000 >> componentType);
}

SceneSystem::ComponentIterator::ComponentIterator(ID node) {
	mask = Slot(node).componentMask;
}

bool SceneSystem::ComponentIterator::Next(ID *outComponentType) {
	if (mask) {
		*outComponentType = __builtin_clz(mask);
		mask ^= (0x80000000 >> (*outComponentType));
		return true;
	} else {
		return false;
	}

}

void SceneSystem::DestroyNode(ID node) {
	ASSERT(NodeValid(node));
	DetachNode(node);
	// tear down children
	auto& slot = Slot(node);
	while(slot.firstChild) {
		DestroyNode(slot.firstChild);
	}
	// tear-down components
	ComponentIterator listComponents(node);
	ID componentType;
	while(listComponents.Next(&componentType)) {
		sComponentManagers[componentType]->DestroyComponent(node);
	}

	// relocate last element into this slot and deallocate the last slot
	sNodeCount--;
	if (sNodeCount && slot.poseIndex != sNodeCount) {
		// relocated last element into this empty slot
		auto& relocatingPose = sNodePoses[sNodeCount];
		auto& relocatingSlot = sNodeSlots[relocatingPose.slotIndex];
		sNodePoses[slot.poseIndex] = relocatingPose;
		relocatingSlot.poseIndex = slot.poseIndex;

		// update parent indices
		int chIndex = slot.poseIndex;
		for(ID cid = relocatingSlot.firstChild; cid; cid=Slot(cid).nextSibling) {
			Pose(cid).parentIndex = chIndex;
		}

		#if !NO_DAG_SORT
		// check if the pose was relocated before it's parent
		if (slot.parent) {
			int pIndex = Slot(slot.parent).poseIndex;
			if (chIndex < pIndex) {
				if (mFirstInvalidDagIndex == -1 || chIndex < mFirstInvalidDagIndex) {
					mFirstInvalidDagIndex = chIndex;
				}
				if (mLastInvalidDagIndex == -1 || pIndex > mLastInvalidDagIndex) {
					mLastInvalidDagIndex = pIndex;
				}				
			}
		}
		#endif
	}

	// Invalidate the slot and enqueue in the freelist
	slot.poseIndex = USHRT_MAX;
	sNodeSlots[sNodeFreelistEnqueue].nextFreeSlot = node & 0xffff;
	sNodeFreelistEnqueue = node & 0xffff;
}
