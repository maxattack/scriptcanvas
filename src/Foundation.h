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
#include <limits.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>
#include "glfw/glfw.h"

#define kMaxNodes 					1024

#define kSegmentResolution			256
#define kMaxSegments				1024
#define kMaxMaterials				64

#define kComponentName				0
#define kComponentCircle			1
#define kComponentSpline			2
#define kMaxComponentTypes			32

typedef uint32_t ID;
struct CommandBuffer;

enum StatusCode {
	OK = 0
	// TODO: all failure codes
};

template<typename T = ID>
struct Handle {
	T id;
	inline Handle(T aId) : id(aId) {}
	inline operator T() const { return id; }
	inline bool operator==(Handle<T> t) const { return id == t.id; }
	inline bool operator!=(Handle<T> t) const { return id != t.id; }
};

#define STATIC_ASSERT(_x)  ((void)sizeof(char[1 - 2*!(_x)]))

#ifndef arraysize
#define arraysize(a)   (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef offsetof
#define offsetof(t,m)  ((uintptr_t)(uint8_t*)&(((t*)0)->m))
#endif

#ifndef MIN
#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#define MAX(a,b)   ((a) > (b) ? (a) : (b))
#endif

#define CLZ(_expr)	(__builtin_clz(_expr))

#ifdef DEBUG
#   define ASSERT(cond) (assert(cond))
#   define LOG(_x)   printf _x
#   define LOG_MSG(_msg)    printf("%s:%d " _msg "\n", __FILE__, __LINE__)
#	define LOG_INT(_expr)	printf("%s:%d " #_expr " = %d\n", __FILE__, __LINE__, (_expr))
#	define LOG_FLOAT(_expr)	printf("%s:%d " #_expr " = %f\n", __FILE__, __LINE__, (_expr))
#	define LOG_VEC(_expr)	{ vec2_t __u__ = (_expr); printf("%s:%d " #_expr " = <%f,%f>\n", __FILE__, __LINE__, __u__.x, __u__.y); }
#else
#   define ASSERT(cond)
#   define LOG(_x)
#   define LOG_MSG(_msg)
#	define LOG_INT(_expr)
#	define LOG_FLOAT(_expr)
#	define LOG_VEC(_expr)
#endif

#define MALLOC malloc
#define FREE free

