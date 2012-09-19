#include "RenderSystem.h"
#include <algorithm>
#include <cstring>


namespace SceneSystem {

//------------------------------------------------------
// INTERNAL DATA
//------------------------------------------------------

struct NodeSlot {
	// A lookup slot for accessing data records,
	// which are reordered to preserve a compact buffer
	ID id;
	uint16_t index;
	uint16_t next;
};

struct ComponentData {
	uint32_t mask;
};

struct TreeData {
	ID parent;
	ID firstChild;
	ID nextSibling;
	ID prevSibling;
};

struct PoseData {
	int parentIndex;
	transform localToParent;
};

//------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------

static uint32_t sNodeCount;
static uint16_t sNodeFreelistEnqueue;
static uint16_t sNodeFreelistDequeue;
static NodeSlot sNodeSlots[MAX_NODES];
static uint16_t sNodeBackBuffer[MAX_NODES];
static TreeData sNodeTrees[MAX_NODES];
static ComponentData sNodeComps[MAX_NODES];
static PoseData sNodePoses[MAX_NODES];
static int mFirstInvalidDagIndex;
static int mLastInvalidDagIndex;
static IManager* sComponentManagers[MAX_COMPONENT_TYPES];

//------------------------------------------------------
// INTERNAL METHODS
//------------------------------------------------------

static NodeSlot& Slot(ID id) {
	ASSERT((id&0xffff)<MAX_NODES);
	return sNodeSlots[id & 0xffff];
}

static TreeData& Heirarchy(ID id) {
	ASSERT(NodeValid(id));
	return sNodeTrees[sNodeSlots[id&0xffff].index];
}

static ComponentData& Component(ID id) {
	ASSERT(NodeValid(id));
	return sNodeComps[sNodeSlots[id&0xffff].index];
}

static PoseData& Pose(ID id) {
	ASSERT(NodeValid(id));
	return sNodePoses[sNodeSlots[id&0xffff].index];
}

//------------------------------------------------------
// PUBLIC METHODS
//------------------------------------------------------

void Initialize() {
	mFirstInvalidDagIndex = -1;
	mLastInvalidDagIndex = -1;
	sNodeCount = 0;
	sNodeFreelistEnqueue = MAX_NODES-1;
	sNodeFreelistDequeue = 0;
	for (unsigned i=0; i<MAX_NODES; ++i) {
		sNodeSlots[i].id = i;
		sNodeSlots[i].index = USHRT_MAX;
		sNodeSlots[i].next = i+1;
	}
}

bool NodeValid(ID id) {
	NodeSlot& slot = sNodeSlots[id & 0xffff];
	return slot.id == id && slot.index != USHRT_MAX;
}

int NodeCount() {
	return sNodeCount;
}

ID CreateNode(ID parent) {
	ASSERT(sNodeCount < MAX_NODES);
	// Allocate a new node at the end of the buffer
	// Dequeue a slot
	auto& slot = sNodeSlots[sNodeFreelistDequeue];
	sNodeFreelistDequeue = slot.next;
	// Give the ID a unique thumbprint
	slot.id = (0xffff & slot.id) + ((((0xffff0000&slot.id)>>16)%0xffff)<<16) + 0x10000;
	// Allocate a new buffer location
	slot.index = sNodeCount++;
	// initialize records
	sNodeBackBuffer[slot.index] = 0xffff & slot.id;
	sNodePoses[slot.index] = { -1, Transform() };
	sNodeComps[slot.index].mask = 0;
	sNodeTrees[slot.index] = { 0, 0, 0, 0 };
	if (parent) {
		AttachNode(parent, slot.id);
	}
	return slot.id;
}

void AttachNode(ID parent, ID child) {
	ASSERT(parent != child);
	auto& childData = Heirarchy(child);
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
	auto& parentData = Heirarchy(parent);
	if (parentData.firstChild) {
		childData.nextSibling = parentData.firstChild;
		Heirarchy(parentData.firstChild).prevSibling = child;
	}
	int pIndex = Slot(parent).index;
	int chIndex = Slot(child).index;
	parentData.firstChild = child;
	Pose(child).parentIndex = pIndex;
	// DAG is invalid if children are listed before parents
	if (chIndex < pIndex) {
		if (mFirstInvalidDagIndex == -1 || chIndex < mFirstInvalidDagIndex) {
			mFirstInvalidDagIndex = chIndex;
		}
		if (mLastInvalidDagIndex == -1 || pIndex > mLastInvalidDagIndex) {
			mLastInvalidDagIndex = pIndex;
		}
	}
}

void DetachNode(ID child) {
	auto& data = Heirarchy(child);
	if (data.parent) {
		auto& parent = Heirarchy(data.parent);
		if (parent.firstChild == child) parent.firstChild = data.nextSibling;
		if (data.nextSibling) Heirarchy(data.nextSibling).prevSibling = data.prevSibling;
		if (data.prevSibling) Heirarchy(data.prevSibling).nextSibling = data.nextSibling;
		data.parent = 0;
		data.nextSibling = 0;
		data.prevSibling = 0;
		Pose(child).parentIndex = -1;
	}
}

ID Parent(ID node) {
	return Heirarchy(node).parent;
}

ChildIterator::ChildIterator(ID node) {
	current = Heirarchy(node).firstChild;
}

bool ChildIterator::Next(ID *outNode) {
	if (current) {
		*outNode = current;
		current = Heirarchy(current).nextSibling;
		return true;
	} else {
		return false;
	}
}

int Index(ID node) {
	return Slot(node).index+1;
}

transform& LocalToParent(ID node) {
	return Pose(node).localToParent;
}

transform LocalToWorld(ID node) {
	auto parent = Heirarchy(node).parent;
	return parent ? LocalToParent(node) * LocalToWorld(parent) : LocalToParent(node);
}



void Update(RenderBuffer *vbuf) {
	// dag-sort slice?
	if(mFirstInvalidDagIndex >= 0) {
		LOG(("DAG FIXING!\n"));
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
			while (parentIndex >= mFirstInvalidDagIndex && !scratchpadMarks[parentIndex]) {
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
					sNodeSlots[sNodeBackBuffer[i]].index, 
					sNodeSlots[sNodeBackBuffer[scratchpad[i]]].index
				);
				std::swap(sNodeBackBuffer[i], sNodeBackBuffer[scratchpad[i]]);
				std::swap(sNodePoses[i], sNodePoses[scratchpad[i]]);
				std::swap(sNodeTrees[i], sNodeTrees[scratchpad[i]]);
				std::swap(sNodeComps[i], sNodeComps[scratchpad[i]]);
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
			if (sNodeTrees[i].parent) {
				sNodePoses[i].parentIndex = Slot(sNodeTrees[i].parent).index;
			}
			
		}
		mFirstInvalidDagIndex = -1;
		mLastInvalidDagIndex = -1;
	}
	// fast-compute local-to-world buffer
	vbuf->transforms[0] = Transform();
	for(int i=0; i<sNodeCount; ++i) {
		vbuf->transforms[i+1] = sNodePoses[i].localToParent * vbuf->transforms[sNodePoses[i].parentIndex+1];
	}
}



void RegisterComponentManager(ID componentType, IManager* pMgr) {
	ASSERT((componentType) < MAX_COMPONENT_TYPES);
	ASSERT(sComponentManagers[componentType] == 0);
	sComponentManagers[componentType] = pMgr;
}

void AddComponent(ID node, ID componentType) {
	ASSERT(sComponentManagers[componentType]);
	ASSERT(!HasComponent(node, componentType));
	Component(node).mask |= (0x80000000 >> componentType);
	sComponentManagers[componentType]->CreateComponent(node);
}

bool HasComponent(ID node, ID componentType) {
	return Component(node).mask & (0x80000000 >> componentType);
}

void RemoveComponent(ID node, ID componentType) {
	ASSERT(sComponentManagers[componentType]);
	ASSERT(HasComponent(node, componentType));
	sComponentManagers[componentType]->DestroyComponent(node);
	Component(node).mask ^= (0x80000000 >> componentType);
}

ComponentIterator::ComponentIterator(ID node) {
	mask = Component(node).mask;
}

bool ComponentIterator::Next(ID *outComponentType) {
	if (mask) {
		*outComponentType = __builtin_clz(mask);
		mask ^= (0x80000000 >> (*outComponentType));
		return true;
	} else {
		return false;
	}

}

void DestroyNode(ID node) {
	ASSERT(NodeValid(node));
	DetachNode(node);
	// tear down children
	auto& data = Heirarchy(node);
	while(data.firstChild) {
		DestroyNode(data.firstChild);
	}
	// tear-down components
	ComponentIterator listComponents(node);
	ID componentType;
	while(listComponents.Next(&componentType)) {
		sComponentManagers[componentType]->DestroyComponent(node);
	}

	// release node record
	auto& slot = Slot(node);

	// relocate last element into this slot and deallocate the last slot
	sNodeCount--;
	if (sNodeCount) {
		// move last element into this place
		sNodeBackBuffer[slot.index] = sNodeBackBuffer[sNodeCount];
		sNodeTrees[slot.index] = sNodeTrees[sNodeCount];
		sNodeComps[slot.index] = sNodeComps[sNodeCount];
		sNodePoses[slot.index] = sNodePoses[sNodeCount];
		
		// Update the slot of the last element
		sNodeSlots[sNodeBackBuffer[sNodeCount]].index = slot.index;
		
		// check that DAG is still valid
		auto& hdata = sNodeTrees[slot.index];
		int chIndex = slot.index;
		if (hdata.parent) {
			int pIndex = Slot(hdata.parent).index;
			if (chIndex < pIndex) {
				if (mFirstInvalidDagIndex == -1 || chIndex < mFirstInvalidDagIndex) {
					mFirstInvalidDagIndex = chIndex;
				}
				if (mLastInvalidDagIndex == -1 || pIndex > mLastInvalidDagIndex) {
					mLastInvalidDagIndex = pIndex;
				}				
			}
		}
		for(ID cid = hdata.firstChild; cid; cid=Heirarchy(cid).nextSibling) {
			Pose(cid).parentIndex = chIndex;
		}
	}

	// Invalidate the slot and enqueue in the freelist
	slot.index = USHRT_MAX;
	sNodeSlots[sNodeFreelistEnqueue].next = node & 0xffff;
	sNodeFreelistEnqueue = node & 0xffff;
}

}