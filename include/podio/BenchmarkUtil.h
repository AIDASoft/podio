#ifndef PODIO_BENCHMARKUTIL_H
#define PODIO_BENCHMARKUTIL_H

#include <chrono>
#include <functional>
#include <utility>

namespace podio::benchmark {
using ClockT = std::chrono::high_resolution_clock;

/**
 * Run a member function and record the duration. Return the result and the
 * duration in a pair.
 */
template <class Obj, typename MemberFunc, typename... Args>
inline std::pair<std::invoke_result_t<MemberFunc, Obj, Args...>, ClockT::duration>
run_member_timed(Obj& obj, MemberFunc func, Args&&... args) {
  const auto start = ClockT::now();
  const auto retval = std::invoke(func, obj, std::forward<Args>(args)...);
  const auto end = ClockT::now();

  return std::make_pair(retval, end - start);
}

/**
 * Run a member function without return value and record the duration. Return
 * the duration and only use the side-effects of the member function. Can't get
 * this to work in the above version with a void return value, so that is why we
 * have a dedicated function for void functions here.
 */
template <class Obj, typename MemberFunc, typename... Args>
inline ClockT::duration run_void_member_timed(Obj& obj, MemberFunc func, Args&&... args) {
  const auto start = ClockT::now();
  std::invoke(func, obj, std::forward<Args>(args)...);
  const auto end = ClockT::now();

  return end - start;
}

} // namespace podio::benchmark

#endif
