#include "argparseUtils.h"
#include "tabulate.h"

#include "podio/Frame.h"
#include "podio/Reader.h"
#include "podio/podioVersion.h"
#include "podio/utilities/MiscHelpers.h"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <ranges>
#include <string>
#include <tuple>

namespace rv = std::ranges::views;

template <>
struct fmt::formatter<podio::version::Version> : ostream_formatter {};

struct ParsedArgs {
  std::string inputFile{};
  std::string category{"events"};
  std::vector<size_t> events{0};
  std::string dumpEDM{};
  bool detailed{false};
};

constexpr auto usageMsg = R"(usage: podio-dump [-h] [-c CATEGORY] [-e ENTRIES] [-d] [--version] inputfile)";

constexpr auto helpMsg = R"(
Dump contents of a podio file to stdout

positional arguments:
  inputfile             Name of the file to dump content from

options:
  -h, --help            show this help message and exit
  -c CATEGORY, --category CATEGORY
                        Which Frame category to dump
  -e ENTRIES, --entries ENTRIES
                        Which entries to print. A single number, comma separated list of numbers or "first:last" for an inclusive range of entries. Defaults to the first entry.
  -d, --detailed        Dump the full contents not just the collection info
  --dump-edm DUMP_EDM   Dump the specified EDM definition from the file in yaml format
  --version             show program's version number and exit
)";

void printUsageAndExit() {
  fmt::println(stderr, "{}", usageMsg);
  std::exit(1);
}

auto getArgumentValueOrExit(const std::vector<std::string>& argv, std::vector<std::string>::const_iterator it) {
  const int argc = argv.size();
  const auto index = std::distance(argv.begin(), it);
  if (index > argc - 2) {
    printUsageAndExit();
  }
  return argv[index + 1];
}

std::vector<size_t> parseEventRange(const std::string_view evtRange) {
  auto parseError = [evtRange]() {
    fmt::println(stderr, "error: argument -e/--entries: '{}' cannot be parsed into a list of entries", evtRange);
    std::exit(1);
  };

  // Split by ',' and transform into a range of string views
  auto splitRange = evtRange | rv::split(',') |
      rv::transform([](auto&& subrange) { return std::string_view(subrange.begin(), subrange.end()); });

  if (std::ranges::distance(splitRange) == 1) {
    // Only one entry, check if it's a range (start:end)
    auto colonSplitRange = evtRange | rv::split(':') |
        rv::transform([](auto&& subrange) { return std::string_view(subrange.begin(), subrange.end()); });

    const auto it = std::ranges::begin(colonSplitRange);
    const auto nextIt = std::ranges::next(it);

    if (std::ranges::distance(colonSplitRange) == 1) {
      return {parseSizeOrExit(*it)};
    } else if (std::ranges::distance(colonSplitRange) == 2 && !(*nextIt).empty()) {
      size_t start = parseSizeOrExit(*it);
      size_t stop = parseSizeOrExit(*nextIt);
      std::vector<size_t> events(stop - start + 1);
      std::iota(events.begin(), events.end(), start);
      return events;
    } else {
      parseError();
    }
  } else {
    std::vector<size_t> events;
    events.reserve(std::ranges::distance(splitRange));
    std::ranges::transform(splitRange, std::back_inserter(events), parseSizeOrExit);
    return events;
  }

  parseError();
  return {};
}

ParsedArgs parseArgs(std::vector<std::string> argv) {
  // find help or version
  if (const auto it = findFlags(argv, "-h", "--help", "--version"); it != argv.end()) {
    if (*it == "--version") {
      fmt::println("podio {}", podio::version::build_version);
    } else {
      fmt::print("{}\n{}", usageMsg, helpMsg);
    }
    std::exit(0);
  }

  ParsedArgs args;
  // detailed flag
  if (const auto it = findFlags(argv, "-d", "--detailed"); it != argv.end()) {
    args.detailed = true;
    argv.erase(it);
  }
  // category
  if (const auto it = findFlags(argv, "-c", "--category"); it != argv.end()) {
    args.category = getArgumentValueOrExit(argv, it);
    argv.erase(it, it + 2);
  }
  // event range
  if (const auto it = findFlags(argv, "-e", "--entries"); it != argv.end()) {
    args.events = parseEventRange(*(it + 1));
    argv.erase(it, it + 2);
  }
  // dump-edm
  if (const auto it = findFlags(argv, "--dump-edm"); it != argv.end()) {
    args.dumpEDM = getArgumentValueOrExit(argv, it);
    argv.erase(it, it + 2);
  }

  if (argv.size() != 1) {
    printUsageAndExit();
  }
  args.inputFile = argv[0];

  return args;
}

template <typename T>
consteval const std::string_view getTypeString() {
  using namespace std::string_view_literals;
  if constexpr (std::is_same_v<T, int>) {
    return "int"sv;
  } else if constexpr (std::is_same_v<T, float>) {
    return "float"sv;
  } else if constexpr (std::is_same_v<T, double>) {
    return "double"sv;
  } else if constexpr (std::is_same_v<T, std::string>) {
    return "std::string"sv;
  }

  return "unknown"sv;
}

template <typename T>
void getParameterOverview(const podio::Frame& frame,
                          std::vector<std::tuple<std::string, std::string_view, size_t>>& rows) {
  constexpr auto typeString = getTypeString<T>();
  for (const auto& parKey : podio::utils::sortAlphabeticaly(frame.getParameterKeys<T>())) {
    rows.emplace_back(parKey, typeString, frame.getParameter<std::vector<T>>(parKey)->size());
  }
}

void printFrameOverview(const podio::Frame& frame) {
  fmt::println("Collections:");
  const auto collNames = frame.getAvailableCollections();

  std::vector<std::tuple<std::string, std::string_view, size_t, std::string>> rows;
  rows.reserve(collNames.size());

  for (const auto& name : podio::utils::sortAlphabeticaly(collNames)) {
    const auto coll = frame.get(name);
    rows.emplace_back(name, coll->getValueTypeName(), coll->size(), fmt::format("{:0>8x}", coll->getID()));
  }
  printTable(rows, {"Name", "ValueType", "Size", "ID"});

  fmt::println("\nParameters:");
  std::vector<std::tuple<std::string, std::string_view, size_t>> paramRows{};
  getParameterOverview<int>(frame, paramRows);
  getParameterOverview<float>(frame, paramRows);
  getParameterOverview<double>(frame, paramRows);
  getParameterOverview<std::string>(frame, paramRows);

  printTable(paramRows, {"Name", "Type", "Elements"});
}

void printFrameDetailed(const podio::Frame& frame) {
  fmt::println("Collections:");
  const auto collNames = frame.getAvailableCollections();
  for (const auto& name : podio::utils::sortAlphabeticaly(collNames)) {
    const auto coll = frame.get(name);
    fmt::println("{}", name);
    coll->print();
    fmt::println("");
  }

  fmt::println("\nParameters:");
  frame.getParameters().print();
  fmt::println("");
}

void printGeneralInfo(const podio::Reader& reader, const std::string& filename) {
  fmt::println("input file: {}", filename);
  fmt::println("            (written with podio version: {})", reader.currentFileVersion());
  fmt::println("\ndatamodel model definitions stored in this file:");
  for (const auto& modelName : reader.getAvailableDatamodels()) {
    const auto modelVersion = reader.currentFileVersion(modelName);
    if (modelVersion) {
      fmt::println(" - {} ({})", modelName, modelVersion.value());
    } else {
      fmt::println(" - {}", modelName);
    }
  }

  std::vector<std::tuple<std::string, size_t>> rows{};
  for (const auto& cat : reader.getAvailableCategories()) {
    rows.emplace_back(cat, reader.getEntries(std::string(cat)));
  }
  fmt::println("\nFrame categories in this file:");
  printTable(rows, {"Name", "Entries"});
}

int dumpEDMDefinition(const podio::Reader& reader, const std::string& modelName) {
  const auto availModels = reader.getAvailableDatamodels();
  if (const auto it = std::ranges::find(availModels, modelName); it == availModels.end()) {
    fmt::println(stderr, "ERROR: cannot dump model '{}' (not present in file)", modelName);
    return 1;
  }
  fmt::println("{}", reader.getDatamodelDefinition(modelName));
  return 0;
}

void printFrame(const podio::Frame& frame, const std::string& category, size_t iEntry, bool detailed) {
  fmt::println("{:#^82}", fmt::format(" {}: {} ", category, iEntry));
  if (detailed) {
    printFrameDetailed(frame);
  } else {
    printFrameOverview(frame);
  }
}

int main(int argc, char* argv[]) {
  // We strip the executable name off directly for parsing
  const auto args = parseArgs({argv + 1, argv + argc});

  auto reader = podio::makeReader(args.inputFile);
  if (!args.dumpEDM.empty()) {
    return dumpEDMDefinition(reader, args.dumpEDM);
  }

  printGeneralInfo(reader, args.inputFile);

  for (const auto event : args.events) {
    try {
      const auto& frame = reader.readFrame(args.category, event);
      printFrame(frame, args.category, event, args.detailed);
    } catch (std::runtime_error& err) {
      fmt::println(stderr, "{}", err.what());
      return 1;
    }
  }

  return 0;
}
