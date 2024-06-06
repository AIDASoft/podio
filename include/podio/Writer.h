#ifndef PODIO_WRITER_H
#define PODIO_WRITER_H

#include "podio/Frame.h"

namespace podio {

class Writer {
public:
  // ROOT dictionary generation needs this to be public for some reason
  struct WriterConcept {
    virtual ~WriterConcept() = default;

    virtual void writeFrame(const podio::Frame& frame, const std::string& category,
                            const std::vector<std::string>& collections) = 0;
    virtual void finish() = 0;
  };

private:
  template <typename T>
  struct WriterModel final : WriterConcept {
    WriterModel(std::unique_ptr<T> writer) : m_writer(std::move(writer)) {
    }
    WriterModel(const WriterModel&) = delete;
    WriterModel& operator=(const WriterModel&) = delete;
    WriterModel(WriterModel&&) = default;
    WriterModel& operator=(WriterModel&&) = default;

    ~WriterModel() = default;

    void writeFrame(const podio::Frame& frame, const std::string& category,
                    const std::vector<std::string>& collections) override {
      return m_writer->writeFrame(frame, category, collections);
    }
    void finish() override {
      return m_writer->finish();
    }
    std::unique_ptr<T> m_writer{nullptr};
  };

  std::unique_ptr<WriterConcept> m_self{nullptr};

public:
  template <typename T>
  Writer(std::unique_ptr<T> reader) : m_self(std::make_unique<WriterModel<T>>(std::move(reader))) {
  }

  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;
  Writer(Writer&&) = default;
  Writer& operator=(Writer&&) = default;

  ~Writer() = default;

  void writeFrame(const podio::Frame& frame, const std::string& category) {
    return m_self->writeFrame(frame, category, frame.getAvailableCollections());
  }
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collections) {
    return m_self->writeFrame(frame, category, collections);
  }
  void writeEvent(const podio::Frame& frame) {
    writeFrame(frame, podio::Category::Event, frame.getAvailableCollections());
  }
  void writeEvent(const podio::Frame& frame, const std::vector<std::string>& collections) {
    writeFrame(frame, podio::Category::Event, collections);
  }
  void finish() {
    return m_self->finish();
  }
};

Writer makeWriter(const std::string& filename, const std::string& type = "default");

} // namespace podio

#endif // PODIO_WRITER_H
