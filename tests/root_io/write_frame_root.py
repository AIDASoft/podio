#!/usr/bin/env python3
"""Script to write a Frame in ROOT format"""

from podio import test_utils

from podio.root_io import Writer  # pylint: disable=wrong-import-position


test_utils.write_file(Writer, "example_frame_with_py.root")
