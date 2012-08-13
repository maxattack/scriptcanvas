#pragma once

/*
Tables are represent compact linear arrays of records,
endower with an index array which provides cheap (3 op)
lookup of records using 32-bit handles.

Tables are very cache friendly, and simple to synchronize
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

struct IndexRecord {
    ID id;
    uint16_t index;
    uint16_t next;
};

template<typename Record, unsigned Capacity>
class Table {
private:
    uint32_t mCount;
    uint16_t mFreelistEnqueue;
    uint16_t mFreelistDequeue;
    IndexRecord mIndex[Capacity];
    Record mRecords[Capacity];

public:
    Table();

    inline bool Contains(ID id) const {
        // use the lower-bits to find the record
        const IndexRecord *p = mIndex + (id & INDEX_MASK);
        return p->id == id && p->index != USHRT_MAX;
    }

    inline Record& operator[](ID id) { ASSERT(Contains(id)); return mRecords[mIndex[id & INDEX_MASK].index]; }
    inline Record* Begin() const { return mRecords; }
    inline Record* End() const { return mRecords + mCount; }

    ID Add();
    void Remove(ID id);
};

template<typename Record, unsigned Capacity>
Table<Record, Capacity>::Table() : 
    mCount(0), 
    mFreelistEnqueue(Capacity-1), 
    mFreelistDequeue(0) {
    STATIC_ASSERT(Capacity <= MAX_CAPACITY);
    // initialize the free queue linked-list
    for(unsigned i=0; i<Capacity; ++i) {
        mIndex[i].id = i;
        mIndex[i].next = i+1;
    }
}

template<typename Record, unsigned Capacity>
void Table<Record, Capacity>::Remove(ID id) {
    // assuming IDs are valid in production
    ASSERT(Contains(id));
    // lookup the index record
    IndexRecord &in = mIndex[id & INDEX_MASK];
    // move the last record into this slot
    Record& record = mRecords[in.index];
    record = mRecords[--mCount];
    // update the index from the moved record
    mIndex[record.id & INDEX_MASK].index = in.index;
    // free up this index record and enqueue
    in.index = USHRT_MAX;
    mIndex[mFreelistEnqueue].next = id & INDEX_MASK;
    mFreelistEnqueue = id & INDEX_MASK;
}

template<typename Record, unsigned Capacity>
ID Table<Record, Capacity>::Add() {
    ASSERT(mCount < Capacity);
    // dequeue a new index record - we do this in FIFO order so that
    // we don't "thrash" a record with interleaved add-remove calls
    // and use up the higher-order bits of the id
    IndexRecord &in = mIndex[mFreelistDequeue];
    mFreelistDequeue = in.next;
    // increment the higher-order bits of the id
    in.id += NEW_ID_ADD;
    // push a new record into the buffer
    in.index = mCount++;
    // write the id to the record
    return mRecords[in.index].id = in.id;
}

