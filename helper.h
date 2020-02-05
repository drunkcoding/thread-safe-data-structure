#ifndef _HELPER_H_
#define _HELPER_H_

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <deque>
#include <iterator>
#include <random>

#include "safe_fifo.h"

#define NAMED_BENCHMARK(name, funcname)                  \
  BENCHMARK_PRIVATE_DECLARE(name) =                      \
      (::benchmark::internal::RegisterBenchmarkInternal( \
          new ::benchmark::internal::FunctionBenchmark(#name, funcname)))

#define MAX_BUFFER_SIZE 33                      // maximum slots in a data structure
#define MAX_ELEMENT_SIZE (MAX_BUFFER_SIZE - 1)  // waste one slot for fewer variables and operations
#define NUM_THREADS 4                           // number of threads for concurrent testing, to be a division of MAX_ELEMENT_SIZE

typedef std::size_t DataType;
typedef std::deque<DataType> ContainerType;

template <class TestClass>
class TesterBase : public ::testing::Test {
 protected:
  void SetUp() override {
    std::random_device rd;
    std::uniform_int_distribution<DataType> dist(0, UINT32_MAX / 2);
    for (int i = 0; i < MAX_ELEMENT_SIZE; i++) true_data_.push_back(dist(rd));
  }

  /*
  * concurrent do not support copy, use external method to copyout
  * not garentee consistent in MPMC
  */
  ContainerType copyout(TestClass& test) {
    ContainerType target;
    while (!test.empty()) {
      target.push_back(test.top());
      test.pop();
    }
    return target;
  }

  /*
  * compare content of test, assert on any difference
  * not garentee consistent in MPMC
  */
  void AssertSetDifference(const ContainerType& target) {
    ASSERT_EQ(target.size(), MAX_ELEMENT_SIZE);

    ContainerType diff;
    std::set_difference(target.begin(), target.end(), true_data_.begin(),
                        true_data_.end(), std::inserter(diff, diff.begin()));
    ASSERT_EQ(diff.empty(), true);

    diff.clear();
    std::set_difference(true_data_.begin(), true_data_.end(), target.begin(),
                        target.end(), std::inserter(diff, diff.begin()));
    ASSERT_EQ(diff.empty(), true);
  }

 protected:
  ContainerType true_data_;
  TestClass test_data_;
};

using MyTypes = ::testing::Types<LockGuardFIFO<DataType, MAX_BUFFER_SIZE>,
                                 LockFreeFIFO<DataType, MAX_BUFFER_SIZE> >;
TYPED_TEST_SUITE(TesterBase, MyTypes);

#endif