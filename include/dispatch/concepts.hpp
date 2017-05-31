#pragma once
#include <experimental/type_traits>

namespace dispatch {

  template<template<typename ...> typename Op, typename... Args>
  using is_detected = std::experimental::is_detected<Op, Args...>;

  template<typename T, typename S>
  using comparable_t = decltype(std::declval<std::decay_t<T>>() == std::declval<std::decay_t<S>>());

  template<typename T, typename S>
  static constexpr bool Comparable() {
    return is_detected<comparable_t, T, S>{};
  }
  /*

  // TupleAccess
  template<typename T>
  using TupleAccess = is_detected<
        decltype(std::get<std::declval<std::size_t>()>(std::declval<T>())), T
      >;

  // PairAccess
  // TODO: Make sure that these compose
  template<typename T>
  using PairAccess = is_detected<
      std::void_t<
          decltype(T.first),
          decltype(T.second),
          TupleAccess<T>::type>,
      T>;
      */

  // Constant
  // A Constant is a type that wraps a runtime value with an accessor "data()"
  // It must also be EqualityComparable to the underlying type.


  // Invokable (?)


}  // namespace dispatch
