// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#include "fatum/looper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace test {

struct MockListener {
  MOCK_METHOD1(call, void (fatum::IterationEvent const&));

  void operator() (fatum::IterationEvent const& evt) {
    call(evt);
  }
}; // struct MockListener

struct MockMainTask {
  MOCK_METHOD1(call, void (std::chrono::microseconds));

  void operator() (std::chrono::microseconds time_delta) {
    call(time_delta);
  }
}; // struct MockMainTask

template <class ...ArgTypes, class FunctorType>
std::function<void (ArgTypes...)> wrap(FunctorType &functor) {
  return [&functor] (ArgTypes ...args) { functor(args...); };
}

} // namespace test

using namespace test;
using namespace testing;

TEST(fatum_Looper, test_main_task_called) {
  fatum::Looper<> tested_loop;

  MockMainTask mock_task;
  EXPECT_CALL(mock_task, call(_)).WillOnce(Throw("exception"));

  tested_loop.prepare(wrap<std::chrono::microseconds>(mock_task));
  try{
    tested_loop();
  }catch(const char*){
    return;
  }
}

