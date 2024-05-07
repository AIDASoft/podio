# PODIO Collection as a *Container*

Comparison of the PODIO `Collection`s with a C++ named requirement [*Container*](https://en.cppreference.com/w/cpp/named_req/Container).

The PODIO `Collection`s are move-only classes with emphasis on the distinction between mutable and immutable access to the elements.

### Types

| Name | Type | Requirements | Fulfilled by Collection? | Comment |
|------|------|--------------|--------------------------|---------|
| `value_type` | `T` | *Erasable* | ❌ no | defined as immutable component type|
| `reference` | `T&` |  | ❌ no | not defined |
| `const_reference` | `const T&` | | ❌ no | not defined |
| `iterator` | Iterator whose value type is `T` | *LegacyForwardIterator* convertible to `const_iterator` | ❌ no | not *LegacyForwardIterator*, not convertible to `const_iterator`|
| `const_iterator` | Constant iterator whose value type is `T` | *LegacyForwardIterator* | ❌ no | value type is mutable component type, not *LegacyForwardIterator*
| `difference_type`| Signed integer | Must be the same as `iterator_traits::difference_type` for `iterator` and `const_iterator` | ❌ no | `iterator_traits::difference_type` doesn't |
| `size_type` | Unsigned integer | Large enough to represent all positive values of `difference_type` | ✔️ yes |  |

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
| `a.cbegin()` | `const_iterator` | Same as `const_cast<const C&>(a).begin()` | ✔️ yes |
| `a.cend()` | `const_iterator` | Same as `const_cast<const C&>(a).end()`|  ✔️ yes |
| `a == b` | Convertible to `bool` | Same as `std::equal(a.begin(), a.end(), b.begin(), b.end())`| ❌ no | not defined |
| `a != b` | Convertible to `bool` | Same as `!(a == b)` | ❌ no | not defined |
| `a.swap(b)` | `void` | Exchanges the values of `a` and `b` | ❌ no | not defined |
| `swap(a,b)` | `void` | Same as `a.swap(b)`| ❌ no | not defined |
| `a.size()` | `size_type` | Same as `std::distance(a.begin(), a.end())` | ✔️ yes |
| `a.max_size()` | `size_type` | `b.size()` where b is the largest possible container | ✔️ yes | not defined |
| `a.empty()` | Convertible to `bool` | Same as `a.begin() == a.end()` | ✔️ yes |

## Collection iterator as a *Iterator*

### LegacyIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| *CopyConstructible* | ❌ no / ❌ no | |
| *CopyAssignable* | ❌ no / ❌ no | |
| *Destructible* | ✔️ yes / ✔️ yes | |
| *Swappable* | ✔️ yes / ✔️ yes | |
| `std::iterator_traits::value_type` (Until C++20 ) | ❌ no / ❌ no | |
| `std::iterator_traits::difference_type` | ❌ no / ❌ no | |
| `std::iterator_traits::reference` | ❌ no / ❌ no | |
| `std::iterator_traits::pointer` | ❌ no / ❌ no | |
| `std::iterator_traits::iterator_category` | ❌ no / ❌ no | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `*r` | Unspecified | Dereferenceable | ❌ no / ❌ no | |
| `++r` | `It&` | Incrementable | ❌ no / ❌ no | |

### LegacyInputIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| *LegacyIterator* | ❌ no | ❌ no | |
| *EqualityComparable* | ✔️ yes | ✔️ yes | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `i != j` | Convertible to `bool` | Same as `!(i==j)` | ❌ no / ❌ no | |
| `*i` | Reference, convertible to `value_type` | | ❌ no / ❌ no | |
| `i->m` | | Same as `(*i).m` | ❌ no / ❌ no | |
| `++r` | `It&` | | ❌ no / ❌ no | |
| `(void)r++` | | Same as `(void)++r` | ❌ no / ❌ no | |
| `*r++` | Convertible to `value_type` | Same as `value_type x = *r; ++r; return x;` | ❌ no / ❌ no | |


### LegacyForwardIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| *LegacyInputIterator* | ❌ no / ❌ no | |
| *DefaultConstructible* | ❌ no / ❌ no | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `i++` | `It` | Same as `It ip = i; ++i; return ip;` | ❌ no / ❌ no | |
| `*i++` | `reference` | | ❌ no / ❌ no | |

## Collection and standard algorithms

### Non-modifying sequence operations

| Algorithm | Compatible with Collection? | Comment |
| ----------|-----------------------------|---------|
| `all_of`            | | |
| `any_of`            | | |
| `none_of`           | | |
| `for_each`          | | |
| `for_each_n`        | | |
| `count`             | | |
| `count_if`          | | |
| `mismatch`          | | |
| `find`              | | |
| `find_if`           | | |
| `find_if_not`       | | |
| `find_end`          | | |
| `find_first_of`     | | |
| `adjacent_find`     | | |
| `search`            | | |
| `search_n`          | | |

### Modifying sequence operations

### Partitioning operations

### Sorting operations

### Binary search operations (on sorted ranges)

### Other operations on sorted ranges

### Set operations (on sorted ranges)

### Heap operations

### Minimum/maximum operations

### Comparison operations

### Permutation operations

## Standard ranges
