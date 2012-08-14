#pragma once
#include <limits.h>
#include <stdint.h>
#include "math/Transform.h"

#define T_INDEX_MASK 0xffff
#define T_NEW_ID_ADD 0x10000
#define T_CAPACITY 512
#define T_MAX_DEPTH 16

typedef uint32_t TID;


class TransformTree {
private:
    struct IndexSlot {
        TID id;
        uint16_t index;
        uint16_t next;
    };

    uint32_t mBottomLevel;
    uint16_t mFreelistEnqueue;
    uint16_t mFreelistDequeue;
    uint16_t mCounts[T_MAX_DEPTH];

    IndexSlot mIndex[T_CAPACITY];
    TID mIndexToId[T_CAPACITY];
    Transform mLocal[T_CAPACITY];
    uint16_t mParent[T_CAPACITY];
    Transform mWorld[T_CAPACITY];

public:
    TransformTree();

    inline bool Contains(TID id) const {
        const IndexSlot& slot = mIndex[id & T_INDEX_MASK];
        return slot.id == id && slot.index != USHRT_MAX;
    }

    inline uint32_t Count() const { return mCounts[mBottomLevel]; }
    inline Transform& operator[](TID id) { return mLocal[mIndex[id & T_INDEX_MASK].index]; }
    inline Transform GetWorld(TID id) const {  return mWorld[mIndex[id & T_INDEX_MASK].index]; }

    unsigned LevelOf(TID id) const;

    TID AddRoot();
    TID AddChild(TID parent);
    void RemoveTree(TID id);
    void Remove(TID id);
    void Update();

private:

    IndexSlot& AllocateSlot(unsigned index);
    void MoveData(unsigned src, unsigned dst);
};

