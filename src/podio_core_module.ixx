// SPDX-License-Identifier: Apache-2.0
// Module interface for podio.core - ROOT-independent podio functionality
//
// This module exports the core podio types and interfaces that don't depend on ROOT.
// It can be used by datamodel modules and user code that doesn't need ROOT I/O.

module;

// Global module fragment - includes go here
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Podio core headers (module-safe - no ROOT dependency)
#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/DatamodelRegistry.h"
#include "podio/GenericParameters.h"
#include "podio/ICollectionProvider.h"
#include "podio/LinkCollection.h"
#include "podio/LinkNavigator.h"
#include "podio/ObjectID.h"
#include "podio/RelationRange.h"
#include "podio/SchemaEvolution.h"
#include "podio/UserDataCollection.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/utilities/Glob.h"

export module podio.core;

// Export podio core interfaces and types
export namespace podio {
// Core collection interfaces
using podio::CollectionBase;
using podio::CollectionIDTable;
using podio::CollectionReadBuffers;
using podio::CollectionWriteBuffers;
using podio::ICollectionProvider;
using podio::UserDataCollection;

// Object identification
using podio::ObjectID;

// Data model registry
using podio::DatamodelRegistry;
using podio::RelationNameMapping;
using podio::RelationNames;

// Generic parameters for metadata
using podio::GenericParameters;

// Relations and navigation
using podio::Link;
using podio::LinkCollectionIterator;
using podio::LinkNavigator;
using podio::RelationRange;

// Schema evolution
using podio::Backend;
using podio::SchemaEvolution;
using podio::SchemaVersionT;

// Utility functions
namespace utils {
  using podio::utils::expand_glob;
  using podio::utils::is_glob_pattern;
} // namespace utils
} // namespace podio
