// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#ifndef FATUM_HANDLER_HPP_
#define FATUM_HANDLER_HPP_

#include "fatum/looper.hpp"

namespace fatum {

class Handler {
 public:
  Handler(Looper &looper)
      : looper_(looper) {
  }
  void post(Looper::Task task) {
    looper_.enqueue(task);
  }
 private:
  Looper &looper_;
}; // class Handler

} // namespace fatum

#endif // include guard

