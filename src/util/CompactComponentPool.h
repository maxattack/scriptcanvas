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
#include "../Foundation.h"

// A utility for making component buffers
// that are keyed by node IDs by employing
// some internal implementation details.

template<typename T>
class CompactComponentPool {
public:
	struct Slot {
		ID node;
		T component;
	};
	
private:

	uint32_t mCount;
	uint16_t mIndex[kMaxNodes];
	Slot mBuffer[kMaxNodes];

public:
	CompactComponentPool();

	bool IsActive(ID node) const { return mIndex[node & 0xffff] != 0xffff; }

	void Alloc(ID node);
	void Free(ID node);
	T& operator[](ID node);
	T operator[](ID node) const;
	int Count() const { return mCount; }
    Slot* Begin() { return mBuffer; }
    Slot* End() { return mBuffer + mCount; }
	
};

template<typename T>
CompactComponentPool<T>::CompactComponentPool() : mCount(0) {
	for(int i=0; i<kMaxNodes; ++i) {
		mIndex[i] = 0xffff;
	}
}

template<typename T>
void CompactComponentPool<T>::Alloc(ID node) {
	ASSERT(mCount < kMaxNodes);
	mIndex[node & 0xffff] = mCount;
	mBuffer[mCount].node = node;
	mCount++;
}

template<typename T>
void CompactComponentPool<T>::Free(ID node) {
	mCount--;
	if (mCount > 0) {
		mBuffer[mIndex[node & 0xffff]] = mBuffer[mCount];
		mIndex[mBuffer[mCount].node] = mIndex[node & 0xffff];
	}
	mIndex[node&0xffff] = USHRT_MAX;
}

template<typename T>
T& CompactComponentPool<T>::operator[](ID node) {
	ASSERT(IsActive(node));
	return mBuffer[mIndex[node & 0xffff]].component;
}

template<typename T>
T CompactComponentPool<T>::operator[](ID node) const {
	ASSERT(IsActive(node));
	return mBuffer[mIndex[node & 0xffff]].component;
}
