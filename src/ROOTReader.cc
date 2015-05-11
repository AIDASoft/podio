// ROOT specific includes
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"

// podio specific includes
#include "podio/ROOTReader.h"
#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"

namespace podio {

  void ROOTReader::readCollectionIDTable(){
    m_table = new CollectionIDTable();
    auto metadatatree = static_cast<TTree*>(m_file->Get("metadata"));
    metadatatree->SetBranchAddress("CollectionIDs",&m_table);
    metadatatree->GetEntry(0);
  }

  CollectionBase* ROOTReader::readCollection(const std::string& name) {
    // has the collection already been constructed?
    auto p = std::find_if(begin(m_inputs), end(m_inputs),
        [name](ROOTReader::Input t){ return t.second == name;});
    if (p != end(m_inputs)){
      return p->first;
    }
    auto branch = m_eventTree->GetBranch(name.c_str());
    if (branch == nullptr) return nullptr;
    CollectionBase* collection = nullptr;
    auto dataClassName = branch->GetClassName();
    auto theClass = gROOT->GetClass(dataClassName);
    if (theClass == nullptr) return nullptr;
    void* buffer = theClass->New();
    // now create the transient collections
    // some workaround until gcc supports regex properly:
    auto dataClassString = std::string(dataClassName);
    auto start = dataClassString.find("<");
    auto end   = dataClassString.find(">");
    //getting "TypeCollection" out of "vector<TypeData>"
    auto classname = dataClassString.substr(start+1, end-start-5);
    auto collectionClassName = classname+"Collection";
    auto collectionClass = gROOT->GetClass(collectionClassName.c_str());
    if (collectionClass == nullptr) return nullptr;
    collection = static_cast<CollectionBase*>(collectionClass->New());
    // connect buffer, collection and branch
    collection->setBuffer(buffer);
    branch->SetAddress(collection->getBufferAddress());
    m_inputs.emplace_back(std::make_pair(collection,name));
    branch->GetEntry(m_eventNumber);
    // load the collections containing references
    auto refCollections = collection->referenceCollections();
    if (refCollections != nullptr) {
      for (int i = 0, end = refCollections->size(); i!=end; ++i){
        branch = m_eventTree->GetBranch((name+"#"+std::to_string(i)).c_str());
        branch->SetAddress(&(*refCollections)[i]);
        branch->GetEntry(m_eventNumber);
      }
    }
    auto id = m_table->collectionID(name);
    collection->setID(id);
    collection->prepareAfterRead();
    return collection;
  }

  void ROOTReader::openFile(const std::string& filename){
    m_file = new TFile(filename.c_str(),"READ","data file");
    m_eventTree = static_cast<TTree*>( m_file->Get("events") );
    readCollectionIDTable();
  }

  void ROOTReader::readEvent(){
    m_eventTree->GetEntry();
    // first prepare all collections in memory...
    for(auto inputs : m_inputs){
      inputs.first->prepareAfterRead();
    }
    // ...then clean-up the references between them
//    for(auto inputs : m_inputs){
  //    inputs.first->setReferences(m_registry);

  //  }
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
    return m_eventTree->GetEntries();
  }

  void ROOTReader::goToEvent(unsigned eventNumber) {
    m_eventNumber = eventNumber;
    m_inputs.clear();
  }


} //namespace
