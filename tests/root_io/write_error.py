#!/usr/bin/env python3
"""Check that finish() reports write failures instead of silently succeeding."""

import os
import random
import resource
import signal
import sys
import tempfile

from podio import Frame, root_io


def check_write_error(writer_name):
    """Limit file growth only after write_frame() has successfully buffered data."""
    with tempfile.TemporaryDirectory(prefix="podio_write_error_") as tmpdir:
        filename = os.path.join(tmpdir, "events.root")
        writer = getattr(root_io, writer_name)(filename)
        frame = Frame()
        # Keep enough poorly compressible data buffered in RNTuple to exceed the
        # limit during finish(), while staying well below its cluster size.
        payload = random.Random(42).getrandbits(8 * 500_000).to_bytes(500_000, "little")
        frame.put_parameter("payload", payload.hex())
        writer.write_frame(frame, "events")

        # RNTuple needs room to create its metadata header before the buffered
        # event data is flushed by ROOT's non-throwing writer destructor. TTree
        # may already have flushed its events, so fail its next write instead.
        headroom = 64 * 1024 if writer_name == "RNTupleWriter" else 0
        limit = os.path.getsize(filename) + headroom
        old_limit = resource.getrlimit(resource.RLIMIT_FSIZE)
        old_handler = signal.signal(signal.SIGXFSZ, signal.SIG_IGN)
        error = None
        try:
            resource.setrlimit(resource.RLIMIT_FSIZE, (limit, old_limit[1]))
            try:
                writer.finish()
            except Exception as exc:  # pylint: disable=broad-except
                error = exc
        finally:
            resource.setrlimit(resource.RLIMIT_FSIZE, old_limit)
            signal.signal(signal.SIGXFSZ, old_handler)

        if error is None:
            raise AssertionError("finish() silently succeeded after a write failure")
        # An exception while creating the metadata header does not exercise the
        # swallowed destructor errors that this test is intended to catch.
        if "Failed to write " not in str(error):
            raise AssertionError(f"Unexpected failure during finish(): {error}") from error

        # A failed finalization must not be retried by subsequent finish() calls
        # or by the writer's destructor / Python's atexit handler.
        writer.finish()


if __name__ == "__main__":
    check_write_error(sys.argv[1])
