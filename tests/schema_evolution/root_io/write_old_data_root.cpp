#include "schema_evolution/write_old_data.h"

#include "podio/ROOTFrameWriter.h"

int main() {
  return writeData<podio::ROOTFrameWriter>("example_data_old_schema.root");
}
