#include "datamodel/ExampleClusterCollection.h"
#include "podio/DataSource.h"
#include "podio/Reader.h"

#include <algorithm>
#include <iostream>
#include <string>

ROOT::VecOps::RVec<float> getEnergy(const ExampleClusterCollection& inColl) {
  ROOT::VecOps::RVec<double> result;

  for (const auto& cluster : inColl) {
    result.push_back(cluster.energy());
  }

  return result;
}

int main(int argc, const char* argv[]) {
  std::string inputFile = "example_frame.root";
  if (argc == 2) {
    inputFile = argv[1];
  } else if (argc > 2) {
    std::cout << "Wrong number of arguments" << std::endl;
    std::cout << "Usage: " << argv[0] << " FILE" << std::endl;
    return 1;
  }

  auto dframe = podio::CreateDataFrame(inputFile);
  dframe.Describe().Print();
  std::cout << std::endl;

  const auto expectedCollNames = [&inputFile]() {
    auto reader = podio::makeReader(inputFile);
    auto cols = reader.readNextEvent().getAvailableCollections();
    std::ranges::sort(cols);
    return cols;
  }();
  const auto allColNames = [&dframe]() {
    auto cols = dframe.GetColumnNames();
    std::ranges::sort(cols);
    return cols;
  }();

  if (!std::ranges::equal(expectedCollNames, allColNames)) {
    std::cerr << "Column names are note as expected\n  expected: [";
    for (const auto& name : expectedCollNames) {
      std::cerr << name << " ";
    }
    std::cerr << "]\n    actual: [";
    for (const auto& name : allColNames) {
      std::cerr << name << " ";
    }
    std::cerr << "]" << std::endl;

    return EXIT_FAILURE;
  }

  auto cluterEnergy = dframe.Define("cluster_energy", getEnergy, {"clusters"}).Histo1D("cluster_energy");
  cluterEnergy->Print();

  dframe = podio::CreateDataFrame(inputFile, {"hits"});
  if (dframe.GetColumnNames()[0] != "hits") {
    std::cerr << "Limiting to only one collection didn't work as expected" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
