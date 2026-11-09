#include "MurmurHash3.h"

#include "catch2/catch_test_macros.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

TEST_CASE("MurmurHash3 accepts unaligned input", "[basics]") {
  const auto checkAlignment = [](auto hash, auto expected) {
    alignas(uint64_t) std::array<uint8_t, 64> input{};
    alignas(uint64_t) std::array<uint8_t, 72> shifted{};
    for (size_t i = 0; i < input.size(); ++i) {
      input[i] = static_cast<uint8_t>(37 * i + 11);
    }

    for (const auto seed : {0u, 0x9747b28cu}) {
      for (int length = 0; length <= static_cast<int>(input.size()); ++length) {
        hash(input.data(), length, seed, expected.data());
        for (size_t offset = 0; offset < alignof(uint64_t); ++offset) {
          CAPTURE(seed, length, offset);
          std::memcpy(shifted.data() + offset, input.data(), length);
          decltype(expected) actual{};
          hash(shifted.data() + offset, length, seed, actual.data());
          CHECK(actual == expected);
        }
      }
    }
  };

  SECTION("x86 32-bit") {
    checkAlignment(MurmurHash3_x86_32, std::array<uint32_t, 1>{});
  }
  SECTION("x86 128-bit") {
    checkAlignment(MurmurHash3_x86_128, std::array<uint32_t, 4>{});
  }
  SECTION("x64 128-bit") {
    checkAlignment(MurmurHash3_x64_128, std::array<uint64_t, 2>{});
  }
}
