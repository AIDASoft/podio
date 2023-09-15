#!/usr/bin/env python3
"""Script to write a Frame in ROOT format"""

import os
import sys

sys.path.append(os.path.join(os.environ.get("PODIO_BASE"), "python", "podio"))

import test_utils
from podio.root_io import Writer

test_utils.write_file(Writer, "example_frame_with_py.root")
