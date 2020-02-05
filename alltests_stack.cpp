#include <deque>
#include <iostream>
#include <random>
#include <stack>
#include <thread>
#include <unordered_set>
#include <vector>
#include "helper.h"
#include "safe_stack.h"

#define MAX_BUFFER_SIZE 33
#define NUM_THREADS 4

template <class Container>
void generate_data(Container& data) {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, UINT32_MAX / 2);
  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) data.push_back(tmp);
}

/*
 * testing basic function and property of FIFO
 */
TEST(Blocking, SerialDeque) {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, UINT32_MAX / 2);
  std::deque<std::size_t> d;
  LockGuardFIFO<std::size_t, MAX_BUFFER_SIZE> fifo;
  std::size_t tmp;
  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) {
    tmp = dist(rd);
    d.push_back(tmp);
    fifo.push(tmp);
  }

  // fifo.print();

  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) {
    auto v_true = d.front();
    auto v_test = fifo.top();
    EXPECT_EQ(v_true, v_test);
    d.pop_front();
    fifo.pop();
  }
}

/*
 * testing basic function and property of stack
 */
TEST(Blocking, SerialStack) {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, UINT32_MAX / 2);
  std::stack<std::size_t> s;
  LockGuardStack<std::size_t, MAX_BUFFER_SIZE> stack;
  std::size_t tmp;
  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) {
    tmp = dist(rd);
    s.push(tmp);
    stack.push(tmp);
  }

  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) {
    auto v_true = s.top();
    auto v_test = stack.top();
    EXPECT_EQ(v_true, v_test);
    s.pop();
    stack.pop();
  }
}

template <typename TestClass>
void insert_test(TestClass& test, std::unordered_set<std::size_t> ref,
                 int pos) {
  auto it = ref.begin();
  std::advance(it, pos * (MAX_BUFFER_SIZE - 1) / NUM_THREADS);
  for (int i = 0; i < (MAX_BUFFER_SIZE - 1) / NUM_THREADS; i++)
    test.push(*(it++));
}

/*
 * testing parallel property of FIFO under multiple producor and no comsumer
 * to ensure that we do not miss any insert
 */
TEST(Blocking, ParallelDeque_MPNC) {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, UINT32_MAX / 2);

  std::unordered_set<std::size_t> true_reference;
  LockGuardFIFO<std::size_t, MAX_BUFFER_SIZE> fifo;
  for (int i = 1; i <= MAX_BUFFER_SIZE - 1; i++) {
    true_reference.insert(dist(rd));
  }

  std::vector<std::thread*> thread_buf;
  for (int i = 0; i < NUM_THREADS; i++) {
    std::thread* t = new std::thread(
        insert_test<LockGuardFIFO<std::size_t, MAX_BUFFER_SIZE>>,
        std::ref(fifo), true_reference, i);
    thread_buf.push_back(t);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    thread_buf[i]->join();
    delete thread_buf[i];
  }

  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) {
    auto v_test = fifo.top();
    EXPECT_EQ(true_reference.count(v_test), 1);
    fifo.pop();
  }
}

/*
 * testing parallel property of stack under multiple producor and no comsumer
 * to ensure that we do not miss any insert
 */
TEST(Blocking, ParallelStack_MPNC) {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, UINT32_MAX / 2);

  std::unordered_set<std::size_t> true_reference;
  LockGuardStack<std::size_t, MAX_BUFFER_SIZE> stack;
  for (int i = 1; i <= MAX_BUFFER_SIZE - 1; i++) {
    true_reference.insert(dist(rd));
  }

  std::vector<std::thread*> thread_buf;
  for (int i = 0; i < NUM_THREADS; i++) {
    std::thread* t = new std::thread(
        insert_test<LockGuardStack<std::size_t, MAX_BUFFER_SIZE>>,
        std::ref(stack), true_reference, i);
    thread_buf.push_back(t);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    thread_buf[i]->join();
    delete thread_buf[i];
  }

  for (int i = 0; i < MAX_BUFFER_SIZE - 1; i++) {
    auto v_test = stack.top();
    EXPECT_EQ(true_reference.count(v_test), 1);
    stack.pop();
  }
}