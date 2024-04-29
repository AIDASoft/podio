#ifndef PODIO_WRITER_H
#define PODIO_WRITER_H

#include "podio/Frame.h"
#include "podio/podioVersion.h"

namespace podio {

class Writer {
public:
  struct WriterConcept {
    virtual ~WriterConcept() = default;

    virtual void writeFrame(const podio::Frame& frame, const std::string& category) = 0;
    virtual void writeFrame(const podio::Frame& frame, const std::string& category,
                            const std::vector<std::string>& collections) = 0;
    virtual void writeEvent(const podio::Frame& frame) = 0;
    virtual void writeEvent(const podio::Frame& frame, const std::vector<std::string>& collections) = 0;
    virtual void finish() = 0;
  };

  template <typename T>
  struct WriterModel final : public WriterConcept {
    WriterModel(std::unique_ptr<T> writer) : m_writer(std::move(writer)) {
    }
    WriterModel(const WriterModel&) = delete;
    WriterModel& operator=(const WriterModel&) = delete;
    WriterModel(WriterModel&&) = default;
    WriterModel& operator=(WriterModel&&) = default;

    ~WriterModel() = default;

    void writeFrame(const podio::Frame& frame, const std::string& category) override {
      return m_writer->writeFrame(frame, category);
    }
    void writeFrame(const podio::Frame& frame, const std::string& category,
                    const std::vector<std::string>& collections) override {
      return m_writer->writeFrame(frame, category, collections);
    }
    void writeEvent(const podio::Frame& frame) override {
      return writeFrame(frame, podio::Category::Event);
    }
    void writeEvent(const podio::Frame& frame, const std::vector<std::string>& collections) override {
      return writeFrame(frame, podio::Category::Event, collections);
    }
    void finish() override {
      return m_writer->finish();
    }
    std::unique_ptr<T> m_writer{nullptr};
  };

  std::unique_ptr<WriterConcept> m_self{nullptr};

  template <typename T>
  Writer(std::unique_ptr<T> reader) : m_self(std::make_unique<WriterModel<T>>(std::move(reader))) {
  }

  Writer(const Writer&) = delete;
  Writer& operator=(const Writer&) = delete;
  Writer(Writer&&) = default;
  Writer& operator=(Writer&&) = default;

  ~Writer() = default;

  void writeFrame(const podio::Frame& frame, const std::string& category) {
    return m_self->writeFrame(frame, category);
  }
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collections) {
    return m_self->writeFrame(frame, category, collections);
  }
  void writeEvent(const podio::Frame& frame) {
    return writeFrame(frame, podio::Category::Event);
  }
  void writeEvent(const podio::Frame& frame, const std::vector<std::string>& collections) {
    return writeFrame(frame, podio::Category::Event, collections);
  }
  void finish() {
    return m_self->finish();
  }
};

Writer makeWriter(const std::string& filename, const std::string& type = "default");

} // namespace podio

#endif // PODIO_WRITER_H
