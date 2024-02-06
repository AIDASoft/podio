#include "read_new_data.h"

#include "podio/ROOTReader.h"

int main() {
  return read_new_data<podio::ROOTReader>("example_data_old_schema.root");
}
