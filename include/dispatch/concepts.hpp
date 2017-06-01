#pragma once
#include <experimental/type_traits>
#include <tuple>

namespace dispatch {

  template<template<typename ...> typename Op, typename... Args>
  using is_detected = std::experimental::is_detected<Op, Args...>;

  template<typename T, typename S>
  using comparable_t = decltype(std::declval<std::decay_t<T>>() == std::declval<std::decay_t<S>>());

  template<typename T, typename S>
  static constexpr bool Comparable() {
    return is_detected<comparable_t, T, S>{};
  }

  // TupleAccess
  // TODO: generalize to non-std-tuples, customization point
  template<typename T>
  using tuple_access_t = decltype(std::get<std::declval<std::size_t>()>(std::declval<T>()));

  template<typename T>
  static constexpr bool TupleAccess() {
    return is_detected<tuple_access_t, std::decay_t<T>>{};
  }
  template<typename T>
  using pair_access_t = std::void_t<decltype(std::declval<T>().first), decltype(std::declval<T>().second)>;

  template<typename T>
  static constexpr bool PairAccess() {
    return is_detected<pair_access_t, std::decay_t<T>>{};
  }

  // Constant
  // A Constant is a type that wraps a runtime value with an accessor "data()"
  // It must also be EqualityComparable to the underlying type.
  template<typename T>
  using data_accessor_t = decltype(T::data());

  template<typename T>
  static constexpr bool Constant() {
    return std::conjunction<
      is_detected<data_accessor_t, T>,
      std::bool_constant<Comparable<T, decltype(T::data())>()>>{};
  }

  // TODO: Invokable

}  // namespace dispatch
