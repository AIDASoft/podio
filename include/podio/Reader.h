#ifndef PODIO_READER_H
#define PODIO_READER_H

#include "podio/Frame.h"
#include "podio/podioVersion.h"

namespace podio {

class Reader {
public:
  struct ReaderConcept {
    virtual ~ReaderConcept() = default;

    virtual podio::Frame readNextFrame(const std::string& name) = 0;
    virtual podio::Frame readFrame(const std::string& name, size_t index) = 0;
    virtual size_t getEntries(const std::string& name) = 0;
    virtual podio::Frame readNextEvent() = 0;
    virtual podio::Frame readEvent(size_t index) = 0;
    virtual size_t getEvents() = 0;
    virtual podio::version::Version currentFileVersion() const = 0;
  };

  template <typename T>
  struct ReaderModel : public ReaderConcept {
    ReaderModel(T* reader) : m_reader(reader) {
    }
    ReaderModel(const ReaderModel&) = delete;
    ReaderModel& operator=(const ReaderModel&) = delete;

    podio::Frame readNextFrame(const std::string& name) override {
      auto maybeFrame = m_reader->readNextEntry(name);
      if (maybeFrame) {
        return std::move(maybeFrame);
      }
      throw std::runtime_error("Could not read frame (reading beyond bounds?)");
    }
    podio::Frame readNextEvent() override {
      return readNextFrame(podio::Category::Event);
    }

    podio::Frame readFrame(const std::string& name, size_t index) override {
      auto maybeFrame = m_reader->readEntry(name, index);
      if (maybeFrame) {
        return std::move(maybeFrame);
      }
      throw std::runtime_error("Could not read frame (reading beyond bounds?)");
    }
    podio::Frame readEvent(size_t index) override {
      return readFrame(podio::Category::Event, index);
    }
    size_t getEntries(const std::string& name) override {
      return m_reader->getEntries(name);
    }
    size_t getEvents() override {
      return getEntries(podio::Category::Event);
    }
    podio::version::Version currentFileVersion() const override {
      return m_reader->currentFileVersion();
    }
    std::unique_ptr<T> m_reader;
  };

  std::unique_ptr<ReaderConcept> m_self{nullptr};

  template <typename T>
  Reader(std::unique_ptr<T>);

  podio::Frame readNextFrame(const std::string& name) {
    return m_self->readNextFrame(name);
  }
  podio::Frame readNextEvent() {
    return readNextFrame(podio::Category::Event);
  }
  podio::Frame readFrame(const std::string& name, size_t index) {
    return m_self->readFrame(name, index);
  }
  podio::Frame readEvent(size_t index) {
    return readFrame(podio::Category::Event, index);
  }
  size_t getEntries(const std::string& name) {
    return m_self->getEntries(name);
  }
  size_t getEvents() {
    return getEntries(podio::Category::Event);
  }
  podio::version::Version currentFileVersion() const {
    return m_self->currentFileVersion();
  }
};

std::unique_ptr<Reader> makeReader(const std::string& filename);
std::unique_ptr<Reader> makeReader(const std::vector<std::string>& filename);

} // namespace podio

#endif // PODIO_READER_H
