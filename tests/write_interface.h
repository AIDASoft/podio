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
