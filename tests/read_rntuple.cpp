
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

int main() {
      // read back the collection data from file
      auto model = RNTupleModel::Create();
      auto model_metadata = RNTupleModel::Create();
      // as per open lhcb example
      auto field = RFieldBase::Create("mcparticles",  "vector<ExampleMCData>").Unwrap();
      model->AddField(std::move(field));
      auto metadata = model_metadata->MakeField<std::tuple<int, std::string, bool>>("metadata");
      void *fieldDataPtr = model->GetDefaultEntry()->GetValue("mcparticles").GetRawPtr();
      auto ntuple = RNTupleReader::Open(std::move(model), "events", "example_rntuple.root");
      ntuple->Show(0);
      int _i = 0;
      for (auto entryId : *ntuple) {
        if (_i++ > 10) continue;
        
        ntuple->LoadEntry(entryId);
        std::cout << (static_cast<std::vector<ExampleMCData>*>(fieldDataPtr))->size() << std::endl;
      }
  return 0;
}
