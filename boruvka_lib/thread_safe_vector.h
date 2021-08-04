#pragma once

#include <mutex>
#include <shared_mutex>
#include <vector>


template <typename T>
class ThreadSafeVector {
 public:
  ThreadSafeVector() {
  }

  T operator[](size_t index) const {
    std::shared_lock r_lock(block_);
    return vector_[index];
  }

  size_t Size() const {
    std::shared_lock r_lock(block_);
    return vector_.size();
  }

  void PushBack(const T& value) {
    std::unique_lock wlocker(write_block_);
    std::unique_lock rwlocker(block_, std::defer_lock_t());
    if (Size() == vector_.capacity()) {
      rwlocker.lock();
    }
    vector_.push_back(value);
  }

  std::vector<T> copy() const {
    std::unique_lock rwlocker(block_);
    return vector_;
  }

 private:
  mutable std::shared_mutex block_;
  std::mutex write_block_;
  std::vector<T> vector_;
};
