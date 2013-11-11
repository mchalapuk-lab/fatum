// author: Kacper Gotfryd
// license: Apache 2
#ifndef FATUM_STEADY_CLOCK_HPP_
#define FATUM_STEADY_CLOCK_HPP_
namespace fatum {
class SteadyClock {
 public:
  std::chrono::time_point<std::chrono::steady_clock> operator() () {
    return std::chrono::steady_clock::now();
  }
};
}
#endif

