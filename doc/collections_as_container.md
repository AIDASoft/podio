# PODIO Collection as a *Container*

Comparison of the PODIO `Collection`s with a C++ named requirement [*Container*](https://en.cppreference.com/w/cpp/named_req/Container).

The PODIO `Collection`s are move-only classes with emphasis on the distinction between mutable and non-mutable access to the elements.

### Types

| Name | Type | Requirements | Fulfilled by Collection? | Comment |
|------|------|--------------|--------------------------|---------|
| `value_type` | `T` | *Erasable* | ❌ no | defined as non-mutable component type|
| `reference` | `T&` |  | ❌ no | not defined |
| `const_reference` | `const T&` | | ❌ no | not defined |
| `iterator` | Iterator whose value type is `T` | *LegacyForwardIterator* convertible to `const_iterator` | ❌ no | not *LegacyForwardIterator*, not convertible to `const_iterator`|
| `const_iterator` | Constant iterator whose value type is `T` | *LegacyForwardIterator* | ❌ no | value type is mutable component type, not *LegacyForwardIterator*
| `difference_type`| Signed integer | Must be the same as `iterator_traits::difference_type` for `iterator` and `const_iterator` | ❌ no | `iterator_traits::difference_type` is not valid |
| `size_type` | Unsigned integer | Large enough to represent all positive values of difference_type| ✔️ yes |  |

### Member functions and operators

| Expression | Return type | Semantics | Fulfilled by Collection? | Comment |
|------------|-------------|-----------|--------------------------|---------|
| `C()` | `C` | Creates an empty container | ✔️ yes |
| `C(a)` | `C` | Creates a copy of `a` | ❌ no | non-copyable
| `C(rv)` | `C` | Moves `rv` | ✔️ yes |
| `a = b` | `C&` | Destroys or copy-assigns all elements of `a` from elements of `b` | ❌ no | non-copyable
| `a = rv` | `C&` | Destroys or move-assigns all elements of `a` from elements of `rv` |  ✔️ yes |
| `a.~C()` | `void` | Destroys all elements of `a` and frees all memory|  ✔️ yes |
| `a.begin()` | `(const_)iterator` | Iterator to the first element of `a` |  ✔️ yes |
| `a.end()` | `(const_)iterator` | Iterator to one past the last element of `a` | ✔️ yes |
| `a.cbegin()` | `const_iterator` | `const_cast<const C&>(a).begin()` | ✔️ yes |
| `a.cend()` | `const_iterator` | `const_cast<const C&>(a).end()`|  ✔️ yes |
| `a == b` | Convertible to `bool` | `std::equal(a.begin(), a.end(), b.begin(), b.end())`| ❌ no | not defined |
| `a != b` | Convertible to `bool` | `!(a == b)` | ❌ no | not defined |
| `a.swap(b)` | `void` | Exchanges the values of `a` and `b` | ❌ no | not defined |
| `swap(a,b)` | `void` | `a.swap(b)`| ❌ no | not defined |
| `a.size()` | `size_type` | `std::distance(a.begin(), a.end())` | ✔️ yes |
| `a.max_size()` | `size_type` | `b.size()` where b is the largest possible container | ✔️ yes | not defined |
| `a.empty()` | Convertible to `bool` | `a.begin() == a.end()` | ✔️ yes |
