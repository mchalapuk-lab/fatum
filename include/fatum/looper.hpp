// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#ifndef FATUM_LOOPER_HPP_
#define FATUM_LOOPER_HPP_

#include <deque>
#include <condition_variable>
#include <mutex>

#include "fatum/fwd/handler.hpp"
#include "fatum/event.hpp"

namespace fatum {

class Looper {
 public:
  typedef std::function<void (std::chrono::microseconds)> MainTask;
  typedef std::function<void ()> ExtraTask;
  typedef std::function<void (fatum::IterationEvent const&)> Listener;

  Looper(size_t min_freq = 60,
         size_t max_freq = 60,
         Listener const& listener = [] (fatum::IterationEvent const&) {}) :
      min_iter_duration_(freqToIterDuration(max_freq)),
      max_iter_duration_(freqToIterDuration(min_freq)),
      listener_(listener),
      event_{0,
             std::chrono::microseconds::zero(),
             std::chrono::microseconds::zero()} {
  }
  Looper(Looper const&) = delete;

  void setMinFrequency(size_t hz) {
    enqueue([this, hz] { max_iter_duration_ = freqToIterDuration(hz); });
  }
  void setMaxFrequency(size_t hz) {
    enqueue([this, hz] { min_iter_duration_ = freqToIterDuration(hz); });
  }

  void prepare();
  void operator() (MainTask task);
 private:
  typedef std::chrono::steady_clock ClockType;

  std::mutex mutex_;
  std::condition_variable loop_start_;
  std::condition_variable loop_prepared_;

  std::deque<ExtraTask> task_queue_;
  MainTask main_task_;

  std::chrono::microseconds min_iter_duration_;
  std::chrono::microseconds max_iter_duration_;
  Listener listener_;
  IterationEvent event_;

  void enqueue(ExtraTask task) {
    std::lock_guard<std::mutex> lock(mutex_);
    task_queue_.push_back(task);
  }
  size_t getTaskCount() {
    std::lock_guard<std::mutex> lock(mutex_);
    return task_queue_.size();
  }

  static std::chrono::microseconds freqToIterDuration(size_t hz) {
    if (!hz) {
      return std::chrono::microseconds::zero();
    }
    static const std::chrono::microseconds one_second = std::chrono::seconds(1);
    std::chrono::microseconds duration = one_second / hz;
    return duration;
  }

  void loop();

  friend class Handler;
}; // class Looper

} // namespace fatum

#endif // include guard

