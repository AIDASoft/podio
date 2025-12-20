// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 PODIO Contributors
// AUTOMATICALLY GENERATED - Test module for podio.core

module;

// Global module fragment - includes go here
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

// Podio core headers (module-safe - no ROOT dependency)
#include "podio/CollectionBase.h"
#include "podio/ICollectionProvider.h"
#include "podio/SchemaEvolution.h"
#include "podio/CollectionBuffers.h"

export module podio.core;

// Export podio core interfaces
export namespace podio {
  // Core base classes
  using podio::CollectionBase;
  using podio::ICollectionProvider;

  // Schema evolution
  using podio::SchemaEvolution;
  using podio::SchemaVersionT;
  using podio::Backend;

  // Collection buffers
  using podio::CollectionReadBuffers;
  using podio::CollectionWriteBuffers;

  // RelationNames is forward-declared in CollectionBase.h
  // We don't re-export it here to avoid conflicts
}

// Export utility namespace if needed
export namespace podio::detail {
  // Any detail types that need to be exposed
}
