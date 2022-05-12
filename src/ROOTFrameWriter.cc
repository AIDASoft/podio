#include "podio/ROOTFrameWriter.h"
#include "podio/CollectionBase.h"
#include "podio/Frame.h"
#include "podio/GenericParameters.h"

#include "podio/podioVersion.h"
#include "rootUtils.h"

#include "TTree.h"

namespace podio {

ROOTFrameWriter::ROOTFrameWriter(const std::string& filename) {
  m_file = std::make_unique<TFile>(filename.c_str(), "recreate");

  m_metaTree = new TTree(root_utils::metaTreeName, "metadata tree for podio I/O functionality");
  m_metaTree->SetDirectory(m_file.get());
}

void ROOTFrameWriter::writeFrame(const podio::Frame& frame) {
  std::vector<StoreCollection> collections;
  collections.reserve(m_collsToWrite.size());

  for (const auto& name : m_collsToWrite) {
    auto* coll = frame.getCollectionForWrite(name);
    collections.emplace_back(name, const_cast<podio::CollectionBase*>(coll));
  }

  if (!m_dataTree) {
    // If we do not have a data tree yet, initialize it and create all the necessary branches in the process
    std::tie(m_dataTree, m_collectionBranches) =
        initTree(collections, const_cast<podio::GenericParameters&>(frame.getGenericParametersForWrite()),
                 frame.getCollectionIDTableForWrite(), "events");

  } else {
    // TODO: Can this be done without the const_cast? (also above)
    resetBranches(m_collectionBranches, collections,
                  &const_cast<podio::GenericParameters&>(frame.getGenericParametersForWrite()));
  }

  m_dataTree->Fill();
}

std::tuple<TTree*, std::vector<root_utils::CollectionBranches>>
ROOTFrameWriter::initTree(const std::vector<StoreCollection>& collections,
                          /*const*/ podio::GenericParameters& parameters, podio::CollectionIDTable&& idTable,
                          const std::string& category) {
  auto* tree = new TTree(category.c_str(), (category + " data tree").c_str());
  tree->SetDirectory(m_file.get());

  // create the necessary branches for storing the metadata and connect them to
  // the book-keeping members
  m_idTable = std::move(idTable);
  m_metaTree->Branch(root_utils::idTableName(category).c_str(), &m_idTable);

  m_collectionInfo.reserve(collections.size());
  m_metaTree->Branch(root_utils::collInfoName(category).c_str(), &m_collectionInfo);

  auto allBranches = std::vector<root_utils::CollectionBranches>{};
  allBranches.reserve(collections.size() + 1); // collections + parameters

  // First collections
  for (auto& [name, coll] : collections) {
    root_utils::CollectionBranches branches;
    const auto buffers = coll->getBuffers();

    // data buffer branch, only for non-subset collections
    if (buffers.data) {
      auto bufferDataType = "vector<" + coll->getDataTypeName() + ">";
      branches.data = tree->Branch(name.c_str(), bufferDataType.c_str(), buffers.data);
    }

    // reference collections
    if (auto refColls = buffers.references) {
      int i = 0;
      for (auto& c : (*refColls)) {
        const auto brName = root_utils::refBranch(name, i++);
        branches.refs.push_back(tree->Branch(brName.c_str(), c.get()));
      }
    }

    // vector members
    if (auto vmInfo = buffers.vectorMembers) {
      int i = 0;
      for (auto& [type, vec] : (*vmInfo)) {
        const auto typeName = "vector<" + type + ">";
        const auto brName = root_utils::vecBranch(name, i++);
        branches.vecs.push_back(tree->Branch(brName.c_str(), typeName.c_str(), vec));
      }
    }

    allBranches.push_back(branches);
    m_collectionInfo.emplace_back(m_idTable.collectionID(name), coll->getTypeName(), coll->isSubsetCollection());
  }

  // Also make branches for the parameters
  root_utils::CollectionBranches branches;
  branches.data = tree->Branch(root_utils::paramBranchName, &parameters);
  allBranches.push_back(branches);

  return {tree, allBranches};
}

void ROOTFrameWriter::resetBranches(std::vector<root_utils::CollectionBranches>& branches,
                                    const std::vector<ROOTFrameWriter::StoreCollection>& collections,
                                    /*const*/ podio::GenericParameters* parameters) {
  size_t iColl = 0;
  for (auto& coll : collections) {
    const auto& collBranches = branches[iColl];
    root_utils::setCollectionAddresses(coll.second->getBuffers(), collBranches);
    iColl++;
  }

  branches.back().data->SetAddress(&parameters);
}

void ROOTFrameWriter::registerForWrite(const std::string& name) {
  m_collsToWrite.push_back(name);
}

void ROOTFrameWriter::finish() {
  // Store the current podio build version into the meta data tree
  auto podioVersion = podio::version::build_version;
  m_metaTree->Branch(root_utils::versionBranchName, &podioVersion);

  m_metaTree->Fill();

  m_file->Write();
  m_file->Close();
}

} // namespace podio
