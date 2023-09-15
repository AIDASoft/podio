#!/usr/bin/env python3
"""Script to write a Frame in ROOT format"""

import os
import sys

sys.path.append(os.path.join(os.environ.get("PODIO_BASE"), "python", "podio"))

import test_utils  # pylint: disable=import-error, disable=wrong-import-position

from podio.root_io import Writer  # pylint: disable=wrong-import-position


test_utils.write_file(Writer, "example_frame_with_py.root")
