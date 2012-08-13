#pragma once

#include <cstdio>
#include <cassert>

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

#ifdef DEBUG
#   define ASSERT(cond) (assert(cond))
#   define LOG(_x)   printf _x
#else
#   define ASSERT(cond)
#   define LOG(_x)
#endif

