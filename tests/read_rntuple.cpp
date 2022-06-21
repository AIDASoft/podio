
#include <cstdint>

#include <iostream>
#include <string>
#include <tuple>
#include <TRandom.h>
#include <TClass.h>
#include <TFile.h>

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

#include "podio/ObjectID.h"

#include "datamodel/ExampleMCCollection.h"

// todo: temporary readtest, replace after ROOTRNTupleReader implemented


using RNTupleModel = ROOT::Experimental::RNTupleModel;
using RNTupleReader = ROOT::Experimental::RNTupleReader;
using RNTupleWriter = ROOT::Experimental::RNTupleWriter;
using RFieldBase = ROOT::Experimental::Detail::RFieldBase;
using namespace ROOT::Experimental;

int main() {


   auto ntuple = RNTupleReader::Open("events", "example_rntuple.root");

   // Display the schema of the ntuple
   ntuple->PrintInfo();

   // Display information about the storage layout of the data
   ntuple->PrintInfo(ENTupleInfo::kStorageDetails);

   // Display the first entry; no model was defined, hence we use kCompleteJ    SON to force showing all fields
   ntuple->Show(0, ENTupleShowFormat::kCompleteJSON);


     // {
//        std::cout << "Create ..." <<std::endl;
//      // read back the metadata from file 
//      auto model_metadata = RNTupleModel::Create();
//        std::cout << "make_fields ..." <<std::endl;
//      auto collIds = model_metadata->MakeField<std::vector<int>>("CollectionIDs");
//      //auto collNames = model_metadata->MakeField<std::vector<std::string>>("CollectionNames");
//      //auto collTypes = model_metadata->MakeField<std::vector<std::string>>("CollectionTypes");
//      //auto isSub = model_metadata->MakeField<std::vector<bool>>("IsSubsetCollection");
//      // as per open lhcb example
//        std::cout << "open..." <<std::endl;
//      auto ntuple_m = RNTupleReader::Open(std::move(model_metadata), "metadata", "example_rntuple.root");
//        std::cout << "metadata" <<std::endl;
//      ntuple_m->Show(0);
//      int _ii = 0;
//      for (auto entryId : *ntuple_m) {
//        if (_ii++ > 10) continue;
//        std::cout << _ii <<std::endl;
//        
//        ntuple_m->LoadEntry(entryId);
//        for (int i = 0; i < collIds->size(); i++) {
//
//
//        //std::cout << "collnames " << (*collNames)[i] << " " << (*collIds)[i] << "\t" << (*collTypes)[i] << "\t" << (*isSub)[i] << std::endl;
//      }
//      }
//      }
//      {
//
//      // read back the collection data from file
//      auto model = RNTupleModel::Create();
//      // as per open lhcb example
//      auto field = RFieldBase::Create("mcparticles",  "vector<ExampleMCData>").Unwrap();
//      model->AddField(std::move(field));
//      void *fieldDataPtr = model->GetDefaultEntry()->GetValue("mcparticles").GetRawPtr();
//      auto ntuple = RNTupleReader::Open(std::move(model), "events", "example_rntuple.root");
//      ntuple->Show(0);
//      int _i = 0;
//      for (auto entryId : *ntuple) {
//        if (_i++ > 10) continue;
//        
//        ntuple->LoadEntry(entryId);
//        std::cout << (static_cast<std::vector<ExampleMCData>*>(fieldDataPtr))->size() << std::endl;
//      }
//      }
  return 0;
}
