#ifndef _STACK_H_
#define _STACK_H_

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <utility>

template <typename T, std::size_t N>
class LockGuardStack {
 public:
  bool full() { return N == head_; }
  bool empty() { return 1 == head_; }

  const T& top() { return buf_[head_-1]; }

  bool pop() {
    const std::lock_guard<std::mutex> lock(lk_);
    if (empty()) return true;
    head_ = head_ - 1;
    return true;
  }
  std::pair<T*, bool> push(const T& value) {
    const std::lock_guard<std::mutex> lock(lk_);
    if (full()) return std::make_pair(nullptr, true);
    buf_[head_] = value;
    head_ = head_ + 1;
    return std::make_pair(&buf_[head_ - 1], true);
  }

  LockGuardStack() {}
  ~LockGuardStack() {}

 private:
  LockGuardStack(LockGuardStack const&) = delete;
  void operator=(LockGuardStack const&) = delete;

 private:
  std::array<T, N> buf_;
  std::size_t head_ = 1;
  std::mutex lk_;
};

template <typename T, std::size_t N>
class LockFreeStack {
 public:
  static LockFreeStack* instance() {
    static LockFreeStack instance;
    return &instance;
  }

  bool full() { return N == head_.load(std::memory_order_acquire); }
  bool empty() { return 1 == head_.load(std::memory_order_acquire); }

  const T& top() { return buf_[head_.load(std::memory_order_acquire) - 1]; }
  bool pop(bool force = false) {
    std::size_t head = head_.load(std::memory_order_acquire);
    if (empty()) return true;
    while (force &&
           !head_.compare_exchange_strong(head, head - 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed) &&
           !empty())
      head = head - 1;
    return force ? true : head_.compare_exchange_strong(head, head - 1);
  }
  std::pair<T*, bool> push(T& value, bool force = false) {
    std::size_t head = head_.load(std::memory_order_acquire);
    if (full()) return std::make_pair(nullptr, true);
    while (force &&
           !head_.compare_exchange_strong(head, head + 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed) &&
           !full())
      head = head + 1;
    if (!force && !head_.compare_exchange_strong(head, head + 1,
                                                 std::memory_order_release,
                                                 std::memory_order_relaxed))
      return std::make_pair(nullptr, false);
    buf_[head] = value;
    return std::make_pair(&buf_[head], true);
  }

  LockFreeStack() {}
  ~LockFreeStack() {}

 private:
  LockFreeStack(LockFreeStack const&) = delete;
  void operator=(LockFreeStack const&) = delete;

 private:
  std::array<T, N> buf_;
  std::atomic<std::size_t> head_{1};
};

#endif