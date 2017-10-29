# petra

C++17 metaprogramming library for transforming runtime values to compile-time values and types.

[![Build Status](https://travis-ci.org/jacquelinekay/petra.svg?branch=master)](https://travis-ci.org/jacquelinekay/petra)

[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)

## Motivation

From template metaprogramming to `constexpr`, C++ offers powerful and expressive tools for writing code which is guaranteed to execute at compile time and therefore can be optimized out at runtime. A "compile-time value" (a non-type template parameter or the result of a core constant expression) can be arbitrarily used in a program anywhere a "runtime-determined value" (e.g. a value read from `stdin` or from a file) can. But the reverse is not true:

```c++
template<std::size_t N>
auto fill_array() {
  std::array<int, N> buckets;
  /* ... */
}

int main(int argc, char** argv) {
  int x = atoi(argv[1]);
  assert(x < 10);
  auto result = fill_array<x>();  // compiler error
}
```

What if you want to use a runtime-determined value in a constexpr context?

Petra makes this easy:

```c++
template<std::size_t N>
auto fill_array() {
  std::array<int, N> buckets;
  /* ... */
}

int main(int argc, char** argv) {
  int i = atoi(argv[1]);
  constexpr auto get_result = petra::make_sequential_table<10>([](auto&& x){
    return fill_array<x()>();
  });
  auto result = get_result(i);
}
```

If you're an experienced library writer, you may have already identified the technique used to implement this example: a compile-time jump table. This construct forms the basis of `variant` and `tuple`.

Petra provides runtime-to-compile-time jump tables for all primitive integral types, and for enum types. `sequential_table` expects the inputs to be integers from 0 to N, while `switch_table` expects an arbitrary sequence of integers of the same type. But what if you want to transform more complicated data types, like strings?

Petra supports mapping strings from runtime `const char*`'s to a compile-time string representation when the set of strings is known at compile time, by way of a string hash with constant runtime complexity.

This example simply prints a string read in from the command line. However, notice how that the string is accessed as a static member of a type-level string:

```c++
int main(int argc, char** argv) {
  const char* input = argv[1];

  auto callback = [](auto&& token) {
    std::cout << "hash value: " << std::decay_t<decltype(token)>::value()
              << "\n";
  };

  auto map_to_type = petra::make_string_map(callback, "dog"_s, "fish"_s, "cat"_s);

  map_to_type(input);
}
```

### Error handling

How do Petra's map constructs (`string_map`, `sequential_table`, etc.) handle inputs which are outside of the valid set of compile-time keys?

By default, the library passes a result of an empty library-specific type, `petra::InvalidInputError`, to the user callback.The user callback must put error handling logic in an `if constexpr` branch, or provide an overload set which handles this case.

You can opt out of this behavior in the constructor/factory functions for `switch_table`, but this will only work if your callback returns `void`.

For `string_map`, it should be noted that the string hashing algorithm used has collisions outside of the input set, so string inputs outside of the input set should be considered UB. For best results, use a large input set.

## Build and Install

Petra is tested with Clang trunk and C++17. You will also need a bleeding edge standard library version for `std::apply`, `std::is_detected`, etc. `libc++` 5 and `libstdc++` 6 both work. CMake currently does not check this, so if you're having problems compiling, make sure your environment variables are set properly.

```
git clone git@github.com:jacquelinekay/petra.git
cd petra
mkdir build
cd build

cmake .. <optionally specify -DCMAKE_CXX_FLAGS="-stdlib=libc++", -DCMAKE_CXX_COMPILER=clang++, etc.>
cmake --build .
```

By default, this will build a variety of tests and example executables.

Run the tests:

```
cmake --build . --target test
```

Example binaries will be in the `examples` folder of the build directory.

You can also install the headers with:

```
cmake --build . --target install
```

## Summary of headers

Petra's utilities include:

- `switch_table.hpp`: Given an integer sequence at compile-time, constructs a constexpr mapping from an integer value to the `std::integral_constant` representing the same value, so that a (possibly runtime-determined) integer value can be used in a template context.
- `sequential_table.hpp`: A specialization of `switch_table` for sequential integers.
- `enum_map.hpp`: A wrapper around `switch_table` for enum types.
- `chd.hpp`: Given a set of strings at compile time, constructs a hash from a `const char*` to an integer index with constant runtime complexity. (Petra also provides its own compile-time string class.) Petra's interface supports heterogenous types in the input set; that is, you could hash a set containing both strings and integers. `chd` also has customization points for computing a hash for a user-provided type. The algorithm is based heavily on Steve Hanov's implementation in his [blog post](http://stevehanov.ca/blog/index.php?id=119).
- `linear_hash.hpp`: A linear time complexity hash meant to be used as a fallback for `chd` when given small input sets, which it cannot handle.
- `string_map.hpp`: A convenience wrapper around `chd` which maps runtime strings to compile-time strings.
- `map.hpp`: A heterogenous map with variant-like access. Its keys are specified at compile time, but the values are mutable at runtime (as long as they do not change type). The API uses the library's built-in `expected` type, which is provided in `expected.hpp`.

If these features sound useful or interesting to you, check out the `examples` folder to see how they are used.

## Roadmap

- [ ] Variant implementation
- [ ] Update benchmark generation
  - [ ] Deserialization
  - [ ] Compiler AST
  - [ ] State machine
- [ ] C++14 support (probably using Hana backend)
- [ ] Fuzz tests and more testing in general
- [ ] Demonstrate more complicated "Constant" data structures
- [ ] Support more string hash algorithms
