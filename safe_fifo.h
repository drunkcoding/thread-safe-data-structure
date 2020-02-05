#ifndef _SAFE_FIFO_H_
#define _SAFE_FIFO_H_

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>
#include <atomic>

template <typename T, std::size_t N>
class LockGuardFIFO {
 public:
  bool empty() { return tail_ == (head_ + 1) % N; }
  bool full() { return tail_ == head_; }

  const T& top() { return buf_[(head_ + 1) % N]; }

  bool pop() {
    const std::lock_guard<std::mutex> lock(lk_);
    if (empty()) return true;
    head_ = (head_ + 1) % N;
    return true;
  }
  std::pair<T*, bool> push(const T& value) {
    const std::lock_guard<std::mutex> lock(lk_);
    if (full()) return std::make_pair(nullptr, true);
    buf_[tail_] = value;
    tail_ = (tail_ + 1) % N;
    return std::make_pair(&buf_[(tail_ + N - 1) % N], true);
  }

  LockGuardFIFO() {}
  ~LockGuardFIFO() {}

 private:
  LockGuardFIFO(LockGuardFIFO const&) = delete;
  void operator=(LockGuardFIFO const&) = delete;

 private:
  std::array<T, N> buf_;
  std::size_t head_ = 0;
  std::size_t tail_ = 1;
  bool full_ = false;
  std::mutex lk_;
};

template <typename T, std::size_t N>
class LockFreeFIFO {
 public:
  static LockFreeFIFO* instance() {
    static LockFreeFIFO instance;
    return &instance;
  }

  bool empty() {
    return tail_.load(std::memory_order_acquire) ==
           (head_.load(std::memory_order_acquire) + 1) % N;
  }
  bool full() {
    return tail_.load(std::memory_order_acquire) ==
           head_.load(std::memory_order_acquire);
  }

  const T& top() {
    return buf_[(head_.load(std::memory_order_acquire) + 1) % N];
  }
  bool pop(bool force = false) {
    std::size_t head = head_.load(std::memory_order_acquire);
    if (empty()) return true;
    while (force &&
           !head_.compare_exchange_strong(head, (head + 1) % N,
                                          std::memory_order_release,
                                          std::memory_order_relaxed) &&
           !empty())
      head = (head + 1) % N;
    return force ? true
                 : head_.compare_exchange_strong(head, (head + 1) % N,
                                                 std::memory_order_release,
                                                 std::memory_order_relaxed);
  }
  std::pair<T*, bool> push(const T& value, bool force = false) {
    std::size_t tail = tail_.load(std::memory_order_acquire);
    if (full()) return std::make_pair(nullptr, true);
    while (force &&
           !tail_.compare_exchange_strong(tail, (tail + 1) % N,
                                          std::memory_order_release,
                                          std::memory_order_relaxed) &&
           !full())
      tail = (tail + 1) % N;
    if (!force && !tail_.compare_exchange_strong(tail, (tail + 1) % N,
                                                 std::memory_order_release,
                                                 std::memory_order_relaxed))
      return std::make_pair(nullptr, false);
    buf_[tail] = value;
    return std::make_pair(&buf_[tail], true);
  }

  LockFreeFIFO() {}
  ~LockFreeFIFO() {}

 private:
  LockFreeFIFO(LockFreeFIFO const&) = delete;
  void operator=(LockFreeFIFO const&) = delete;

 private:
  std::array<T, N> buf_;
  std::atomic<std::size_t> head_{0};
  std::atomic<std::size_t> tail_{1};
};

#endif