// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once

#include <type_traits>

#include "petra/utilities.hpp"

namespace petra {

  // requires Error to be default constructible
  template<typename T, typename Error>
  struct Expected {
    constexpr Expected(T&& value) : value_(value), is_valid(true) {}

    constexpr Expected(Error&& error) : error_(error), is_valid(false) {}

    constexpr Expected(const T& value) : value_(value), is_valid(true) {}

    constexpr Expected(const Error& error) : error_(error), is_valid(false) {}

    constexpr bool valid() const noexcept { return is_valid; }

    constexpr bool errored() const noexcept { return !is_valid; }

    constexpr auto value() {
      if (valid()) {
        if constexpr (utilities::is_reference_wrapper<T>{}) {
          return &(value_.get());
        } else {
          return &value_;
        }
      } else {
        if constexpr (utilities::is_reference_wrapper<T>{}) {
          return static_cast<typename T::type*>(nullptr);
        } else {
          return static_cast<T*>(nullptr);
        }
      }
    }

    constexpr Error* error() {
      if (errored()) {
        return &error_;
      } else {
        return nullptr;
      }
    }

    template<typename Visitor, typename Handler>
    constexpr decltype(auto) visit(Visitor&& visitor, Handler&& error_handler) {
      if (valid()) {
        return visitor(value_);
      } else {
        return error_handler(error_);
      }
    }

    // unwrap this and then wrap the result of callback in another Expected type
    template<typename Callback>
    constexpr decltype(auto) then(Callback&& callback) {
      using R = std::result_of_t<Callback(T)>;
      using E = Expected<R, Error>;
      if (valid()) {
        if constexpr (!std::is_same<R, void>{}) {
          return E(callback(value_));
        } else {
          callback(value_);
          return E();
        }
      } else {
        return E(error_);
      }
    }

  private:
    union {
      Error error_;
      T value_;
    };
    bool is_valid;
  };

  template<typename Error>
  struct Expected<void, Error> {
    constexpr Expected() : is_valid(true) {}

    constexpr Expected(Error&& error) : error_(error), is_valid(false) {}

    constexpr Expected(const Error& error) : error_(error), is_valid(false) {}

    constexpr bool valid() const noexcept { return is_valid; }

    constexpr bool errored() const noexcept { return !is_valid; }

    constexpr Error* error() {
      if (errored()) {
        return &error_;
      } else {
        return nullptr;
      }
    }

    template<typename Visitor, typename Handler>
    constexpr decltype(auto) visit(Visitor&& visitor, Handler&& error_handler) {
      if (valid()) {
        return visitor();
      } else {
        return error_handler(error_);
      }
    }

    // unwrap this and then wrap the result of callback in another Expected type
    template<typename Callback>
    constexpr decltype(auto) then(Callback&& callback) {
      if (valid()) {
        using R = std::result_of_t<Callback()>;
        return Expected<R, Error>(callback());
      } else {
        return Expected<void, Error>(error_);
      }
    }

  private:
    Error error_;
    bool is_valid;
  };

  template<typename T, typename E, typename... Args>
  static decltype(auto) emplace_expected_value(Args&&... args) {
    return Expected<T, E>(T(std::forward<Args>(args)...));
  }

  template<typename T, typename E, typename... Args>
  static decltype(auto) emplace_expected_error(Args&&... args) {
    return Expected<T, E>(E(std::forward<Args>(args)...));
  }

}  // namespace petra
