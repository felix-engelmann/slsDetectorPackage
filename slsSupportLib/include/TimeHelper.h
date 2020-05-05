#pragma once
#include <chrono>

#include "TypeTraits.h"
namespace sls {
namespace time {
using ns = std::chrono::nanoseconds;
using us = std::chrono::microseconds;
using ms = std::chrono::milliseconds;
using s = std::chrono::seconds;

// Absolute value of std::chrono::duration
template <class Rep, class Period>
constexpr std::chrono::duration<Rep, Period>
abs(std::chrono::duration<Rep, Period> d) {
    return d >= d.zero() ? d : -d;
}

} // namespace time
} // namespace sls