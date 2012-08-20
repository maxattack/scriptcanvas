#include "RenderSystem.h"

namespace SceneSystem {

//------------------------------------------------------
// INTERNAL DATA
//------------------------------------------------------

struct NodeSlot {
	// A lookup slot for accessing data records,
	// we are reordered to preserve a compact buffer
	// in the NodeManager
	ID id;
	uint16_t index;
	uint16_t next;
	uint32_t componentMask;
};

struct NodeData {
	// Heirarchy information
	ID parent;
	ID firstChild;
	ID nextSibling;
	ID prevSibling;
};

// Idea -- data which doesn't *need* to be compact
// can be stored in the slot, and not in the relocated
// buffers?  Honestly, the only important thing to be
// in DAG-order is the transforms and the parents, right?

struct NodeManager {
	uint32_t count;						// number of nodes
	uint16_t freelistEnqueue;			// unused-slot queue
	uint16_t freelistDequeue;
	NodeSlot slots[MAX_NODES];			// lookup slots
	uint16_t backBuffer[MAX_NODES];		// record->slot back map
	NodeData dataBuffer[MAX_NODES];		// records
	Transform poseBuffer[MAX_NODES];

	NodeManager();
	bool IsValid(ID id);
	NodeSlot& SlotFor(ID id);
	NodeData& operator[](ID id);
	Transform& TransformFor(ID id);

	ID Alloc();
	void Free(ID id);
};


//------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------

static NodeManager nodes;
static IManager* gComponentManagers[MAX_COMPONENT_TYPES];

//------------------------------------------------------
// NODE MANAGER IMPLEMENTATION
//------------------------------------------------------

NodeManager::NodeManager() : 
	count(0),
	freelistEnqueue(MAX_NODES-1),
	freelistDequeue(0) 
{
	// Initialize free slot queue
	for (unsigned i=0; i<MAX_NODES; ++i) {
		slots[i].id = i;
		slots[i].next = i+1;
	}
}

bool NodeManager::IsValid(ID id) {
	// Validate that the slot ID matches and is in use
	NodeSlot& slot = slots[id & 0xffff];
	return slot.id == id && slot.index != USHRT_MAX;
}

NodeSlot& NodeManager::SlotFor(ID id) {
	return slots[id & 0xffff];
}

NodeData& NodeManager::operator[](ID id) {
	// Lookup the pointer from the slot
	ASSERT(IsValid(id));
	return dataBuffer[slots[id&0xffff].index];
}

Transform& NodeManager::TransformFor(ID id) {
	// Lookup the pointer from the slot
	ASSERT(IsValid(id));
	return poseBuffer[slots[id&0xffff].index];
}

ID NodeManager::Alloc() {
	ASSERT(count < MAX_NODES);
	// Dequeue a slot
	NodeSlot& slot = slots[freelistDequeue];
	freelistDequeue = slot.next;
	// Give the ID a unique thumbprint
	slot.id = (0xffff & slot.id) + ((((0xffff0000&slot.id)>>16)%0xffff)<<16) + 0x10000;
	// Allocate a new buffer location
	slot.index = count++;
	// Back-map the index to the slot
	backBuffer[slot.index] = 0xffff & slot.id;
	return slot.id;
}

void NodeManager::Free(ID id) {
	ASSERT(IsValid(id));
	NodeSlot& slot = slots[id & 0xffff];
	// Reduce slice size
	count--;
	// Copy last element into this place
	backBuffer[slot.index] = backBuffer[count];
	dataBuffer[slot.index] = dataBuffer[count];
	poseBuffer[slot.index] = poseBuffer[count];
	// Update the slot of the last element
	slots[backBuffer[count]].index = slot.index;
	// Invalidate the slot and enqueue in the freelist
	slot.index = USHRT_MAX;
	slots[freelistEnqueue].next = id & 0xffff;
	freelistEnqueue = id & 0xffff;
}

//------------------------------------------------------
// PUBLIC METHODS
//------------------------------------------------------

int NodeCount() {
	return nodes.count;
}

ID CreateNode(ID parent) {
	ID result = nodes.Alloc();
	nodes.TransformFor(result) = Transform::Identity();
	nodes.SlotFor(result).componentMask = 0x00000000;
	NodeData &data = nodes[result];
	data.parent = 0;
	data.firstChild = 0;
	data.nextSibling = 0;
	data.prevSibling = 0;
	if (parent) {
		AttachNode(parent, result);
	}
	return result;
}

void AttachNode(ID parent, ID child) {
	NodeData& childData = nodes[child];
	if (childData.parent && childData.parent != parent) {
		DetachNode(child);
	}
	childData.parent = parent;
	NodeData& parentData = nodes[parent];
	if (parentData.firstChild) {
		childData.nextSibling = parentData.firstChild;
		nodes[parentData.firstChild].prevSibling = child;
	}
	parentData.firstChild = child;
}

void DetachNode(ID child) {
	NodeData& data = nodes[child];
	if (data.parent) {
		NodeData& parent = nodes[data.parent];
		if (parent.firstChild == child) parent.firstChild = data.nextSibling;
		if (data.nextSibling) nodes[data.nextSibling].prevSibling = data.prevSibling;
		if (data.prevSibling) nodes[data.prevSibling].nextSibling = data.nextSibling;
		data.parent = 0;
		data.nextSibling = 0;
		data.prevSibling = 0;
	}
}

ID Parent(ID node) {
	return nodes[node].parent;
}

ChildIterator::ChildIterator(ID node) {
	current = nodes[node].firstChild;
}

bool ChildIterator::Next(ID *outNode) {
	if (current) {
		*outNode = current;
		current = nodes[current].nextSibling;
		return true;
	} else {
		return false;
	}
}

Transform& Pose(ID node) {
	return nodes.TransformFor(node);
}

uint16_t GetIndex(ID node) {
	return nodes.SlotFor(node).index;
}

Transform WorldPose(ID node) {
	return nodes[node].parent ? Pose(node) * WorldPose(nodes[node].parent) : Pose(node);
}

static void UpdateChildren(RenderBuffer *vbuf, int i) {
	ID child = nodes.dataBuffer[i].firstChild;
	do {
		int idx = GetIndex(child);
		vbuf->transforms[idx] = nodes.poseBuffer[idx] * vbuf->transforms[i];
		if (nodes.dataBuffer[idx].firstChild) {
			UpdateChildren(vbuf, idx);
		}
		child = nodes.dataBuffer[idx].nextSibling;
	} while(child);
}

void Update(RenderBuffer *vbuf) {
	// This function could use some love -- reordering transforms
	// into DAG order if dirty and then computing world transforms
	// in one memory-friendly batch pass.  This is *not* meant as the
	// final implementation (though it *is* at least better than hopping
	// around dynamic memory).
	for(int i=0; i<nodes.count; ++i) {
		if (!nodes.dataBuffer[i].parent) {
			vbuf->transforms[i] = nodes.poseBuffer[i];
			if (nodes.dataBuffer[i].firstChild) {
				UpdateChildren(vbuf, i);
			}
		}
	}
}



void RegisterComponentManager(ID componentType, IManager* pMgr) {
	ASSERT((componentType) < MAX_COMPONENT_TYPES);
	ASSERT(gComponentManagers[componentType] == 0);
	gComponentManagers[componentType] = pMgr;
}

void AddComponent(ID node, ID componentType) {
	ASSERT(gComponentManagers[componentType]);
	ASSERT(!HasComponent(node, componentType));
	nodes.SlotFor(node).componentMask |= (0x80000000 >> componentType);
	gComponentManagers[componentType]->CreateComponent(node);
}

bool HasComponent(ID node, ID componentType) {
	return nodes.SlotFor(node).componentMask & (0x80000000 >> componentType);
}

void RemoveComponent(ID node, ID componentType) {
	ASSERT(gComponentManagers[componentType]);
	ASSERT(HasComponent(node, componentType));
	gComponentManagers[componentType]->DestroyComponent(node);
	nodes.SlotFor(node).componentMask ^= (0x80000000 >> componentType);
}

ComponentIterator::ComponentIterator(ID node) {
	mask = nodes.SlotFor(node).componentMask;
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
	DetachNode(node);
	// tear down children
	NodeData& data = nodes[node];
	while(data.firstChild) {
		DestroyNode(data.firstChild);
	}
	// tear-down components
	ComponentIterator listComponents(node);
	ID componentType;
	while(listComponents.Next(&componentType)) {
		gComponentManagers[componentType]->DestroyComponent(node);
	}
	// release node records to manaager
	nodes.Free(node);
}

}