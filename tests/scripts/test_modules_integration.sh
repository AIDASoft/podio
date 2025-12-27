#!/bin/bash
# Integration test script for C++ modules functionality
# This script tests that a downstream project can successfully use podio libraries
# that were built with C++ modules enabled

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${SCRIPT_DIR}/modules_integration_test"

# Check if modules are enabled
if [ "${PODIO_ENABLE_CXX_MODULES}" != "ON" ]; then
    echo "C++ modules are not enabled. Skipping integration test."
    exit 0
fi

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Cannot run integration test."
    exit 1
fi

if ! command -v ninja &> /dev/null; then
    echo "Ninja not found. Cannot run integration test."
    exit 1
fi

echo "Running C++ modules integration test..."
echo "Test directory: ${TEST_DIR}"

# Create a temporary test directory
rm -rf "${TEST_DIR}"
mkdir -p "${TEST_DIR}"
cd "${TEST_DIR}"

# Create a minimal CMakeLists.txt for a downstream project
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.29)
project(PodioModulesIntegrationTest CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(podio REQUIRED)

# Test: Use podio and datamodel via traditional includes
# (Module imports across build boundaries not yet working reliably with GCC)
add_executable(test_traditional_includes test_traditional.cpp)
target_link_libraries(test_traditional_includes PRIVATE podio::podio)
EOF

# Create test source that uses traditional includes
cat > test_traditional.cpp << 'EOF'
#include "podio/CollectionIDTable.h"
#include "podio/ObjectID.h"
#include <iostream>

int main() {
  podio::ObjectID id{42, 1};

  if (id.index != 42 || id.collectionID != 1) {
    std::cerr << "ObjectID test failed!" << std::endl;
    return 1;
  }

  podio::CollectionIDTable table;
  auto collID = table.add("test_collection");

  if (!table.collectionID("test_collection").has_value()) {
    std::cerr << "CollectionIDTable test failed!" << std::endl;
    return 1;
  }

  std::cout << "Traditional includes test passed!" << std::endl;
  std::cout << "This confirms that podio libraries built with modules enabled" << std::endl;
  std::cout << "can still be consumed via traditional header includes." << std::endl;
  return 0;
}
EOF

# Try to build the downstream project
echo "Configuring downstream project..."
cmake -G Ninja \
      -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER:-g++}" \
      -DCMAKE_PREFIX_PATH="${PODIO_INSTALL_DIR:-${PODIO_BUILD_BASE}}" \
      -B build \
      .

echo "Building downstream project..."
cmake --build build

# Run the test
echo "Running test_traditional_includes..."
./build/test_traditional_includes

echo ""
echo "Integration test passed!"
echo "Note: Direct module imports are not yet tested due to limitations in"
echo "GCC's module system when crossing build boundaries. Consumers should"
echo "continue to use traditional header includes until module support matures."

# Cleanup
cd "${SCRIPT_DIR}"
rm -rf "${TEST_DIR}"

exit 0
