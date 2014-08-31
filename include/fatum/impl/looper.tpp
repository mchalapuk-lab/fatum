// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab

#ifndef FATUM_LOOPER_HPP_
#error "Use looper.hpp"
#endif 

#include <thread>

namespace fatum {

template<class ClockType, class MainTask, class ExtraTask>
void Looper<ClockType, MainTask, ExtraTask>::operator() () {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!main_task_) {
      loop_start_.wait(lock);
    }
    event_.iteration_ = 1;
  }
  loop();
}

template<class ClockType, class MainTask, class ExtraTask>
void Looper<ClockType, MainTask, ExtraTask>::prepare(MainTask task) {
  std::unique_lock<std::mutex> lock(mutex_);
  main_task_ = task;
  loop_start_.notify_one();
}

template <class Rep, class Period>
std::chrono::microseconds microsec_cast(std::chrono::duration<Rep, Period> d) {
  return std::chrono::duration_cast<std::chrono::microseconds>(d);
}

template<class ClockType, class MainTask, class ExtraTask>
void Looper<ClockType, MainTask, ExtraTask>::loop() {
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

} // namespace fatum

