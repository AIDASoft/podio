#include "podio/utilities/ArrowFrameConverter.h"
#include "podio/Frame.h"
#include "podio/utilities/ArrowConverterRegistry.h"
#include "podio/utilities/ArrowTypeRegistry.h"
#include "podio/utilities/ArrowUtils.h"

#include <arrow/api.h>
#include <stdexcept>

namespace podio {

std::shared_ptr<arrow::DataType> objectRefType() {
  return arrow::struct_({
      arrow::field("collectionID", arrow::uint32(), false),
      arrow::field("index", arrow::int32(), false),
  });
}

namespace {

  void checkStatus(const arrow::Status& status, const std::string& msg) {
    podio::arrow_utils::checkStatus(status, msg);
  }

  template <typename T, typename BuilderType>
  std::shared_ptr<arrow::Array> buildParamMap(const podio::GenericParameters& params) {
    auto* pool = arrow::default_memory_pool();
    auto keyBuilder = std::make_shared<arrow::StringBuilder>(pool);
    auto valValueBuilder = std::make_shared<BuilderType>(pool);
    auto valListBuilder = std::make_shared<arrow::ListBuilder>(pool, valValueBuilder);

    arrow::MapBuilder mapBuilder(pool, keyBuilder, valListBuilder);

    checkStatus(mapBuilder.Append(), "Failed to start map entry");

    const auto& mapData = params.getMap<T>();
    for (const auto& [key, vec] : mapData) {
      checkStatus(keyBuilder->Append(key), "Failed to append map key");
      checkStatus(valListBuilder->Append(), "Failed to start map list value");
      for (const auto& val : vec) {
        checkStatus(valValueBuilder->Append(val), "Failed to append map list element");
      }
    }

    std::shared_ptr<arrow::Array> mapArray;
    checkStatus(mapBuilder.Finish(&mapArray), "Failed to finish map builder");
    return mapArray;
  }

  std::shared_ptr<arrow::Array> convertParameters(const podio::GenericParameters& params) {
    auto intMap = buildParamMap<int, arrow::Int32Builder>(params);
    auto floatMap = buildParamMap<float, arrow::FloatBuilder>(params);
    auto doubleMap = buildParamMap<double, arrow::DoubleBuilder>(params);
    auto stringMap = buildParamMap<std::string, arrow::StringBuilder>(params);

    std::vector<std::shared_ptr<arrow::Field>> fields = {
        arrow::field("int_params", arrow::map(arrow::utf8(), arrow::list(arrow::int32())), true),
        arrow::field("float_params", arrow::map(arrow::utf8(), arrow::list(arrow::float32())), true),
        arrow::field("double_params", arrow::map(arrow::utf8(), arrow::list(arrow::float64())), true),
        arrow::field("string_params", arrow::map(arrow::utf8(), arrow::list(arrow::utf8())), true),
    };

    auto structResult = arrow::StructArray::Make({intMap, floatMap, doubleMap, stringMap}, fields);
    if (!structResult.ok()) {
      throw std::runtime_error("Failed to make GenericParameters struct array: " + structResult.status().ToString());
    }
    return structResult.ValueOrDie();
  }

  std::shared_ptr<arrow::Array> convertSubsetCollection(const podio::CollectionBase* coll) {
    coll->prepareForWrite();
    auto buffers = const_cast<podio::CollectionBase*>(coll)->getBuffers();
    if (!buffers.references || buffers.references->empty()) {
      throw std::runtime_error("Subset collection buffers do not contain references");
    }
    const auto& refIDs = *(buffers.references->at(0));

    auto type = arrow::list(podio::objectRefType());
    std::unique_ptr<arrow::ArrayBuilder> builder;
    auto status = arrow::MakeBuilder(arrow::default_memory_pool(), type, &builder);
    if (!status.ok()) {
      throw std::runtime_error("Failed to create builder for subset collection: " + status.ToString());
    }

    auto* collectionBuilder = static_cast<arrow::ListBuilder*>(builder.get());
    auto* objectBuilder = static_cast<arrow::StructBuilder*>(collectionBuilder->value_builder());
    auto* collIdBuilder = static_cast<arrow::UInt32Builder*>(objectBuilder->child(0));
    auto* indexBuilder = static_cast<arrow::Int32Builder*>(objectBuilder->child(1));

    checkStatus(collectionBuilder->Append(), "Failed to append to collectionBuilder");
    for (const auto& objId : refIDs) {
      checkStatus(objectBuilder->Append(), "Failed to append to objectBuilder");
      checkStatus(collIdBuilder->Append(objId.collectionID), "Failed to append collectionID");
      checkStatus(indexBuilder->Append(objId.index), "Failed to append index");
    }

    std::shared_ptr<arrow::Array> array;
    checkStatus(collectionBuilder->Finish(&array), "Failed to finish collectionBuilder");
    return array;
  }

} // namespace

std::shared_ptr<arrow::Table> convertFrameToTable(const podio::Frame& frame,
                                                  const std::vector<std::string>& collsToWrite) {
  std::vector<std::shared_ptr<arrow::Field>> fields;
  std::vector<std::shared_ptr<arrow::Array>> arrays;

  fields.reserve(collsToWrite.size() + 1);
  arrays.reserve(collsToWrite.size() + 1);

  for (const auto& collName : collsToWrite) {
    const auto* coll = frame.getCollectionForWrite(collName);
    if (!coll) {
      throw std::runtime_error("Collection '" + collName + "' not found in Frame");
    }

    std::string typeName = std::string(coll->getValueTypeName());

    std::shared_ptr<arrow::DataType> arrowType;
    std::shared_ptr<arrow::Array> array;

    if (coll->isSubsetCollection()) {
      arrowType = arrow::list(podio::objectRefType());
      array = convertSubsetCollection(coll);
    } else {
      arrowType = podio::ArrowTypeRegistry::instance().getType(typeName);
      if (!arrowType) {
        throw std::runtime_error("Arrow type not registered for value type '" + typeName + "'");
      }

      auto converter = podio::ArrowConverterRegistry::instance().getConverter(typeName);
      if (!converter) {
        throw std::runtime_error("Arrow converter not registered for value type '" + typeName + "'");
      }

      array = converter(coll);
    }

    if (!array) {
      throw std::runtime_error("Arrow converter returned null for collection '" + collName + "'");
    }

    if (array->length() != 1) {
      throw std::runtime_error("Arrow converter for collection '" + collName + "' returned length " +
                               std::to_string(array->length()) + ", expected 1");
    }

    auto validateStatus = array->ValidateFull();
    if (!validateStatus.ok()) {
      throw std::runtime_error("Validation failed for collection '" + collName + "': " + validateStatus.ToString());
    }

    // Attach "value_type", "is_subset", and "coll_id" metadata to the field
    auto metadata = arrow::KeyValueMetadata::Make(
        {"value_type", "is_subset", "coll_id"},
        {typeName, coll->isSubsetCollection() ? "1" : "0", std::to_string(coll->getID())});
    auto field = arrow::field(collName, arrowType, /*nullable=*/true, std::move(metadata));

    fields.push_back(std::move(field));
    arrays.push_back(std::move(array));
  }

  // Convert and add frame parameters
  auto paramArray = convertParameters(frame.getParameters());
  auto paramField = arrow::field("frame_parameters", paramArray->type(), /*nullable=*/true);
  fields.push_back(std::move(paramField));
  arrays.push_back(std::move(paramArray));

  auto schema = arrow::schema(std::move(fields));
  auto batch = arrow::RecordBatch::Make(std::move(schema), 1, std::move(arrays));

  auto batchValidateStatus = batch->ValidateFull();
  if (!batchValidateStatus.ok()) {
    throw std::runtime_error("Failed to build a valid Arrow RecordBatch: " + batchValidateStatus.ToString());
  }

  auto tableResult = arrow::Table::FromRecordBatches({std::move(batch)});
  if (!tableResult.ok()) {
    throw std::runtime_error("Failed to construct Arrow Table: " + tableResult.status().ToString());
  }

  return std::move(tableResult).ValueOrDie();
}

} // namespace podio
