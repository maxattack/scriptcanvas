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
#include "../Foundation.h"

#define MAX_CAPACITY (64*1024)

struct CompactPoolSlot {
    ID id;
    uint16_t index;
    uint16_t next;
};

template<typename T, int N>
class CompactPool {
private:
    uint32_t mCount;
    uint16_t mFreelistEnqueue;
    uint16_t mFreelistDequeue;
    CompactPoolSlot mSlot[N];
	uint16_t mBackBuffer[N];
    T mBuffer[N];

public:
    CompactPool();

    bool IsActive(ID id) const {
        // use the lower-bits to find the record
        auto& p = mSlot[id & 0xffff];
        return p.id == id && p.index != USHRT_MAX;
    }

	
    T& operator[](ID id) { ASSERT(IsActive(id)); return mBuffer[mSlot[id & 0xffff].index]; }
    T operator[](ID id) const { ASSERT(IsActive(id)); return mBuffer[mSlot[id & 0xffff].index]; }
    ID TakeOut();
    void PutBack(ID id);
    
	int Count() { return mCount; }
	ID GetID(T* record) { return mSlot[mBackBuffer[record-mBuffer]].id; }
    uint16_t Index(ID id) { return mSlot[id & 0xffff].index; }
    T* Begin() { return mBuffer; }
    T* End() { return mBuffer + mCount; }
};

template<typename T, int N>
CompactPool<T, N>::CompactPool() : 
    mCount(0), 
    mFreelistEnqueue(N-1), 
    mFreelistDequeue(0) {
    ASSERT(N <= MAX_CAPACITY);
    // initialize the free queue linked-list
    for(unsigned i=0; i<N; ++i) {
        mSlot[i].id = i;
        mSlot[i].next = i+1;
    }
}

template<typename T, int N>
ID CompactPool<T, N>::TakeOut() {
    ASSERT(mCount < N);
    // dequeue a new index record - we do this in FIFO order so that
    // we don't "thrash" a record with interleaved add-remove calls
    // and use up the higher-order bits of the id
    CompactPoolSlot &in = mSlot[mFreelistDequeue];
    mFreelistDequeue = in.next;
    // increment the higher-order bits of the id
    in.id = (0xffff & in.id) + (0xffff0000 & (in.id + 0x10000));
    // push a new record into the buffer
    in.index = mCount++;
    // write the id to the record
    //return mBuffer[in.index].id = in.id;
	mBackBuffer[in.index] = 0xffff & in.id;
	return in.id;
}

template<typename T, int N>
void CompactPool<T, N>::PutBack(ID id) {
    // assuming IDs are valid in production
    ASSERT(IsActive(id));
    // lookup the index record
    CompactPoolSlot &in = mSlot[id & 0xffff];
    // move the last record into this slot
    T& record = mBuffer[in.index];
    record = mBuffer[--mCount];
    // update the index from the moved record
    //mSlot[record.id & 0xffff].index = in.index;
	mSlot[mBackBuffer[mCount]].index = in.index;
    // free up this index record and enqueue
    in.index = USHRT_MAX;
    mSlot[mFreelistEnqueue].next = id & 0xffff;
    mFreelistEnqueue = id & 0xffff;
}
