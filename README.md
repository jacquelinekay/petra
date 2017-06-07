# petra

C++17 metaprogramming library for transforming runtime values to compile-time values and types.

[![Build Status](https://travis-ci.org/jacquelinekay/petra.svg?branch=master)](https://travis-ci.org/jacquelinekay/petra)

[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)

## Motivation

From template metaprogramming to the `constexpr` keyword, C++ offers powerful and expressive tools for writing code which is guaranteed to execute at compile time and therefore can be optimized out at runtime. A "compile-time value" (a non-type template parameter or the result of a core constant expression) can be arbitrarily used in a program anywhere a "runtime-determined value" (e.g. a value read from `stdin` or from a file) can. The rules of the language prevent us from arbitrarily declaring a value assigned from a non-constant expression as `constexpr`:

```
int main(int argc, char** argv) {
  int x = atoi(argv[1]);
  assert(x < 10);
  constexpr int y = x;  // Compiler error

  std::array<int, y> buckets;
}
```

But what if the programmer wants to use a runtime-determined value in a constexpr context?

TODO:
```
int main(int argc, char** argv) {
  int x = atoi(argv[1]);
  constexpr auto unwrap = petra::make_sequential_table<10>([](auto&& x){ return x(); });
  constexpr int y = unwrap(i);
  std::array<int, y> buckets;
}
```

### Integers

### Strings

### Aggregate literal types

### Building more complex data structures

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

## Usage

Petra's utilities include:

- `switch_table.hpp`: Given an integer sequence at compile-time, constructs a constexpr mapping from an integer value to the `std::integral_constant` representing the same value, so that a (possibly runtime-determined) integer value can be used in a template context.
- `sequential_table.hpp`: A specialization of `switch_table` for sequential integers.
- `chd.hpp`: Given a set of strings at compile time, constructs a hash from a `const char*` to an integer index with constant runtime complexity. (Petra also provides its own compile-time string class.) Petra's interface supports heterogenous types in the input set; that is, you could hash a set containing both strings and integers. `chd` also has customization points for computing a hash for a user-provided type. The algorithm is based heavily on Steve Hanov's implementation in his [blog post](http://stevehanov.ca/blog/index.php?id=119).
- `linear_hash.hpp`: A linear time complexity hash meant to be used as a fallback for `chd` when given small input sets, which it cannot handle.
- `callback_table.hpp`: Building on `chd` and `sequential_table`, a system for triggering callbacks given a heterogenous set of inputs.
- `map.hpp`: A heterogenous map with variant-like access.

If you find these features useful or interesting, check out `examples` to see how they are used.

## Roadmap

- [ ] GCC support
- [ ] C++14 support (probably using Hana backend)
- [ ] Fuzz tests
- [ ] Update benchmark generation
- [ ] Demonstrate more complicated "Constant" data structures
- [ ] Support more string hash algorithms
