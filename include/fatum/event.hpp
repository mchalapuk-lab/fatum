// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#ifndef FATUM_EVENT_HPP_
#define FATUM_EVENT_HPP_

namespace fatum {

struct IterationEvent {
  size_t iteration_;
  std::chrono::microseconds work_time_;
  std::chrono::microseconds sleep_time_;
}; // struct IterationEvent

} // namespace fatum

#endif // include guard

