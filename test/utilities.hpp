#pragma once

#ifdef NDEBUG

#undef NDEBUG
#ifdef assert
#undef assert
#endif  // ifdef assert
#include <cassert>

#define NDEBUG

#else
#include <cassert>
#endif  // ifdef NDEBUG

#ifndef PETRA_ASSERT
#define PETRA_ASSERT(...) assert(__VA_ARGS__)
#endif  // ifdef PETRA_ASSERT
