#ifndef PODIO_FRAME_H
#define PODIO_FRAME_H

#include "podio/CollectionBase.h"
#include "podio/GenericParameters.h"
#include "podio/utilities/TypeHelpers.h"

#include <initializer_list>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace podio {

/// Alias template for enabling overloads only for Collections
template <typename T>
using EnableIfCollection = typename std::enable_if_t<isCollection<T>>;

/// Alias template for enabling overloads only for Collection r-values
template <typename T>
using EnableIfCollectionRValue = typename std::enable_if_t<isCollection<T> && !std::is_lvalue_reference_v<T>>;

/**
 * Frame class that serves as a container of collection and meta data.
 */
class Frame {
  /**
   * Internal abstract interface for the type-erased implementation of the Frame
   * class
   */
  struct FrameConcept {
    virtual ~FrameConcept() = default;
    virtual const podio::CollectionBase* get(const std::string& name) const = 0;
    virtual const podio::CollectionBase* put(std::unique_ptr<podio::CollectionBase> coll, const std::string& name) = 0;
    virtual podio::GenericParameters& parameters() = 0;
    virtual const podio::GenericParameters& parameters() const = 0;
  };

  /**
   * The interface implementation of the abstract FrameConcept that is necessary
   * for a type-erased implementation of the Frame class
   */
  struct FrameModel final : FrameConcept {

    FrameModel() = default;
    ~FrameModel() = default;
    FrameModel(const FrameModel&) = delete;
    FrameModel& operator=(const FrameModel&) = delete;
    FrameModel(FrameModel&&) = default;
    FrameModel& operator=(FrameModel&&) = default;

    /** Try and get the collection from the internal storage and return a
     * pointer to it if found. Otherwise return a nullptr
     */
    const podio::CollectionBase* get(const std::string& name) const final;

    /** Try and place the collection into the internal storage and return a
     * pointer to it. If a collection already exists or insertion fails, return
     * a nullptr
     */
    const podio::CollectionBase* put(std::unique_ptr<CollectionBase> coll, const std::string& name) final;

    podio::GenericParameters& parameters() override {
      return m_parameters;
    }
    const podio::GenericParameters& parameters() const override {
      return m_parameters;
    };

  private:
    using CollectionMapT = std::unordered_map<std::string, std::unique_ptr<podio::CollectionBase>>;

    CollectionMapT m_collections{};          ///< The internal map for storing unpacked collections
    podio::GenericParameters m_parameters{}; ///< The generic parameter store for this frame
    mutable std::mutex m_mapMutex{};         ///< The mutex for guarding the internal collection map
  };

  std::unique_ptr<FrameConcept> m_self; ///< The internal concept pointer through which all the work is done

public:
  /** Empty Frame constructor
   */
  Frame();

  // The frame is a non-copyable type
  Frame(const Frame&) = delete;
  Frame& operator=(const Frame&) = delete;

  Frame(Frame&&) = default;
  Frame& operator=(Frame&&) = default;

  /** Frame destructor */
  ~Frame() = default;

  /** Get a collection from the Frame
   */
  template <typename CollT, typename = EnableIfCollection<CollT>>
  const CollT& get(const std::string& name) const;

  /** (Destructively) move a collection into the Frame and get a const reference
   * back for further use
   */
  template <typename CollT, typename = EnableIfCollectionRValue<CollT>>
  const CollT& put(CollT&& coll, const std::string& name);

  /** Add a value to the parameters of the Frame (if the type is supported).
   * Copy the value into the internal store
   */
  template <typename T, typename = podio::EnableIfValidGenericDataType<T>>
  void putParameter(const std::string& key, T value) {
    m_self->parameters().setValue(key, value);
  }

  /** Add a string value to the parameters of the Frame by copying it. Dedicated
   * overload for enabling the on-the-fly conversion on the string literals.
   */
  void putParameter(const std::string& key, std::string value) {
    putParameter<std::string>(key, std::move(value));
  }

  /** Add a vector of strings to the parameters of the Frame (via copy).
   * Dedicated overload for enabling on-the-fly conversions of initializer_list
   * of string literals.
   */
  void putParameter(const std::string& key, std::vector<std::string> values) {
    putParameter<std::vector<std::string>>(key, std::move(values));
  }

  /** Add a vector of values into the parameters of the Frame. Overload for
   * catching on-the-fly conversions of initializer_lists of values.
   */
  template <typename T, typename = std::enable_if_t<detail::isInTuple<T, SupportedGenericDataTypes>>>
  void putParameter(const std::string& key, std::initializer_list<T>&& values) {
    putParameter<std::vector<T>>(key, std::move(values));
  }

  /** Retrieve parameters via key from the internal store. Return type will
   * either by a const reference or a value depending on the desired type.
   */
  template <typename T, typename = podio::EnableIfValidGenericDataType<T>>
  podio::GenericDataReturnType<T> getParameter(const std::string& key) const {
    return m_self->parameters().getValue<T>(key);
  }
};

// implementations below

Frame::Frame() : m_self(std::make_unique<FrameModel>()) {
}

template <typename CollT, typename>
const CollT& Frame::get(const std::string& name) const {
  const auto* coll = dynamic_cast<const CollT*>(m_self->get(name));
  if (coll) {
    return *coll;
  }
  // TODO: Handle non-existing collections
  static const auto emptyColl = CollT();
  return emptyColl;
}

template <typename CollT, typename>
const CollT& Frame::put(CollT&& coll, const std::string& name) {
  const auto* retColl = static_cast<const CollT*>(m_self->put(std::make_unique<CollT>(std::move(coll)), name));
  if (retColl) {
    return *retColl;
  }
  // TODO: Handle collision case
  static const auto emptyColl = CollT();
  return emptyColl;
}

const podio::CollectionBase* Frame::FrameModel::get(const std::string& name) const {
  {
    std::lock_guard lock(m_mapMutex);
    if (const auto it = m_collections.find(name); it != m_collections.end()) {
      return it->second.get();
    }
  }

  return nullptr;
}

const podio::CollectionBase* Frame::FrameModel::put(std::unique_ptr<podio::CollectionBase> coll,
                                                    const std::string& name) {
  {
    std::lock_guard lock(m_mapMutex);
    auto [it, success] = m_collections.try_emplace(name, std::move(coll));
    if (success) {
      return it->second.get();
    }
  }

  return nullptr;
}
} // namespace podio

#endif // PODIO_FRAME_H
