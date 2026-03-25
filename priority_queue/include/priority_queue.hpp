#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cmath>       // maybe you need it
#include <cstddef>     // for size_t
#include <functional>  // for std::less
#include <iostream>
#include <ostream>

#include "exceptions.hpp"

namespace sjtu {

template <class T, class Compare = std::less<T>>
class priority_queue {
 public:
  struct Node {
    size_t degree_ = 0;
    T data_;
    Node *brother_ = nullptr;
    Node *child_ = nullptr;
  };
  size_t size_ = 0;
  Node *root_= nullptr;
  void copy(Node *root, Node *other_root) {
    if (other_root->child_ != nullptr) {
      root->child_ = new Node {
        .degree_ = other_root->child_->degree_, 
        .data_ = other_root->child_->data_
      };
      copy(root->child_, other_root->child_);
    }
    if (other_root->brother_ != nullptr) {
      root->brother_ = new Node {
        .degree_ = other_root->brother_->degree_, 
        .data_ = other_root->brother_->data_
      };
      copy(root->brother_, other_root->brother_);
    }
  }
  void clear(Node *root) {
    if (root == nullptr) {
      return ;
    }
    clear(root->child_);
    clear(root->brother_);
    delete root;
  }
  inline void swap(Node *&ptr1, Node *&ptr2) {
    Node *tmp = ptr1;
    ptr1 = ptr2;
    ptr2 = tmp;
  }
  Node *mergeTree(Node *root1, Node *root2) { // caller need to keep the root's brother
    if (root1 == nullptr) {
      return root2;
    }
    if (root2 == nullptr) {
      return root1;
    }
    if (Compare()(root1->data_, root2->data_)) {
      swap(root1, root2);
    }
    root2->brother_ = root1->child_;
    root1->child_ = root2;
    root1->brother_ = nullptr;
    root1->degree_++;
    return root1;
  }
  void checkmerge(Node *root1, Node *root2) {
    if (root1 == nullptr || root2 == nullptr) {
      return ;
    }
    Node *ptr1 = root1;
    Node *ptr2 = root2;
    Node *carry = nullptr;
    size_t cnt = 0;
    while (ptr1 != nullptr && ptr2 != nullptr) {
      //std::cerr << ptr1 << " " << ptr2 << std::endl;
      Node *brother1 = ptr1->brother_;
      Node *brother2 = ptr2->brother_;
      if (carry == nullptr) {
        if (ptr1->degree_ == ptr2->degree_) { //011
          if (Compare()(ptr1->data_, ptr2->data_)) {
            carry = new Node {
              .degree_ = ptr2->degree_ + 1,
              .data_ = ptr2->data_, 
            };
          } else {
            carry = new Node {
              .degree_ = ptr1->degree_ + 1,
              .data_ = ptr1->data_, 
            };
          }
          ptr1 = brother1;
          ptr2 = brother2;
        } else if (ptr1->degree_ < ptr2->degree_) { //010
          ptr1 = brother1;
        } else { //001
          ptr2 = brother2;
        }
      } else {
        if (carry->degree_ < ptr1->degree_) {
          if (carry->degree_ < ptr2->degree_) { //100
            delete carry;
            carry = nullptr;
          } else { // 101
            Node *tmp = carry;
            if (Compare()(tmp->data_, ptr2->data_)) {
              carry = new Node {
                .degree_ = ptr2->degree_ + 1,
                .data_ = ptr2->data_, 
              };
            } else {
              carry = new Node {
                .degree_ = tmp->degree_ + 1,
                .data_ = tmp->data_, 
              };
            }
            delete tmp;
            ptr2 = brother2;
          }
        } else {
          if (carry->degree_ < ptr2->degree_) { //110
            Node *tmp = carry;
            if (Compare()(tmp->data_, ptr1->data_)) {
              carry = new Node {
                .degree_ = ptr1->degree_ + 1,
                .data_ = ptr1->data_, 
              };
            } else {
              carry = new Node {
                .degree_ = tmp->degree_ + 1,
                .data_ = tmp->data_, 
              };
            }
            delete tmp;
            //carry = mergeTree(carry, ptr1);
            ptr1 = brother1;
          } else { // 111
            Node *tmp = carry;
            if (Compare()(ptr1->data_, ptr2->data_)) {
              carry = new Node {
                .degree_ = ptr2->degree_ + 1,
                .data_ = ptr2->data_, 
              };
            } else {
              carry = new Node {
                .degree_ = ptr1->degree_ + 1,
                .data_ = ptr1->data_, 
              };
            }
            delete tmp;
            //carry = mergeTree(ptr1, ptr2);
            ptr1 = brother1;
            ptr2 = brother2;
          }
        }
      }
    }
    if (ptr1 != nullptr) {
      if (carry != nullptr) {
        while (ptr1 != nullptr && carry->degree_ == ptr1->degree_) {
          Node *brother1 = ptr1->brother_;
          Node *tmp = carry;
          if (Compare()(tmp->data_, ptr1->data_)) {
            carry = new Node {
              .degree_ = ptr1->degree_ + 1,
              .data_ = ptr1->data_, 
            };
          } else {
            carry = new Node {
              .degree_ = tmp->degree_ + 1,
              .data_ = tmp->data_, 
            };
          }
          delete tmp;
          //carry = mergeTree(carry, ptr1);
          ptr1 = brother1;
        }
      }
    }
    if (ptr2 != nullptr) {
      if (carry != nullptr) {
        while (ptr2 != nullptr && carry->degree_ == ptr2->degree_) {
          Node *brother2 = ptr2->brother_;
          Node *tmp = carry;
          if (Compare()(tmp->data_, ptr2->data_)) {
            carry = new Node {
              .degree_ = ptr2->degree_ + 1,
              .data_ = ptr2->data_, 
            };
          } else {
            carry = new Node {
              .degree_ = tmp->degree_ + 1,
              .data_ = tmp->data_, 
            };
          }
          delete tmp;
          //carry = mergeTree(carry, ptr2);
          ptr2 = brother2;
        }
      }
    }
    delete carry;
    return ;
  }
  Node *mergeHeap(Node *root1, Node *root2) {
    if (root1 == nullptr) {
      return root2;
    }
    if (root2 == nullptr) {
      return root1;
    }
    Node *new_root = nullptr;
    Node **cur = &new_root;
    Node *ptr1 = root1;
    Node *ptr2 = root2;
    Node *carry = nullptr;
    size_t cnt = 0;
    while (ptr1 != nullptr && ptr2 != nullptr) {
      Node *brother1 = ptr1->brother_;
      Node *brother2 = ptr2->brother_;
      if (carry == nullptr) {
        if (ptr1->degree_ == ptr2->degree_) { //011
          carry = mergeTree(ptr1, ptr2);
          ptr1 = brother1;
          ptr2 = brother2;
        } else if (ptr1->degree_ < ptr2->degree_) { //010
          *cur = ptr1;
          cur = &((*cur)->brother_);
          ptr1 = brother1;
        } else { //001
          *cur = ptr2;
          cur = &((*cur)->brother_);
          ptr2 = brother2;
        }
      } else {
        if (carry->degree_ < ptr1->degree_) {
          if (carry->degree_ < ptr2->degree_) { //100
            *cur = carry;
            cur = &((*cur)->brother_);
            carry = nullptr;
          } else { // 101
            carry = mergeTree(carry, ptr2);
            ptr2 = brother2;
          }
        } else {
          if (carry->degree_ < ptr2->degree_) { //110
            carry = mergeTree(carry, ptr1);
            ptr1 = brother1;
          } else { // 111
            *cur = carry;
            cur = &((*cur)->brother_);
            carry = mergeTree(ptr1, ptr2);
            ptr1 = brother1;
            ptr2 = brother2;
          }
        }
      }
    }
    if (ptr1 != nullptr) {
      if (carry != nullptr) {
        while (ptr1 != nullptr && carry->degree_ == ptr1->degree_) {
          Node *brother1 = ptr1->brother_;
          carry = mergeTree(carry, ptr1);
          ptr1 = brother1;
        }
        *cur = carry;
        carry = nullptr;
        cur = &((*cur)->brother_);
      }
      if (ptr1 != nullptr) {
        *cur = ptr1;
        cur = &((*cur)->brother_);
      }
    }
    if (ptr2 != nullptr) {
      if (carry != nullptr) {
        while (ptr2 != nullptr && carry->degree_ == ptr2->degree_) {
          Node *brother2 = ptr2->brother_;
          carry = mergeTree(carry, ptr2);
          ptr2 = brother2;
        }
        *cur = carry;
        carry = nullptr;
        cur = &((*cur)->brother_);
      }
      if (ptr2 != nullptr) {
        *cur = ptr2;
        cur = &((*cur)->brother_);
      }
    }
    if (carry != nullptr) {
      *cur = carry;
      cur = &((*cur)->brother_);
    }
    //*cur = nullptr;
    return new_root;
  }
  priority_queue() = default;
  priority_queue(const priority_queue &other) {
    size_ = other.size_;
    if (other.root_ != nullptr) {
      root_ = new Node {
        .degree_ = other.root_->degree_, 
        .data_ = other.root_->data_
      };
      copy(root_, other.root_);
    }
    
  }
  ~priority_queue() {
    std::cerr << size_ << std::endl;
    clear(root_);
    size_ = 0;
  }

  priority_queue& operator=(priority_queue other) {
    swap(root_, other.root_);
    size_ = other.size_;
    return *this;
  }

  /**
   * @brief Push one element into the queue.
   * @note Its time complexity shall be O(log n).
   */
  void push(const T &data) {
    Node *new_node = new Node {
      .degree_ = 0, 
      .data_ = data
    };
    try {
      checkmerge(root_, new_node);
    } catch(...) {
      delete new_node;
      throw;
    }
    root_ = mergeHeap(root_, new_node);
    size_++;
  }

  /**
   * @return A const reference of the top element in the queue.
   * @throws container_is_empty when the top element does not exist.
   * @note Its time complexity shall be O(1).
   */
  const T& top() const {
    if (empty()) {
      throw container_is_empty();
    }
    //std::cerr << root_ << std::endl;
    Node *ans = root_;
    for (Node *ptr = root_->brother_; ptr != nullptr; ptr = ptr->brother_) {
      //std::cerr << ptr << std::endl;
      if (Compare()(ans->data_, ptr->data_)) {
        ans = ptr;
      }
      //std::cerr << ans << " " << ptr << std::endl;
    }
    return ans->data_;
  }

  Node *reverse(Node *ptr) {
    Node *pre = nullptr;
    Node *cur = ptr;
    while(cur != nullptr) {
      Node *tmp = cur->brother_;
      cur->brother_ = pre;
      pre = cur;
      cur = tmp;
    }
    return pre;
  }

  /**
   * @brief remove the top element in the queue.
   * @throws container_is_empty when the top element does not exist.
   * @note Its time complexity shall be O(n).
   */
  void pop() {
    if (empty()) {
      throw container_is_empty();
    }
    //std::cerr << root_ << std::endl;
    Node *pre_top = nullptr;
    Node *top = root_;
    for (Node *ptr = root_->brother_, *pre = root_; ptr != nullptr; pre = ptr, ptr = ptr->brother_) {
      if (Compare()(top->data_, ptr->data_)) {
        top = ptr;
        pre_top = pre;
      } 
    }
    //std::cerr << "find the top" << std::endl;
    if (pre_top != nullptr) {
      pre_top->brother_ = top->brother_;
    } else {
      root_ = root_->brother_;
    }
    //std::cerr << top->child_ << std::endl;
    Node *tmp = reverse(top->child_);
    delete top;
    //std::cerr << "finish reverse" << std::endl;
    try {
      checkmerge(root_, tmp);
    } catch (...) {
      //std::cerr << "exception" << std::endl;
      reverse(tmp);
      throw;
    }
    //std::cerr << "finish check" << std::endl;
    root_ = mergeHeap(root_, tmp);
    //std::cerr << "finish merge" << std::endl;
    //std::cerr << root_ << " " << tmp << std::endl;
    //std::cerr << root_ << std::endl;
    //std::cerr << root_->data_ << " " << tmp->data_ << std::endl;
    size_--;
    //std::cerr << size_ << std::endl;
    //std::cerr << root_ << std::endl;
  }

  /**
   * @return number of elements in the queue.
   */
  size_t size() const {
    return size_;
  }

  /**
   * @return whether there is any element in the queue.
   */
  bool empty() const {
    return size_ == 0;
  }

  /**
   * @brief Clear all elements in the queue.
   * @note Its time complexity shall be O(n).
   */
  void clear() {
    clear(root_);
    root_ = nullptr;
    size_ = 0;
  }

  /**
   * @brief Merge element sets of two queues.
   * @note Its time complexity shall be O(log n).
   */
  void merge(priority_queue &other) {
    if (&other == this) {
      return ;
    }
    //priority_queue new_this(*this), new_other(other);
    checkmerge(root_, other.root_);
    root_ = mergeHeap(root_, other.root_);
    size_ += other.size_;
    other.root_ = nullptr;
    other.size_ = 0;
  }
};

}  // namespace sjtu

#endif