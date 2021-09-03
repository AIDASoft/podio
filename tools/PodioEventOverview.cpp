#include <iostream>
#include <iomanip>
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/SIOReader.h"
#include "podio/CollectionBase.h"
#include "podio/tools.h"

int main(int argc, char* argv[]){
  
  if(argc<2){
    std::cout<<"ERROR FileName not received"<<std::endl;
    return 1;
  }
  std::string FileName{argv[1]};
  
  //Declaring the reader object and openning the file
  auto reader = getReader(FileName);
  reader->openFile(FileName);
  int eventNumber=reader->getEntries();
  int readEvent=1;
  int startEvent=0;
  if(argc==3){
    try {
      readEvent = std::stoi(argv[2])+1;
      startEvent= readEvent-1;
      if(readEvent==-1)	{
	readEvent=eventNumber;
	startEvent=0;
      }
      
    } catch (std::invalid_argument& ex) {
      std::cerr << "Cannot convert " << argv[2] << " to an event number: " << ex.what() << std::endl;
      return 1;
    }
  }
  if(argc==4){
    try{
      startEvent = std::stoi(argv[2]);
      readEvent  = std::stoi(argv[3]);
      
    } catch (std::invalid_argument& ex) {
      std::cerr << "Cannot convert " << argv[2] << " to an event number: " << ex.what() << std::endl;
      return 1;
    }
  }

  if (startEvent>=readEvent){
    std::cout<<"Can not use specified range"<<std::endl;
  }
  
  if (readEvent > eventNumber) {
    std::cerr << "Only have " << eventNumber << " events to read. " << std::endl;
    readEvent = eventNumber;
  }
  
  //Printing important info of the file
  std::cout<<"FileName: "<<FileName<<std::endl;
  std::cout<<"Number of events: "<<eventNumber<<std::endl;
  std::cout<<std::endl;
  
  //Getting Table containing the info about the collections
  const auto collIDTable = reader->getCollectionIDTable();
  const auto collNames = collIDTable->names();
  //Iterating over all events to get size for each event
  for(int i=startEvent; i<readEvent; i++){

    std::cout<<"Event Number "<<i<<std::endl;
    std::cout<<std::left<<std::setw(30)<<"Name"<<std::left<<std::setw(40)<<"Type"<<"Colection Size"<<std::endl;
    std::cout<<"-------------------------------------------------------------------------------------"<<std::endl;

    reader->readEvent();
    //Iterating over the collections to get each kind of data type
    for (const auto& name : collNames) {
      
      const auto coll = reader->readCollection(name);
      auto size = coll->size();
      const auto type = coll->getValueTypeName();
      //Printing Collection Names
      std::cout<<std::left<<std::setw(30)<<name<<std::left<<std::setw(40)<<type<<size<<"\n";
    }
    reader->endOfEvent();

    std::cout<<std::endl;
  }
  return 0;
}
