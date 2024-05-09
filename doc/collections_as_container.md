# PODIO Collection as a *Container*

Comparison of the PODIO `Collection`s with a C++ named requirement [*Container*](https://en.cppreference.com/w/cpp/named_req/Container).

The PODIO `Collection`s are move-only classes with emphasis on the distinction between mutable and immutable access to the elements.

### Container Types

| Name | Type | Requirements | Fulfilled by Collection? | Comment |
|------|------|--------------|--------------------------|---------|
| `value_type` | `T` | *[Erasable](https://en.cppreference.com/w/cpp/named_req/Erasable)* | ✔️ yes | Defined as immutable component type |
| `reference` | `T&` |  | ❌ no | Not defined |
| `const_reference` | `const T&` | | ❌ no | Not defined |
| `iterator` | Iterator whose `value_type` is `T` | [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) convertible to `const_iterator` | ❌ no | `iterator::value_type` not defined, not [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) ([see below](#legacyforwarditerator)), not convertible to `const_iterator`|
| `const_iterator` | Constant iterator whose `value_type` is `T` | [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no | `const_iterator::value_type` not defined, not [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) ([see below](#legacyforwarditerator))
| `difference_type`| Signed integer | Must be the same as `std::iterator_traits::difference_type` for `iterator` and `const_iterator` | ❌ no | `std::iterator_traits::difference_type` not defined |
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
| `swap(a,b)` | `void` | Same as `a.swap(b)` | ✔️ yes | `a.swap(b)` not defined |
| `a.size()` | `size_type` | Same as `std::distance(a.begin(), a.end())` | ✔️ yes |
| `a.max_size()` | `size_type` | `b.size()` where b is the largest possible container | ✔️ yes | |
| `a.empty()` | Convertible to `bool` | Same as `a.begin() == a.end()` | ✔️ yes |

## Collection iterators as an *Iterator*

### Iterator summary

| Named requirement | Collection::`iterator`| Collection::`const_iterator`|
|-------------------|-----------------------|-----------------------------|
| [LegacyIterator](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no ([see below](#legacyiterator)) | ❌ no ([see below](#legacyiterator)) |
| [LegacyInputIterator](https://en.cppreference.com/w/cpp/named_req/InputIterator) | ❌ no ([see below](#legacyinputiterator)) | ❌ no ([see below](#legacyinputiterator)) |
| [LegacyForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no ([see below](#legacyforwarditerator)) | ❌ no ([see below](#legacyforwarditerator)) |
| [LegacyOutputIterator](https://en.cppreference.com/w/cpp/named_req/OutputIterator) | ❌ no ([see below](#legacyoutputiterator)) | ❌ no ([see below](#legacyoutputiterator)) |

### LegacyIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*CopyConstructible*](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) | ❌ no / ❌ no | Move constructor and copy constructor not defined |
| [*CopyAssignable*](https://en.cppreference.com/w/cpp/named_req/CopyAssignable) | ❌ no / ❌ no | Move assignment and copy assignment not defined |
| [*Destructible*](https://en.cppreference.com/w/cpp/named_req/Destructible) | ✔️ yes / ✔️ yes | |
| [*Swappable*](https://en.cppreference.com/w/cpp/named_req/Swappable) | ✔️ yes / ✔️ yes | |
| `std::iterator_traits::value_type` (Until C++20 ) | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::difference_type` | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::reference` | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::pointer` | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::iterator_category` | ❌ no / ❌ no | Not defined |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `*r` | Unspecified | Dereferenceable | ✔️ yes / ✔️ yes | |
| `++r` | `It&` | Incrementable | ✔️ yes / ✔️ yes | |

### LegacyInputIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*LegacyIterator*](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no / ❌ no | [See above](#legacyiterator) |
| [*EqualityComparable*](https://en.cppreference.com/w/cpp/named_req/EqualityComparable) | ✔️ yes / ✔️ yes | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `i != j` |  Contextually convertible to `bool` | Same as `!(i==j)` | ✔️ yes / ✔️ yes | |
| `*i` | `reference`, convertible to `value_type` | | ❌ no / ❌ no | `reference` and `value_type` not defined |
| `i->m` | | Same as `(*i).m` | ✔️ yes / ✔️ yes | |
| `++r` | `It&` | | ✔️ yes / ✔️ yes | |
| `(void)r++` | | Same as `(void)++r` | ❌ no / ❌ no | Post-increment not defined |
| `*r++` | Convertible to `value_type` | Same as `value_type x = *r; ++r; return x;` | ❌ no / ❌ no | Post-increment and `value_type` not defined |

### LegacyForwardIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*LegacyInputIterator*](https://en.cppreference.com/w/cpp/named_req/InputIterator) | ❌ no / ❌ no | [See above](#legacyinputiterator)|
| [*DefaultConstructible*](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible) | ❌ no / ❌ no | Value initialization not defined |
| If mutable iterator then `reference` same as `value_type&` or `value_type&&`, otherwise same as `const value_type&` or `const value_type&&` | ❌ no / ❌ no | `reference` and `value_type` not defined |
| [Multipass guarantee](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no / ❌ no | Copy constructor not defined |
| [Singular iterators](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no / ❌ no | Value initialization not defined |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `i++` | `It` | Same as `It ip = i; ++i; return ip;` | ❌ no / ❌ no | Post-increment not defined |
| `*i++` | `reference` | | ❌ no / ❌ no | Post-increment and `reference` not defined|

### LegacyOutputIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*LegacyIterator*](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no / ❌ no | [See above](#legacyiterator) |
| Is pointer type or class type | ✔️ yes / ✔️ yes | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `*r = o` | | | ❌ no / ❌ no | |
| `++r` | `It&` | | ✔️ yes / ✔️ yes  | |
| `r++` | Convertible to `const It&` | Same as `It temp = r; ++r; return temp;` | ❌ no / ❌ no | Post-increment not defined |
| `*r++ = o` | | Same as `*r = o; ++r;`| ✔️ yes / ❌ no | |

## Collection as AllocatorAwareContainer

The C++ standard specifies [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer) for containers that can use other allocators beside the default allocator.

PODIO collections don't provide customization point for allocators and use only the default allocator. Therefore they are not *AllocatorAwareContainers*.

### AllocatorAwareContainer types

| Name |  Requirements | Fulfilled by Collection? | Comment |
|------|--------------|--------------------------|---------|
| `allocator_type`  | `allocator_type::value_type` same as `value_type` | ❌ no | `allocator_type` not defined |

### AllocatorAwareContainer expression and statements

The PODIO Collections currently are not checked against expression and statements requirements for *AllocatorAwareContainer*.

## Collection iterators and standard iterator adaptors

| Adaptor | Compatible with Collection? | Comment |
|---------|-----------------------------|---------|
| `std::reverse_iterator` | ❌ no | `iterator` and `const_iterator` not *LegacyBidirectionalIterator* or `std::bidirectional_iterator` |
| `std::back_insert_iterator` | ❌ no | |
| `std::front_insert_iterator` | ❌ no | `push_front` not defined |
| `std::insert_iterator` | ❌ no | `insert` not defined |
| `std::const_iterator` | ❌ no | `iterator` and `const_iterator` not *LegacyInputIterator* or `std::input_iterator` |
| `std::move_iterator` | ❌ no | Move from collection conflicts collection elements ownership semantic |
| `std::counted_iterator` | ✔️ yes | |


## Collection and standard algorithms

### Non-modifying sequence operations

| Algorithm | Compatible with Collection? | Comment |
| ----------|-----------------------------|---------|
| `std::all_of`            | | |
| `std::any_of`            | | |
| `std::none_of`           | | |
| `std::for_each`          | | |
| `std::for_each_n`        | | |
| `std::count`             | | |
| `std::count_if`          | | |
| `std::mismatch`          | | |
| `std::find`              | | |
| `std::find_if`           | | |
| `std::find_if_not`       | | |
| `std::find_end`          | | |
| `std::find_first_of`     | | |
| `std::adjacent_find`     | | |
| `std::search`            | | |
| `std::search_n`          | | |

### Modifying sequence operations

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::copy`                 |                             |         |
| `std::copy_if`              |                             |         |
| `std::copy_n`               |                             |         |
| `std::copy_backward`        |                             |         |
| `std::move`                 |                             |         |
| `std::move_backward`        |                             |         |
| `std::fill`                 |                             |         |
| `std::fill_n`               |                             |         |
| `std::transform`            |                             |         |
| `std::generate`             |                             |         |
| `std::generate_n`           |                             |         |
| `std::remove`               |                             |         |
| `std::remove_if`            |                             |         |
| `std::remove_copy`          |                             |         |
| `std::remove_copy_if`       |                             |         |
| `std::replace`              |                             |         |
| `std::replace_if`           |                             |         |
| `std::replace_copy`         |                             |         |
| `std::replace_copy_if`      |                             |         |
| `std::swap`                 |                             |         |
| `std::swap_ranges`          |                             |         |
| `std::iter_swap`            |                             |         |
| `std::reverse`              |                             |         |
| `std::reverse_copy`         |                             |         |
| `std::rotate`               |                             |         |
| `std::rotate_copy`          |                             |         |
| `std::shift_left`           |                             |         |
| `std::shift_right`          |                             |         |
| `std::random_shuffle`       |                             |         |
| `std::shuffle`              |                             |         |
| `std::sample`               |                             |         |
| `std::unique`               |                             |         |
| `std::unique_copy`          |                             |         |

### Partitioning Operations

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::is_partitioned`       |                             |         |
| `std::partition`            |                             |         |
| `std::partition_copy`       |                             |         |
| `std::stable_partition`     |                             |         |
| `std::partition_point`      |                             |         |

### Sorting Operations

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::is_sorted`            |                             |         |
| `std::is_sorted_until`      |                             |         |
| `std::sort`                 |                             |         |
| `std::partial_sort`         |                             |         |
| `std::partial_sort_copy`    |                             |         |
| `std::stable_sort`          |                             |         |
| `std::nth_element`          |                             |         |

### Binary Search Operations (on Sorted Ranges)

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::lower_bound`          |                             |         |
| `std::upper_bound`          |                             |         |
| `std::binary_search`        |                             |         |
| `std::equal_range`          |                             |         |

### Other Operations on Sorted Ranges

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::merge`                 |                             |         |
| `std::inplace_merge`         |                             |         |

### Set Operations (on Sorted Ranges)

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::includes`              |                             |         |
| `std::set_difference`        |                             |         |
| `std::set_intersection`      |                             |         |
| `std::set_symmetric_difference` |                          |         |
| `std::set_union`             |                             |         |

### Heap Operations

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::is_heap`               |                             |         |
| `std::is_heap_until`         |                             |         |
| `std::make_heap`             |                             |         |
| `std::push_heap`             |                             |         |
| `std::pop_heap`              |                             |         |
| `std::sort_heap`             |                             |         |

### Minimum/Maximum Operations

| Algorithm            | Compatible with Collection? | Comment |
|----------------------|-----------------------------|---------|
| `std::max`                   |                             |         |
| `std::max_element`           |                             |         |
| `std::min`                   |                             |         |
| `std::min_element`           |                             |         |
| `std::minmax`                |                             |         |
| `std::minmax_element`        |                             |         |
| `std::clamp`                 |                             |         |


### Comparison operations

| Algorithm | Compatible with Collection? | Comment |
| ----------|-----------------------------|---------|
| `std::equal`                             | | |
| `std::lexicographical_compare`           | | |
| `std::lexicographical_compare_three_way` | | |

### Permutation operations

| Algorithm | Compatible with Collection? | Comment |
| ----------|-----------------------------|---------|
| `std::is_permutation`    | | |
| `std::next_permutation ` | | |
| `std::prev_permutation`  | | |

## Standard ranges
