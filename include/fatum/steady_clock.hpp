// author: Kacper Gotfryd
// license: Apache 2
#ifndef FATUM_STEADY_CLOCK_HPP_
#define FATUM_STEADY_CLOCK_HPP_
namespace fatum {
class steady_clock {
 public:
  std::chrono::time_point<std::chrono::steady_clock> now() {
    return std::chrono::steady_clock::now();
  }
};
}
#endif

