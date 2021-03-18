#include "rootUtils.h"

// podio specific includes
#include "podio/ROOTReader.h"
#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"
#include "podio/GenericParameters.h"

// ROOT specific includes
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeCache.h"
#include "TClass.h"


namespace podio {

  std::pair<TTree*, unsigned> ROOTReader::getLocalTreeAndEntry(const std::string& treename) {
    auto localEntry = m_chain->LoadTree(m_eventNumber);
    auto* tree = static_cast<TTree*>(m_chain->GetFile()->Get(treename.c_str()));
    return {tree, localEntry};
  }

  GenericParameters* ROOTReader::readEventMetaData(){
    GenericParameters* emd = new GenericParameters() ;
    auto [evt_metadatatree, entry] = getLocalTreeAndEntry("evt_metadata");
    auto* branch = root_utils::getBranch(evt_metadatatree, "evtMD");
    branch->SetAddress(&emd);
    evt_metadatatree->GetEntry(entry);
    return emd ;
  }

  std::map<int,GenericParameters>* ROOTReader::readCollectionMetaData(){
    auto* emd = new std::map<int,GenericParameters> ;
    auto* col_metadatatree = getLocalTreeAndEntry("col_metadata").first;
    auto* branch = root_utils::getBranch(col_metadatatree, "colMD");
    branch->SetAddress(&emd);
    col_metadatatree->GetEntry(0);
    return emd ;
  }

  std::map<int,GenericParameters>* ROOTReader::readRunMetaData(){
    auto* emd = new std::map<int,GenericParameters> ;
    auto* run_metadatatree = getLocalTreeAndEntry("run_metadata").first;
    auto* branch = root_utils::getBranch(run_metadatatree, "runMD");
    branch->SetAddress(&emd);
    run_metadatatree->GetEntry(0);
    return emd ;
  }


  CollectionBase* ROOTReader::readCollection(const std::string& name) {
    // has the collection already been constructed?
    auto p = std::find_if(begin(m_inputs), end(m_inputs),
        [&name](const ROOTReader::Input& t){ return t.second == name;});
    if (p != end(m_inputs)){
      return p->first;
    }

    // Do we know about this collection? If so, read it otherwise we can do nothing
    if (const auto& info = m_storedClasses.find(name); info != m_storedClasses.end()) {
      return getCollection(*info);
    }

    // Otherwise this collection is not stored in this file
    return nullptr;
  }

  CollectionBase* ROOTReader::getCollection(const std::pair<std::string, CollectionInfo>& collInfo) {
    const auto& name = collInfo.first;
    const auto [theClass, collectionClass, index] = collInfo.second;
    auto& branches = m_collectionBranches[index];

    auto* collection = root_utils::prepareCollection(theClass, collectionClass);

    const auto localEntry = m_chain->LoadTree(m_eventNumber);
    // After switching trees in the chain, branch pointers get invalidated so
    // they need to be reassigned.
    // NOTE: root 6.22/06 requires that we get completely new branches here,
    // with 6.20/04 we could just re-set them
    if (localEntry == 0) {
      branches.data = root_utils::getBranch(m_chain, name.c_str());

      // reference collections
      auto* refCollections = collection->referenceCollections();
      for (size_t i = 0; i < refCollections->size(); ++i) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs[i] = root_utils::getBranch(m_chain, brName.c_str());
      }

      // vector members
      if (auto* vecMembers = collection->vectorMembers()) {
        for (size_t i = 0; i < vecMembers->size(); ++i) {
          const auto brName = root_utils::vecBranch(name, i);
          branches.vecs[i] = root_utils::getBranch(m_chain, brName.c_str());
        }
      }
    }

    root_utils::setCollectionAddresses(collection, branches);

    return readCollectionData(branches, collection, localEntry, name);
  }

  CollectionBase* ROOTReader::readCollectionData(const root_utils::CollectionBranches& branches, CollectionBase* collection, Long64_t entry, const std::string& name) {
    // Read all data
    if (branches.data) branches.data->GetEntry(entry);
    for (auto* br : branches.refs) br->GetEntry(entry);
    for (auto* br : branches.vecs) br->GetEntry(entry);

    // do the unpacking
    const auto id = m_table->collectionID(name);
    collection->setID(id);
    collection->prepareAfterRead();

    m_inputs.emplace_back(std::make_pair(collection, name));
    return collection;
  }

  void ROOTReader::createCollectionBranches(const std::vector<std::tuple<int, std::string, std::string>>& collInfo) {
    size_t collectionIndex{0};

    for (const auto& info : collInfo) {
      const auto& [collID, bufferClassName, collectionClassName] = info;
      // We only write collections that are in the collectionIDTable, so no need
      // to check here
      const auto name = m_table->name(collID);

      root_utils::CollectionBranches branches{};

      auto bufferClass = bufferClassName.empty() ? nullptr : TClass::GetClass(bufferClassName.c_str());
      auto collectionClass = TClass::GetClass(collectionClassName.c_str());

      // Need the collection here to get all the branches. Since we are not
      // handing this one off to the EventStore, we have to manage it ourselves
      auto collection = std::unique_ptr<podio::CollectionBase>(root_utils::prepareCollection(bufferClass, collectionClass));

      if (not collection->isReferenceCollection()) {
        // This branch is guaranteed to exist, since only collections that are
        // also written to file are in the info metadata that we work with here
        branches.data = root_utils::getBranch(m_chain, name.c_str());
      }

      auto refCollections = collection->referenceCollections();
      for (size_t i = 0; i < refCollections->size(); ++i) {
        const auto brName = root_utils::refBranch(name, i);
        branches.refs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
      }

      if (auto vecMembers = collection->vectorMembers()) {
        for (size_t i = 0; i < vecMembers->size(); ++i) {
          const auto brName = root_utils::vecBranch(name, i);
          branches.vecs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
        }
      }

      m_storedClasses.emplace(
        name, std::make_tuple(bufferClass, collectionClass, collectionIndex++)
      );
      m_collectionBranches.push_back(branches);
    }
  }

  void ROOTReader::openFile(const std::string& filename){
    openFiles({filename});
  }

  void ROOTReader::openFiles(const std::vector<std::string>& filenames){
    m_chain = new TChain("events");
    for (const auto& filename:  filenames) {
      m_chain->Add(filename.c_str());
    }
    // read the meta data and build the collectionBranches cache
    // NOTE: This is a small pessimization, if we do not read all collections
    // afterwards, but it makes the handling much easier in general
    auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
    // auto collectionInfo = std::make_unique<std::vector<std::tuple<int, TClass*, TClass*>>>();
    auto collectionInfo = new std::vector<std::tuple<int, std::string, std::string>>;
    m_table = new CollectionIDTable();
    metadatatree->SetBranchAddress("CollectionIDs", &m_table);
    metadatatree->SetBranchAddress("CollectionTypeInfo", &collectionInfo);
    metadatatree->GetEntry(0);

    createCollectionBranches(*collectionInfo);
    delete collectionInfo;
  }

  void ROOTReader::closeFile(){
    closeFiles();
  }

  void ROOTReader::closeFiles() {
    delete m_chain;
  }

  void ROOTReader::readEvent(){
    m_chain->GetEntry(m_eventNumber);
    // first prepare all collections in memory...
    for(auto inputs : m_inputs){
      inputs.first->prepareAfterRead();
    }
    // ...then clean-up the references between them
//    for(auto inputs : m_inputs){
  //    inputs.first->setReferences(m_registry);

  //  }
  }
  bool ROOTReader::isValid() const {
    return m_chain->GetFile()->IsOpen() && !m_chain->GetFile()->IsZombie();
  }

  ROOTReader::~ROOTReader() {
    // delete all collections
    // at the moment it is done in the EventStore;
    // TODO: who deletes the buffers?
  }

  void ROOTReader::endOfEvent() {
    ++m_eventNumber;
    m_inputs.clear();
  }

  unsigned ROOTReader::getEntries() const {
    return m_chain->GetEntries();
  }

  void ROOTReader::goToEvent(unsigned eventNumber) {
    m_eventNumber = eventNumber;
    m_inputs.clear();
  }


} //namespace
