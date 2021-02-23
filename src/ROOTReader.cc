// podio specific includes
#include "podio/ROOTReader.h"
#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"
#include "podio/GenericParameters.h"

// ROOT specific includes
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
#include "TTreeCache.h"


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
        [&name](ROOTReader::Input t){ return t.second == name;});
    if (p != end(m_inputs)){
      return p->first;
    }

    // Have we read this collection before? If so: use the cached information to
    // speed up reading
    if (const auto& info = m_storedClasses.find(name); info != m_storedClasses.end()) {
      return getCollection(*info);
    }

    // Otherwise do some setup first and then directly read the collection
    if (auto branch = root_utils::getBranch(m_chain, name.c_str())) {
      const std::string dataClassName = branch->GetClassName();
      const auto* theClass = gROOT->GetClass(dataClassName.c_str());
      if (theClass == nullptr) return nullptr;
      // now create the transient collections
      // some workaround until gcc supports regex properly:
      auto dataClassString = std::string(dataClassName);
      auto start = dataClassString.find("<");
      auto end   = dataClassString.find(">");
      //getting "TypeCollection" out of "vector<TypeData>"
      auto classname = dataClassString.substr(start+1, end-start-5);
      auto collectionClassName = classname+"Collection";
      const auto* collectionClass = gROOT->GetClass(collectionClassName.c_str());
      if (collectionClass == nullptr) return nullptr;

      // create the branches and cache them
      root_utils::CollectionBranches branches;
      branches.data = branch;

      auto* collection = root_utils::prepareCollection(theClass, collectionClass);

      if (auto refCollections = collection->referenceCollections()) {
        for (size_t i = 0; i < refCollections->size(); ++i) {
          const auto brName = root_utils::refBranch(name, i);
          branches.refs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
        }
      }
      if (auto vecMembers = collection->vectorMembers()) {
        for (size_t i = 0; i < vecMembers->size(); ++i) {
          const auto brName = root_utils::vecBranch(name, i);
          branches.vecs.push_back(root_utils::getBranch(m_chain, brName.c_str()));
        }
      }

      // cache the information
      const auto collInfo = std::make_tuple(theClass, collectionClass, m_collectionIndex++);
      m_storedClasses[name] = collInfo;
      m_collectionBranches.push_back(branches);

      // connect the branches and the buffers
      root_utils::setCollectionAddresses(collection, branches);

      return readCollectionData(branches, collection, 0, name);
    }

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
      if (auto* refCollections = collection->referenceCollections()) {
        for (size_t i = 0; i < refCollections->size(); ++i) {
          const auto brName = root_utils::refBranch(name, i);
          branches.refs[i] = root_utils::getBranch(m_chain, brName.c_str());
        }
      }

      // vector members
      if (auto* vecMembers = collection->vectorMembers()) {
        for (size_t i = 0; i < vecMembers->size(); ++i) {
          const auto brName = root_utils::vecBranch(name, i);
          branches.vecs[i] = root_utils::getBranch(m_chain, brName.c_str());
        }
      }
    }

    // set the addresses
    root_utils::setCollectionAddresses(collection, branches);

    return readCollectionData(branches, collection, localEntry, name);
  }

  CollectionBase* ROOTReader::readCollectionData(const root_utils::CollectionBranches& branches, CollectionBase* collection, Long64_t entry, const std::string& name) {
    // Read all data
    branches.data->GetEntry(entry);
    for (auto* br : branches.refs) br->GetEntry(entry);
    for (auto* br : branches.vecs) br->GetEntry(entry);

    // do the unpacking
    const auto id = m_table->collectionID(name);
    collection->setID(id);
    collection->prepareAfterRead();

    m_inputs.emplace_back(std::make_pair(collection, name));
    return collection;
  }

  void ROOTReader::openFile(const std::string& filename){
    openFiles({filename});
  }

  void ROOTReader::openFiles(const std::vector<std::string>& filenames){
    m_chain = new TChain("events");
    for (const auto& filename:  filenames) {
      m_chain->Add(filename.c_str());
    }
    m_table = new CollectionIDTable();
    auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
    metadatatree->SetBranchAddress("CollectionIDs", &m_table);
    metadatatree->GetEntry(0);
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
