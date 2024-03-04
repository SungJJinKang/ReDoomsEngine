#pragma once

#if (defined(COMPILER_MSVC) == false) && (defined(COMPILER_LLVM) == false)
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC
#else

#undef COMPILER_LLVM
#define COMPILER_LLVM
#endif
#endif

#ifdef COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SEE/NEON optimizations are not available for this compiler yet!!!!
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

#ifndef U8_MAX
#define U8_MAX 255
#endif


#ifndef U16_MAX
#define U16_MAX 65535
#endif


#ifndef INT32_MIN
#define INT32_MIN ((int32_t)0x80000000)
#endif

#ifndef U16_MAX
#define INT32_MAX ((int32_t)0x7fffffff)
#endif

#ifndef UINT32_MIN
#define UINT32_MIN 0
#endif


#ifndef UINT32_MAX
#define UINT32_MAX ((uint32_t)-1)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX ((uint64_t)-1)
#endif

#ifndef FLOAT32_MAX
#define FLOAT32_MAX FLT_MAX
#endif

#ifndef FLOAT32_MIN
#define FLOAT32_MIN -FLT_MAX
#endif
