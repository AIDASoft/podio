#ifndef PODIO_MAYBESHAREDPTR_H
#define PODIO_MAYBESHAREDPTR_H

#include <atomic>

namespace podio {

namespace detail {
  /// Tag struct to create MaybeSharedPtr instances that initially own their
  /// managed pointer and hence will be created with a control block (ownership of
  /// the managed pointer may still change later!)
  struct MarkOwnedTag {};
}

constexpr static auto MarkOwned [[maybe_unused]] = detail::MarkOwnedTag{};

/// "Semi-smart" pointer class for pointers that at some point during their
/// lifetime might hand over management to another entitity. E.g. Objects that
/// are added to a collection will hand over the management of their Obj* to
/// collection. In such a case two things need to be considered:
/// - Other Objects with the same Obj* instance should not delete the managed
///   Obj*, even if the last Object goes out of scope
/// - Even if the managed Obj* is gone (e.g. collection has gone out of scope or
///   was cleared), the remaining Object instances should still be able to
///   gracefully destruct, even if they are at this point merely an "empty husk"
/// The MaybeSharedPtr achieves this by having an optional control block that
/// controls the lifetime of itself and potentially the managed Obj*.
template<typename T>
class MaybeSharedPtr {
public:
  /// Constructor from raw pointer. Assumes someone else manages the pointer
  /// already
  explicit MaybeSharedPtr(T* p) : m_ptr(p) {}

  /// Constructor from a raw pointer assuming ownership in the process
  explicit MaybeSharedPtr(T* p, detail::MarkOwnedTag) : m_ptr(p), m_ctrlBlock(new ControlBlock()) {}

  /// Copy constructor
  MaybeSharedPtr(const MaybeSharedPtr& other) : m_ptr(other.m_ptr), m_ctrlBlock(other.m_ctrlBlock) {
    // Increase the reference count if there is a control block
    m_ctrlBlock && m_ctrlBlock->count++;
  }

  /// Assignment operator
  MaybeSharedPtr& operator=(MaybeSharedPtr other) {
    swap(*this, other);
    return *this;
  }

  /// Move constructor
  MaybeSharedPtr(MaybeSharedPtr&& other) : m_ptr(other.m_ptr), m_ctrlBlock(other.m_ctrlBlock) {
    other.m_ptr = nullptr;
    other.m_ctrlBlock = nullptr;
  }

  /// Destructor
  ~MaybeSharedPtr() {
    // Only if we have a control block, do we assume that we have any
    // responsibility in cleaning things up
    if (m_ctrlBlock && --m_ctrlBlock->count == 0) {
      // When the reference count reaches 0 we have to clean up control block in
      // any case, but first we have to find out whether we also need to clean
      // up the "managed" pointer
      if (m_ctrlBlock->owned) {
        delete m_ptr;
      }
      delete m_ctrlBlock;
    }
  }

  /// Get a raw pointer to the managed pointer. Do not change anything
  /// concerning the management of the pointer
  T* get() const { return m_ptr; }

  /// Get a raw pointer to the managed pointer and assume ownership.
  T* release() {
    // From now on we only need to keep track of the control block
    m_ctrlBlock->owned = false;
    return m_ptr;
  }

  operator bool() const { return m_ptr; }

  T* operator->() { return m_ptr; }
  const T* operator->() const { return m_ptr; }

  T& operator*() { return *m_ptr; }
  const T& operator*() const { return *m_ptr; }

  template<typename U>
  friend void swap(MaybeSharedPtr<U>& a, MaybeSharedPtr<U>& b);

  // avoid a bit of typing for having all the necessary combinations of
  // comparison operators
#define DECLARE_COMPARISON_OPERATOR(op)                                                 \
  template<typename U>                                                                  \
  friend bool operator op (const MaybeSharedPtr<U>& lhs, const MaybeSharedPtr<U>& rhs); \
  template<typename U>                                                                  \
  friend bool operator op (const MaybeSharedPtr<U>& lhs, const U* rhs);                 \
  template<typename U>                                                                  \
  friend bool operator op (const U* lhs, const MaybeSharedPtr<U>& rhs);                 \

  DECLARE_COMPARISON_OPERATOR(==)
  DECLARE_COMPARISON_OPERATOR(!=)
  DECLARE_COMPARISON_OPERATOR(<)
#undef DECLARE_COMPARISON_OPERATOR

private:
  /// Simple control structure that controls the behavior of the
  /// MaybeSharedPtr destructor. Keeps track of how many references of the
  /// ControlBlock are currently still alive and whether the managed pointer
  /// should be destructed alongside the ControlBlock, once the reference count
  /// reaches 0.
  struct ControlBlock {
    std::atomic<unsigned> count{1}; ///< reference count
    std::atomic<bool> owned{true}; ///< ownership flag for the managed pointer. true == we manage the pointer
  };

  T* m_ptr{nullptr};
  ControlBlock* m_ctrlBlock{nullptr};
};

template<typename T>
void swap(MaybeSharedPtr<T>& a, MaybeSharedPtr<T>& b) {
  using std::swap;
  swap(a.m_ptr, b.m_ptr);
  swap(a.m_ctrlBlock, b.m_ctrlBlock);
}

// helper macro for avoiding a bit of typing/repetition
#define DEFINE_COMPARISON_OPERATOR(op)                                          \
template<typename U>                                                            \
bool operator op (const MaybeSharedPtr<U>& lhs, const MaybeSharedPtr<U>& rhs) { \
  return lhs.m_ptr op rhs.m_ptr;                                                \
}                                                                               \
template<typename U>                                                            \
bool operator op (const MaybeSharedPtr<U>& lhs, const U* rhs) {                 \
  return lhs.m_ptr op rhs;                                                      \
}                                                                               \
template<typename U>                                                            \
bool operator op (const U* lhs, const MaybeSharedPtr<U>& rhs) {                 \
  return lhs op rhs.m_ptr;                                                      \
}                                                                               \

DEFINE_COMPARISON_OPERATOR(==)
DEFINE_COMPARISON_OPERATOR(!=)
DEFINE_COMPARISON_OPERATOR(<)
#undef DEFINE_COMPARISON_OPERATOR

}

#endif // PODIO_MAYBESHAREDPTR_H
