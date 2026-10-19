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

# Iterate without querying the number of entries first, since querying might
# trigger some eager loading (depending on backend) that does not happen when
# just iterating the entries
reader = podio.reading.get_reader(files)
n_events = sum(1 for _ in reader.get("events"))
assert n_events == 20, f"Expected 20 events from iterating, got {n_events}"
