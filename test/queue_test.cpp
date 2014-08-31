// author: Maciej Chałapuk
// license: Apache2
// vim: ts=2 sw=2 expandtab
#include "fatum/queue/blocking.hpp"

#include <functional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace test {

template <typename T>
class fatum_Queue : public ::testing::Test {
};

struct IntFactory {
  typedef int ItemType;
  IntFactory() : next(0) {}
  int operator() () { return next++; }
 private:
  int next;
};

template <class QueueType_, class ItemFactoryType_>
struct TypeParam {
  typedef QueueType_ QueueType;
  typedef ItemFactoryType_ ItemFactoryType;
  typedef typename ItemFactoryType::ItemType ItemType;
};

} // namespace test

using namespace test;
using namespace testing;

TYPED_TEST_CASE_P(fatum_Queue);

TYPED_TEST_P(fatum_Queue, test_pushed_item_can_be_pulled) {
  typename TypeParam::QueueType tested_queue;
  typename TypeParam::ItemFactoryType create_item;

  auto tested_item = create_item();
  tested_queue.push(tested_item);
  tested_queue.pop([&tested_item](typename TypeParam::ItemType &item) {
		    ASSERT_EQ(tested_item, item);
		  });
}

TYPED_TEST_P(fatum_Queue, test_3_pushed_items_can_be_pulled) {
  typename TypeParam::QueueType tested_queue;
  typename TypeParam::ItemFactoryType create_item;

  auto tested_item0 = create_item();
  auto tested_item1 = create_item();
  auto tested_item2 = create_item();
  tested_queue.push(tested_item0);
  tested_queue.push(tested_item1);
  tested_queue.push(tested_item2);

  typedef typename TypeParam::ItemType ItemType;
  std::set<ItemType> popped_items;
  tested_queue.pop([&popped_items](ItemType &item) {
		    popped_items.insert(item);
		  });

  ASSERT_TRUE(popped_items.find(tested_item0) != popped_items.end());
  ASSERT_TRUE(popped_items.find(tested_item1) != popped_items.end());
  ASSERT_TRUE(popped_items.find(tested_item2) != popped_items.end());
}

TYPED_TEST_P(fatum_Queue, test_2_items_pushed_with_one_call_can_be_pulled) {
  typename TypeParam::QueueType tested_queue;
  typename TypeParam::ItemFactoryType create_item;

  auto tested_item0 = create_item();
  auto tested_item1 = create_item();
  tested_queue.push(tested_item0, tested_item1);

  typedef typename TypeParam::ItemType ItemType;
  std::set<ItemType> popped_items;
  tested_queue.pop([&popped_items](ItemType &item) {
		    popped_items.insert(item);
		  });

  ASSERT_TRUE(popped_items.find(tested_item0) != popped_items.end());
  ASSERT_TRUE(popped_items.find(tested_item1) != popped_items.end());
}

REGISTER_TYPED_TEST_CASE_P(fatum_Queue,
                           test_pushed_item_can_be_pulled,
                           test_3_pushed_items_can_be_pulled,
                           test_2_items_pushed_with_one_call_can_be_pulled);

typedef ::testing::Types<
    TypeParam<fatum::queue::Blocking<int>, IntFactory>
    > fatum_QueueTypes;
INSTANTIATE_TYPED_TEST_CASE_P(fatum_QueueParameterized,
                              fatum_Queue,
                              fatum_QueueTypes);

