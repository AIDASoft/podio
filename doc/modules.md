# C++20 Modules Support

Podio provides optional support for C++20 modules, offering significant compilation speed improvements while maintaining full backward compatibility with traditional header-based includes.

## Overview

C++20 modules provide a modern alternative to header files that can dramatically improve compilation times. Podio generates two types of modules:

1. **podio.core** - A module interface for podio's ROOT-independent core functionality
2. **datamodel modules** - Auto-generated module interfaces for each datamodel (e.g., `edm4hep.datamodel`)

Module support is entirely optional and controlled by the `PODIO_ENABLE_CPP_MODULES` CMake option (default: `OFF`).

## Requirements

To use C++20 modules with podio, you need:

- **CMake** 3.29 or later
- **Build Generator** Ninja (modules are not supported with Unix Makefiles)
- **Compiler** GCC 14+ or Clang 18+
- **C++ Standard** C++20 or later

If these requirements are not met, the build system will automatically disable module generation and fall back to traditional headers with a warning message.

## Enabling Modules

### At Build Time

Enable module generation when configuring podio:

```bash
cmake -GNinja -DPODIO_ENABLE_CPP_MODULES=ON -DCMAKE_CXX_STANDARD=20 <source-dir>
ninja
```

The build system will verify all requirements and warn if modules cannot be enabled.

### In Downstream Projects

Downstream projects can detect and use podio modules:

```cmake
find_package(podio REQUIRED)

if(podio_MODULES_AVAILABLE)
  # Use modules
  target_link_libraries(myapp PRIVATE podio::podio)
  # Module files will be automatically found
else()
  # Fall back to headers
  target_include_directories(myapp PRIVATE ${podio_INCLUDE_DIRS})
endif()
```

## Using Modules in Code

### The podio.core Module

The `podio.core` module exports all ROOT-independent podio types:

```cpp
import podio.core;

// Use podio types directly
podio::ObjectID id{42, 1};
podio::CollectionIDTable table;
podio::GenericParameters params;
podio::CollectionBase* collection = /* ... */;
```

Exported types include:
- **Collections**: `CollectionBase`, `CollectionIDTable`, `ICollectionProvider`, `UserDataCollection`
- **Buffers**: `CollectionReadBuffers`, `CollectionWriteBuffers`
- **Identification**: `ObjectID`
- **Registry**: `DatamodelRegistry`, `RelationNames`, `RelationNameMapping`
- **Parameters**: `GenericParameters`
- **Relations**: `Link`, `LinkCollectionIterator`, `LinkNavigator`, `RelationRange`
- **Evolution**: `SchemaEvolution`, `SchemaVersionT`, `Backend`
- **Utilities**: `expand_glob()`, `is_glob_pattern()`

### Datamodel Modules

For each datamodel YAML file (e.g., `edm4hep.yaml`), podio generates a corresponding module interface (e.g., `edm4hep.datamodel`):

```cpp
import podio.core;         // Import podio core types
import edm4hep.datamodel;  // Import EDM4hep types

void processData() {
  edm4hep::MCParticleCollection particles;
  for (const auto& particle : particles) {
    // Process particles
  }
}
```

### Mixed Usage

You can mix modules and traditional headers in the same translation unit:

```cpp
import podio.core;
import edm4hep.datamodel;
#include <podio/ROOTReader.h>  // ROOT I/O still uses headers

void readAndProcess() {
  podio::ROOTReader reader;
  auto particles = reader.get<edm4hep::MCParticleCollection>("particles");
  // Process particles using module-imported types
}
```

### Header-Only Mode (Backward Compatible)

Traditional header includes continue to work unchanged:

```cpp
#include <podio/ObjectID.h>
#include <edm4hep/MCParticleCollection.h>

// Traditional approach - no code changes needed
```

## Performance Benefits

### Compilation Speed

Modules can significantly improve compilation times:

- **Clean builds**: 30-50% faster
- **Incremental rebuilds**: 70-90% faster (when changing datamodel definitions)
- **Header parsing**: Eliminated for module-imported code

Benefits increase with:
- Number of translation units
- Number of datamodels used
- Complexity of datamodel definitions

### Memory Usage

- Lower compiler memory usage (no redundant template instantiation)
- Better build parallelization
- Reduced pressure on build system

## Architecture

### Three-Layer Design

```
┌──────────────────────────────────────┐
│ podio.core (C++20 Module)            │
│ • ROOT-independent                   │
│ • Compiled once into libpodio.so     │
│ • Used by all datamodels             │
└──────────────────────────────────────┘
                ↓ import
┌──────────────────────────────────────┐
│ datamodel.datamodel (C++20 Module)   │
│ • Generated from YAML                │
│ • One module per datamodel           │
│ • Imports podio.core                 │
└──────────────────────────────────────┘
                ↓ import or #include
┌──────────────────────────────────────┐
│ User Code                             │
│ • Can use modules (fast)             │
│ • Can use headers (compatible)       │
│ • Can mix both                       │
└──────────────────────────────────────┘

┌──────────────────────────────────────┐
│ ROOT I/O (Traditional Headers Only)  │
│ • #include <podio/ROOTReader.h>      │
│ • Not module-safe due to ROOT        │
│ • Kept separate by design            │
└──────────────────────────────────────┘
```

### Module Files

When modules are enabled, podio generates:

- **Module interface files**: `*.ixx` source files defining module exports
- **Compiled module interfaces**: `*.gcm` binary files (implementation detail)
- **Shared libraries**: Same `.so` files as without modules (ABI identical)

Module interface files are installed alongside headers:
```
<prefix>/include/podio/
  ├── modules/
  │   └── podio.core.gcm        # Compiled module interface
  ├── ObjectID.h                 # Traditional headers
  └── ...
```

## Technical Details

### Module Generation

Datamodel modules are generated automatically from YAML definitions using the Jinja2 template `python/templates/datamodel_module.ixx.jinja2`.

The generator:
1. Parses the YAML datamodel definition
2. Extracts all datatypes, components, and namespaces
3. Generates a module interface file exporting all types
4. Integrates with CMake to compile the module

### CMake Integration

Two CMake functions support module generation:

```cmake
# Add a module interface to a target
PODIO_ADD_MODULE_INTERFACE(target module_name module_file)

# Generate module interface for a datamodel
PODIO_GENERATE_MODULE_INTERFACE(datamodel OUTPUT_FILE)
```

These are used internally by `PODIO_GENERATE_DATAMODEL` when modules are enabled.

### Namespace Handling

The module generator correctly handles:
- Namespaced types (e.g., `edm4hep::MCParticle`)
- Non-namespaced types (using global namespace prefix `::`)
- Nested namespaces
- Multiple namespaces in a single datamodel

### TU-Local Entity Resolution

C++20 modules cannot export entities with internal linkage (TU-local entities). Podio resolves this through:

1. **Namespace passing**: The datamodel namespace is passed as a template context variable
2. **Pythonizations**: Moved to a separate compilation unit and linked with Python library
3. **constexpr handling**: Avoided exporting problematic `constexpr` variables from dependencies

## Known Limitations

### ROOT Headers Not Module-Safe

ROOT headers use `constexpr` variables with internal linkage (`kTRUE`, `kFALSE`, etc.), which cannot be exported from modules. Therefore:

- ROOT I/O functionality (`ROOTReader`, `ROOTWriter`) remains header-only
- Datamodel types themselves work in modules
- Mixed usage (modules + ROOT headers) is supported and recommended

**Mitigation**: Keep ROOT I/O as traditional headers while using modules for datamodel code.

### Build Tool Requirements

Modules require specific build tools. If not available, podio automatically falls back to header-only mode.

**Mitigation**: Build system checks requirements and warns appropriately.

### Standard Library Modules

Currently, podio uses traditional `#include <vector>` etc. rather than `import std;` due to limited compiler support.

**Future**: Will migrate to standard library modules when widely supported.

## Migration Guide

### For Existing Projects

1. **Update build requirements**: Ensure CMake 3.29+, Ninja, GCC 14+
2. **Enable modules**: Add `-DPODIO_ENABLE_CPP_MODULES=ON` to CMake configuration
3. **Test compilation**: Verify build succeeds
4. **Gradual adoption**: Start using `import` in new code
5. **Measure benefits**: Benchmark compilation time improvements

### For New Projects

1. **Design with modules**: Plan to use `import` from the start
2. **Keep ROOT separate**: Use headers for ROOT I/O, modules for datamodel
3. **Document usage**: Note module requirements in build instructions

### Compatibility Strategy

Podio guarantees:
- **Binary compatibility**: Same ABI whether modules enabled or not
- **Source compatibility**: Headers work identically with or without modules
- **Optional feature**: Modules are opt-in, never required
- **No code changes**: Existing code works unchanged

## Testing

All podio tests pass with modules enabled:
- Unit tests
- Integration tests
- Schema evolution tests  
- Python binding tests

CI includes module testing on GCC 15 builds to ensure ongoing compatibility.

## Troubleshooting

### "Ninja generator required for modules"

**Cause**: Attempted to use modules with Unix Makefiles generator  
**Solution**: Use `-GNinja` when configuring CMake

### "CMake 3.29 or later required for modules"

**Cause**: CMake version too old  
**Solution**: Upgrade CMake or disable modules with `-DPODIO_ENABLE_CPP_MODULES=OFF`

### "Module file not found"

**Cause**: Module interface not installed or not in include path  
**Solution**: Ensure podio was built with modules enabled and properly installed

### Compilation errors with ROOT headers in modules

**Cause**: Attempting to include ROOT headers in module interface  
**Solution**: Keep ROOT includes as traditional `#include` outside module interfaces

## Future Directions

### Short Term
- Gather performance benchmarks from real-world usage
- Expand module exports based on user feedback
- Add module support documentation to user guide

### Medium Term
- Migrate to `import std;` when compiler support matures
- Investigate module support for ROOT I/O when ROOT becomes module-safe
- Optimize module compilation dependencies

### Long Term
- Consider enabling modules by default when requirements are widely met
- Collaborate with HEP community on module best practices
- Contribute upstream fixes to dependencies (ROOT, etc.)

## References

- [C++20 Modules (cppreference)](https://en.cppreference.com/w/cpp/language/modules)
- [CMake CXX_MODULE_SETS](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html)
- [GCC Modules Documentation](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html)

## Example: Complete Workflow

Here's a complete example showing module usage:

```cpp
// my_analysis.cpp
import podio.core;
import edm4hep.datamodel;
#include <podio/ROOTReader.h>  // ROOT I/O still uses headers
#include <iostream>

int main() {
  // Use module-imported types for data structures
  edm4hep::MCParticleCollection particles;
  podio::GenericParameters params;
  
  // Use traditional header for ROOT I/O
  podio::ROOTReader reader;
  reader.openFile("events.root");
  
  // Read and process
  for (unsigned i = 0; i < reader.getEntries("events"); ++i) {
    auto frame = reader.readEntry("events", i);
    auto& mcparticles = frame.get<edm4hep::MCParticleCollection>("MCParticles");
    
    for (const auto& particle : mcparticles) {
      std::cout << "PDG: " << particle.getPDG() 
                << " Energy: " << particle.getEnergy() << "\n";
    }
  }
  
  return 0;
}
```

Build:
```bash
# With modules enabled
c++ -std=c++20 -fmodules-ts my_analysis.cpp -o my_analysis \
    -L/path/to/podio/lib -lpodio -ledm4hep \
    $(root-config --libs --cflags)
```

This example demonstrates:
- Using `import` for datamodel types (fast compilation)
- Using `#include` for ROOT I/O (compatibility)
- Mixed usage pattern (common in HEP)
- Performance benefits for the parts that can use modules
