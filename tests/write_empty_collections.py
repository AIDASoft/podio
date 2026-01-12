#!/usr/bin/env python3
"""Write a frame while explicitly passing an empty collections list.

This is a regression test helper for the python writer bindings to ensure that
passing an empty list of collections behaves like the C++ writers:
- collections=None -> write all collections
- collections=[]   -> write no collections (parameters only)
"""

import ROOT  # type: ignore

# ROOT is a dynamic module; silence static type checkers.
if ROOT.gSystem.Load("libTestDataModelDict") < 0:  # type: ignore[attr-defined]
    raise RuntimeError("Could not load TestDataModel dictionary")

from ROOT import ExampleHitCollection  # pylint: disable=wrong-import-position

from podio import Frame, reading, root_io  # pylint: disable=wrong-import-position


def create_frame():
    """Create a frame with one collection and some parameters"""
    frame = Frame()

    hits = ExampleHitCollection()
    hits.create(0xBAD, 0.0, 0.0, 0.0, 23.0)
    frame.put(hits, "hits")

    frame.put_parameter("an_int", 42)
    frame.put_parameter("greetings", ["from", "python"])

    return frame


def assert_empty_collections(frame):
    """Assert that the given frame has no persisted collections"""
    if frame.getAvailableCollections():
        raise RuntimeError("Expected no persisted collections")

    try:
        frame.get("hits")
    except KeyError:
        pass
    else:
        raise RuntimeError("Collection 'hits' should not be persisted")

    if frame.get_parameter("an_int") != 42:
        raise RuntimeError("Parameter 'an_int' not stored correctly")
    if frame.get_parameter("greetings") != ["from", "python"]:
        raise RuntimeError("Parameter 'greetings' not stored correctly")


def write_file(filename):
    """Write a ROOT file passing an empty collections list"""
    if not filename.endswith(".root"):
        raise ValueError("This test helper expects a .root output file")

    writer = root_io.Writer(filename)
    frame = create_frame()

    # The important part: explicitly pass an empty list
    writer.write_frame(frame, "events", [])
    writer._writer.finish()  # pylint: disable=protected-access

    # Use the standard (TTree) reader inference and validate contents.
    reader = reading.get_reader(filename)
    if not isinstance(reader, root_io.Reader):
        raise RuntimeError("Expected the regular ROOT TTree reader")

    events = reader.get("events")
    read_frame = next(iter(events))
    assert_empty_collections(read_frame)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("outputfile", help="Output file name")

    args = parser.parse_args()
    write_file(args.outputfile)
