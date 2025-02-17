"""Small test case for checking get_reader working with
a single file, list of files, and a glob pattern"""

import podio

assert podio.utils.is_glob_pattern("example_frame_?.root")
files = podio.utils.expand_glob("example_frame_?.root")
assert files == ["example_frame_0.root", "example_frame_1.root"]

reader = podio.reading.get_reader("example_frame.root")
assert len(reader.get("events")) == 10
reader = podio.reading.get_reader(files)
assert len(reader.get("events")) == 20
