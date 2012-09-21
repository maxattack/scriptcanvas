#pragma once
#include "Types.h"


// A utility for making component buffers
// that are keyed by node IDs by employing
// some internal implementation details.

template<typename T>
class CompactComponentPool {
private:
	uint32_t mCount;
	uint16_t mIndex[MAX_NODES];
	T mBuffer[MAX_NODES];

public:
	CompactComponentPool();
	void Alloc(ID node);
	void Free(ID node);
	T& operator[](ID node);
	T operator[](ID node) const;
	int Count() const { return mCount; }
    T* Begin() { return mBuffer; }
    T* End() { return mBuffer + mCount; }
	
};

template<typename T>
CompactComponentPool<T>::CompactComponentPool() : mCount(0) {
}

template<typename T>
void CompactComponentPool<T>::Alloc(ID node) {
	ASSERT(mCount < MAX_NODES);
	mIndex[node & 0xffff] = mCount;
	mCount++;
}

template<typename T>
void CompactComponentPool<T>::Free(ID node) {
	mCount--;
	if (mCount > 0) {
		mBuffer[mIndex[node & 0xffff]] = mBuffer[mCount];
		mIndex[mBuffer[mCount].node] = mIndex[node & 0xffff];
	}
}

template<typename T>
T& CompactComponentPool<T>::operator[](ID node) {
	return mBuffer[mIndex[node & 0xffff]];
}

template<typename T>
T CompactComponentPool<T>::operator[](ID node) const {
	return mBuffer[mIndex[node & 0xffff]];
}
