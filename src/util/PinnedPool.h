#pragma once

/*
PinnedPools represent an array of objects endowed
with a simple "in-use" bitmask to determine which
records in the pool are available to allocate.

Unlike the CompactPool, PinnedPools do not defragment
memory, so there's less copying and access-indirection,
however it could become fragmented and less cache-friendly.

Furthermore, IDs do not use the upper 16-bits to descriminate
between slot re-use like in CompactPools, and may therefore
cause slot-ambiguity issues :P
*/

#include <limits.h>
#include <stdint.h>
#include "Macros.h"

#if (__GNUC__ && !__clang__)|| __has_builtin(__builtin_clz)
#define CLZ(x) __builtin_clz(x)
#else
#define CLZ(x) _BitScanReverse(x)
#endif

typedef uint32_t ID; // For compatibility w/ ComponentIDs, handles are just 24-bit
#define MAX_CAPACITY (64*1024)

// TODO propery test

template<typename T>
class PinnedPool {
private:
    int mCount;
    int mCapacity;
    int mWorkingMaskIndex;
    uint32_t* mMask;
    T* mBuffer;

public:
    PinnedPool(int capacity, uint32_t* mask, T* buffer);

    bool IsActive(ID id) const;
    T& operator[](ID id) { ASSERT(IsActive(id)); return mBuffer[id & 0xffff]; }
    ID TakeOut();
    void PutBack(ID id);

    class Iterator {
    private:
        friend class PinnedPool<T>;
        PinnedPool<T> * mPool;
        int mMaskIndex;
        int mCurrentMask;
        int mCount;
        ID mCurrent;

        Iterator(PinnedPool<T> *p);

    public:
        bool Next();
        ID Current() { ASSERT(mCount>0); return mCurrent; }
        operator T*();
        T& operator*();
        T* operator->();
    };

    Iterator Enumerate() { return Iterator(this); }
};

template<typename T, int Capacity>
class StaticPinnedPool : public PinnedPool<T> {
private:
	uint32_t mMask[(Capacity+31)>>5];
	T mBuffer[Capacity];
public:
	StaticPinnedPool() : PinnedPool<T>(Capacity, mMask, mBuffer) {}
};

template<typename T>
PinnedPool<T>::PinnedPool(int capacity, uint32_t* mask, T* buffer) :
    mCount(0),
    mCapacity(capacity),
    mWorkingMaskIndex(0),
    mMask(mask),
    mBuffer(buffer) {
    ASSERT(mCapacity < MAX_CAPACITY);
    for(int i=0; i<(mCapacity+31)>>5; ++i) {
        mMask[i] = 0;
    }
}

template<typename T>
bool PinnedPool<T>::IsActive(ID id) const {
    id &= 0xffff;
    return mMask[id >> 5] & (0x80000000 >> (id%32));
}

template<typename T>
ID PinnedPool<T>::TakeOut() {
    ASSERT(mCount < mCapacity);
    int i=mWorkingMaskIndex;
    while(mMask[i] == 0xffffffff) { ++i; }
    int localIndex = 0;
    while ((0x80000000 >> localIndex) & mMask[i]) { localIndex++; }
    mMask[i] |= (0x80000000 >> localIndex);
    mWorkingMaskIndex = mMask[i] == 0xffffffff ? i+1 : i;
    mCount++;
    return localIndex + (i << 5) + 0x10000; // no way to count like in CompactPool :*(
}

template<typename T>
void PinnedPool<T>::PutBack(ID id) {
    ASSERT(IsActive(id));
    id &= 0xffff;
    int maskIndex = id >> 5;
    mMask[maskIndex] ^= (0x80000000 >> (id%32));
    if (maskIndex < mWorkingMaskIndex) { mWorkingMaskIndex = maskIndex; }
    mCount--;
}

template<typename T>
PinnedPool<T>::Iterator::Iterator(PinnedPool<T> *p) : 
    mPool(p), 
    mMaskIndex(0), 
    mCurrentMask(mPool->mMask[0]),
    mCount(0),
    mCurrent(0) {
}

template<typename T>
bool PinnedPool<T>::Iterator::Next() {
    if (mCount >= mPool->mCount) {
        return false;
    }
    while(!mCurrentMask) {
        mMaskIndex++;
        mCurrentMask = mPool->mMask[mMaskIndex];
    }
    int localIndex = CLZ(mCurrentMask);
    mCurrentMask ^= 0x80000000 >> localIndex;
    mCurrent = (mMaskIndex << 5) + localIndex;
    mCount++;
    return true;
}

template<typename T>
PinnedPool<T>::Iterator::operator T*() {
    return mPool->mBuffer + Current();
}

template<typename T>
T& PinnedPool<T>::Iterator::operator*() {
    return mPool->mBuffer[Current()];
}

template<typename T>
T* PinnedPool<T>::Iterator::operator->() {
    return mPool->mBuffer + Current();
}

