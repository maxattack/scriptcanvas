// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#pragma once

// A super-simple object pool that doesn't allow IsActive() testing or
// Iteration, but is much more efficient.

// Might consider adding some slower checks to DEBUG builds.

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
