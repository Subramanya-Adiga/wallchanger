#pragma once
#include <mutex>
#include <queue>

namespace wallchanger::net {
template <typename T> class connect_queue {
public:
  connect_queue() = default;
  connect_queue(const connect_queue &) = delete;
  connect_queue &operator=(const connect_queue &) = delete;
  connect_queue(connect_queue &&) noexcept = default;
  connect_queue &operator=(connect_queue &&) noexcept = default;
  ~connect_queue() { clear(); }

  [[nodiscard]] const T &front() {
    std::scoped_lock lock(m_queue_mutex);
    return m_queue.front();
  }

  [[nodiscard]] const T &back() {
    std::scoped_lock lock(m_queue_mutex);
    return m_queue.back();
  }

  [[nodiscard]] T pop_front() {
    std::scoped_lock lock(m_queue_mutex);
    auto return_type = std::move(m_queue.front());
    m_queue.pop_front();
    return return_type;
  }

  [[nodiscard]] T pop_back() {
    std::scoped_lock lock(m_queue_mutex);
    auto return_type = std::move(m_queue.back());
    m_queue.pop_back();
    return return_type;
  }

  void push_front(const T &data) {
    std::scoped_lock lock(m_queue_mutex);
    m_queue.push_front(data);
    std::unique_lock<std::mutex> ulock(m_blocking_mutex);
    m_cv_blocking.notify_one();
  }

  void push_back(const T &data) {
    std::scoped_lock lock(m_queue_mutex);
    m_queue.push_back(data);
    std::unique_lock<std::mutex> ulock(m_blocking_mutex);
    m_cv_blocking.notify_one();
  }

  [[nodiscard]] bool empty() {
    std::scoped_lock lock(m_queue_mutex);
    return m_queue.empty();
  }

  [[nodiscard]] size_t size() {
    std::scoped_lock lock(m_queue_mutex);
    return m_queue.size();
  }

  void clear() {
    std::scoped_lock lock(m_queue_mutex);
    m_queue.clear();
  }

  void wait() {
    while (empty()) {
      std::unique_lock<std::mutex> ulock(m_blocking_mutex);
      m_cv_blocking.wait(ulock);
    }
  }

private:
  std::mutex m_queue_mutex;
  std::deque<T> m_queue;
  std::condition_variable m_cv_blocking;
  std::mutex m_blocking_mutex;
};
} // namespace wallchanger::net