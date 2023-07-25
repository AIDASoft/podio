#!/usr/bin/env python3
"""Script to write a Frame in SIO format"""

from podio import test_utils
from podio.sio_io import Writer

test_utils.write_file(Writer, "example_frame_with_py.sio")
