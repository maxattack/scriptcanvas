#pragma once
#include <limits.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>
#include <GL/glfw.h>

#define kMaxNodes 					1024
#define kMaxComponentTypes			32

#define kMaxSegments				1024
#define kMaxMaterials				64

#define kComponentCircle			0
#define kComponentSpline			1
#define kSegmentResolution			256

typedef uint32_t ID;

enum StatusCode {
	OK = 0
	// TODO: all failure codes
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

#ifdef DEBUG
#   define ASSERT(cond) (assert(cond))
#   define LOG(_x)   printf _x
#   define LOG_MSG(_msg)    printf("%s:%d " _msg "\n", __FILE__, __LINE__)
#	define LOG_INT(_expr)	printf("%s:%d " #_expr " = %d\n", __FILE__, __LINE__, (_expr))
#	define LOG_FLOAT(_expr)	printf("%s:%d " #_expr " = %f\n", __FILE__, __LINE__, (_expr))
#	define LOG_VEC(_expr)	{ float2 __u__ = (_expr); printf("%s:%d " #_expr " = <%f,%f>\n", __FILE__, __LINE__, __u__.x, __u__.y); }
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

