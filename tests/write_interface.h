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

  // Two entries for a >15-char category name used in test_transient_category_strings.
  frameWriter.writeFrame(makeFrame(0), "events_extended");
  frameWriter.writeFrame(makeFrame(1), "events_extended");
}

#endif // PODIO_TESTS_WRITE_INTERFACE_H
