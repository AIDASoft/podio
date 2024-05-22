#ifndef PODIO_TESTS_WRITE_INTERFACE_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_WRITE_INTERFACE_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "write_frame.h"

#include "podio/Writer.h"

void write_frames(podio::Writer& frameWriter) {

  for (int i = 0; i < 10; ++i) {
    auto frame = makeFrame(i);
    frameWriter.writeFrame(frame, podio::Category::Event, collsToWrite);
  }

  for (int i = 100; i < 110; ++i) {
    auto frame = makeFrame(i);
    frameWriter.writeFrame(frame, "other_events");
  }
}

#endif // PODIO_TESTS_WRITE_INTERFACE_H
