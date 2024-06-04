# PODIO Collection as a *Container*

Comparison of the PODIO `Collection`s with a C++ named requirement [*Container*](https://en.cppreference.com/w/cpp/named_req/Container).

The PODIO `Collection`s interface was designed to mimic the standard *Container* interface, in particular `std::vector`. Perfect compliance with the *Container* is not achieved as the `Collection`s are concerned with additional semantics such as mutable/immutable element access, associations and relations, and IO which that are not part of *Container*.

On the implementation level most of the differences with respect to the *Container* comes from the fact that in order to satisfy the additional semantics a `Collection` doesn't directly store [user layer objects](design.md#the-user-layer). Instead, [data layer objects](design.md#the-internal-data-layer) are stored and user layer objects are constructed and returned when needed. Similarly, the `Collection` iterators operate on the user layer objects but don't expose `Collection`'s storage directly to the users. Instead, they construct and return user layer objects when needed.
In other words, a `Collection` utilizes the user layer type as a reference type instead of using plain references (`&` or `&&`) to stored data layer types.

As a consequence some of the **standard algorithms may not work** with PODIO `Collection` iterators.  See [standard algorithm documentation](#collection-and-standard-algorithms) below.

The following tables list the compliance of a PODIO generated collection with the *Container* named requirement, stating which member types, interfaces, or concepts are fulfilled and which are not. Additionally, there are some comments explaining missing parts or pointing out differences in behaviour.

### Container Types

| Name | Type | Requirements | Fulfilled by Collection? | Comment |
|------|------|--------------|--------------------------|---------|
| `value_type` | `T` | *[Erasable](https://en.cppreference.com/w/cpp/named_req/Erasable)* | ✔️ yes | Defined as immutable component type |
| `reference` | `T&` |  | ❌ no | Not defined |
| `const_reference` | `const T&` | | ❌ no | Not defined |
| `iterator` | Iterator whose `value_type` is `T` | [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) convertible to `const_iterator` | ❌ no | Defined as podio `MutableCollectionIterator`. `iterator::value_type` not defined, not [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) ([see below](#legacyforwarditerator)), not convertible to `const_iterator`|
| `const_iterator` | Constant iterator whose `value_type` is `T` | [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no | Defined as podio `CollectionIterator`. `const_iterator::value_type` not defined, not [*LegacyForwardIterator*](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) ([see below](#legacyforwarditerator))
| `difference_type`| Signed integer | Must be the same as `std::iterator_traits::difference_type` for `iterator` and `const_iterator` | ❌ no | `std::iterator_traits::difference_type` not defined |
| `size_type` | Unsigned integer | Large enough to represent all positive values of `difference_type` | ✔️ yes |  |

### Container member functions and operators

| Expression | Return type | Semantics | Fulfilled by Collection? | Comment |
|------------|-------------|-----------|--------------------------|---------|
| `C()` | `C` | Creates an empty container | ✔️ yes | |
| `C(a)` | `C` | Creates a copy of `a` | ❌ no | Not defined, non-copyable by design |
| `C(rv)` | `C` | Moves `rv` | ✔️ yes | |
| `a = b` | `C&` | Destroys or copy-assigns all elements of `a` from elements of `b` | ❌ no | Not defined, non-copyable by design |
| `a = rv` | `C&` | Destroys or move-assigns all elements of `a` from elements of `rv` |  ✔️ yes | |
| `a.~C()` | `void` | Destroys all elements of `a` and frees all memory|  ✔️ yes | Invalidates all handles retrieved from this collection |
| `a.begin()` | `(const_)iterator` | Iterator to the first element of `a` |  ✔️ yes | |
| `a.end()` | `(const_)iterator` | Iterator to one past the last element of `a` | ✔️ yes | |
| `a.cbegin()` | `const_iterator` | Same as `const_cast<const C&>(a).begin()` | ✔️ yes | |
| `a.cend()` | `const_iterator` | Same as `const_cast<const C&>(a).end()`|  ✔️ yes | |
| `a == b` | Convertible to `bool` | Same as `std::equal(a.begin(), a.end(), b.begin(), b.end())`| ❌ no | Not defined |
| `a != b` | Convertible to `bool` | Same as `!(a == b)` | ❌ no | Not defined |
| `a.swap(b)` | `void` | Exchanges the values of `a` and `b` | ❌ no | Not defined |
| `swap(a,b)` | `void` | Same as `a.swap(b)` | ❌ no | `a.swap(b)` not defined |
| `a.size()` | `size_type` | Same as `std::distance(a.begin(), a.end())` | ✔️ yes | |
| `a.max_size()` | `size_type` | `b.size()` where b is the largest possible container | ✔️ yes | |
| `a.empty()` | Convertible to `bool` | Same as `a.begin() == a.end()` | ✔️ yes | |

## Collection as an *AllocatorAwareContainer*

The C++ standard specifies [AllocatorAwareContainer](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer) for containers that can use other allocators beside the default allocator.

PODIO collections don't provide a customization point for allocators and use only the default allocator. Therefore they are not *AllocatorAwareContainers*.

### AllocatorAwareContainer types

| Name |  Requirements | Fulfilled by Collection? | Comment |
|------|--------------|--------------------------|---------|
| `allocator_type`  | `allocator_type::value_type` same as `value_type` | ❌ no | `allocator_type` not defined |

### *AllocatorAwareContainer* expression and statements

The PODIO Collections currently are not checked against expression and statements requirements for *AllocatorAwareContainer*.

## Collection iterators as an *Iterator*

The C++ specifies a set of named requirements for iterators. Starting with C++20 the standard specifies also iterator concepts. The requirements imposed by the concepts and named requirements are similar but not identical.

In the following tables a convention from `Collection` is used: `iterator` stands for PODIO `MutableCollectionIterator` and `const_iterator` stands for PODIO `CollectionIterator`.
### Iterator summary

| Named requirement | `iterator` | `const_iterator` |
|-------------------|-----------------------|-----------------------------|
| [LegacyIterator](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no ([see below](#legacyiterator)) | ❌ no ([see below](#legacyiterator)) |
| [LegacyInputIterator](https://en.cppreference.com/w/cpp/named_req/InputIterator) | ❌ no ([see below](#legacyinputiterator)) | ❌ no ([see below](#legacyinputiterator)) |
| [LegacyForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no ([see below](#legacyforwarditerator)) | ❌ no ([see below](#legacyforwarditerator)) |
| [LegacyOutputIterator](https://en.cppreference.com/w/cpp/named_req/OutputIterator) | ❌ no ([see below](#legacyoutputiterator)) | ❌ no ([see below](#legacyoutputiterator)) |
| [LegacyBidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator) | ❌ no | ❌ no |
| [LegacyRandomAccessIterator](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator) | ❌ no | ❌ no |
| [LegacyContiguousIterator](https://en.cppreference.com/w/cpp/named_req/ContiguousIterator) | ❌ no | ❌ no |

| Concept | `iterator` | `const_iterator` |
|---------|------------------------|------------------------------|
| `std::indirectly_readable` | ❌ no | ❌ no |
| `std::indirectly_writable` | ❌ no | ❌ no |
| `std::weakly_incrementable` | ❌ no | ❌ no |
| `std::incrementable` | ❌ no | ❌ no |
| `std::input_or_output_iterator` | ❌ no | ❌ no |
| `std::input_iterator` | ❌ no | ❌ no |
| `std::output_iterator` | ❌ no | ❌ no |
| `std::forward_iterator` | ❌ no | ❌ no |
| `std::bidirectional_iterator` | ❌ no | ❌ no |
| `std::random_access_iterator` | ❌ no | ❌ no |
| `std::contiguous_iterator` | ❌ no | ❌ no |

### LegacyIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*CopyConstructible*](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) | ❌ no / ❌ no | Move constructor and copy constructor not defined |
| [*CopyAssignable*](https://en.cppreference.com/w/cpp/named_req/CopyAssignable) | ❌ no / ❌ no | Move assignment and copy assignment not defined |
| [*Destructible*](https://en.cppreference.com/w/cpp/named_req/Destructible) | ✔️ yes / ✔️ yes | |
| [*Swappable*](https://en.cppreference.com/w/cpp/named_req/Swappable) | ❌ no / ❌ no | |
| `std::iterator_traits::value_type` (Until C++20 ) | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::difference_type` | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::reference` | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::pointer` | ❌ no / ❌ no | Not defined |
| `std::iterator_traits::iterator_category` | ❌ no / ❌ no | Not defined |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `*r` | Unspecified | | ✔️ yes / ✔️ yes | |
| `++r` | `It&` | | ✔️ yes / ✔️ yes | |

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

In addition to the *LegacyForwardIterator* the C++ standard specifies also the *mutable LegacyForwardIterator*, which is both *LegacyForwardIterator* and *LegacyOutputIterator*. The term **mutable** used in this context doesn't imply mutability in the sense used in the PODIO.


| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*LegacyInputIterator*](https://en.cppreference.com/w/cpp/named_req/InputIterator) | ❌ no / ❌ no | [See above](#legacyinputiterator)|
| [*DefaultConstructible*](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible) | ❌ no / ❌ no | Value initialization not defined |
| If *mutable* iterator then `reference` same as `value_type&` or `value_type&&`, otherwise same as `const value_type&` or `const value_type&&` | ❌ no / ❌ no | `reference` and `value_type` not defined |
| [Multipass guarantee](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no / ❌ no | Copy constructor not defined |
| [Singular iterators](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) | ❌ no / ❌ no | Value initialization not defined |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `i++` | `It` | Same as `It ip = i; ++i; return ip;` | ❌ no / ❌ no | Post-increment not defined |
| `*i++` | `reference` | | ❌ no / ❌ no | Post-increment and `reference` not defined |

### LegacyOutputIterator

| Requirement | Fulfilled by `iterator`/`const_iterator`? | Comment |
|-------------|-------------------------------------------|---------|
| [*LegacyIterator*](https://en.cppreference.com/w/cpp/named_req/Iterator) | ❌ no / ❌ no | [See above](#legacyiterator) |
| Is pointer type or class type | ✔️ yes / ✔️ yes | |

| Expression | Return type | Semantics | Fulfilled by `iterator`/`const_iterator`? | Comment |
|------------|-------------|-----------|-------------------------------------------|---------|
| `*r = o` | | | ❗ attention / ❗ attention | Defined but an assignment doesn't modify objects inside collection |
| `++r` | `It&` | | ✔️ yes / ✔️ yes  | |
| `r++` | Convertible to `const It&` | Same as `It temp = r; ++r; return temp;` | ❌ no / ❌ no | Post-increment not defined |
| `*r++ = o` | | Same as `*r = o; ++r;`| ❌ no / ❌ no | Post-increment not defined |

## Collection iterators and standard iterator adaptors

| Adaptor | Compatible with Collection? | Comment |
|---------|-----------------------------|---------|
| `std::reverse_iterator` | ❌ no | `iterator` and `const_iterator` not *LegacyBidirectionalIterator* or `std::bidirectional_iterator` |
| `std::back_insert_iterator` | ❗ attention | Compatible only with SubsetCollections, otherwise throws `std::invalid_argument` |
| `std::front_insert_iterator` | ❌ no | `push_front` not defined |
| `std::insert_iterator` | ❌ no | `insert` not defined |
| `std::const_iterator` | ❌ no | `iterator` and `const_iterator` not *LegacyInputIterator* or `std::input_iterator` |
| `std::move_iterator` | ❌ no | `iterator` and `const_iterator` not *LegacyInputIterator* or `std::input_iterator` |
| `std::counted_iterator` |  ❌ no | `iterator` and `const_iterator` not `std::input_or_output_iterator` |


## Collection and standard algorithms

Most of the standard algorithms require the iterators to be at least *InputIterator*. The iterators of PODIO collection don't fulfil this requirement, therefore they are not compatible with standard algorithms according to the specification.

In practice, some algorithms may still compile with the collections depending on the implementation of a given algorithm. In general, the standard **algorithms mutating a collection will give wrong results**, while the standard algorithms not mutating a collection in principle should give correct results if they compile.

## Standard range algorithms

The standard range algorithm use constrains to operate at least on `std::input_iterator`s and `std::ranges::input_range`s. The iterators of PODIO collection don't model these concepts, therefore can't be used with standard range algorithms. The range algorithms won't compile with PODIO `Collection` iterators.
