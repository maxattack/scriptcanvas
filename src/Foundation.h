#pragma once
#include <limits.h>
#include <stdint.h>
#include <cstdio>
#include <cassert>

#define MAX_NODES 				1024
#define MAX_COMPONENT_TYPES		32

typedef uint32_t ID;

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
#   define LOG_MSG(_msg)    printf("%s:%d " _msg "\n", __FILE__, __LINE__)
#	define LOG_INT(_expr)	printf("%s:%d " #_expr " = %d\n", __FILE__, __LINE__, (_expr))
#else
#   define ASSERT(cond)
#   define LOG(_x)
#   define LOG_MSG(_msg)
#	define LOG_INT(_expr)
#endif

#define MALLOC malloc
#define FREE free

