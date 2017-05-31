#pragma once

#ifdef NDEBUG

#undef NDEBUG
#include <assert.h>

#define NDEBUG

#endif

#define DISPATCH_ASSERT(...) assert(__VA_ARGS__)
