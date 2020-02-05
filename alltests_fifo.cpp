#include <iostream>
#include <stack>
#include <thread>
#include <unordered_set>
#include <vector>
#include "helper.h"

/*
 * testing basic function and property of FIFO
 */
TYPED_TEST(TesterBase, SerialFIFO) {
  TypeParam test_data;
  for (auto& v : this->true_data_) test_data.push(v);
  auto target = this->copyout(test_data);
  this->AssertSetDifference(target);
}

template <class TestClass>
  void insert_test(TestClass& test, const ContainerType true_data, int pos) {
    auto it = true_data.begin();
    std::advance(it, pos * MAX_ELEMENT_SIZE / NUM_THREADS);
    for (int i = 0; i < MAX_ELEMENT_SIZE / NUM_THREADS; i++) test.push(*(it++));
  }

/*
 * testing parallel property of FIFO under multiple producor and no comsumer
 * to ensure that we do not miss or overlap any insert
 */
TYPED_TEST(TesterBase, ParallelFIFO_MPNC) {
  TypeParam test_data;

  std::vector<std::thread> thread_buf;
  for (int i = 0; i < NUM_THREADS; i++)
    thread_buf.emplace_back(insert_test<TypeParam>, std::ref(test_data), this->true_data_, i);

  for (int i = 0; i < NUM_THREADS; i++) thread_buf[i].join();

  auto target = this->copyout(test_data);
  this->AssertSetDifference(target);
}