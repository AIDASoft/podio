// ROOT specific includes
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
#include "TTreeCache.h"

// podio specific includes
#include "podio/ROOTReader.h"
#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"

namespace podio {


  CollectionBase* ROOTReader::readCollection(const std::string& name) {
    // has the collection already been constructed?
    auto p = std::find_if(begin(m_inputs), end(m_inputs),
        [name](ROOTReader::Input t){ return t.second == name;});
    if (p != end(m_inputs)){
      return p->first;
    }
    auto branch = m_chain->GetBranch(name.c_str());
    if (nullptr == branch) {
      return nullptr;
    }


    // look for involved classes
    TClass* theClass(nullptr);
    TClass* collectionClass(nullptr);
    auto result = m_storedClasses.find(name);
    if (result != m_storedClasses.end()) {
      theClass = result->second.first;
      collectionClass = result->second.second;
    } else {
      auto dataClassName = branch->GetClassName();
      theClass = gROOT->GetClass(dataClassName);
      if (theClass == nullptr) return nullptr;
      // now create the transient collections
      // some workaround until gcc supports regex properly:
      auto dataClassString = std::string(dataClassName);
      auto start = dataClassString.find("<");
      auto end   = dataClassString.find(">");
      //getting "TypeCollection" out of "vector<TypeData>"
      auto classname = dataClassString.substr(start+1, end-start-5);
      auto collectionClassName = classname+"Collection";
      collectionClass = gROOT->GetClass(collectionClassName.c_str());
      if (collectionClass == nullptr) return nullptr;
      // cache classes found for future usage
      m_storedClasses[name] = std::pair<TClass*,TClass*>(theClass, collectionClass);
    }
    // now create buffers and collections
    void* buffer = theClass->New();
    CollectionBase* collection = nullptr;
    collection = static_cast<CollectionBase*>(collectionClass->New());
    // connect buffer, collection and branch
    collection->setBuffer(buffer);
    branch->SetAddress(collection->getBufferAddress());
    m_inputs.emplace_back(std::make_pair(collection,name));
    Long64_t localEntry = m_chain->LoadTree(m_eventNumber);
    // After switching trees in the chain, branch pointers get invalidated
    // so they need to be reassigned as well as addresses
    if(localEntry == 0){
        branch = m_chain->GetBranch(name.c_str());
        branch->SetAddress(collection->getBufferAddress());
    }
    branch->GetEntry(localEntry);
    // load the collections containing references
    auto refCollections = collection->referenceCollections();

    if (refCollections != nullptr) {
      for (int i = 0, end = refCollections->size(); i!=end; ++i){
        branch = m_chain->GetBranch((name+"#"+std::to_string(i)).c_str());
        branch->SetAddress(&(*refCollections)[i]);
        branch->GetEntry(localEntry);
      }
    }
    auto id = m_table->collectionID(name);
    collection->setID(id);
    collection->prepareAfterRead();
    return collection;
  }

  void ROOTReader::openFiles(const std::vector<std::string>& filenames){
    m_chain = new TChain("events");
    for (const auto& filename:  filenames) {
      m_chain->Add(filename.c_str());
    }
    CollectionIDTable* l_table = new CollectionIDTable();
    auto metadatatree = static_cast<TTree*>(m_chain->GetFile()->Get("metadata"));
    metadatatree->SetBranchAddress("CollectionIDs",&l_table);
    metadatatree->GetEntry(0);
    auto l_names = l_table->names();
    std::vector<int> l_collectionIDs;
    for (auto name: l_names) {
      l_collectionIDs.push_back(l_table->collectionID(name));

    }
    m_table = new CollectionIDTable(l_collectionIDs, l_names);
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
