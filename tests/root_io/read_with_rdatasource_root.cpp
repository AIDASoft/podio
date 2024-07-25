#include "datamodel/ExampleClusterCollection.h"
#include "podio/DataSource.h"

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

  auto cluterEnergy = dframe.Define("cluster_energy", getEnergy, {"clusters"}).Histo1D("cluster_energy");
  cluterEnergy->Print();

  return EXIT_SUCCESS;
}
