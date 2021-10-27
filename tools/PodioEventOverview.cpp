#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/utilities.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

constexpr const auto usage_short = R"(usage: PodioEventOverview inputfile [[event] [first-event last-event]] [-h|--help]
)";

constexpr const auto usage_long = R"ulong(Print event contents: collection names, types and number of elements) of events or the first event in the file (default)

positional arguments:
  inputfile               The file to dump
  event                   Dump only this event (-1 for all events)
  first-event last event  Dump all events in this range [first-event, last-event)

optional arguments:
  -h, --help              show ths help message and exit
)ulong";


int main(int argc, char* argv[]){
  
  if(argc<2) {
    std::cout << usage_short;
    return 1;
  }
  if (std::find_if(argv, argv + argc, [](std::string arg) { return arg == "-h" || arg == "--help" ;} ) != argv + argc) {
    std::cout << usage_short << usage_long;
    return 1;
  }

  const std::string FileName{argv[1]};
  
  //Declaring the reader object and opening the file
  auto reader = podio::utils::createReader(FileName);
  if (!reader) {
    std::cerr << "Cannot deduce the reader that would be necessary for reading the file: " << FileName << std::endl;
    return 1;
  }
  reader->openFile(FileName);
  const int eventNumber=reader->getEntries();
  int endEvent = 1; // default print the first event
  int startEvent = 0;
  if (argc==3) {
    try {
      startEvent = std::stoi(argv[2]);
      if (startEvent >= eventNumber) {
        startEvent = eventNumber - 1;
      }
      endEvent = startEvent + 1;

      if (startEvent == -1) { // all events
        endEvent = eventNumber;
        startEvent = 0;
      }
    } catch (std::invalid_argument& ex) {
      std::cerr << "Cannot convert " << argv[2] << " to an event number: " << ex.what() << std::endl;
      return 1;
    }
  }
  if (argc==4) {
    try{
      startEvent = std::stoi(argv[2]);
      endEvent = std::stoi(argv[3]);
      if (startEvent < 0) {
        startEvent = 0;
      }
      if (endEvent > eventNumber) {
        endEvent = eventNumber;
      }
    } catch (std::invalid_argument& ex) {
      std::cerr << "Cannot convert " << argv[2] << " or " << argv[3] << " to an event number: " << ex.what() << std::endl;
      return 1;
    }
  }

  //Printing important info of the file
  std::cout << "input file: " << FileName << std::endl;
  std::cout << "Number of events: " << eventNumber << std::endl;
  std::cout << std::endl;
  
  //Getting Table containing the info about the collections
  const auto collIDTable = reader->getCollectionIDTable();
  const auto collNames = collIDTable->names();
  //Iterating over all events to get size for each event
  for (int i = startEvent; i < endEvent; i++) {

    std::cout << "Event Number " << i << std::endl;
    std::cout << std::left << std::setw(30) <<"Name" << std::left << std::setw(40) << "Type" << "Collection Size" <<std::endl;
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;

    reader->readEvent();
    //Iterating over the collections to get each kind of data type
    for (const auto& name : collNames) {
      
      const auto coll = reader->readCollection(name);
      auto size = coll->size();
      const auto type = coll->getValueTypeName();
      //Printing Collection Names
      std::cout << std::left << std::setw(30) << name << std::left << std::setw(40) << type << size <<"\n";
    }
    reader->endOfEvent();

    std::cout << std::endl;
  }
  return 0;
}
