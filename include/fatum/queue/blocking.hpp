// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#ifndef FATUM_QUEUE_BLOCKING_HPP_
#define FATUM_QUEUE_BLOCKING_HPP_

#include <deque>
#include <mutex>

namespace fatum {
namespace queue {

template <class ItemType>
class Blocking {
 public:
  template <class ...ArgTypes>
  void push(ItemType const& item, ArgTypes&&... optional) {
    std::lock_guard<std::mutex> lock(mutex_);
    push_(item, std::forward<ArgTypes>(optional)...);
  }

  template <class VisitorType>
  void pop(VisitorType const& visit) {
    std::lock_guard<std::mutex> lock(mutex_);

    while (storage_.size()) {
      visit(storage_.front());
      storage_.pop_front();
    }
  }
 private:
  std::deque<ItemType> storage_;
  std::mutex mutex_;

  template <class ...ArgTypes>
  void push_(ItemType const& item, ArgTypes&&... optional) {
    storage_.push_back(item);
    push_(std::forward<ArgTypes>(optional)...);
  }
  void push_() {
  }
}; // class Blocking

} // namespace queue
} // namespace fatum

#endif // include guard


