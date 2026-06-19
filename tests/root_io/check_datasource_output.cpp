/**
 * Checker for the datasource snapshot output produced by use_datasource.py.
 */

#include <TFile.h>
#include <TTree.h>

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#define CHECK(condition, msg)                                                                                          \
  if (!(condition)) {                                                                                                  \
    throw std::runtime_error(std::string("check_datasource_output: ") + (msg));                                        \
  }

#define CHECK_EQUAL(actual, expected, label)                                                                           \
  if (std::abs((actual) - (expected)) >= std::numeric_limits<double>::min()) {                                         \
    throw std::runtime_error(std::string("check_datasource_output: ") + (label) + " value mismatch: got " +            \
                             std::to_string(actual) + ", expected " + std::to_string(expected));                       \
  }

int main(int argc, const char* argv[]) {
  std::string inputFile = "datasource_snapshot.root";
  if (argc == 2) {
    inputFile = argv[1];
  } else if (argc > 2) {
    std::cout << "Usage: " << argv[0] << " [FILE]" << std::endl;
    return 1;
  }

  TFile f(inputFile.c_str(), "READ");
  if (f.IsZombie()) {
    std::cerr << "Could not open " << inputFile << std::endl;
    return EXIT_FAILURE;
  }

  auto* tree = f.Get<TTree>("events");
  CHECK(tree != nullptr, "TTree 'events' not found in " + inputFile)
  CHECK(tree->GetEntries() == 10, "Expected 10 entries in snapshot, got " + std::to_string(tree->GetEntries()))

  int event_number = 0;
  double hit_energy_0 = 0., hit_energy_1 = 0., hit_energy_sum = 0.;
  double cluster_energy_0 = 0., cluster_energy_1 = 0., cluster_energy_2 = 0.;

  double composite_hit_energy_0 = 0., composite_hit_energy_1 = 0.;

  double sub_cluster_energy_0 = 0., sub_cluster_energy_1 = 0.;

  double link0_weight = 0., link0_from_energy = 0., link0_to_energy = 0.;
  double link1_weight = 0., link1_from_energy = 0., link1_to_energy = 0.;

  tree->SetBranchAddress("event_number", &event_number);
  tree->SetBranchAddress("hit_energy_0", &hit_energy_0);
  tree->SetBranchAddress("hit_energy_1", &hit_energy_1);
  tree->SetBranchAddress("hit_energy_sum", &hit_energy_sum);
  tree->SetBranchAddress("cluster_energy_0", &cluster_energy_0);
  tree->SetBranchAddress("cluster_energy_1", &cluster_energy_1);
  tree->SetBranchAddress("cluster_energy_2", &cluster_energy_2);
  tree->SetBranchAddress("composite_hit_energy_0", &composite_hit_energy_0);
  tree->SetBranchAddress("composite_hit_energy_1", &composite_hit_energy_1);
  tree->SetBranchAddress("sub_cluster_energy_0", &sub_cluster_energy_0);
  tree->SetBranchAddress("sub_cluster_energy_1", &sub_cluster_energy_1);
  tree->SetBranchAddress("link0_weight", &link0_weight);
  tree->SetBranchAddress("link0_from_energy", &link0_from_energy);
  tree->SetBranchAddress("link0_to_energy", &link0_to_energy);
  tree->SetBranchAddress("link1_weight", &link1_weight);
  tree->SetBranchAddress("link1_from_energy", &link1_from_energy);
  tree->SetBranchAddress("link1_to_energy", &link1_to_energy);

  for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
    tree->GetEntry(entry);

    const int i = event_number; // event_number == i by construction
    CHECK(i >= 0 && i < 10, "event_number out of expected range [0,9]: " + std::to_string(i))
    const std::string ev = " (event " + std::to_string(i) + ")";

    CHECK_EQUAL(hit_energy_0, 23. + i, "hit_energy_0" + ev)
    CHECK_EQUAL(hit_energy_1, 12. + i, "hit_energy_1" + ev)
    CHECK_EQUAL(hit_energy_sum, 35. + 2. * i, "hit_energy_sum" + ev)

    CHECK_EQUAL(cluster_energy_0, 23. + i, "cluster_energy_0" + ev)
    CHECK_EQUAL(cluster_energy_1, 12. + i, "cluster_energy_1" + ev)
    CHECK_EQUAL(cluster_energy_2, 35. + 2. * i, "cluster_energy_2" + ev)

    CHECK_EQUAL(hit_energy_sum, cluster_energy_2, "hit_energy_sum vs cluster_energy_2" + ev)
    CHECK_EQUAL(hit_energy_0 + hit_energy_1, cluster_energy_2, "hit0+hit1 vs cluster_energy_2" + ev)
    CHECK_EQUAL(cluster_energy_0 + cluster_energy_1, cluster_energy_2, "sub-cluster sum vs cluster_energy_2" + ev)

    CHECK_EQUAL(composite_hit_energy_0, 23. + i, "composite_hit_energy_0 (cluster->hit relation)" + ev)
    CHECK_EQUAL(composite_hit_energy_1, 12. + i, "composite_hit_energy_1 (cluster->hit relation)" + ev)

    CHECK_EQUAL(composite_hit_energy_0, hit_energy_0, "composite_hit_energy_0 vs hit_energy_0" + ev)
    CHECK_EQUAL(composite_hit_energy_1, hit_energy_1, "composite_hit_energy_1 vs hit_energy_1" + ev)

    CHECK_EQUAL(sub_cluster_energy_0, 23. + i, "sub_cluster_energy_0 (cluster->cluster relation)" + ev)
    CHECK_EQUAL(sub_cluster_energy_1, 12. + i, "sub_cluster_energy_1 (cluster->cluster relation)" + ev)

    CHECK_EQUAL(sub_cluster_energy_0, cluster_energy_0, "sub_cluster_energy_0 vs cluster_energy_0" + ev)
    CHECK_EQUAL(sub_cluster_energy_1, cluster_energy_1, "sub_cluster_energy_1 vs cluster_energy_1" + ev)

    CHECK_EQUAL(link0_weight, 0.0, "link0_weight" + ev)
    CHECK_EQUAL(link0_from_energy, 23. + i, "link0_from_energy (link->hit relation)" + ev)
    CHECK_EQUAL(link0_to_energy, 12. + i, "link0_to_energy (link->cluster relation)" + ev)
    CHECK_EQUAL(link0_from_energy, hit_energy_0, "link0_from_energy vs hit_energy_0" + ev)
    CHECK_EQUAL(link0_to_energy, cluster_energy_1, "link0_to_energy vs cluster_energy_1" + ev)

    CHECK_EQUAL(link1_weight, 0.5, "link1_weight" + ev)
    CHECK_EQUAL(link1_from_energy, 12. + i, "link1_from_energy (link->hit relation)" + ev)
    CHECK_EQUAL(link1_to_energy, 23. + i, "link1_to_energy (link->cluster relation)" + ev)
    CHECK_EQUAL(link1_from_energy, hit_energy_1, "link1_from_energy vs hit_energy_1" + ev)
    CHECK_EQUAL(link1_to_energy, cluster_energy_0, "link1_to_energy vs cluster_energy_0" + ev)
  }

  std::cout << "check_datasource_output: all checks passed" << std::endl;
  return EXIT_SUCCESS;
}
