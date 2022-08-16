#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

constexpr int nExpectedEvents = 2000;
using StringVec = std::vector<std::string>;

bool verifyTree(TTree* tree, int expectedEntries, const StringVec& expectedBranches) {
  const std::string treeName = tree->GetName();
  if (tree->GetEntries() != expectedEntries) {
    std::cerr << "Tree \'" << treeName << "\' should have " << expectedEntries << " but has " << tree->GetEntries()
              << std::endl;
    return false;
  }

  const auto* branches = tree->GetListOfBranches();
  for (const auto& branch : expectedBranches) {
    bool found = false;
    for (int i = 0; i < branches->GetEntries(); ++i) {
      if (branch == branches->At(i)->GetName()) {
        found = true;
        break;
      }
    }
    if (!found) {
      std::cerr << "Branch \'" << branch << "\' was expected to be in Tree \'" << treeName
                << "\' but could not be found" << std::endl;
      return false;
    }
  }

  if ((unsigned)branches->GetEntries() != expectedBranches.size()) {
    std::cerr << "Tree \'" << treeName << "\' has additional, unexpected branches" << std::endl;
    return false;
  }

  return true;
}

void verifyBMFile(const char* fileName, const StringVec& setupBranches, const StringVec& eventBranches,
                  int expectedEvents = nExpectedEvents) {
  TFile* bmFile = TFile::Open(fileName);
  if (!bmFile) {
    std::cerr << "Benchmark file \'" << fileName << "\' does not exist!" << std::endl;
    std::exit(1);
  }
  if (!verifyTree(static_cast<TTree*>(bmFile->Get("setup_times")), 1, setupBranches)) {
    std::cerr << "In file \'" << fileName << "\' setup_times Tree does not have the expected entries" << std::endl;
    bmFile->Close();
    std::exit(1);
  }
  if (!verifyTree(static_cast<TTree*>(bmFile->Get("event_times")), expectedEvents, eventBranches)) {
    std::cerr << "In file \'" << fileName << "\' event_times Tree does not have the expected entries" << std::endl;
    bmFile->Close();
    std::exit(1);
  }

  bmFile->Close();
}

/**
 * We can't really make any checks on the actual execution times, but we can at
 * least verify that the expected timing points are here.
 */
int main(int, char* argv[]) {
  const StringVec writeBMSetupBranches = {"constructor", "finish", "register_for_write"};
  const StringVec writeBMEventBranches = {"write_event"};
  verifyBMFile(argv[1], writeBMSetupBranches, writeBMEventBranches);

  const StringVec readBMSetupBranches = {"constructor", "open_file", "close_file", "get_entries",
                                         "read_collection_ids"};
  const StringVec readBMEventBranches = {"read_collections", "read_ev_md",   "read_run_md",
                                         "read_coll_md",     "end_of_event", "read_event"};
  verifyBMFile(argv[2], readBMSetupBranches, readBMEventBranches);

  return 0;
}
