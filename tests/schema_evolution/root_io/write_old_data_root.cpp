#include "schema_evolution/write_old_data.h"

#include "podio/ROOTWriter.h"

int main() {
  return writeData<podio::ROOTWriter>("example_data_old_schema.root");
}
