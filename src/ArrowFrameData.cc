#include "podio/utilities/ArrowFrameData.h"
#include "podio/utilities/ArrowConverterRegistry.h"

#include <algorithm>
#include <arrow/api.h>
#include <stdexcept>
#include <type_traits>

namespace podio {

namespace {

  template <typename ArrayType, typename ValueType>
  void extractMap(const arrow::StructArray* struct_array, const std::string& fieldName, int64_t rowIndex,
                  podio::GenericParameters* params) {
    auto map_array = std::static_pointer_cast<arrow::MapArray>(struct_array->GetFieldByName(fieldName));
    if (!map_array) {
      return;
    }
    auto keys_array = std::static_pointer_cast<arrow::StringArray>(map_array->keys());
    auto items_array = map_array->items();
    auto list_items = std::static_pointer_cast<arrow::ListArray>(items_array);

    int32_t start = map_array->value_offset(rowIndex);
    int32_t end = map_array->value_offset(rowIndex + 1);

    auto val_array = std::static_pointer_cast<ArrayType>(list_items->values());

    for (int32_t i = start; i < end; ++i) {
      std::string key = keys_array->GetString(i);

      int32_t list_start = list_items->value_offset(i);
      int32_t list_end = list_items->value_offset(i + 1);

      std::vector<ValueType> vals;
      vals.reserve(list_end - list_start);
      for (int32_t j = list_start; j < list_end; ++j) {
        if constexpr (std::is_same_v<ArrayType, arrow::StringArray>) {
          vals.push_back(val_array->GetString(j));
        } else {
          vals.push_back(val_array->Value(j));
        }
      }
      params->set(key, vals);
    }
  }

  std::shared_ptr<arrow::Array> getChunkOrThrow(const std::shared_ptr<arrow::ChunkedArray>& chunked_array,
                                                int64_t rowIndex, int64_t& remaining_idx) {
    remaining_idx = rowIndex;
    for (const auto& c : chunked_array->chunks()) {
      if (remaining_idx < c->length()) {
        return c;
      }
      remaining_idx -= c->length();
    }
    throw std::runtime_error("Row index out of bounds in chunked array");
  }

} // namespace

ArrowFrameData::ArrowFrameData(std::shared_ptr<arrow::Table> table, int64_t rowIndex,
                               const std::vector<std::string>& collsToRead) :
    m_table(std::move(table)), m_rowIndex(rowIndex), m_availableCollections(), m_idTable() {
  if (!m_table) {
    throw std::runtime_error("ArrowTable is null");
  }
  if (m_rowIndex < 0 || m_rowIndex >= m_table->num_rows()) {
    throw std::runtime_error("ArrowTable row index out of bounds");
  }

  std::vector<uint32_t> ids;
  std::vector<std::string> names;

  if (!collsToRead.empty()) {
    auto missing_coll = std::find_if(collsToRead.begin(), collsToRead.end(), [this](const std::string& coll) {
      return m_table->GetColumnByName(coll) == nullptr;
    });
    if (missing_coll != collsToRead.end()) {
      throw std::runtime_error("Collection '" + *missing_coll + "' not found in category.");
    }
  }

  auto schema = m_table->schema();
  for (int i = 0; i < schema->num_fields(); ++i) {
    auto field = schema->field(i);
    if (field->name() == "frame_parameters") {
      continue;
    }
    if (collsToRead.empty() || std::find(collsToRead.begin(), collsToRead.end(), field->name()) != collsToRead.end()) {
      m_availableCollections.push_back(field->name());
    }

    auto metadata = field->metadata();
    if (!metadata) {
      throw std::runtime_error("Missing metadata for collection column: " + field->name());
    }
    auto collIdIdx = metadata->FindKey("coll_id");
    if (collIdIdx == -1) {
      throw std::runtime_error("Missing coll_id in metadata for collection: " + field->name());
    }

    uint32_t collId = std::stoul(metadata->value(collIdIdx));
    if (std::find(ids.begin(), ids.end(), collId) != ids.end() ||
        std::find(names.begin(), names.end(), field->name()) != names.end()) {
      throw std::runtime_error("Duplicate collection ID or name: " + field->name());
    }
    ids.push_back(collId);
    names.push_back(field->name());
  }
  m_idTable = podio::CollectionIDTable(std::move(ids), std::move(names));
}

std::optional<podio::CollectionReadBuffers> ArrowFrameData::getCollectionBuffers(const std::string& name) {
  if (std::find(m_availableCollections.begin(), m_availableCollections.end(), name) == m_availableCollections.end()) {
    return std::nullopt;
  }

  auto chunked_array = m_table->GetColumnByName(name);
  if (!chunked_array) {
    return std::nullopt;
  }

  auto field = m_table->schema()->GetFieldByName(name);
  auto metadata = field->metadata();

  auto typeNameIdx = metadata->FindKey("value_type");
  auto isSubsetIdx = metadata->FindKey("is_subset");

  if (typeNameIdx == -1 || isSubsetIdx == -1) {
    throw std::runtime_error("Missing value_type or is_subset in metadata for: " + name);
  }

  std::string typeName = metadata->value(typeNameIdx);
  bool isSubset = (metadata->value(isSubsetIdx) == "1");

  auto schemaVersionIdx = metadata->FindKey("schema_version");
  uint32_t schemaVersion = 1;
  if (schemaVersionIdx != -1) {
    schemaVersion = std::stoul(metadata->value(schemaVersionIdx));
  }

  int64_t remaining_idx = 0;
  auto chunk = getChunkOrThrow(chunked_array, m_rowIndex, remaining_idx);

  auto reader = podio::ArrowConverterRegistry::instance().getReader(typeName);
  if (!reader) {
    throw std::runtime_error("No Arrow reader registered for type: " + typeName);
  }

  return reader(chunk, remaining_idx, isSubset, schemaVersion);
}

std::unique_ptr<podio::GenericParameters> ArrowFrameData::getParameters() {
  auto chunked_array = m_table->GetColumnByName("frame_parameters");
  if (!chunked_array) {
    return std::make_unique<podio::GenericParameters>();
  }
  int64_t remaining_idx = 0;
  auto chunk = getChunkOrThrow(chunked_array, m_rowIndex, remaining_idx);

  auto struct_array = std::static_pointer_cast<arrow::StructArray>(chunk);
  auto params = std::make_unique<podio::GenericParameters>();

  extractMap<arrow::Int32Array, int32_t>(struct_array.get(), "int_params", remaining_idx, params.get());
  extractMap<arrow::FloatArray, float>(struct_array.get(), "float_params", remaining_idx, params.get());
  extractMap<arrow::DoubleArray, double>(struct_array.get(), "double_params", remaining_idx, params.get());
  extractMap<arrow::StringArray, std::string>(struct_array.get(), "string_params", remaining_idx, params.get());

  return params;
}

std::vector<std::string> ArrowFrameData::getAvailableCollections() const {
  return m_availableCollections;
}

podio::CollectionIDTable ArrowFrameData::getIDTable() const {
  return {m_idTable.ids(), m_idTable.names()};
}

} // namespace podio
