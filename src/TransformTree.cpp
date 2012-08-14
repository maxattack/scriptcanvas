#include "TransformTree.h"
#include <cstdio>

// TODO: DEBUG PARENT UPDATE ON SHUFFLE / WRITE UNIT TESTS

TransformTree::TransformTree() :
    mBottomLevel(0),
    mFreelistEnqueue(T_CAPACITY-1), 
    mFreelistDequeue(0) {
    mCounts[0] = 0;
    // initialize FIFO queue for free index slots
    for(unsigned i=0; i<T_CAPACITY; ++i) {
        mIndex[i].id = i;
        mIndex[i].next = i+1;
    }
}

unsigned TransformTree::LevelOf(TID id) const {
    const unsigned idx = mIndex[id & T_INDEX_MASK].index;
    unsigned result = 0;
    while(idx >= mCounts[result]) {
        result++;
    }
    return result;
}

TID TransformTree::AddRoot() {
    for(unsigned i=mBottomLevel; i>0; --i) {
        MoveData(mCounts[i-1], mCounts[i]);
    }
    IndexSlot& slot = AllocateSlot(mCounts[0]);
    for(unsigned i=0; i<=mBottomLevel; ++i) {
        mCounts[i]++;
    }
    return slot.id;
}

TID TransformTree::AddChild(TID parent) {
    const unsigned levelOfParent = LevelOf(parent);
    if (levelOfParent == mBottomLevel) {
        mCounts[levelOfParent+1] = mCounts[levelOfParent] + 1;
        IndexSlot& slot = AllocateSlot(mCounts[levelOfParent]);
        mParent[slot.index] = mIndex[parent & T_INDEX_MASK].index;
        mBottomLevel++;
        return slot.id;
    } else {
        for(unsigned i=mBottomLevel; i>levelOfParent+1; --i) {
            MoveData(mCounts[i-1], mCounts[i]);
        }
        IndexSlot& slot = AllocateSlot(mCounts[levelOfParent+1]);
        for(unsigned i=levelOfParent+1; i<=mBottomLevel; ++i) {
            mCounts[i]++;
        }
        mParent[slot.index] = mIndex[parent & T_INDEX_MASK].index;
        return slot.id;
    }
}

void TransformTree::Remove(TID id) {
    IndexSlot& slot = mIndex[id & T_INDEX_MASK];
    unsigned level = 0;
    while(slot.index >= mCounts[level]) { level++; }
    if (slot.index < mCounts[level]-1) {
        MoveData(mCounts[level]-1, slot.index);
    }
    if (level == mBottomLevel) {
        if (level > 0 && mCounts[level] == mCounts[level-1]) {
            mBottomLevel--;
        } else {
            mCounts[mBottomLevel]--;
        }
    } else {
        for(unsigned i=level+1; i<=mBottomLevel; ++i) {
            MoveData(mCounts[i]-1, mCounts[i-1]-1);
        }
        for(unsigned i=level; i<=mBottomLevel; ++i) {
            mCounts[i]--;
        }
    }
    slot.index = USHRT_MAX;
    mIndex[mFreelistEnqueue].next = id & T_INDEX_MASK;
    mFreelistEnqueue = id & T_INDEX_MASK;
}

void TransformTree::RemoveTree(TID id) {
    unsigned idx = mIndex[id & T_INDEX_MASK].index;
    unsigned level = LevelOf(id);
    if (level < mBottomLevel) {
        for(unsigned i=mCounts[level]; i<mCounts[level+1]; ++i) {
            if (mParent[i] == idx) {
                RemoveTree(mIndexToId[i]);
            }
        }
    }
    Remove(id);
}

void TransformTree::Update() {
    const unsigned cnt = Count();
    for(unsigned i=0; i<mCounts[0]; ++i) {
        mWorld[i] = mLocal[i];
    }
    for(unsigned i=mCounts[0]; i<cnt; ++i) {
        mWorld[i] = mLocal[i] * mWorld[mParent[i]];
    }
}

TransformTree::IndexSlot& TransformTree::AllocateSlot(unsigned index) {
    IndexSlot &slot = mIndex[mFreelistDequeue];
    mFreelistDequeue = slot.next;
    // increment the higher-order bits of the id
    slot.id += T_NEW_ID_ADD;
    slot.index = index;
    mIndexToId[slot.index] = slot.id;
    return slot;
}

void TransformTree::MoveData(unsigned src, unsigned dst) {
    unsigned level = LevelOf(mIndexToId[src]);
    mIndex[mIndexToId[src] & T_INDEX_MASK].index = dst;
    mIndexToId[dst] = mIndexToId[src];
    mParent[dst] = mParent[src];
    mLocal[dst] = mLocal[src];
    if (level < mBottomLevel) {
        if (dst > src) {
            for (unsigned i=mCounts[level]+1; i<=mCounts[level+1]; ++i) {
                if (mParent[i] == src) { mParent[i] = dst; }
            }
        } else {
            for (unsigned i=mCounts[level]; i<mCounts[level+1]; ++i) {
                if (mParent[i] == src) { mParent[i] = dst; }
            }
        }
    }
}
