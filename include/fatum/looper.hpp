// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#ifndef FATUM_LOOPER_HPP_
#define FATUM_LOOPER_HPP_

#include <deque>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "fatum/fwd/handler.hpp"
#include "fatum/event.hpp"
#include "fatum/steady_clock.hpp"

namespace fatum {

template<class ClockType = SteadyClock, 
class MainTask = std::function<void (std::chrono::microseconds)>, 
class ExtraTask = std::function<void ()>>
class Looper {
 public:
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

  void prepare() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (!main_task_) {
        loop_start_.wait(lock);
      }
      event_.iteration_ = 1;
      loop_prepared_.notify_one();  
    }
    loop();
  }

  void operator() (MainTask task) {
    std::unique_lock<std::mutex> lock(mutex_);
    main_task_ = task;
    loop_start_.notify_one();

    if (!event_.iteration_) {
      loop_prepared_.wait(lock);
    }
  }

 private:
  ClockType clock_;

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

  template <class Rep, class Period> 
  std::chrono::microseconds microsec_cast(std::chrono::duration<Rep, Period> d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d);
  }

  void loop() {
    auto prev_iter_start = clock_();
    
    while (true) {
      auto curr_iter_start = clock_();
      auto time_delta = curr_iter_start - prev_iter_start;
      prev_iter_start = curr_iter_start;
      size_t task_count = getTaskCount();
      
      while (task_count--) {
        task_queue_.front()();
        std::lock_guard<std::mutex> lock(mutex_);
        task_queue_.pop_front();
      }
      main_task_(microsec_cast(time_delta));

      if (min_iter_duration_.count()) {
        auto curr_iter_end = clock_();
        auto work_time = curr_iter_end - curr_iter_start;
        event_.work_time_ = microsec_cast(work_time);
        event_.sleep_time_ = min_iter_duration_ - event_.work_time_;

        listener_(event_);
        std::this_thread::sleep_for(event_.sleep_time_);
      }
      event_.iteration_ += 1;
    }
  }

  friend class Handler;
}; // class Looper

} // namespace fatum

#endif // include guard

