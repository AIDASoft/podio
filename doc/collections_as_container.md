# PODIO Collection as a *Container*

Comparison of the PODIO `Collection`s with a C++ named requirement [*Container*](https://en.cppreference.com/w/cpp/named_req/Container).

The PODIO `Collection`s are move-only classes with emphasis on the distinction between mutable and immutable access to the elements.

### Container Types

| Name | Type | Requirements | Fulfilled by Collection? | Comment |
|------|------|--------------|--------------------------|---------|
| `value_type` | `T` | *[Erasable](https://en.cppreference.com/w/cpp/named_req/Erasable)* | ✔️ yes | Defined as immutable component type |
| `reference` | `T&` |  | ❌ no | Not defined |
| `const_reference` | `const T&` | | ❌ no | Not defined |
| `iterator` | Iterator whose `value_type` is `T` | [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) convertible to `const_iterator` | ❌ no | `iterator` doesn't have `value_type`, not [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) ([see below](#legacyforwarditerator)), not convertible to `const_iterator`|
| `const_iterator` | Constant iterator whose `value_type` is `T` | [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no | `const_iterator` doesn't have `value_type`, not [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) ([see below](#legacyforwarditerator))
| `difference_type`| Signed integer | Must be the same as `std::iterator_traits::difference_type` for `iterator` and `const_iterator` | ❌ no | `std::iterator_traits::difference_type` doesn't exist |
| `size_type` | Unsigned integer | Large enough to represent all positive values of `difference_type` | ✔️ yes |  |

### Container member functions and operators

| Expression | Return type | Semantics | Fulfilled by Collection? | Comment |
|------------|-------------|-----------|--------------------------|---------|
| `C()` | `C` | Creates an empty container | ✔️ yes |
| `C(a)` | `C` | Creates a copy of `a` | ❌ no | Not defined, non-copyable by design
| `C(rv)` | `C` | Moves `rv` | ✔️ yes |
| `a = b` | `C&` | Destroys or copy-assigns all elements of `a` from elements of `b` | ❌ no | Not defined, non-copyable by design
| `a = rv` | `C&` | Destroys or move-assigns all elements of `a` from elements of `rv` |  ✔️ yes |
| `a.~C()` | `void` | Destroys all elements of `a` and frees all memory|  ✔️ yes |
| `a.begin()` | `(const_)iterator` | Iterator to the first element of `a` |  ✔️ yes |
| `a.end()` | `(const_)iterator` | Iterator to one past the last element of `a` | ✔️ yes |
| `a.cbegin()` | `const_iterator` | Same as `const_cast<const C&>(a).begin()` | ✔️ yes |
| `a.cend()` | `const_iterator` | Same as `const_cast<const C&>(a).end()`|  ✔️ yes |
| `a == b` | Convertible to `bool` | Same as `std::equal(a.begin(), a.end(), b.begin(), b.end())`| ❌ no | Not defined |
| `a != b` | Convertible to `bool` | Same as `!(a == b)` | ❌ no | Not defined |
| `a.swap(b)` | `void` | Exchanges the values of `a` and `b` | ❌ no | Not defined |
| `swap(a,b)` | `void` | Same as `a.swap(b)`| ❌ no | Not defined |
| `a.size()` | `size_type` | Same as `std::distance(a.begin(), a.end())` | ✔️ yes |
| `a.max_size()` | `size_type` | `b.size()` where b is the largest possible container | ✔️ yes | |
| `a.empty()` | Convertible to `bool` | Same as `a.begin() == a.end()` | ✔️ yes |

## Collection iterators as a *Iterator*

### Iterator summary

| Named requirement | Collection::`iterator`| Collection::`const_iterator`|
|-------------------|-----------------------|-----------------------------|
| [LegacyIterator](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no ([see below](#legacyiterator)) | ❌ no ([see below](#legacyiterator)) |
| [LegacyInputIterator](https://en.cppreference.com/w/cpp/named_req/InputIterator) | ❌ no ([see below](#legacyinputiterator)) | ❌ no ([see below](#legacyinputiterator)) |
| [LegacyForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no ([see below](#legacyforwarditerator)) | ❌ no ([see below](#legacyforwarditerator)) |

### LegacyIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*CopyConstructible*](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) | ❌ no / ❌ no | |
| [*CopyAssignable*](https://en.cppreference.com/w/cpp/named_req/CopyAssignable) | ❌ no / ❌ no | |
| [*Destructible*](https://en.cppreference.com/w/cpp/named_req/Destructible) | ✔️ yes / ✔️ yes | |
| [*Swappable*](https://en.cppreference.com/w/cpp/named_req/Swappable) | ✔️ yes / ✔️ yes | |
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
| [*LegacyIterator*](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no / ❌ no | |
| [*EqualityComparable*](https://en.cppreference.com/w/cpp/named_req/EqualityComparable) | ✔️ yes / ✔️ yes | |

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
| [*LegacyInputIterator*](https://en.cppreference.com/w/cpp/named_req/InputIterator) | ❌ no / ❌ no | |
| [*DefaultConstructible*](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible) | ❌ no / ❌ no | |
| If immutable `reference` same as `value_type&` or `value_type&&`, otherwise same as `const value_type&` or `const value_type&&` | ❌ no / ❌ no | |
| Multipass guarantee | ❌ no / ❌ no | |
| Singular iterators | ❌ no / ❌ no | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `i++` | `It` | Same as `It ip = i; ++i; return ip;` | ❌ no / ❌ no | |
| `*i++` | `reference` | | ❌ no / ❌ no | |

## Collection iterators and standard iterator adapters

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
