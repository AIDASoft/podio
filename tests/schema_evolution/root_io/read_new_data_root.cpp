#include "schema_evolution/read_new_data.h"

#include "podio/ROOTFrameReader.h"

int main() {
  return read_new_data<podio::ROOTFrameReader>("example_data_old_schema.root");
}
