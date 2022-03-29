#ifndef PODIO_FRAME_H
#define PODIO_FRAME_H

#include "podio/CollectionBase.h"

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace podio {
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

  private:
    using CollectionMapT = std::unordered_map<std::string, std::unique_ptr<podio::CollectionBase>>;

    CollectionMapT m_collections{};
  };

  std::unique_ptr<FrameConcept> m_self;

public:
  Frame();

  /** Get a collection from the Frame
   */
  template <typename CollT>
  const CollT& get(const std::string& name) const;

  /** (Destructively) move a collection into the Frame and get a const reference
   * back for further use
   */
  template <typename CollT, typename = std::enable_if_t<!std::is_lvalue_reference_v<CollT>>>
  const CollT& put(CollT&& coll, const std::string& name);
};

// implementations below

Frame::Frame() : m_self(std::make_unique<FrameModel>()) {
}

template <typename CollT>
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
  if (const auto it = m_collections.find(name); it != m_collections.end()) {
    return it->second.get();
  }

  return nullptr;
}

const podio::CollectionBase* Frame::FrameModel::put(std::unique_ptr<podio::CollectionBase> coll,
                                                    const std::string& name) {
  auto [it, success] = m_collections.try_emplace(name, std::move(coll));
  if (success) {
    return it->second.get();
  }

  return nullptr;
}
} // namespace podio

#endif // PODIO_FRAME_H
