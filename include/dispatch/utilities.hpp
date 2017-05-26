#pragma once

#include <experimental/type_traits>
#include <utility>
#include <tuple>

namespace dispatch {
namespace utilities {

  struct Str {
    const char* data;
    std::size_t len;
    constexpr std::size_t length() const { return len; }
  };

  constexpr unsigned length(const char* str) {
    return *str ? 1 + length(str + 1) : 0;
  }

  constexpr auto make_str(const char* data) {
    return Str{data, length(data)};
  }

  constexpr bool compare(const Str& a, const Str& b) {
    if (a.len != b.len) {
      return false;
    }

    for (std::size_t i = 0; i < a.len; ++i) {
      if (a.data[i] != b.data[i]) {
        return false;
      }
    }
    return true;
  }

}  // namespace utilities
}  // namespace dispatch

// Hm
template<size_t Max, typename Cur, typename ...Strings>
constexpr auto max_string_length_recursive(const std::tuple<Cur, Strings...>& str) {
  constexpr unsigned Size = std::decay_t<Cur>::value().size();
  if constexpr (Size > Max) {
    if constexpr (sizeof...(Strings) > 0) {
      return max_string_length_recursive<Size>(std::make_tuple(Strings{}...));
    } else {
      return Size;
    }
  } else {
    if constexpr (sizeof...(Strings) > 0) {
      return max_string_length_recursive<Max>(std::make_tuple(Strings{}...));
    } else {
      return Max;
    }
  }
}

// XXX
template<typename ...Strings>
constexpr auto max_string_length(const Strings&... strs) {
  return max_string_length_recursive<0>(std::make_tuple(strs...));
}

template<size_t I, size_t ...Sequence, size_t ...Indices>
constexpr unsigned access_sequence_helper(
    const std::index_sequence<Sequence...>&, const std::index_sequence<Indices...>&) {
  return ((Indices == I ? Sequence : 0) + ... );
}

// utility to access the Ith element of an index sequence
template<size_t I, size_t ...Sequence>
constexpr unsigned access_sequence(const std::index_sequence<Sequence...>& seq) {
  return access_sequence_helper<I>(
    seq, std::make_index_sequence<sizeof...(Sequence)>{});
}

// Reverse operation: given an item in the sequence, return its index

template<size_t I, size_t ...Sequence>
constexpr unsigned map_to_index() {
  return access_sequence_helper<I>(
    std::make_index_sequence<sizeof...(Sequence)>{}, std::index_sequence<Sequence...>{});
}
template<size_t I, size_t ...Sequence>
constexpr unsigned map_to_index(std::index_sequence<Sequence...>) {
  return map_to_index<I, Sequence...>();
}

template<size_t I, typename T, size_t... J>
constexpr unsigned map_to_index_helper(T&& t, std::index_sequence<J...>) {
  return ((I == std::get<J>(t) ? J : 0) + ...);
}

template<size_t I, typename T>
constexpr unsigned map_to_index(T&& t) {
  return map_to_index_helper<I>(t, std::make_index_sequence<std::tuple_size<std::decay_t<T>>{}>{});
}


template<typename T, typename S>
using comparable_exp = decltype(std::declval<std::decay_t<T>>() == std::declval<std::decay_t<S>>());

template<template<typename ...> typename Op, typename... Args>
using is_detected = std::experimental::is_detected<Op, Args...>;

template<typename T, typename S>
using comparable = is_detected<comparable_exp, T, S>;

template<typename F, typename X>
struct fold_wrapper {
  F f;
  X state;

  template<typename Arg>
  constexpr auto operator>>=(Arg&& arg) {
    auto result = f(state, arg.state);
    return fold_wrapper<F, decltype(result)>{f, result};
  }
};

template <typename F, typename... Xs>
constexpr auto fold_left(const F& f, Xs&&... xs) {
  auto result = (... >>= fold_wrapper<F, Xs>{f, xs});
  return result.state;
}

template <typename F, typename Init, std::size_t... I>
constexpr auto fold_left(const F& f, Init&& init, std::index_sequence<I...>) {
  // auto result = (... >>= fold_wrapper<F, Xs>{f, xs});
  auto result = (fold_wrapper<F, Init>{f, init} >>= ...
      >>= fold_wrapper<F, std::integral_constant<std::size_t, I>>{f, std::integral_constant<std::size_t, I>{}});
  return result.state;
}

template<typename Integral>
constexpr bool in_sequence(const Integral& i) {
  return false;
}
// check if the first element is in the following sequence
template<typename Integral, typename T, typename... Ts>
constexpr bool in_sequence(const Integral& i, const T& t, const Ts&... ts) {
  if constexpr (comparable<Integral, T>{}) {
    if (i == t) {
      return true;
    }
  }
  return in_sequence(i, ts...);
}

template<typename Integral, std::size_t... J>
constexpr bool in_sequence(const Integral& I, std::index_sequence<J...>) {
  return in_sequence(I, J...);
}

// check if an element is in the tuple
template<typename Integral, typename T>
constexpr bool in_tuple(const Integral& i, const T& t) {
  constexpr auto x = [](auto&& i, auto&&... args) {
    return in_sequence(i, args...);
  };
  if constexpr (std::tuple_size<std::decay_t<T>>{} == 0) {
    return false;
  } else {
    return std::apply(x, std::tuple_cat(std::make_tuple(i), t));
  }
}

template<std::size_t... Result>
constexpr auto make_unique_sequence(std::index_sequence<Result...>&& result, std::index_sequence<>) {
  return result;
}

// do we need to ensure that order is preserved?
template<std::size_t... Result, std::size_t X, std::size_t... Sequence>
constexpr auto make_unique_sequence(std::index_sequence<Result...>&& result, std::index_sequence<X, Sequence...>) {
  if constexpr (in_sequence(X, Result...)) {
    return make_unique_sequence(std::index_sequence<Result...>{}, std::index_sequence<Sequence...>{});
  } else {
    return make_unique_sequence(std::index_sequence<Result..., X>{}, std::index_sequence<Sequence...>{});
  }
}

template<std::size_t... Sequence>
constexpr auto as_tuple(std::index_sequence<Sequence...>) {
  return std::make_tuple(Sequence...);
}

template <std::size_t Skip, typename T, std::size_t ...I>
constexpr auto get_elements(T&& t, std::index_sequence<I...>) {
  return std::make_tuple(std::get<I + Skip>(t)...);
}

template <std::size_t I, typename T>
constexpr auto get_elements_until(T&& t) {
  return get_elements<0>(t, std::make_index_sequence<I>{});
};


template <std::size_t I, typename T>
constexpr auto get_elements_after(T&& t) {
  constexpr std::size_t size = std::tuple_size<std::decay_t<T>>{};
  if constexpr (I + 1 > size) {
    return std::make_tuple();
  } else {
    return get_elements<I + 1>(t, std::make_index_sequence<size - I - 1>{});
  }
}

template<typename... Ts>
constexpr auto pop_front(std::tuple<Ts...>&& t) {
  return std::make_pair(std::get<0>(t), get_elements_after<0>(t));
}

constexpr auto pop_front(const std::index_sequence<>&) {
  return std::index_sequence<>{};
}

template<std::size_t I, std::size_t... Is>
constexpr auto pop_front(const std::index_sequence<I, Is...>&) {
  return std::make_pair(I, std::index_sequence<Is...>{});
}

template<typename F, typename T, std::size_t ...I>
constexpr auto times_helper(F&& f, T&& initial, std::index_sequence<I...>) {
  return fold_left(
    [f](auto t, auto) {
      return f(t);
    },
    initial, I...
  );
}

template<std::size_t I, typename F, typename T>
constexpr auto times(F&& f, T&& initial) {
  if constexpr (I == 0) {
    return initial;
  } else {
    return times_helper(f, initial, std::make_index_sequence<I>{});
  }
}

template<typename Tuple>
constexpr auto tuple_size(Tuple&& t) {
  return std::tuple_size<std::decay_t<Tuple>>{};
}

template<size_t I, size_t... J>
constexpr auto append(std::index_sequence<J...>) {
  return std::index_sequence<J..., I>{};
}

template<typename Tuple, typename Elem>
constexpr auto append(Tuple&& t, Elem && e) {
  return std::tuple_cat(t, std::make_tuple(e));
}

template<std::size_t I, typename T, typename Elem>
constexpr auto insert_at(T&& t, Elem&& elem) {
  return std::tuple_cat(append(get_elements_until<I>(t), elem), get_elements_after<I>(t));
  // return std::tuple_cat(get_elements_until<I>(t), std::make_tuple(elem), get_elements_after<I>(t));
}

constexpr auto concatenate() {
  return std::index_sequence<>{};
}

template<size_t... I, size_t... J>
constexpr auto concatenate(std::index_sequence<I...>, std::index_sequence<J...>) {
  return std::index_sequence<I..., J...>{};
}

template<size_t... I, size_t... J, typename... T>
constexpr auto concatenate(std::index_sequence<I...>&& is, std::index_sequence<J...>&& js, T&&... ts) {
  return concatenate(concatenate(is, js), ts...);
}

// precondition: J is a subset of I
// Choose everything in I that is not in J
template<size_t... I, size_t... J>
constexpr auto difference(std::index_sequence<I...>, std::index_sequence<J...>) {
  constexpr auto f = [](auto&& seq, auto&& i) {
    constexpr size_t index = std::decay_t<decltype(i)>::value;
    if constexpr (in_sequence(index, J...)) {
      return seq;
    } else {
      return append<index>(seq);
    }
  };
  return fold_left(f, std::index_sequence<>{}, std::integral_constant<std::size_t, I>{}...);
}
