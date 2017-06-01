#pragma once

#ifdef NDEBUG

#undef NDEBUG
#include <assert.h>

#define NDEBUG

#endif

#define PETRA_ASSERT(...) assert(__VA_ARGS__)
