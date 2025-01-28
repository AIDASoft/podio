"""Small test case for checking get_reader working with
a single file, list of files, and a glob pattern"""

import podio

reader = podio.reading.get_reader("example_frame.root")
assert len(reader.get("events")) == 10
reader = podio.reading.get_reader(["example_frame_0.root", "example_frame_1.root"])
assert len(reader.get("events")) == 20
reader = podio.reading.get_reader("example_frame_?.root")
assert len(reader.get("events")) == 20
