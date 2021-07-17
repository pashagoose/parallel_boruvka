#include <atomic>
#include <optional>

template <typename T>
class MPSCQueue {
 public:
  MPSCQueue() = default;

  ~MPSCQueue() {
    auto current_head = head_.load();
    while (current_head) {
      head_.store(current_head->next);
      delete current_head;
      current_head = head_.load();
    }
  }

  void Push(const T& value) {
    Node* ptr_node = new Node(value);
    for (;;) {
      ptr_node->next = head_.load();
      Node* cpy = ptr_node->next;
      if (head_.compare_exchange_weak(cpy, ptr_node)) {
        return;
      }
    }
  }

  std::optional<T> Pop() {
    auto current_head = head_.load();
    for (;;) {
      if (!current_head) {
        return std::nullopt;
      }
      if (head_.compare_exchange_weak(current_head, current_head->next)) {
        break;
      }
      current_head = head_.load();
    }
    std::optional<T> result(current_head->element);
    delete current_head;
    return result;
  }

 private:
  
  struct Node {
    Node* next;
    T element;

    Node() : next(nullptr) {}

    Node(const T& value) : next(nullptr), element(value) {}
  };

  std::atomic<Node*> head_{nullptr};
};