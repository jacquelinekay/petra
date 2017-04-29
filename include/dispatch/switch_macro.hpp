#pragma once

#include <cassert>
#include <vrm/pp/for_each.hpp>

#define MAKE_CASE(Index, FunctionT, Arg) \
  case Arg: \
    return FunctionT<Index>{}();

#define MAKE_SWITCH_JUMP_TABLE(FunctionT, ...) \
  [](auto i) { \
    switch(i) { \
      VRM_PP_FOREACH_REVERSE(MAKE_CASE, FunctionT, __VA_ARGS__) \
      default: \
        /* TODO */ \
        assert(false); \
    } \
  }

