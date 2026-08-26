# Reading and writing podio files

Podio supports file I/O through multiple backends in both C++ and Python. Below
are the recommended approaches for reading and writing these files, as well as
additional notes to guide usage.

## C++

Podio provides generic and format-specific I/O interfaces.

### Reading

The preferred way to read any ROOT file is with the generic reader factory:

```cpp
#include <podio/Reader.h>

auto reader = podio::makeReader(filename);
auto event = reader.getNextEvent();
```

Here, `filename` can also be a vector of strings or [POSIX glob pattern](https://www.man7.org/linux/man-pages/man7/glob.7.html) (on platforms supporting `glob.h`), allowing you to read multiple
files at once. This will automatically inspect the first file to select the
appropriate backend, creating either a TTree or RNTuple reader as needed. Mixing
TTree or RNTuples with the same reader is not supported.

Alternatively, you can instantiate format-specific readers directly:

```cpp
#include <podio/ROOTReader.h>     // For TTrees
#include <podio/RNTupleReader.h>  // For RNTuples

auto ttreeReader = podio::ROOTReader();       // For TTree files (.root)
ttreeReader.openFile(filename);               // Open a TTree file
auto rntupleReader = podio::RNTupleReader();  // For RNTuple files (.root)

for (size_t i = 0; i < ttreeReader.getEntries(podio::Category::Event); ++i) {
  auto frame = podio::Frame(ttreeReader.readNextEntry(podio::Category::Event));
  ...
}
```

### Writing

For writing, use the generic writer factory:

```cpp
#include <podio/Writer.h>

auto writer = podio::makeWriter(filename);
writer.writeFrame(frame, podio::Category::Event, collsToWrite);
```

By default, TTrees are written. To explicitly select an output backend, provide
the type:

```cpp
auto ttreeWriter = podio::makeWriter(filename, "root");         // Use TTree
auto rntupleWriter = podio::makeWriter(filename, "rntuple");    // Use RNTuple
auto parquetWriter = podio::makeWriter(filename, "parquet");    // Use Arrow/Parquet
```

The format can also be set by the environment variable `PODIO_DEFAULT_WRITE_RNTUPLE`. If
the environment variable is set **to a non-empty string**, RNTuples will be the default.

**File extensions:**
- `.root`: Uses the default backend (TTree or RNTuple if `PODIO_DEFAULT_WRITE_RNTUPLE`
  is set to a non-empty string), unless specified.
- `.sio`: Uses the SIO writer.
- `.podio_parquet`: Uses the Arrow/Parquet writer.
- Other extensions are not allowed.

Specific writers for each backend are also available:

```cpp
#include <podio/ROOTWriter.h>     // For TTree output
#include <podio/RNTupleWriter.h>  // For RNTuple output
#include <podio/ArrowWriter.h>    // For Arrow/Parquet output

podio::ROOTWriter writer(filename);              // For TTree output
podio::RNTupleWriter rntupleWriter(filename);    // For RNTuple output
podio::ArrowWriter parquetWriter(directoryName); // For Arrow/Parquet output
```

### Arrow/Parquet I/O

The Arrow/Parquet backend writes a directory with the `.podio_parquet`
extension. The directory contains one Parquet file per category and a
`metadata.json` file with the podio metadata needed for reading.

```cpp
#include <podio/Reader.h>
#include <podio/Writer.h>

auto writer = podio::makeWriter("events.podio_parquet", "parquet");
writer.writeFrame(frame, podio::Category::Event);
writer.finish();

auto reader = podio::makeReader("events.podio_parquet");
auto event = reader.readEvent(0);
```

The backend-specific classes can also be used directly:

```cpp
#include <podio/ArrowReader.h>
#include <podio/ArrowWriter.h>

podio::ArrowWriter writer("events.podio_parquet");
writer.writeFrame(frame, podio::Category::Event);
writer.finish();

podio::ArrowReader reader;
reader.openFile("events.podio_parquet");
auto event = podio::Frame(reader.readEntry(podio::Category::Event, 0));
```

The default compression for Arrow/Parquet output is controlled at configure
time with the `PODIO_ARROW_DEFAULT_COMPRESSION` CMake option. Supported values
are `UNCOMPRESSED`, `SNAPPY`, and `ZSTD`.

```{note}
Note that the generic readers and writers have methods that are not available in
the backend-specific classes. For example, the generic reader has a
`readNextEvent()` method that reads the next `podio::Frame` in the category of
events, while using the backend-specific readers, one would use `readNextEntry`
and pass the category and build a Frame with the result. Similarly, the generic
writer has a `writeEvent()` method that writes a `podio::Frame` in the category
used for events.
```

## Python

Python does not currently provide a single generic Reader or Writer class, but includes helpers to select the correct one at runtime.

### Reading

Use the helper function to obtain the appropriate reader:

```python
from podio.reading import get_reader

reader = get_reader(filename)
events = reader.get(category)
```

Here, `filename` can be a string or a list of strings, allowing you to read
multiple files at once. The function will automatically select the correct reader
depending on the format of the first file.
Alternatively, instantiate backend-specific readers explicitly:

```python
from podio.root_io import Reader         # For TTrees
from podio.root_io import RNTupleReader  # For RNTuples
from podio.arrow_io import Reader as ArrowReader

reader = Reader(filename)                # For TTree files (.root)
rntuple_reader = RNTupleReader(filename) # For RNTuple files (.root)
arrow_reader = ArrowReader(directory)    # For Arrow/Parquet directories (.podio_parquet)
```

### Writing

Similarly, use the appropriate writer class for the file format:

```python
from podio.root_io import Writer         # For TTrees
from podio.root_io import RNTupleWriter  # For RNTuples
from podio.arrow_io import Writer as ArrowWriter

writer = Writer(filename)                # For TTree output (.root)
rntuple_writer = RNTupleWriter(filename) # For RNTuple output (.root)
arrow_writer = ArrowWriter(directory)    # For Arrow/Parquet output (.podio_parquet)

writer.write_frame(frame, category)
arrow_writer.write_frame(frame, category)
```
