#include "argparseUtils.h"

#include "podio/Frame.h"
#include "podio/Reader.h"
#include "podio/Writer.h"

#include "TInterpreter.h"

#include <fmt/core.h>

#include <stdexcept>
#include <string>
#include <vector>

constexpr auto usageMsg = R"(usage: podio-filter-events [-o outputfile] [-c CATEGORY] filter-def inputfile)";
constexpr auto helpMsg = R"(
Filter events according to the definition in filter-def

filter-def needs to be a c++ file that contains a filter function with the following signature

bool keepFrame(const podio::Frame&)

If it returns true the Frame will be stored to the output file otherwise it will be discarded.

positional arguments 
  filter-def            Name of the file that contains the definition of the filter function
  inputfile             Name of the input file to filter

options:
  -h, --help            Show this help message and exit
  -c CATEGORY, --category CATEGORY
                        Which Frame category to filter (all others will remain unchanged!).
                        Defaults to "events".
  -o outputfile, --output outputfile
                        The name of the output file that is created. Defaults to filtered.root
)";

struct ParsedArgs {
  std::string inputFile{};
  std::string filterDef{};
  std::string outputFile{"filtered.root"};
  std::string category{"events"};
  bool verbose{false};
};

ParsedArgs parseArgs(std::vector<std::string> argv) {
  printHelpAndExit(argv, usageMsg, helpMsg);

  ParsedArgs args;
  if (const auto it = findFlags(argv, "-c", "--category"); it != argv.end()) {
    args.category = getArgumentValueOrExit(argv, it, usageMsg);
    argv.erase(it, it + 2);
  }
  if (const auto it = findFlags(argv, "-o", "--output"); it != argv.end()) {
    args.outputFile = getArgumentValueOrExit(argv, it, usageMsg);
    argv.erase(it, it + 2);
  }
  if (const auto it = findFlags(argv, "-v", "--verbose"); it != argv.end()) {
    args.verbose = true;
    argv.erase(it);
  }
  if (argv.size() != 2) {
    printUsageAndExit(usageMsg);
  }
  args.filterDef = argv[0];
  args.inputFile = argv[1];

  return args;
}

using FilterFunc = bool(const podio::Frame&);

FilterFunc* jitFilterFunc(const std::string& filterDefFile) {

  if (gInterpreter->LoadFile(filterDefFile.c_str()) != 0) {
    fmt::println(stderr, "Cannot JIT compile file {}", filterDefFile);
    std::exit(1);
  }

  if (const auto filterFunc = gInterpreter->FindSym("_Z9keepFrameRKN5podio5FrameE")) {
    return reinterpret_cast<FilterFunc*>(filterFunc);
  }

  fmt::println(stderr, "Could not find a 'keepFrame' function in {}", filterDefFile);
  std::exit(1);
}

int main(int argc, char* argv[]) {
  const auto args = parseArgs({argv + 1, argv + argc});

  auto reader = podio::makeReader(args.inputFile);
  auto writer = podio::makeWriter(args.outputFile);

  const auto filterFunc = jitFilterFunc(args.filterDef);

  const auto nEntries = reader.getEntries(args.category);
  fmt::println("Filtering file '{}' with '{}' entries using filter definition from '{}' applying it to category: {}",
               args.inputFile, nEntries, args.filterDef, args.category);

  for (size_t i = 0; i < nEntries; ++i) {
    if (args.verbose) {
      fmt::println("Processing event {}", i);
    }
    try {
      const auto& frame = reader.readFrame(args.category, i);
      if (filterFunc(frame)) {
        fmt::println("Filter decision passed");
        writer.writeFrame(frame, args.category);
      }
    } catch (std::runtime_error& err) {
      fmt::println(stderr, "{}", err.what());
      return 1;
    }
  }

  return 0;
}
