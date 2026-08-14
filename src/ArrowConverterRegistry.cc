#include "podio/utilities/ArrowConverterRegistry.h"
#include "podio/utilities/BackendLibraryLoader.h"
#include "podio/utilities/ArrowTypeRegistry.h"
#include "podio/utilities/ArrowUtils.h"
#include "podio/UserDataCollection.h"
#include "podio/CollectionBufferFactory.h"
#include <arrow/api.h>

namespace podio {

template <typename T, typename BuilderType, typename ArrayType>
void registerPrimitiveConverter(ArrowConverterRegistry& registry) {
  const std::string typeName = userDataTypeName<T>();
  registry.registerConverter(typeName, [typeName](const podio::CollectionBase* coll) {
    const auto* concreteColl = static_cast<const podio::UserDataCollection<T>*>(coll);
    auto type = podio::ArrowTypeRegistry::instance().getType(typeName);
    
    std::unique_ptr<arrow::ArrayBuilder> builder;
    auto status = arrow::MakeBuilder(arrow::default_memory_pool(), type, &builder);
    if (!status.ok()) {
      throw std::runtime_error("Failed to create builder for primitive type " + typeName);
    }
    auto* collectionBuilder = static_cast<arrow::ListBuilder*>(builder.get());
    auto* valueBuilder = static_cast<BuilderType*>(collectionBuilder->value_builder());

    arrow_utils::checkStatus(collectionBuilder->Append(), "Failed to append to collectionBuilder");
    for (const auto& val : concreteColl->vec()) {
      arrow_utils::checkStatus(valueBuilder->Append(val), "Failed to append primitive value");
    }
    std::shared_ptr<arrow::Array> array;
    arrow_utils::checkStatus(collectionBuilder->Finish(&array), "Failed to finish collectionBuilder");
    return array;
  });

  registry.registerReader(typeName, [](std::shared_ptr<arrow::Array> array, int64_t rowIndex, bool isSubset, podio::SchemaVersionT version) -> std::optional<podio::CollectionReadBuffers> {
    auto buffers = podio::CollectionBufferFactory::instance().createBuffers(std::string(podio::userDataCollTypeName<T>()), version, isSubset);
    if (!buffers) return std::nullopt;
    
    auto list_array = std::static_pointer_cast<arrow::ListArray>(array);
    auto obj_array = list_array->value_slice(rowIndex);
    auto val_array = std::static_pointer_cast<ArrayType>(obj_array);
    
    auto* dataVec = buffers->dataAsVector<T>();
    size_t collection_size = val_array->length();
    dataVec->reserve(collection_size);
    for (size_t i = 0; i < collection_size; ++i) {
      dataVec->push_back(val_array->Value(i));
    }
    
    return buffers;
  });
}

ArrowConverterRegistry& ArrowConverterRegistry::mutInstance() {
  static ArrowConverterRegistry registry;
  static bool registered = false;
  if (!registered) {
    registered = true;
    registerPrimitiveConverter<int8_t, arrow::Int8Builder, arrow::Int8Array>(registry);
    registerPrimitiveConverter<int16_t, arrow::Int16Builder, arrow::Int16Array>(registry);
    registerPrimitiveConverter<int32_t, arrow::Int32Builder, arrow::Int32Array>(registry);
    registerPrimitiveConverter<int64_t, arrow::Int64Builder, arrow::Int64Array>(registry);
    registerPrimitiveConverter<uint8_t, arrow::UInt8Builder, arrow::UInt8Array>(registry);
    registerPrimitiveConverter<uint16_t, arrow::UInt16Builder, arrow::UInt16Array>(registry);
    registerPrimitiveConverter<uint32_t, arrow::UInt32Builder, arrow::UInt32Array>(registry);
    registerPrimitiveConverter<uint64_t, arrow::UInt64Builder, arrow::UInt64Array>(registry);
    registerPrimitiveConverter<float, arrow::FloatBuilder, arrow::FloatArray>(registry);
    registerPrimitiveConverter<double, arrow::DoubleBuilder, arrow::DoubleArray>(registry);
  }
  return registry;
}

ArrowConverterRegistry const& ArrowConverterRegistry::instance() {
  return mutInstance();
}

void ArrowConverterRegistry::registerConverter(const std::string& typeName, CreatorFunc&& converter) {
  m_registry[typeName] = std::move(converter);
}

ArrowConverterRegistry::CreatorFunc ArrowConverterRegistry::getConverter(const std::string& typeName) const {
  loadArrowLibraries();
  auto it = m_registry.find(typeName);
  if (it != m_registry.end()) {
    return it->second;
  }
  return nullptr;
}

void ArrowConverterRegistry::registerReader(const std::string& typeName, BufferReaderFunc&& reader) {
  m_readerRegistry[typeName] = std::move(reader);
}

ArrowConverterRegistry::BufferReaderFunc ArrowConverterRegistry::getReader(const std::string& typeName) const {
  loadArrowLibraries();
  auto it = m_readerRegistry.find(typeName);
  if (it != m_readerRegistry.end()) {
    return it->second;
  }
  return nullptr;
}

void loadArrowLibraries() {
  static podio::utilities::BackendLibraryLoader me("PODIO_ARROW_PATH", "PodioArrow", "Arrow");
}

} // namespace podio
