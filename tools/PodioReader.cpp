#include <iostream>
#include <iomanip>

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/SIOReader.h"
#include "podio/CollectionBase.h"


std::unique_ptr<podio::IReader> getReader(const std::string& FileName);

int main(int argc, char* argv[]){
  
  //Declaring reader object
  if(argc!=2)return 0;
  std::string FileName{argv[1]};

  
  auto reader=getReader(FileName);
  reader->openFile(FileName);
    
  //Getting Table containing the info about the collections
  const auto collIDTable = reader->getCollectionIDTable();
  const auto collNames = collIDTable->names();
  
  //Setting the ASCIIWriter and EventStore classes
  auto store = podio::EventStore();
  store.setReader(&*reader);
  auto writer = podio::ASCIIWriter("info.txt",&store);
  
  //Iterating over the collections to get each kind of data type
  for (const auto& name : collNames) {
    const auto coll = reader->readCollection(name);
    const auto type = coll->getValueTypeName();    

    coll->registerForWrite(writer, name);
  }
  
  writer.writeEvent();
  
  return 0;
}

std::unique_ptr<podio::IReader> getReader(const std::string& FileName){
  
  std::unique_ptr<podio::IReader> reader = nullptr;
  if(FileName.substr(FileName.length()-4)=="root"){
    return std::make_unique<podio::ROOTReader>();}
  else{
    return std::make_unique<podio::SIOReader>();}
  
}
