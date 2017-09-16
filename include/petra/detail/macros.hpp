#pragma once

#ifdef PETRA_ENABLE_CPP14
#define PETRA_AUTO(Typename) typename Typename, Typename
#else
#define PETRA_AUTO(Typename) auto
#endif  // PETRA_ENABLE_CPP14

#ifdef __clang__
#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }
#else
#define PETRA_NOEXCEPT_FUNCTION_BODY(...)                                      \
  { return __VA_ARGS__; }
#endif  //__clang__

#ifdef __clang__
#define PETRA_NOEXCEPT_CHECK(...) noexcept(noexcept(__VA_ARGS__))
#else
#define PETRA_NOEXCEPT_CHECK(...)
#endif  //__clang__
