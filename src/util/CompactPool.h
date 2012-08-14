#pragma once

/*
CompactPools are represent compact linear arrays of records,
endower with an index array which provides cheap (3 op)
lookup of records using 32-bit handles.

CompactPools are very cache friendly, and simple to synchronize
over multiple threads, making them ideal for creating fast,
cache-friendly homogenous batch processes.

Furthermore, tables and table IDs are trivial to serialize
and block-allocate without worrying about pointer-fixup.

Often, you may need to copy this source and modify it to meet
your needs (e.g. one index into multiple buffers, special 
sorting rules for buffers, etc).  In this case, pay attention
to the comments.
*/

#include <limits.h>
#include <stdint.h>
#include <cstdlib>
#include "Macros.h"

typedef uint32_t ID; // record handle

#define INDEX_MASK 0xffff
#define NEW_ID_ADD 0x10000
#define MAX_CAPACITY (64*1024)

struct PoolIndex {
    ID id;
    uint16_t index;
    uint16_t next;
};

template<typename T>
class CompactPool {
private:
    uint32_t mCount;
    uint16_t mFreelistEnqueue;
    uint16_t mFreelistDequeue;
    int mCapacity;
    PoolIndex* mIndex;
    T* mBuffer;

public:
    CompactPool(int capacity, PoolIndex* indexBuffer, T* recordBuffer);

    bool Contains(ID id) const {
        // use the lower-bits to find the record
        const PoolIndex *p = mIndex + (id & INDEX_MASK);
        return p->id == id && p->index != USHRT_MAX;
    }

    T& operator[](ID id) { ASSERT(Contains(id)); return mBuffer[mIndex[id & INDEX_MASK].index]; }
    T* Begin() const { return mBuffer; }
    T* End() const { return mBuffer + mCount; }
    ID TakeOut();
    void PutBack(ID id);
};

template<typename T>
CompactPool<T>::CompactPool(int capacity, PoolIndex* indexBuffer, T* recordBuffer) : 
    mCount(0), 
    mFreelistEnqueue(capacity-1), 
    mFreelistDequeue(0),
    mCapacity(capacity),
    mIndex(indexBuffer),
    mBuffer(recordBuffer) {
    ASSERT(mCapacity <= MAX_CAPACITY);
    // initialize the free queue linked-list
    for(unsigned i=0; i<mCapacity; ++i) {
        mIndex[i].id = i;
        mIndex[i].next = i+1;
    }
}

template<typename T>
ID CompactPool<T>::TakeOut() {
    ASSERT(mCount < mCapacity);
    // dequeue a new index record - we do this in FIFO order so that
    // we don't "thrash" a record with interleaved add-remove calls
    // and use up the higher-order bits of the id
    PoolIndex &in = mIndex[mFreelistDequeue];
    mFreelistDequeue = in.next;
    // increment the higher-order bits of the id
    in.id += NEW_ID_ADD;
    // push a new record into the buffer
    in.index = mCount++;
    // write the id to the record
    return mBuffer[in.index].id = in.id;
}

template<typename T>
void CompactPool<T>::PutBack(ID id) {
    // assuming IDs are valid in production
    ASSERT(Contains(id));
    // lookup the index record
    PoolIndex &in = mIndex[id & INDEX_MASK];
    // move the last record into this slot
    T& record = mBuffer[in.index];
    record = mBuffer[--mCount];
    // update the index from the moved record
    mIndex[record.id & INDEX_MASK].index = in.index;
    // free up this index record and enqueue
    in.index = USHRT_MAX;
    mIndex[mFreelistEnqueue].next = id & INDEX_MASK;
    mFreelistEnqueue = id & INDEX_MASK;
}
