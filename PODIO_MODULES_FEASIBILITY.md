# Podio C++20 Modules Feasibility Analysis

**Date**: 2025-12-19  
**Project**: podio (Event Data Model I/O library)

## Executive Summary

**Podio has excellent potential for C++20 modules**, but requires careful separation of:
1. **Core datamodel code** (module-safe) - Objects, Collections, pure interfaces
2. **ROOT I/O backend** (NOT module-safe) - ROOTReader, ROOTWriter, Frame with ROOT

The code generation templates already produce clean, separable code. This is a **MUCH better candidate** for modularization than downstream projects (EICrecon) because podio controls the generated code.

## Current Architecture

### Generated Code Structure (per datatype)

For each datatype (e.g., `ExampleHit`), podio generates:
```
ExampleHit.h              # Immutable object interface
MutableExampleHit.h       # Mutable object interface
ExampleHitObj.h           # Internal object implementation (POD)
ExampleHitData.h          # POD data structure
ExampleHitCollection.h    # Collection container
ExampleHitCollectionData.h # Collection data buffer
```

### Core Dependencies

**Clean (no ROOT)**:
- `podio/CollectionBase.h` - Abstract collection interface
- `podio/ICollectionProvider.h` - Collection provider interface  
- `podio/SchemaEvolution.h` - Schema evolution system (ROOT-agnostic)
- `podio/utilities/MaybeSharedPtr.h` - Utility templates
- Standard library only

**ROOT-dependent**:
- `podio/ROOTReader.h` / `podio/ROOTWriter.h`
- `podio/RNTupleReader.h` / `podio/RNTupleWriter.h`
- `podio/ROOTFrameData.h`
- `podio/utilities/RootHelpers.h`
- `podio/Frame.h` (contains ROOT I/O methods)

## Proposed Module Structure

### Option 1: Two-Module Approach (RECOMMENDED)

```
┌─────────────────────────────────────────────────┐
│ Module: edm4hep.datamodel                       │
│ ✅ MODULE-SAFE                                  │
│                                                 │
│ Exports:                                        │
│  - All generated datatypes (ExampleHit, etc.)   │
│  - All collections (ExampleHitCollection, etc.) │
│  - CollectionBase, ICollectionProvider          │
│  - SchemaEvolution                              │
│                                                 │
│ Does NOT include:                               │
│  - ROOT I/O (ROOTReader/Writer)                 │
│  - Frame (has ROOT methods)                     │
│  - SIO I/O                                      │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│ Traditional Headers: edm4hep.io                 │
│ ❌ NOT MODULE-SAFE (uses ROOT)                  │
│                                                 │
│ Includes:                                       │
│  #include <edm4hep/ROOTReader.h>                │
│  #include <edm4hep/ROOTWriter.h>                │
│  #include <edm4hep/Frame.h>                     │
│  #include <ROOT/...>                            │
└─────────────────────────────────────────────────┘
```

**Usage**:
```cpp
// In algorithm implementation
import edm4hep.datamodel;  // ✅ Module - objects & collections

// In I/O code (when needed)
#include <podio/ROOTReader.h>  // ❌ Traditional header - ROOT I/O
```

### Option 2: Three-Module Approach (FINE-GRAINED)

```
Module: podio.core
  ├─ CollectionBase
  ├─ ICollectionProvider
  ├─ SchemaEvolution
  └─ Utilities (MaybeSharedPtr, etc.)

Module: edm4hep.datamodel
  ├─ All generated objects
  ├─ All generated collections
  └─ Uses: podio.core

Traditional Headers: edm4hep.io
  ├─ ROOTReader/Writer
  ├─ RNTupleReader/Writer
  ├─ Frame
  └─ Uses: ROOT headers
```

## Implementation Strategy

### Phase 1: Template Modifications

**Add module interface file generation**:

New template: `datamodel_module.ixx.jinja2`
```cpp
// Generated module for {{ package_name }}
module;

// Global module fragment - standard library only
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

// Podio core (module-safe parts)
#include "podio/CollectionBase.h"
#include "podio/ICollectionProvider.h"
#include "podio/SchemaEvolution.h"

// Include all generated headers
{% for datatype in datatypes %}
#include "{{ package_name }}/{{ datatype }}.h"
#include "{{ package_name }}/Mutable{{ datatype }}.h"
#include "{{ package_name }}/{{ datatype }}Collection.h"
{% endfor %}

export module {{ package_name }}.datamodel;

// Export all datatypes
export namespace {{ package_name }} {
  {% for datatype in datatypes %}
  using {{ package_name }}::{{ datatype }};
  using {{ package_name }}::Mutable{{ datatype }};
  using {{ package_name }}::{{ datatype }}Collection;
  {% endfor %}
}

// Export podio interfaces
export namespace podio {
  using podio::CollectionBase;
  using podio::ICollectionProvider;
  using podio::SchemaEvolution;
}
```

### Phase 2: Separate Frame Implementation

**Problem**: `Frame.h` includes both:
- Pure collection management (module-safe)
- ROOT I/O methods (NOT module-safe)

**Solution**: Split into:
```cpp
// podio/Frame.h - Pure interface (module-safe)
class Frame {
  // Collection access methods (no ROOT)
  template<typename T>
  const T& get(const std::string& name) const;
  
  void put(CollectionBase* coll, const std::string& name);
};

// podio/FrameROOTIO.h - ROOT I/O extension (NOT module-safe)
#include "podio/Frame.h"
#include <ROOT/...>

class FrameROOTIO : public Frame {
  // ROOT-specific I/O methods
  void writeToROOT(TFile* file);
  static Frame readFromROOT(TFile* file);
};
```

### Phase 3: CMake Integration

Add option:
```cmake
option(PODIO_ENABLE_MODULES "Generate C++20 module interfaces" OFF)

if(PODIO_ENABLE_MODULES)
  if(CMAKE_VERSION VERSION_LESS 3.29)
    message(WARNING "C++20 modules require CMake 3.29+")
    set(PODIO_ENABLE_MODULES OFF)
  elseif(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    message(WARNING "C++20 modules not supported with Unix Makefiles")
    set(PODIO_ENABLE_MODULES OFF)
  endif()
endif()

if(PODIO_ENABLE_MODULES)
  # Generate .ixx module interface files
  # Add to datamodel library as FILE_SET CXX_MODULES
endif()
```

## Benefits

### For Podio Users (like EICrecon)

**Before (current)**:
```cpp
#include <edm4hep/MCParticleCollection.h>  // Pulls in ROOT transitively
#include <edm4hep/CalorimeterHitCollection.h>
#include <edm4hep/TrackCollection.h>
// Every TU recompiles all these headers + ROOT
```

**After (with modules)**:
```cpp
import edm4hep.datamodel;  // Pre-compiled module - instant
// Collections, objects available immediately
// NO ROOT dependency for pure datamodel code
```

### Compilation Speed

**Conservative estimate**:
- EDM headers are included in 100+ translation units in typical HEP projects
- Each TU currently recompiles ~50-100 EDM headers + podio + partial ROOT
- Module compilation: Pay once, reuse everywhere

**Expected improvement**:
- **30-50% reduction** in compile time for clean builds
- **50-70% reduction** for incremental builds (when EDM interface changes)
- Much better than EICrecon's 5-15% because EDM types are EVERYWHERE

### Memory Usage

- Compiler memory usage reduced (modules parsed once, not per-TU)
- Build parallelism improved (no template instantiation redundancy)

## Technical Challenges

### Challenge 1: ROOT Dependency Separation

**Issue**: Some users need ROOT I/O, others just need datatypes

**Solution**: 
- Module exports datatypes only
- ROOT I/O remains traditional headers
- Users choose what they need

```cpp
// Algorithm code (no ROOT)
import edm4hep.datamodel;
auto hits = event.get<edm4hep::CalorimeterHitCollection>("hits");

// I/O code (needs ROOT)
#include <podio/ROOTReader.h>
import edm4hep.datamodel;
auto reader = podio::ROOTReader();
```

### Challenge 2: Backward Compatibility

**Issue**: Existing code uses `#include`

**Solution**: Keep both!
```cpp
// Still works
#include <edm4hep/MCParticleCollection.h>

// New way
import edm4hep.datamodel;
```

Generate both traditional headers AND module interface.

### Challenge 3: Template Instantiation

**Issue**: Collections are templates, need careful export

**Solution**: Already handled! Generated collections use explicit types:
```cpp
// Not a template
class MCParticleCollection : public CollectionBase {
  // Concrete implementation for MCParticle
};
```

### Challenge 4: Cross-Datamodel Dependencies

**Issue**: edm4eic depends on edm4hep

**Solution**: Module imports:
```cpp
export module edm4eic.datamodel;

import edm4hep.datamodel;  // Import dependency

export namespace edm4eic {
  // Can use edm4hep types
}
```

## Implementation Roadmap

### Sprint 1: Core Infrastructure (1-2 weeks)
1. Add `datamodel_module.ixx.jinja2` template
2. Implement CMake option `PODIO_ENABLE_MODULES`
3. Test with simple datamodel (no ROOT)
4. Verify module generation works

### Sprint 2: Podio Core Separation (2-3 weeks)
1. Audit podio headers for ROOT dependencies
2. Split `Frame.h` into Frame + FrameROOTIO
3. Create `podio.core` module for base interfaces
4. Ensure SchemaEvolution is module-safe

### Sprint 3: ROOT I/O Boundary (1-2 weeks)
1. Document which headers are module-safe vs not
2. Create migration guide for users
3. Add CMake checks for generator/compiler support
4. Test with edm4hep/edm4eic

### Sprint 4: Testing & Documentation (2 weeks)
1. Build tests with modules enabled
2. Benchmark compilation speed improvements
3. Update documentation
4. Create example projects using modules

**Total effort**: 6-9 weeks for complete implementation

## Compatibility Requirements

### Build System
- **CMake 3.29+** (for C++20 module support)
- **Ninja generator** (Unix Makefiles don't support modules)
- **GCC 14+ or Clang 18+** (mature module implementations)

### Downstream Projects
- Can mix modules and traditional headers
- Gradual migration possible
- No breaking changes if optional

## Recommendation

**STRONGLY RECOMMENDED** to implement C++20 modules in podio because:

### ✅ Pros
1. **Podio controls the generated code** - can make it module-safe
2. **High impact** - EDM headers used everywhere, 30-50% speedup realistic
3. **Clean separation possible** - datamodel vs I/O is natural boundary
4. **No breaking changes** - can offer both traditional and module builds
5. **Future-proof** - positions podio as modern, efficient framework

### ⚠️ Cons  
1. **Requires CMake 3.29+** - May limit some users
2. **Ninja generator required** - Small workflow change
3. **Testing complexity** - Need to test both modes
4. **ROOT still not modularized** - I/O remains traditional headers

### 🎯 Priority
**HIGH** - This is the RIGHT place to add modules in the HEP ecosystem:
- Podio is upstream of most analysis code
- Generated code is clean and controllable
- Benefits cascade to all downstream projects
- Much higher ROI than project-specific attempts

## Next Steps

1. **Prototype**: Create `datamodel_module.ixx.jinja2` template
2. **Test**: Generate module for simple test datamodel
3. **Benchmark**: Measure compilation speed with/without modules
4. **Propose**: Submit RFC to podio project with benchmark results
5. **Implement**: If accepted, follow roadmap above

## Conclusion

**Podio is the IDEAL candidate for C++20 modules in HEP software**. Unlike downstream projects blocked by ROOT, podio can generate module-safe datamodel code while keeping ROOT I/O separate. This would provide 30-50% compilation speedup to the entire HEP ecosystem.

The key insight: **Don't fight ROOT's module incompatibility, work around it** by separating pure datamodel (module) from I/O backends (traditional headers).
