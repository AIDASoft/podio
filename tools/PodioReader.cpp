#include <iostream>
#include <iomanip>

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/SIOReader.h"
#include "podio/CollectionBase.h"
#include "podio/tools.h"

int main(int argc, char* argv[]){
  
  //Declaring reader object
  if(argc!=2){
    std::cout<<"ERROR FileName not received"<<std::endl;
    return 0;
  }
  std::string FileName{argv[1]};
    
  
  auto reader=getReader(FileName);
  reader->openFile(FileName);
    
  //Getting Table containing the info about the collections
  const auto collIDTable = reader->getCollectionIDTable();
  const auto collNames = collIDTable->names();
  
  //Setting the ASCIIWriter and EventStore classes
  auto store = podio::EventStore();
  store.setReader(reader.get());
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
