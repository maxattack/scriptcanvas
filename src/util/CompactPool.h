#pragma once

/*
CompactPools are represent compact linear arrays of records,
endowed with an index array which provides cheap (3 op)
lookup of records using 24-bit handles (could be 32, but we 
reserved the high-order bit for Component TypeIDs).

CompactPools are very cache friendly, and simple to synchronize
over multiple threads, making them ideal for creating fast,
cache-friendly homogenous batch processes.

Furthermore, pools and table IDs are trivial to serialize
and block-allocate without worrying about pointer-fixup.

Often, you may need to copy this source and modify it to meet
your needs (e.g. one index into multiple buffers, special 
sorting rules for buffers, etc).  In this case, pay attention
to the comments.
*/

#include <limits.h>
#include <stdint.h>
#include "Macros.h"
#include "Alloc.h"

typedef uint32_t ID; // For compatibility w/ ComponentIDs, handles are just 24-bit

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
	uint16_t* mBackBuffer;
    T* mBuffer;

public:
    CompactPool(int capacity, PoolIndex* indexBuffer, uint16_t* backBuffer, T* recordBuffer);

    bool IsActive(ID id) const {
        // use the lower-bits to find the record
        PoolIndex *p = mIndex + (id & 0xffff);
        return p->id == (id & 0x00ffffff) && p->index != USHRT_MAX;
    }

	
    T& operator[](ID id) { ASSERT(IsActive(id)); return mBuffer[mIndex[id & 0xffff].index]; }
    ID TakeOut();
    void PutBack(ID id);
    
	int Count() { return mCount; }
	ID GetID(T* record) { return mIndex[mBackBuffer[record-mBuffer]].id; }
    T* Begin() const { return mBuffer; }
    T* End() const { return mBuffer + mCount; }

    class Iterator {
    private:
        friend class CompactPool<T>;
        CompactPool<T> * mPool;
        T* mCurrent;

        Iterator(CompactPool<T> *p);

    public:
        bool Next();
        operator T*() { return mCurrent; }
        T& operator*() { return *mCurrent; }
        T* operator->() { return mCurrent; }
    };

    Iterator Enumerate() { return Iterator(this); }
};

template<typename T, int Capacity>
class StaticCompactPool : public CompactPool<T> {
private:
	PoolIndex mIndexBuffer[Capacity];
	uint16_t mBackBuffer[Capacity];
	T mRecordBuffer[Capacity];
public:
	StaticCompactPool() : CompactPool<T>(Capacity, mIndexBuffer, mBackBuffer, mRecordBuffer) {}
};

template<typename T>
CompactPool<T>::CompactPool(int capacity, PoolIndex* indexBuffer, uint16_t* backBuffer, T* recordBuffer) : 
    mCount(0), 
    mFreelistEnqueue(capacity-1), 
    mFreelistDequeue(0),
    mCapacity(capacity),
    mIndex(indexBuffer),
	mBackBuffer(backBuffer),
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
    auto i = (((in.id & 0x00ff0000) >> 16) % 255) + 1;
    in.id = (0xffff & in.id) | (i << 16);
    // push a new record into the buffer
    in.index = mCount++;
    // write the id to the record
    //return mBuffer[in.index].id = in.id;
	mBackBuffer[in.index] = 0xffff & in.id;
	return in.id;
}

template<typename T>
void CompactPool<T>::PutBack(ID id) {
    // assuming IDs are valid in production
    ASSERT(IsActive(id));
    // lookup the index record
    PoolIndex &in = mIndex[id & 0xffff];
    // move the last record into this slot
    T& record = mBuffer[in.index];
    record = mBuffer[--mCount];
    // update the index from the moved record
    //mIndex[record.id & 0xffff].index = in.index;
	mIndex[mBackBuffer[mCount]].index = in.index;

    // free up this index record and enqueue
    in.index = USHRT_MAX;
    mIndex[mFreelistEnqueue].next = id & 0xffff;
    mFreelistEnqueue = id & 0xffff;
}

template<typename T>
CompactPool<T>::Iterator::Iterator(CompactPool<T> *p) : 
    mPool(p),
    mCurrent(mPool->mBuffer-1) {
}

template<typename T>
bool CompactPool<T>::Iterator::Next() {
    mCurrent++;
    return mCurrent != mPool->mBuffer + mPool->mCount;
}
