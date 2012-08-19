#pragma once
#include "../SceneSystem.h"


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
    T* Begin() const { return (T*)mBuffer; }
    T* End() const { return (T*)mBuffer + mCount; }
	
};

template<typename T>
CompactComponentPool<T>::CompactComponentPool() : mCount(0) {
}

template<typename T>
void CompactComponentPool<T>::Alloc(ID node) {
	mIndex[node & 0xffff] = mCount;
	mCount++;
}

template<typename T>
void CompactComponentPool<T>::Free(ID node) {
	mCount--;
	mBuffer[mIndex[node & 0xffff]] = mBuffer[mCount];
	mIndex[mCount] = mIndex[node & 0xffff];
}

template<typename T>
T& CompactComponentPool<T>::operator[](ID node) {
	return mBuffer[mIndex[node & 0xffff]];
}
