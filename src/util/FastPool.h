#pragma once

/*
	A super-simple object pool that doesn't allow IsActive() testing or
	Iteration, but is much more efficient.
	
	Might consider adding some slower checks to DEBUG builds.
*/

typedef uint32_t ID;

template<typename T>
class FastPool {
private:
	int mCapacity;
	int mCount;
	ID mFirstFree;
	ID mLast;
	T* mBuffer;
	
public:
	FastPool(int capacity, T* buffer) : mCapacity(capacity), mCount(0), mFirstFree(0), mLast(buffer), mBuffer(buffer) {}
	int Count() const { return mCount; }
    T& operator[](ID id) { return mBuffer[id & 0xffff]; }
    ID TakeOut();
    void PutBack(ID id);

};

template<typename T, int Capacity>
class StaticFastPool : public FastPool<T> {
private:
	T mBuffer[Capacity];

public:
	StaticFastPool() : FastPool<T>(Capacity, mBuffer) {}
};

template<typename T>
FastPool<T>::FastPool(int capacity, T* buffer) : 
	mCapacity(capacity), 
	mCount(0), 
	mFirstFree(0), 
	mLast(buffer), 
	mBuffer(buffer) {
	ASSERT(sizeof(T) >= sizeof(ID));
	ASSERT(mCapacity < 16*1024);
}

template<typename T>
ID FastPool<T>::TakeOut() {
	ASSERT(mCount < mCapacity);
	ID result;
	if (mFirstFree) {
		result = mFirstFree;
		mFirstFree = reinterpret_cast<ID>(*mFirstFree);
	} else {
		result = mLast;
		mLast++;
	}
	ASSERT(result & 0xffff == result);
	result |= 0x10000;
	mCount++;
	return result;	
}

template<typename T>
void FastPool<T>::PutBack(ID id) {
	id &= 0xffff;
	ASSERT(id < mLast);
	if (id == mLast - 1) {
		mLast--;
	} else {
		*reinterpret_cast<ID*>(mBuffer + id) = mFirstFree;
		mFirstFree = id;
	}
	mCount--;
}
