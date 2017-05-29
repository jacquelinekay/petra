#pragma once

#include <array>
#include <utility>
#include "dispatch/detail/array_jump_table.hpp"

namespace dispatch {

  template<template<std::size_t> typename F, std::size_t N, typename ...Args>
  constexpr decltype(auto) make_tagged_jump_table() {
    return detail::ArrayTagTable<F, N, Args...>{};
  }

}  // namespace dispatch
